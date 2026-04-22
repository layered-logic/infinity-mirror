#include <string.h>

#include "button_logic.h"
#include "test_harness.h"

/* ---- Primary gesture machine ---- */

static void primary_single_press_fires_single(void)
{
    primary_state_t s = {0};
    ASSERT_EQ(primary_step(&s, GESTURE_INPUT_PRESS, 100), PRIMARY_GESTURE_NONE);
    ASSERT_EQ(primary_step(&s, GESTURE_INPUT_RELEASE, 150), PRIMARY_GESTURE_NONE);
    /* Before DOUBLE_CLICK_MS window closes: nothing. */
    ASSERT_EQ(primary_step(&s, GESTURE_INPUT_TICK, 200), PRIMARY_GESTURE_NONE);
    /* At exactly window close: fires SINGLE. */
    ASSERT_EQ(primary_step(&s, GESTURE_INPUT_TICK,
                           150 + LL_BUTTON_DOUBLE_CLICK_MS),
              PRIMARY_GESTURE_SINGLE);
    /* Click count reset. */
    ASSERT_EQ(s.click_count, 0);
}

static void primary_double_press_fires_double(void)
{
    primary_state_t s = {0};
    primary_step(&s, GESTURE_INPUT_PRESS, 100);
    primary_step(&s, GESTURE_INPUT_RELEASE, 150);
    primary_step(&s, GESTURE_INPUT_PRESS, 200);
    primary_step(&s, GESTURE_INPUT_RELEASE, 250);
    ASSERT_EQ(primary_step(&s, GESTURE_INPUT_TICK,
                           250 + LL_BUTTON_DOUBLE_CLICK_MS),
              PRIMARY_GESTURE_DOUBLE);
}

static void primary_triple_press_fires_triple(void)
{
    primary_state_t s = {0};
    primary_step(&s, GESTURE_INPUT_PRESS, 0);
    primary_step(&s, GESTURE_INPUT_RELEASE, 50);
    primary_step(&s, GESTURE_INPUT_PRESS, 100);
    primary_step(&s, GESTURE_INPUT_RELEASE, 150);
    primary_step(&s, GESTURE_INPUT_PRESS, 200);
    primary_step(&s, GESTURE_INPUT_RELEASE, 250);
    ASSERT_EQ(primary_step(&s, GESTURE_INPUT_TICK,
                           250 + LL_BUTTON_DOUBLE_CLICK_MS),
              PRIMARY_GESTURE_TRIPLE);
}

static void primary_four_clicks_clamp_to_triple(void)
{
    primary_state_t s = {0};
    for (uint32_t t = 0; t < 4; t++) {
        primary_step(&s, GESTURE_INPUT_PRESS, t * 100);
        primary_step(&s, GESTURE_INPUT_RELEASE, t * 100 + 50);
    }
    ASSERT_EQ(primary_step(&s, GESTURE_INPUT_TICK,
                           400 + LL_BUTTON_DOUBLE_CLICK_MS),
              PRIMARY_GESTURE_TRIPLE);
}

static void primary_hold_fires_while_pressed(void)
{
    primary_state_t s = {0};
    primary_step(&s, GESTURE_INPUT_PRESS, 0);
    /* Before threshold: none. */
    ASSERT_EQ(primary_step(&s, GESTURE_INPUT_TICK,
                           LL_BUTTON_LONG_PRESS_MS - 1),
              PRIMARY_GESTURE_NONE);
    /* At threshold: HOLD fires. */
    ASSERT_EQ(primary_step(&s, GESTURE_INPUT_TICK, LL_BUTTON_LONG_PRESS_MS),
              PRIMARY_GESTURE_HOLD);
    /* Subsequent ticks don't re-fire. */
    ASSERT_EQ(primary_step(&s, GESTURE_INPUT_TICK,
                           LL_BUTTON_LONG_PRESS_MS + 100),
              PRIMARY_GESTURE_NONE);
}

static void primary_hold_then_release_doesnt_emit_click(void)
{
    primary_state_t s = {0};
    primary_step(&s, GESTURE_INPUT_PRESS, 0);
    ASSERT_EQ(primary_step(&s, GESTURE_INPUT_TICK, LL_BUTTON_LONG_PRESS_MS),
              PRIMARY_GESTURE_HOLD);
    primary_step(&s, GESTURE_INPUT_RELEASE, LL_BUTTON_LONG_PRESS_MS + 100);
    ASSERT_EQ(s.click_count, 0);
    /* After tick window: no SINGLE emitted. */
    ASSERT_EQ(primary_step(&s, GESTURE_INPUT_TICK,
                           LL_BUTTON_LONG_PRESS_MS + 100
                           + LL_BUTTON_DOUBLE_CLICK_MS),
              PRIMARY_GESTURE_NONE);
}

static void primary_click_then_hold_drops_pending_click(void)
{
    primary_state_t s = {0};
    primary_step(&s, GESTURE_INPUT_PRESS, 0);
    primary_step(&s, GESTURE_INPUT_RELEASE, 50);
    ASSERT_EQ(s.click_count, 1);
    primary_step(&s, GESTURE_INPUT_PRESS, 100);
    ASSERT_EQ(primary_step(&s, GESTURE_INPUT_TICK,
                           100 + LL_BUTTON_LONG_PRESS_MS),
              PRIMARY_GESTURE_HOLD);
    ASSERT_EQ(s.click_count, 0);
}

