/*
 * Button module — ESP-IDF glue around button_logic.
 *
 * Pin config: both buttons active-low with internal pullup. Any-edge ISR
 * samples the current level, posts (pin, level, timestamp) to a queue.
 * The button task debounces, drives the gesture state machines, and
 * translates gestures into state_bus events.
 *
 * Cycle-index ownership: this module holds the color / pattern / brightness
 * cycle indices in static state. When the current state_bus value doesn't
 * match our last-known index (because the app pushed a custom value via
 * WebSocket), we snap to the nearest cycle slot before advancing. See
 * button_logic color_cycle_index_of() and friends.
 *
 * LED feedback during recessed hold / factory reset is deferred to
 * pattern_interp (see memory: project_recessed_button_led_feedback).
 */

#include "button.h"
#include "button_logic.h"

#include <stdint.h>
#include <string.h>

#include "board.h"

#include "driver/gpio.h"
#include "esp_attr.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "state_bus.h"
#include "state_bus_events.h"

static const char *TAG = "button";

#define BUTTON_TASK_STACK   4096
#define BUTTON_TASK_PRIO    (tskIDLE_PRIORITY + 4)
#define BUTTON_QUEUE_LEN    16

/* Raw ISR edge event. */
typedef struct {
    int pin;
    int level;         /* 0 = pressed (active-low), 1 = released */
    uint32_t ms;
} button_isr_evt_t;

static QueueHandle_t g_queue;
static TaskHandle_t g_task;

static primary_state_t g_primary;
static recessed_state_t g_recessed;

/* Per-pin debounce state. */
static int g_primary_last_level = 1;
static uint32_t g_primary_last_change_ms = 0;
static int g_recessed_last_level = 1;
static uint32_t g_recessed_last_change_ms = 0;

static inline uint32_t now_ms(void)
{
    return (uint32_t)(esp_timer_get_time() / 1000);
}

