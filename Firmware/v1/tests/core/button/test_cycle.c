#include "button_logic.h"
#include "test_harness.h"

/* ---- Color cycle ---- */

static void color_exact_matches_resolve_to_own_index(void)
{
    ASSERT_EQ(color_cycle_index_of(0xFF0000u), 0);   /* red */
    ASSERT_EQ(color_cycle_index_of(0x00FFFFu), 8);   /* cyan */
    ASSERT_EQ(color_cycle_index_of(0xFFFFFFu), 16);  /* white */
}

static void color_snaps_to_nearest_when_custom(void)
{
    /* App pushed a near-red with noise — should snap to index 0 (red). */
    ASSERT_EQ(color_cycle_index_of(0xF80403u), 0);
    /* Near-white noisy — should snap to white. */
    ASSERT_EQ(color_cycle_index_of(0xF0F0F0u), 16);
}

static void color_advance_wraps_from_white_to_red(void)
{
    ASSERT_EQ_HEX(color_cycle_advance(0xFFFFFFu), 0xFF0000u);
}

static void color_advance_red_goes_to_red_orange(void)
{
    ASSERT_EQ_HEX(color_cycle_advance(0xFF0000u), 0xFF6000u);
}

static void color_advance_from_custom_color_snaps_then_advances(void)
{
    /* Indigo Signal (#3214FF) is closest to blue-violet index 11 (#2000FF).
     * Advancing should snap there then move to index 12 (#8000FF, violet). */
    ASSERT_EQ(color_cycle_index_of(0x3214FFu), 11);
    ASSERT_EQ_HEX(color_cycle_advance(0x3214FFu), 0x8000FFu);
}

void suite_cycle_color(void)
{
    RUN(color_exact_matches_resolve_to_own_index);
    RUN(color_snaps_to_nearest_when_custom);
    RUN(color_advance_wraps_from_white_to_red);
    RUN(color_advance_red_goes_to_red_orange);
    RUN(color_advance_from_custom_color_snaps_then_advances);
}

/* ---- Pattern cycle ---- */

static void pattern_known_ids_resolve_to_own_index(void)
{
    ASSERT_EQ(pattern_cycle_index_of("solid"),     0);
    ASSERT_EQ(pattern_cycle_index_of("rainbow"),   1);
    ASSERT_EQ(pattern_cycle_index_of("twinkle"),   6);
}

static void pattern_unknown_id_snaps_to_solid(void)
{
    ASSERT_EQ(pattern_cycle_index_of("user_uploaded_3"), 0);
}

static void pattern_null_snaps_to_solid(void)
{
    ASSERT_EQ(pattern_cycle_index_of(NULL), 0);
}

static void pattern_advance_wraps(void)
{
    ASSERT_STR_EQ(pattern_cycle_advance("twinkle"), "solid");
}

static void pattern_advance_from_custom_snaps_then_advances(void)
{
    /* Unknown pattern → snap to 0 (solid) → advance to 1 (rainbow). */
    ASSERT_STR_EQ(pattern_cycle_advance("custom_user_thing"), "rainbow");
}

void suite_cycle_pattern(void)
{
    RUN(pattern_known_ids_resolve_to_own_index);
    RUN(pattern_unknown_id_snaps_to_solid);
    RUN(pattern_null_snaps_to_solid);
    RUN(pattern_advance_wraps);
    RUN(pattern_advance_from_custom_snaps_then_advances);
}

/* ---- Brightness cycle ---- */

static void brightness_exact_matches_resolve_to_own_index(void)
{
    ASSERT_EQ(brightness_cycle_index_of(25),  0);
    ASSERT_EQ(brightness_cycle_index_of(50),  1);
    ASSERT_EQ(brightness_cycle_index_of(75),  2);
    ASSERT_EQ(brightness_cycle_index_of(100), 3);
}

static void brightness_snaps_to_nearest(void)
{
    ASSERT_EQ(brightness_cycle_index_of(60),  1);   /* 50 is closer than 75 */
    ASSERT_EQ(brightness_cycle_index_of(0),   0);   /* floor clamp */
    ASSERT_EQ(brightness_cycle_index_of(200), 3);   /* ceiling clamp */
    ASSERT_EQ(brightness_cycle_index_of(63),  2);   /* 63→75 (dist 12) vs 63→50 (dist 13) */
}

static void brightness_advance_cycles(void)
{
    ASSERT_EQ(brightness_cycle_advance(25),  50);
    ASSERT_EQ(brightness_cycle_advance(50),  75);
    ASSERT_EQ(brightness_cycle_advance(75),  100);
    ASSERT_EQ(brightness_cycle_advance(100), 25);
}

static void brightness_advance_from_custom_snaps_then_advances(void)
{
    /* 60 snaps to 50 (index 1), advance → 75. */
    ASSERT_EQ(brightness_cycle_advance(60), 75);
}

void suite_cycle_brightness(void)
{
    RUN(brightness_exact_matches_resolve_to_own_index);
    RUN(brightness_snaps_to_nearest);
    RUN(brightness_advance_cycles);
    RUN(brightness_advance_from_custom_snaps_then_advances);
}
