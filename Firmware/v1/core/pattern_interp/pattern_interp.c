/*
 * Pattern interpreter — render task + event glue.
 *
 * Threading:
 *   - One render task pinned to core 1 at priority +3, distinct from the
 *     state_bus task (core 0). Decouples LED output from event apply.
 *   - state_bus event handlers run on the state_bus task; they enqueue
 *     a small message to our queue and return immediately. Cue and
 *     pattern-start state are mutated only from the render task — no
 *     locks needed.
 *
 * Frame loop (60 Hz):
 *   1. Drain message queue, fold into cue / pattern_start_ms state.
 *   2. Step cue with TICK so timed cues (red/green) advance.
 *   3. Choose output:
 *      - cue active → render cue at full brightness (visible regardless
 *        of user dim setting — this is feedback, not decoration).
 *      - state.on == false → push black at 0% brightness.
 *      - else → render pattern, push at user brightness.
 */

#include "pattern_interp.h"

#include <stdlib.h>
#include <string.h>

#include "cue_logic.h"
#include "patterns.h"

#include "esp_event.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "led_driver.h"
#include "state_bus.h"
#include "state_bus_events.h"

static const char *TAG = "pattern_interp";

#define FRAME_HZ            60
#define FRAME_PERIOD_MS     (1000 / FRAME_HZ)
#define TASK_STACK          4096
#define TASK_PRIO           (tskIDLE_PRIORITY + 3)
/* All Layered Logic targets (C3, C6) are single-core from FreeRTOS's
 * point of view — C6 has a low-power core that the scheduler doesn't
 * use. Pin to core 0 universally; xTaskCreatePinnedToCore with core 1
 * panics on Unicore builds (assert in freertos_tasks_c_additions.h). */
#define TASK_CORE           0
#define MSG_QUEUE_LEN       16

typedef enum {
    PI_MSG_STATE_CHANGED,
    PI_MSG_HOLD_BEGIN,
    PI_MSG_HOLD_END,
} pi_msg_kind_e;

typedef struct {
    pi_msg_kind_e kind;
    int32_t which;   /* for STATE_CHANGED: ll_state_changed_payload_t.which */
} pi_msg_t;

static QueueHandle_t g_msg_queue;
static TaskHandle_t  g_task;
static cue_t         g_cue;
static uint32_t      g_pattern_start_ms;
static uint8_t      *g_frame_buf;     /* led_count*3 bytes */
static uint16_t      g_led_count;
static bool          g_driver_ready;

static inline uint32_t now_ms(void)
{
    return (uint32_t)(esp_timer_get_time() / 1000);
}

/* ---- state_bus event handlers (run on state_bus task, core 0) ---- */

static void on_state_changed(void *arg, esp_event_base_t base,
                             int32_t id, void *data)
{
    (void)arg; (void)base; (void)id;
    const ll_state_changed_payload_t *p = data;
    pi_msg_t m = { .kind = PI_MSG_STATE_CHANGED, .which = (int32_t)p->which };
    /* Drop on full queue: we re-read state every frame so a missed
     * notification just delays pattern_start_ms reset / factory cue
     * trigger. Render-task drain runs at 60 Hz and queue holds 16. */
    xQueueSend(g_msg_queue, &m, 0);
}

static void on_hold_event(void *arg, esp_event_base_t base,
                          int32_t id, void *data)
{
    (void)arg; (void)base; (void)data;
    pi_msg_t m = {
        .kind = (id == LL_EV_RECESSED_HOLD_BEGIN) ? PI_MSG_HOLD_BEGIN
                                                  : PI_MSG_HOLD_END,
        .which = 0,
    };
    xQueueSend(g_msg_queue, &m, 0);
}

/* ---- Render task (core 1) ---- */

static void drain_messages(uint32_t tick_ms)
{
    pi_msg_t m;
    while (xQueueReceive(g_msg_queue, &m, 0) == pdTRUE) {
        switch (m.kind) {
        case PI_MSG_STATE_CHANGED:
            if (m.which == LL_EV_PATTERN_CHANGE) {
                /* Restart animation timing so the new pattern starts at
                 * frame 0 instead of mid-cycle. */
                g_pattern_start_ms = tick_ms;
            } else if (m.which == LL_EV_FACTORY_RESET) {
                cue_step(&g_cue, CUE_EVENT_FACTORY_RESET, tick_ms);
            }
            break;
        case PI_MSG_HOLD_BEGIN:
            cue_step(&g_cue, CUE_EVENT_HOLD_BEGIN, tick_ms);
            break;
        case PI_MSG_HOLD_END:
            cue_step(&g_cue, CUE_EVENT_HOLD_END, tick_ms);
            break;
        }
    }
}

static void push_frame(uint8_t brightness_pct)
{
    if (!g_driver_ready) return;
    ll_led_driver_write(g_frame_buf, brightness_pct);
}

