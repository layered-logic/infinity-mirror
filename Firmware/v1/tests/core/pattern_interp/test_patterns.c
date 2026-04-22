#include <string.h>

#include "patterns.h"
#include "test_harness.h"

/* ---- Registry lookup ---- */

static void lookup_known_id_returns_matching_def(void)
{
    ASSERT_STR_EQ(patterns_lookup("solid")->id,     "solid");
    ASSERT_STR_EQ(patterns_lookup("rainbow")->id,   "rainbow");
    ASSERT_STR_EQ(patterns_lookup("scanner")->id,   "scanner");
    ASSERT_STR_EQ(patterns_lookup("spinner")->id,   "spinner");
    ASSERT_STR_EQ(patterns_lookup("random")->id,    "random");
    ASSERT_STR_EQ(patterns_lookup("breathing")->id, "breathing");
    ASSERT_STR_EQ(patterns_lookup("twinkle")->id,   "twinkle");
}

static void lookup_unknown_id_returns_solid(void)
{
    ASSERT_STR_EQ(patterns_lookup("not_a_pattern")->id, "solid");
}

static void lookup_null_returns_solid(void)
{
    ASSERT_STR_EQ(patterns_lookup(NULL)->id, "solid");
}

/* ---- Solid ---- */

static void solid_paints_every_led_with_base_color(void)
{
    pattern_ctx_t ctx = {
        .led_count = 4,
        .base_color_rgb = 0x123456u,
        .tick_ms = 1234,
        .pattern_start_ms = 0,
    };
    uint8_t out[12] = {0};
    patterns_render("solid", &ctx, out);
    for (int i = 0; i < 4; i++) {
        ASSERT_EQ(out[i * 3 + 0], 0x12);  /* R */
        ASSERT_EQ(out[i * 3 + 1], 0x34);  /* G */
        ASSERT_EQ(out[i * 3 + 2], 0x56);  /* B */
    }
}

static void solid_ignores_time(void)
{
    pattern_ctx_t a = { .led_count = 2, .base_color_rgb = 0xABCDEFu,
                        .tick_ms = 0, .pattern_start_ms = 0 };
    pattern_ctx_t b = a;
    b.tick_ms = 999999;
    uint8_t out_a[6] = {0}, out_b[6] = {0};
    patterns_render("solid", &a, out_a);
    patterns_render("solid", &b, out_b);
    ASSERT_EQ(memcmp(out_a, out_b, 6), 0);
}

/* ---- Rainbow ---- */

static void rainbow_at_t0_starts_at_red(void)
{
    pattern_ctx_t ctx = {
        .led_count = 8,
        .base_color_rgb = 0,   /* ignored by rainbow */
        .tick_ms = 0,
        .pattern_start_ms = 0,
    };
    uint8_t out[24] = {0};
    patterns_render("rainbow", &ctx, out);
    /* LED 0: hue=0 → pure red. */
    ASSERT_EQ(out[0], 255);
    ASSERT_EQ(out[1], 0);
    ASSERT_EQ(out[2], 0);
}

static void rainbow_advances_over_time(void)
{
    pattern_ctx_t early = { .led_count = 8, .tick_ms = 0,
                            .pattern_start_ms = 0 };
    pattern_ctx_t later = early;
    later.tick_ms = 4000;   /* halfway around the 12s rotation */
    uint8_t a[24] = {0}, b[24] = {0};
    patterns_render("rainbow", &early, a);
    patterns_render("rainbow", &later, b);
    ASSERT(memcmp(a, b, 24) != 0);
}

static void rainbow_each_led_has_distinct_hue(void)
{
    pattern_ctx_t ctx = { .led_count = 8, .tick_ms = 0,
                          .pattern_start_ms = 0 };
    uint8_t out[24] = {0};
    patterns_render("rainbow", &ctx, out);
    /* LED 0 and LED 4 are 180° apart on the wheel — must differ. */
    ASSERT(memcmp(&out[0], &out[12], 3) != 0);
}

/* ---- Spinner ---- */

