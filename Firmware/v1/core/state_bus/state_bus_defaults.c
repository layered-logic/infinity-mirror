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
        .led_count = 32,               /* 6x6 shipping default; NVS overrides */
        .auth_mode = LL_AUTH_OPEN,
        .telemetry_enabled = false,
    };
}
