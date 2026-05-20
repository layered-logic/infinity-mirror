/*
 * Compiled defaults for ll_state_t — pure C, no ESP-IDF.
 *
 * Split from state_bus.c so nvs/ and host-side tests can call into it
 * without pulling in freertos/esp_event. Single source of truth for the
 * "what does the device look like on a factory-fresh unit" answer.
 */

#include "state_bus.h"

void ll_state_defaults(ll_state_t *out)
{
    *out = (ll_state_t){
        .on = false,
        .pattern_id = "solid",
        .base_color_rgb = 0x3214FFu,   /* Indigo Signal */
        .brightness = 75,
        /* TODO: led_count is board-dependent — the 6x6 product SKU wants
         * 32, the 12x12 dev prototype wants 66. Right architecture is for
         * nvs.c to pull LL_LED_COUNT_DEFAULT from board.h after this call,
         * and for state_bus_defaults.c to omit led_count entirely. Until
         * that refactor, the literal here is the shipping 6x6 SKU default,
         * matching the NVS sanitizer fallback in nvs_logic.c. */
        .led_count = 32,
        .auth_mode = LL_AUTH_OPEN,
        .telemetry_enabled = false,
        .name = "",  /* empty = "unnamed; clients fall back to id" */
    };
}