static void spinner_at_t0_lights_only_led_0(void)
{
    pattern_ctx_t ctx = {
        .led_count = 4, .base_color_rgb = 0x00FF00u,
        .tick_ms = 0, .pattern_start_ms = 0,
    };
    uint8_t out[12] = {0};
    patterns_render("spinner", &ctx, out);
    ASSERT_EQ(out[0], 0); ASSERT_EQ(out[1], 0xFF); ASSERT_EQ(out[2], 0);
    for (int i = 3; i < 12; i++) {
        ASSERT_EQ(out[i], 0);
    }
}

static void spinner_advances_one_led_per_60ms(void)
{
    pattern_ctx_t ctx = {
        .led_count = 4, .base_color_rgb = 0x00FF00u,
        .tick_ms = 60, .pattern_start_ms = 0,
    };
    uint8_t out[12] = {0};
    patterns_render("spinner", &ctx, out);
    /* Position 1 lit, others off. */
    ASSERT_EQ(out[0], 0); ASSERT_EQ(out[1], 0); ASSERT_EQ(out[2], 0);
    ASSERT_EQ(out[3], 0); ASSERT_EQ(out[4], 0xFF); ASSERT_EQ(out[5], 0);
    ASSERT_EQ(out[6], 0); ASSERT_EQ(out[7], 0); ASSERT_EQ(out[8], 0);
    ASSERT_EQ(out[9], 0); ASSERT_EQ(out[10], 0); ASSERT_EQ(out[11], 0);
}

static void spinner_wraps_around_ring(void)
{
    pattern_ctx_t ctx = {
        .led_count = 4, .base_color_rgb = 0x00FF00u,
        .tick_ms = 60 * 4, .pattern_start_ms = 0,
    };
    uint8_t out[12] = {0};
    patterns_render("spinner", &ctx, out);
    /* After 4 steps on a 4-LED ring, back at position 0. */
    ASSERT_EQ(out[1], 0xFF);
}

/* ---- Scanner ---- */

static void scanner_at_t0_head_at_led_0(void)
{
    pattern_ctx_t ctx = {
        .led_count = 8, .base_color_rgb = 0xFF0000u,
        .tick_ms = 0, .pattern_start_ms = 0,
    };
    uint8_t out[24] = {0};
    patterns_render("scanner", &ctx, out);
    ASSERT_EQ(out[0], 0xFF);   /* head */
    ASSERT_EQ(out[3], 96);     /* tail at d=1 */
}

static void scanner_at_half_period_head_at_far_end(void)
{
    pattern_ctx_t ctx = {
        .led_count = 8, .base_color_rgb = 0xFF0000u,
        .tick_ms = 1000, .pattern_start_ms = 0,
    };
    uint8_t out[24] = {0};
    patterns_render("scanner", &ctx, out);
    /* Last LED (index 7) should be the head at half period. */
    ASSERT_EQ(out[7 * 3 + 0], 0xFF);
}

/* ---- Breathing ---- */

static void breathing_at_t0_is_floor_brightness(void)
{
    pattern_ctx_t ctx = {
        .led_count = 4, .base_color_rgb = 0xFFFFFFu,
        .tick_ms = 0, .pattern_start_ms = 0,
    };
    uint8_t out[12] = {0};
    patterns_render("breathing", &ctx, out);
    /* Triangle gives 0 at t=0; floor lifts to 20. 255 * 20 / 255 = 20. */
    ASSERT_EQ(out[0], 20);
    ASSERT_EQ(out[1], 20);
    ASSERT_EQ(out[2], 20);
}

static void breathing_at_half_period_is_full(void)
{
    pattern_ctx_t ctx = {
        .led_count = 1, .base_color_rgb = 0xFFFFFFu,
        .tick_ms = 1500, .pattern_start_ms = 0,
    };
    uint8_t out[3] = {0};
    patterns_render("breathing", &ctx, out);
    ASSERT_EQ(out[0], 255);
}

