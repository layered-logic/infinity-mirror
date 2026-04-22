#pragma once

/*
 * WS2812B LED driver — RMT-backed.
 *
 * Owns the GPIO data line (LL_PIN_LED_DATA) and a malloc'd framebuffer
 * of length led_count * 3. Init once, then push frames via write() or
 * paint a solid color via fill()/clear(). Pattern generation and the
 * per-frame render loop belong in pattern_interp/, not here.
 *
 * Thread-safety: all public calls assume a single caller (the pattern
 * loop task). Not re-entrant.
 */

#include <stdint.h>

#include "esp_err.h"

/*
 * Allocate framebuffer + configure RMT channel for `led_count` LEDs.
 * Returns ESP_OK on success, ESP_ERR_INVALID_STATE if already initialized,
 * or the underlying RMT/allocation error.
 */
esp_err_t ll_led_driver_init(uint16_t led_count);

/* Release RMT resources and free the framebuffer. Safe to call uninit'd. */
void ll_led_driver_deinit(void);

/*
 * Push one frame.
 *   rgb              length led_count*3, bytes ordered R,G,B per LED
 *   brightness_pct   0..100, clamped
 *
 * Blocks until the RMT transmit finishes. Returns ESP_ERR_INVALID_STATE
 * if driver isn't initialized.
 */
esp_err_t ll_led_driver_write(const uint8_t *rgb, uint8_t brightness_pct);

/* Paint every LED with the given color and push. Convenience wrapper. */
esp_err_t ll_led_driver_fill(uint8_t r, uint8_t g, uint8_t b,
                             uint8_t brightness_pct);

/* All LEDs off. Equivalent to fill(0,0,0,0). */
esp_err_t ll_led_driver_clear(void);
