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
        /* led_count is board-dependent (6x6 SKU = 32, 12x12 dev proto =
         * 66). This pure-C file can't see board.h, so the literal here is
         * only a placeholder — state_bus.c stamps the real board value
         * (LL_LED_COUNT_DEFAULT) over it on every seed/reset of g_state.
         * Kept as the 6x6 SKU value so the nvs_logic.c sanitizer fallback
         * and the host tests stay consistent. */
        .led_count = 32,
        .auth_mode = LL_AUTH_OPEN,
        .telemetry_enabled = false,
        .name = "",  /* empty = "unnamed; clients fall back to id" */
    };
}