static void breathing_uses_base_color(void)
{
    pattern_ctx_t ctx = {
        .led_count = 1, .base_color_rgb = 0xFF8000u,  /* orange */
        .tick_ms = 1500, .pattern_start_ms = 0,
    };
    uint8_t out[3] = {0};
    patterns_render("breathing", &ctx, out);
    ASSERT_EQ(out[0], 0xFF);
    ASSERT_EQ(out[1], 0x80);
    ASSERT_EQ(out[2], 0);
}

/* ---- Random / twinkle ---- */

static void random_produces_mix_of_lit_and_unlit(void)
{
    pattern_ctx_t ctx = {
        .led_count = 64, .base_color_rgb = 0xFFFFFFu,
        .tick_ms = 100, .pattern_start_ms = 0,
    };
    uint8_t out[64 * 3] = {0};
    patterns_render("random", &ctx, out);
    int lit = 0, unlit = 0;
    for (int i = 0; i < 64; i++) {
        if (out[i * 3] != 0) lit++; else unlit++;
    }
    ASSERT(lit > 0);
    ASSERT(unlit > 0);
}

static void random_changes_between_epochs(void)
{
    pattern_ctx_t a = { .led_count = 32, .base_color_rgb = 0xFFFFFFu,
                        .tick_ms = 0, .pattern_start_ms = 0 };
    pattern_ctx_t b = a;
    b.tick_ms = 500;   /* 5 epochs later */
    uint8_t out_a[32 * 3] = {0}, out_b[32 * 3] = {0};
    patterns_render("random", &a, out_a);
    patterns_render("random", &b, out_b);
    ASSERT(memcmp(out_a, out_b, sizeof(out_a)) != 0);
}

static void twinkle_produces_non_uniform_brightness(void)
{
    pattern_ctx_t ctx = {
        .led_count = 16, .base_color_rgb = 0xFFFFFFu,
        .tick_ms = 0, .pattern_start_ms = 0,
    };
    uint8_t out[16 * 3] = {0};
    patterns_render("twinkle", &ctx, out);
    /* Each LED has its own phase — at any given t, brightnesses differ. */
    int distinct_first_byte = 0;
    uint8_t prev = out[0];
    for (int i = 1; i < 16; i++) {
        if (out[i * 3] != prev) {
            distinct_first_byte++;
            prev = out[i * 3];
        }
    }
    ASSERT(distinct_first_byte > 0);
}

/* ---- Safety ---- */

static void zero_led_count_doesnt_crash(void)
{
    pattern_ctx_t ctx = {
        .led_count = 0, .base_color_rgb = 0xFF0000u,
        .tick_ms = 100, .pattern_start_ms = 0,
    };
    uint8_t dummy = 0xAA;
    patterns_render("rainbow",   &ctx, &dummy);  /* wrote nothing */
    patterns_render("scanner",   &ctx, &dummy);
    patterns_render("spinner",   &ctx, &dummy);
    patterns_render("solid",     &ctx, &dummy);
    patterns_render("breathing", &ctx, &dummy);
    patterns_render("random",    &ctx, &dummy);
    patterns_render("twinkle",   &ctx, &dummy);
    ASSERT_EQ(dummy, 0xAA);
}

void suite_patterns(void)
{
    RUN(lookup_known_id_returns_matching_def);
    RUN(lookup_unknown_id_returns_solid);
    RUN(lookup_null_returns_solid);
    RUN(solid_paints_every_led_with_base_color);
    RUN(solid_ignores_time);
    RUN(rainbow_at_t0_starts_at_red);
    RUN(rainbow_advances_over_time);
    RUN(rainbow_each_led_has_distinct_hue);
    RUN(spinner_at_t0_lights_only_led_0);
    RUN(spinner_advances_one_led_per_60ms);
    RUN(spinner_wraps_around_ring);
    RUN(scanner_at_t0_head_at_led_0);
    RUN(scanner_at_half_period_head_at_far_end);
    RUN(breathing_at_t0_is_floor_brightness);
    RUN(breathing_at_half_period_is_full);
    RUN(breathing_uses_base_color);
    RUN(random_produces_mix_of_lit_and_unlit);
    RUN(random_changes_between_epochs);
    RUN(twinkle_produces_non_uniform_brightness);
    RUN(zero_led_count_doesnt_crash);
}