static void IRAM_ATTR on_gpio_isr(void *arg)
{
    int pin = (int)(intptr_t)arg;
    button_isr_evt_t evt = {
        .pin = pin,
        .level = gpio_get_level(pin),
        .ms = (uint32_t)(esp_timer_get_time() / 1000),
    };
    BaseType_t yield = pdFALSE;
    xQueueSendFromISR(g_queue, &evt, &yield);
    if (yield == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}

/* ---- state_bus fan-out ---- */

static void dispatch_primary(primary_gesture_t g)
{
    const ll_state_t *s = ll_state_bus_get();

    switch (g) {
    case PRIMARY_GESTURE_NONE:
        return;

    case PRIMARY_GESTURE_SINGLE:
        if (!s->on) {
            ll_ev_power_toggle_payload_t p = { .on = true };
            ll_state_bus_post(LL_EV_POWER_TOGGLE, &p);
        } else {
            ll_ev_base_color_payload_t p = {
                .rgb = color_cycle_advance(s->base_color_rgb),
            };
            ll_state_bus_post(LL_EV_BASE_COLOR, &p);
        }
        break;

    case PRIMARY_GESTURE_DOUBLE:
        if (!s->on) {
            /* Per button-interface §4: double-press from off turns on to
             * default state. Power on; the default color/pattern already
             * live in state_bus defaults until NVS overrides them. */
            ll_ev_power_toggle_payload_t p = { .on = true };
            ll_state_bus_post(LL_EV_POWER_TOGGLE, &p);
        } else {
            ll_ev_pattern_change_payload_t p = { 0 };
            const char *next = pattern_cycle_advance(s->pattern_id);
            strncpy(p.id, next, sizeof(p.id) - 1);
            ll_state_bus_post(LL_EV_PATTERN_CHANGE, &p);
        }
        break;

    case PRIMARY_GESTURE_TRIPLE:
        if (s->on) {
            ll_ev_brightness_payload_t p = {
                .value = brightness_cycle_advance(s->brightness),
            };
            ll_state_bus_post(LL_EV_BRIGHTNESS, &p);
        }
        break;

    case PRIMARY_GESTURE_HOLD: {
        ll_ev_power_toggle_payload_t p = { .on = false };
        ll_state_bus_post(LL_EV_POWER_TOGGLE, &p);
        break;
    }
    }

    ESP_LOGI(TAG, "primary gesture %d dispatched", (int)g);
}

static void dispatch_recessed(recessed_gesture_t g)
{
    switch (g) {
    case RECESSED_GESTURE_NONE:
        return;
    case RECESSED_GESTURE_PROVISION:
        ll_state_bus_post(LL_EV_PROVISION_START, NULL);
        ESP_LOGI(TAG, "recessed: provision start");
        break;
    case RECESSED_GESTURE_FACTORY_RESET:
        ll_state_bus_post(LL_EV_FACTORY_RESET, NULL);
        ESP_LOGW(TAG, "recessed: factory reset");
        break;
    }
}

/* ---- edge → gesture input ---- */

static void apply_edge(int pin, int level, uint32_t ms)
{
    if (pin == LL_PIN_BUTTON_PRIMARY) {
        if (level == g_primary_last_level) return;
        if ((ms - g_primary_last_change_ms) < LL_BUTTON_DEBOUNCE_MS) return;
        g_primary_last_level = level;
        g_primary_last_change_ms = ms;
        gesture_input_t in = (level == 0) ? GESTURE_INPUT_PRESS
                                          : GESTURE_INPUT_RELEASE;
        dispatch_primary(primary_step(&g_primary, in, ms));
    } else if (pin == LL_PIN_BUTTON_RESET) {
        if (level == g_recessed_last_level) return;
        if ((ms - g_recessed_last_change_ms) < LL_BUTTON_DEBOUNCE_MS) return;
        g_recessed_last_level = level;
        g_recessed_last_change_ms = ms;
        gesture_input_t in = (level == 0) ? GESTURE_INPUT_PRESS
                                          : GESTURE_INPUT_RELEASE;

        /* Fire transient hold-begin / hold-end so pattern_interp can show
         * the blue-blink cue while the user is still holding. These run
         * alongside the gesture state machine, not through it — the
         * machine only resolves to PROVISION / FACTORY_RESET on thresholds. */
        esp_event_loop_handle_t loop = ll_state_bus_get_loop();
        if (loop) {
            int32_t id = (in == GESTURE_INPUT_PRESS)
                         ? LL_EV_RECESSED_HOLD_BEGIN
                         : LL_EV_RECESSED_HOLD_END;
            esp_event_post_to(loop, LL_STATE_EVENT_BASE, id, NULL, 0, 0);
        }

        dispatch_recessed(recessed_step(&g_recessed, in, ms));
    }
}

/* Smallest deadline across both state machines, in ms-since-boot. Returns
 * UINT32_MAX if nothing is pending. */
static uint32_t next_deadline_ms(void)
{
    uint32_t d = UINT32_MAX;
    if (g_primary.pressed && !g_primary.hold_fired) {
        uint32_t t = g_primary.press_start_ms + LL_BUTTON_LONG_PRESS_MS;
        if (t < d) d = t;
    }
    if (!g_primary.pressed && g_primary.click_count > 0) {
        uint32_t t = g_primary.last_release_ms + LL_BUTTON_DOUBLE_CLICK_MS;
        if (t < d) d = t;
    }
    if (g_recessed.pressed && !g_recessed.factory_fired) {
        uint32_t t = g_recessed.press_start_ms + LL_RECESSED_FACTORY_MS;
        if (t < d) d = t;
    }
    return d;
}

static void button_task(void *arg)
{
    (void)arg;
    for (;;) {
        uint32_t now = now_ms();
        uint32_t deadline = next_deadline_ms();
        TickType_t timeout;
        if (deadline == UINT32_MAX) {
            timeout = portMAX_DELAY;
        } else if (deadline <= now) {
            timeout = 0;
        } else {
            timeout = pdMS_TO_TICKS(deadline - now);
            if (timeout == 0) timeout = 1;  /* don't busy-spin */
        }

        button_isr_evt_t evt;
        BaseType_t got = xQueueReceive(g_queue, &evt, timeout);

        now = now_ms();
        if (got == pdTRUE) {
            apply_edge(evt.pin, evt.level, now);
        }
        /* Always run ticks — a timeout expiry or an edge may have moved
         * the machine past a deadline. */
        dispatch_primary(primary_step(&g_primary, GESTURE_INPUT_TICK, now));
        dispatch_recessed(recessed_step(&g_recessed, GESTURE_INPUT_TICK, now));
    }
}

void ll_button_init(void)
{
    /* Boards without a recessed button signal absence with
     * LL_PIN_BUTTON_RESET = -1 (see boards/c3_devkit.h). All RESET-pin
     * operations are guarded so we don't 1ULL<<-1 (UB) or hand a negative
     * GPIO to the IDF (would ESP_ERROR_CHECK-panic at boot). When absent,
     * the recessed gesture state machine still ticks but never receives
     * input — provisioning + factory reset must come from another path. */
    uint64_t pin_mask = (1ULL << LL_PIN_BUTTON_PRIMARY);
    if (LL_PIN_BUTTON_RESET >= 0) {
        pin_mask |= (1ULL << LL_PIN_BUTTON_RESET);
    }
    gpio_config_t cfg = {
        .pin_bit_mask = pin_mask,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_ANYEDGE,
    };
    ESP_ERROR_CHECK(gpio_config(&cfg));

    g_queue = xQueueCreate(BUTTON_QUEUE_LEN, sizeof(button_isr_evt_t));
    configASSERT(g_queue);

    /* Idempotent: install once per app. OK to ignore ESP_ERR_INVALID_STATE
     * if something else already installed the ISR service. */
    esp_err_t err = gpio_install_isr_service(0);
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) {
        ESP_ERROR_CHECK(err);
    }
    ESP_ERROR_CHECK(gpio_isr_handler_add(LL_PIN_BUTTON_PRIMARY, on_gpio_isr,
                                          (void *)(intptr_t)LL_PIN_BUTTON_PRIMARY));
    if (LL_PIN_BUTTON_RESET >= 0) {
        ESP_ERROR_CHECK(gpio_isr_handler_add(LL_PIN_BUTTON_RESET, on_gpio_isr,
                                              (void *)(intptr_t)LL_PIN_BUTTON_RESET));
    }

    /* Seed debounce state from current pin levels so we don't fire a
     * phantom release at boot if the pin is already high. When the
     * recessed pin is absent, leave g_recessed_last_level at its
     * static-init default of 1 (released) so the apply_edge dispatcher's
     * level-equality early-out keeps the recessed branch quiescent. */
    g_primary_last_level = gpio_get_level(LL_PIN_BUTTON_PRIMARY);
    if (LL_PIN_BUTTON_RESET >= 0) {
        g_recessed_last_level = gpio_get_level(LL_PIN_BUTTON_RESET);
    }

    BaseType_t ok = xTaskCreatePinnedToCore(button_task, "button",
                                            BUTTON_TASK_STACK, NULL,
                                            BUTTON_TASK_PRIO, &g_task, 0);
    configASSERT(ok == pdPASS);

    if (LL_PIN_BUTTON_RESET >= 0) {
        ESP_LOGI(TAG, "init: primary=GPIO%d recessed=GPIO%d",
                 LL_PIN_BUTTON_PRIMARY, LL_PIN_BUTTON_RESET);
    } else {
        ESP_LOGI(TAG, "init: primary=GPIO%d (no recessed button on this board)",
                 LL_PIN_BUTTON_PRIMARY);
    }
}
