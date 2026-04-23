/*
 * Layered Logic Infinity Mirror — Standard variant (Pro).
 *
 * Wiring order matters:
 *   1. nvs_init         — load persisted state (or defaults on first boot).
 *   2. state_bus        — seed the bus with that state; everyone subscribes to its loop.
 *   3. nvs_subscribe    — start debounced save-on-change AFTER the bus exists.
 *   4. provisioning_init — bring up netif/wifi, connect if creds saved.
 *   5. provisioning_subscribe — react to LL_EV_PROVISION_START / FACTORY_RESET.
 *   6. pattern_interp   — registers handlers + brings up led_driver.
 *   7. button           — starts posting events that the others now listen for.
 */

#include <stdio.h>

#include "esp_event.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "board.h"
#include "button.h"
#include "nvs.h"
#include "pattern_interp.h"
#include "provisioning.h"
#include "state_bus.h"
#include "state_bus_events.h"

static const char *TAG = "main";

static void on_state_changed(void *arg, esp_event_base_t base,
                             int32_t id, void *data)
{
    (void)arg; (void)base; (void)id;
    const ll_state_changed_payload_t *p = data;
    const ll_state_t *s = ll_state_bus_get();
    ESP_LOGI(TAG,
             "state changed (field=%d): on=%d pattern=%s brightness=%u color=#%06lX",
             (int)p->which, (int)s->on, s->pattern_id,
             (unsigned)s->brightness, (unsigned long)s->base_color_rgb);
}

void app_main(void)
{
    printf("Layered Logic Infinity Mirror — standard variant\n");
    printf("Board: LED pin=%d, primary btn=%d, reset btn=%d\n",
           LL_PIN_LED_DATA, LL_PIN_BUTTON_PRIMARY, LL_PIN_BUTTON_RESET);
    printf("Default LED count: %d\n", LL_LED_COUNT_DEFAULT);

    ll_state_t initial;
    bool first_boot;
    ESP_ERROR_CHECK(ll_nvs_init(&initial, &first_boot));

    ll_state_bus_init(&initial);

    ESP_ERROR_CHECK(ll_nvs_subscribe());

    ESP_ERROR_CHECK(ll_provisioning_init());
    ESP_ERROR_CHECK(ll_provisioning_subscribe());

    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(
        ll_state_bus_get_loop(),
        LL_STATE_EVENT_BASE, LL_EV_STATE_CHANGED,
        on_state_changed, NULL, NULL));

    ll_pattern_interp_init();
    ll_button_init();

    /* Live behavior:
     *   primary single  → power on, then cycle base color (17 steps)
     *   primary double  → cycle pattern (solid/rainbow/scanner/spinner/
     *                     random/breathing/twinkle)
     *   primary triple  → cycle brightness (25/50/75/100)
     *   primary hold    → power off
     *   recessed press  → blue blink (visual feedback while held)
     *   recessed ≥3s release → provision start
     *   recessed ≥10s hold   → factory reset (red flash → green hold)
     */

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
