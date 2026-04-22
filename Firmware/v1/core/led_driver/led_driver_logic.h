#pragma once

/*
 * Pure-C LED frame encoding — no ESP-IDF dependencies.
 *
 * Split out from led_driver.c so the byte-level work (GRB reorder +
 * per-channel brightness scaling) is testable on the host. Keep this file
 * free of FreeRTOS, RMT, and esp_* includes.
 */

#include <stdint.h>

/*
 * Encode an RGB frame into the GRB-ordered, brightness-scaled buffer that
 * the WS2812 wire format expects.
 *
 *   rgb            in,  length nleds*3, bytes ordered R,G,B,R,G,B,...
 *   nleds          LED count
 *   brightness_pct 0..100; values above 100 are clamped
 *   out_grb        out, length nleds*3, bytes ordered G,R,B,G,R,B,...
 *
 * Scaling is byte * pct / 100 per channel. 0 yields all zero; 100 passes
 * through unchanged. Caller owns both buffers.
 */
void led_driver_encode_frame(const uint8_t *rgb, uint16_t nleds,
                             uint8_t brightness_pct, uint8_t *out_grb);
