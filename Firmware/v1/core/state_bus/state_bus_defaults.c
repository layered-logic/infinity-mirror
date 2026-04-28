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
        /* TODO: led_count is board-dependent — production 6x6 wants 32,
         * V0 12x12 prototype wants 66. Right architecture is for nvs.c
         * to pull LL_LED_COUNT_DEFAULT from board.h after this call, and
         * for state_bus_defaults.c to omit led_count entirely. Until that
         * refactor, the literal here matches the active development
         * target (V0 12x12 prototype on c3_devkit). */
        .led_count = 66,
        .auth_mode = LL_AUTH_OPEN,
        .telemetry_enabled = false,
    };
}
