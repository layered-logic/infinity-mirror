#include "led_driver_logic.h"

void led_driver_encode_frame(const uint8_t *rgb, uint16_t nleds,
                             uint8_t brightness_pct, uint8_t *out_grb)
{
    if (brightness_pct > 100) {
        brightness_pct = 100;
    }

    for (uint16_t i = 0; i < nleds; i++) {
        uint8_t r = rgb[i * 3 + 0];
        uint8_t g = rgb[i * 3 + 1];
        uint8_t b = rgb[i * 3 + 2];

        /* (byte * pct) fits in 16 bits (255*100 = 25500). */
        r = (uint8_t)(((uint16_t)r * brightness_pct) / 100u);
        g = (uint8_t)(((uint16_t)g * brightness_pct) / 100u);
        b = (uint8_t)(((uint16_t)b * brightness_pct) / 100u);

        out_grb[i * 3 + 0] = g;
        out_grb[i * 3 + 1] = r;
        out_grb[i * 3 + 2] = b;
    }
}