static void primary_release_before_long_press_doesnt_fire_hold(void)
{
    primary_state_t s = {0};
    primary_step(&s, GESTURE_INPUT_PRESS, 0);
    primary_step(&s, GESTURE_INPUT_RELEASE, LL_BUTTON_LONG_PRESS_MS - 1);
    ASSERT_EQ(s.click_count, 1);
    ASSERT_EQ(primary_step(&s, GESTURE_INPUT_TICK,
                           LL_BUTTON_LONG_PRESS_MS - 1
                           + LL_BUTTON_DOUBLE_CLICK_MS),
              PRIMARY_GESTURE_SINGLE);
}

static void primary_idle_tick_returns_none(void)
{
    primary_state_t s = {0};
    ASSERT_EQ(primary_step(&s, GESTURE_INPUT_TICK, 0),      PRIMARY_GESTURE_NONE);
    ASSERT_EQ(primary_step(&s, GESTURE_INPUT_TICK, 10000),  PRIMARY_GESTURE_NONE);
}

void suite_gesture_primary(void)
{
    RUN(primary_single_press_fires_single);
    RUN(primary_double_press_fires_double);
    RUN(primary_triple_press_fires_triple);
    RUN(primary_four_clicks_clamp_to_triple);
    RUN(primary_hold_fires_while_pressed);
    RUN(primary_hold_then_release_doesnt_emit_click);
    RUN(primary_click_then_hold_drops_pending_click);
    RUN(primary_release_before_long_press_doesnt_fire_hold);
    RUN(primary_idle_tick_returns_none);
}

/* ---- Recessed gesture machine ---- */

static void recessed_short_press_does_nothing(void)
{
    recessed_state_t s = {0};
    ASSERT_EQ(recessed_step(&s, GESTURE_INPUT_PRESS, 0),
              RECESSED_GESTURE_NONE);
    ASSERT_EQ(recessed_step(&s, GESTURE_INPUT_RELEASE, 500),
              RECESSED_GESTURE_NONE);
}

static void recessed_release_at_provision_threshold_fires_provision(void)
{
    recessed_state_t s = {0};
    recessed_step(&s, GESTURE_INPUT_PRESS, 0);
    ASSERT_EQ(recessed_step(&s, GESTURE_INPUT_RELEASE,
                            LL_RECESSED_PROVISION_MS),
              RECESSED_GESTURE_PROVISION);
}

static void recessed_release_between_provision_and_factory_fires_provision(void)
{
    recessed_state_t s = {0};
    recessed_step(&s, GESTURE_INPUT_PRESS, 0);
    ASSERT_EQ(recessed_step(&s, GESTURE_INPUT_RELEASE, 6000),
              RECESSED_GESTURE_PROVISION);
}

static void recessed_hold_past_factory_threshold_fires_factory_reset(void)
{
    recessed_state_t s = {0};
    recessed_step(&s, GESTURE_INPUT_PRESS, 0);
    /* Tick just below threshold: nothing yet. */
    ASSERT_EQ(recessed_step(&s, GESTURE_INPUT_TICK,
                            LL_RECESSED_FACTORY_MS - 1),
              RECESSED_GESTURE_NONE);
    /* At threshold: fires. */
    ASSERT_EQ(recessed_step(&s, GESTURE_INPUT_TICK,
                            LL_RECESSED_FACTORY_MS),
              RECESSED_GESTURE_FACTORY_RESET);
}

static void recessed_factory_doesnt_refire(void)
{
    recessed_state_t s = {0};
    recessed_step(&s, GESTURE_INPUT_PRESS, 0);
    ASSERT_EQ(recessed_step(&s, GESTURE_INPUT_TICK, LL_RECESSED_FACTORY_MS),
              RECESSED_GESTURE_FACTORY_RESET);
    ASSERT_EQ(recessed_step(&s, GESTURE_INPUT_TICK, 15000),
              RECESSED_GESTURE_NONE);
}

static void recessed_release_after_factory_doesnt_fire_provision(void)
{
    recessed_state_t s = {0};
    recessed_step(&s, GESTURE_INPUT_PRESS, 0);
    recessed_step(&s, GESTURE_INPUT_TICK, LL_RECESSED_FACTORY_MS);
    ASSERT_EQ(recessed_step(&s, GESTURE_INPUT_RELEASE, 12000),
              RECESSED_GESTURE_NONE);
}

void suite_gesture_recessed(void)
{
    RUN(recessed_short_press_does_nothing);
    RUN(recessed_release_at_provision_threshold_fires_provision);
    RUN(recessed_release_between_provision_and_factory_fires_provision);
    RUN(recessed_hold_past_factory_threshold_fires_factory_reset);
    RUN(recessed_factory_doesnt_refire);
    RUN(recessed_release_after_factory_doesnt_fire_provision);
}