static void render_task(void *arg)
{
    (void)arg;
    TickType_t last_wake = xTaskGetTickCount();

    for (;;) {
        uint32_t tick = now_ms();
        drain_messages(tick);
        cue_step(&g_cue, CUE_EVENT_TICK, tick);

        const ll_state_t *s = ll_state_bus_get();

        if (cue_is_active(&g_cue)) {
            cue_render(&g_cue, tick, g_led_count, g_frame_buf);
            push_frame(100);
        } else if (!s->on) {
            memset(g_frame_buf, 0, (size_t)g_led_count * 3);
            push_frame(0);
        } else {
            pattern_ctx_t ctx = {
                .led_count       = g_led_count,
                .base_color_rgb  = s->base_color_rgb,
                .tick_ms         = tick,
                .pattern_start_ms = g_pattern_start_ms,
            };
            patterns_render(s->pattern_id, &ctx, g_frame_buf);
            push_frame(s->brightness);
        }

        vTaskDelayUntil(&last_wake, pdMS_TO_TICKS(FRAME_PERIOD_MS));
    }
}

/* ---- Init ---- */

/* Boot-time welcome sequence. Runs synchronously between led_driver_init
 * and the render task spinning up — owns the framebuffer briefly during
 * its 2-second window with no contention. Each 500ms tick lights up one
 * more side of the frame, additively, until the full strip is lit.
 *
 * Breakpoints derived from led_count so the sequence scales to any board:
 *   side_size  = ceil(led_count / 4)
 *   first_side = led_count - 3 * side_size  (backfills the remainder)
 *
 * For c3_devkit (66 LEDs):  side_size=17, first_side=15  → [15,32,49,66]
 * For prod_v1_pro (32 LEDs): side_size=8,  first_side=8   → [8,16,24,32]
 *
 * Color is bright cyan at 100% brightness for V2 of the OTA-test cycle.
 * V1' shipped with white; cyan here is the visual canary that an OTA
 * swap actually replaced the running code (white → cyan = OTA worked).
 */
static void play_welcome_sequence(void)
{
    if (!g_driver_ready) return;

    const uint16_t side_size = (g_led_count + 3) / 4;
    const uint16_t first_side = (uint16_t)(g_led_count - 3 * side_size);
    const uint16_t breakpoints[4] = {
        first_side,
        (uint16_t)(first_side + side_size),
        (uint16_t)(first_side + 2 * side_size),
        g_led_count,
    };

    memset(g_frame_buf, 0, (size_t)g_led_count * 3);
    for (int t = 0; t < 4; t++) {
        const uint16_t lit = breakpoints[t];
        for (uint16_t i = 0; i < lit; i++) {
            g_frame_buf[i * 3 + 0] = 0x00;  /* R */
            g_frame_buf[i * 3 + 1] = 0xFF;  /* G */
            g_frame_buf[i * 3 + 2] = 0xFF;  /* B */
        }
        (void)ll_led_driver_write(g_frame_buf, 100);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
    memset(g_frame_buf, 0, (size_t)g_led_count * 3);
    ESP_LOGI(TAG, "welcome sequence done");
}

void ll_pattern_interp_init(void)
{
    const ll_state_t *s = ll_state_bus_get();
    g_led_count = s->led_count;
    g_pattern_start_ms = now_ms();

    g_frame_buf = calloc((size_t)g_led_count * 3, 1);
    configASSERT(g_frame_buf);

    esp_err_t err = ll_led_driver_init(g_led_count);
    if (err == ESP_OK) {
        g_driver_ready = true;
    } else {
        ESP_LOGE(TAG, "led_driver_init failed: %s — render loop will run "
                      "but frames will be dropped", esp_err_to_name(err));
    }

    play_welcome_sequence();

    g_msg_queue = xQueueCreate(MSG_QUEUE_LEN, sizeof(pi_msg_t));
    configASSERT(g_msg_queue);

    esp_event_loop_handle_t loop = ll_state_bus_get_loop();
    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(
        loop, LL_STATE_EVENT_BASE, LL_EV_STATE_CHANGED,
        on_state_changed, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(
        loop, LL_STATE_EVENT_BASE, LL_EV_RECESSED_HOLD_BEGIN,
        on_hold_event, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(
        loop, LL_STATE_EVENT_BASE, LL_EV_RECESSED_HOLD_END,
        on_hold_event, NULL, NULL));

    BaseType_t ok = xTaskCreatePinnedToCore(render_task, "pattern_interp",
                                            TASK_STACK, NULL, TASK_PRIO,
                                            &g_task, TASK_CORE);
    configASSERT(ok == pdPASS);

    ESP_LOGI(TAG, "init: %u LEDs @ %d Hz, core %d",
             (unsigned)g_led_count, FRAME_HZ, TASK_CORE);
}
