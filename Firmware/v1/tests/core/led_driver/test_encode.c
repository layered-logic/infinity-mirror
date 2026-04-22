#include <string.h>

#include "led_driver_logic.h"
#include "test_harness.h"

/* ---- led_driver_encode_frame: GRB reorder + brightness scaling ---- */

static void encode_reorders_rgb_to_grb(void)
{
    const uint8_t rgb[3] = { 0x11, 0x22, 0x33 };  /* R=0x11 G=0x22 B=0x33 */
    uint8_t out[3] = {0};
    led_driver_encode_frame(rgb, 1, 100, out);
    ASSERT_EQ(out[0], 0x22);  /* G */
    ASSERT_EQ(out[1], 0x11);  /* R */
    ASSERT_EQ(out[2], 0x33);  /* B */
}

static void encode_full_brightness_passes_through(void)
{
    const uint8_t rgb[6] = { 0xFF, 0x00, 0x80, 0x10, 0x20, 0x40 };
    uint8_t out[6] = {0};
    led_driver_encode_frame(rgb, 2, 100, out);
    /* LED 0: R=FF G=00 B=80 -> GRB = 00,FF,80 */
    ASSERT_EQ(out[0], 0x00);
    ASSERT_EQ(out[1], 0xFF);
    ASSERT_EQ(out[2], 0x80);
    /* LED 1: R=10 G=20 B=40 -> GRB = 20,10,40 */
    ASSERT_EQ(out[3], 0x20);
    ASSERT_EQ(out[4], 0x10);
    ASSERT_EQ(out[5], 0x40);
}

static void encode_zero_brightness_is_all_black(void)
{
    const uint8_t rgb[3] = { 0xFF, 0xFF, 0xFF };
    uint8_t out[3] = { 0xAA, 0xAA, 0xAA };
    led_driver_encode_frame(rgb, 1, 0, out);
    ASSERT_EQ(out[0], 0x00);
    ASSERT_EQ(out[1], 0x00);
    ASSERT_EQ(out[2], 0x00);
}

static void encode_fifty_percent_halves_each_channel(void)
{
    const uint8_t rgb[3] = { 200, 100, 50 };
    uint8_t out[3] = {0};
    led_driver_encode_frame(rgb, 1, 50, out);
    /* 200*50/100 = 100, 100*50/100 = 50, 50*50/100 = 25 */
    ASSERT_EQ(out[0], 50);   /* G */
    ASSERT_EQ(out[1], 100);  /* R */
    ASSERT_EQ(out[2], 25);   /* B */
}

static void encode_brightness_above_100_is_clamped(void)
{
    const uint8_t rgb[3] = { 0x10, 0x20, 0x30 };
    uint8_t clamped[3] = {0};
    uint8_t passthrough[3] = {0};
    led_driver_encode_frame(rgb, 1, 250, clamped);
    led_driver_encode_frame(rgb, 1, 100, passthrough);
    ASSERT_EQ(clamped[0], passthrough[0]);
    ASSERT_EQ(clamped[1], passthrough[1]);
    ASSERT_EQ(clamped[2], passthrough[2]);
}

static void encode_handles_multi_led_frame(void)
{
    const uint16_t N = 32;
    uint8_t rgb[32 * 3];
    uint8_t out[32 * 3];
    for (uint16_t i = 0; i < N; i++) {
        rgb[i * 3 + 0] = (uint8_t)(i * 3);      /* R */
        rgb[i * 3 + 1] = (uint8_t)(i * 3 + 1);  /* G */
        rgb[i * 3 + 2] = (uint8_t)(i * 3 + 2);  /* B */
    }
    led_driver_encode_frame(rgb, N, 100, out);
    for (uint16_t i = 0; i < N; i++) {
        ASSERT_EQ(out[i * 3 + 0], (uint8_t)(i * 3 + 1));  /* G */
        ASSERT_EQ(out[i * 3 + 1], (uint8_t)(i * 3));      /* R */
        ASSERT_EQ(out[i * 3 + 2], (uint8_t)(i * 3 + 2));  /* B */
    }
}

void suite_led_driver_encode(void)
{
    RUN(encode_reorders_rgb_to_grb);
    RUN(encode_full_brightness_passes_through);
    RUN(encode_zero_brightness_is_all_black);
    RUN(encode_fifty_percent_halves_each_channel);
    RUN(encode_brightness_above_100_is_clamped);
    RUN(encode_handles_multi_led_frame);
}
