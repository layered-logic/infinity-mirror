#include "cue_logic.h"
#include "test_harness.h"

/* ---- Transitions ---- */

static void initial_state_is_none(void)
{
    cue_t c = {0};
    ASSERT_EQ(c.state, CUE_STATE_NONE);
    ASSERT(!cue_is_active(&c));
}

static void hold_begin_enters_blink(void)
{
    cue_t c = {0};
    cue_step(&c, CUE_EVENT_HOLD_BEGIN, 100);
    ASSERT_EQ(c.state, CUE_STATE_HOLD_BLINK);
    ASSERT_EQ(c.entered_ms, 100);
    ASSERT(cue_is_active(&c));
}

static void hold_end_returns_to_none(void)
{
    cue_t c = {0};
    cue_step(&c, CUE_EVENT_HOLD_BEGIN, 0);
    cue_step(&c, CUE_EVENT_HOLD_END, 1500);
    ASSERT_EQ(c.state, CUE_STATE_NONE);
    ASSERT(!cue_is_active(&c));
}

static void factory_reset_overrides_hold(void)
{
    cue_t c = {0};
    cue_step(&c, CUE_EVENT_HOLD_BEGIN, 0);
    cue_step(&c, CUE_EVENT_FACTORY_RESET, 10000);
    ASSERT_EQ(c.state, CUE_STATE_FACTORY_RED);
    ASSERT_EQ(c.entered_ms, 10000);
}

static void factory_reset_from_none_works(void)
{
    cue_t c = {0};
    cue_step(&c, CUE_EVENT_FACTORY_RESET, 5000);
    ASSERT_EQ(c.state, CUE_STATE_FACTORY_RED);
}

static void hold_end_during_factory_red_is_ignored(void)
{
    cue_t c = {0};
    cue_step(&c, CUE_EVENT_FACTORY_RESET, 0);
    cue_step(&c, CUE_EVENT_HOLD_END, 200);
    ASSERT_EQ(c.state, CUE_STATE_FACTORY_RED);
}

static void factory_red_advances_to_green_after_1s(void)
{
    cue_t c = {0};
    cue_step(&c, CUE_EVENT_FACTORY_RESET, 0);
    cue_step(&c, CUE_EVENT_TICK, 999);
    ASSERT_EQ(c.state, CUE_STATE_FACTORY_RED);
    cue_step(&c, CUE_EVENT_TICK, 1000);
    ASSERT_EQ(c.state, CUE_STATE_FACTORY_GREEN);
    ASSERT_EQ(c.entered_ms, 1000);
}

static void factory_green_advances_to_none_after_1s(void)
{
    cue_t c = {0};
    cue_step(&c, CUE_EVENT_FACTORY_RESET, 0);
    cue_step(&c, CUE_EVENT_TICK, 1000);   /* into GREEN */
    cue_step(&c, CUE_EVENT_TICK, 1999);
    ASSERT_EQ(c.state, CUE_STATE_FACTORY_GREEN);
    cue_step(&c, CUE_EVENT_TICK, 2000);
    ASSERT_EQ(c.state, CUE_STATE_NONE);
    ASSERT(!cue_is_active(&c));
}

static void hold_begin_while_holding_is_no_op(void)
{
    cue_t c = {0};
    cue_step(&c, CUE_EVENT_HOLD_BEGIN, 100);
    cue_step(&c, CUE_EVENT_HOLD_BEGIN, 200);
    ASSERT_EQ(c.state, CUE_STATE_HOLD_BLINK);
    ASSERT_EQ(c.entered_ms, 100);   /* original entry preserved */
}

/* ---- Rendering ---- */

static void render_hold_blink_alternates_blue_and_off(void)
{
    cue_t c = {0};
    cue_step(&c, CUE_EVENT_HOLD_BEGIN, 0);
    uint8_t out[6] = {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA};

    /* t=0: blue on. */
    cue_render(&c, 0, 2, out);
    ASSERT_EQ(out[0], 0x00); ASSERT_EQ(out[1], 0x00); ASSERT_EQ(out[2], 0xFF);
    ASSERT_EQ(out[3], 0x00); ASSERT_EQ(out[4], 0x00); ASSERT_EQ(out[5], 0xFF);

    /* t=500: blue off. */
    cue_render(&c, 500, 2, out);
    ASSERT_EQ(out[0], 0); ASSERT_EQ(out[1], 0); ASSERT_EQ(out[2], 0);

    /* t=1000: blue back on. */
    cue_render(&c, 1000, 2, out);
    ASSERT_EQ(out[2], 0xFF);
}

static void render_factory_red_alternates_red_and_off(void)
{
    cue_t c = {0};
    cue_step(&c, CUE_EVENT_FACTORY_RESET, 0);
    uint8_t out[3] = {0};

    cue_render(&c, 0, 1, out);
    ASSERT_EQ(out[0], 0xFF); ASSERT_EQ(out[1], 0); ASSERT_EQ(out[2], 0);

    cue_render(&c, 100, 1, out);
    ASSERT_EQ(out[0], 0); ASSERT_EQ(out[1], 0); ASSERT_EQ(out[2], 0);

    cue_render(&c, 200, 1, out);
    ASSERT_EQ(out[0], 0xFF);
}

static void render_factory_green_is_solid_green(void)
{
    cue_t c = {0};
    cue_step(&c, CUE_EVENT_FACTORY_RESET, 0);
    cue_step(&c, CUE_EVENT_TICK, 1000);   /* into GREEN at t=1000 */
    uint8_t out[6] = {0};
    cue_render(&c, 1500, 2, out);
    ASSERT_EQ(out[0], 0); ASSERT_EQ(out[1], 0xFF); ASSERT_EQ(out[2], 0);
    ASSERT_EQ(out[3], 0); ASSERT_EQ(out[4], 0xFF); ASSERT_EQ(out[5], 0);
}

void suite_cue(void)
{
    RUN(initial_state_is_none);
    RUN(hold_begin_enters_blink);
    RUN(hold_end_returns_to_none);
    RUN(factory_reset_overrides_hold);
    RUN(factory_reset_from_none_works);
    RUN(hold_end_during_factory_red_is_ignored);
    RUN(factory_red_advances_to_green_after_1s);
    RUN(factory_green_advances_to_none_after_1s);
    RUN(hold_begin_while_holding_is_no_op);
    RUN(render_hold_blink_alternates_blue_and_off);
    RUN(render_factory_red_alternates_red_and_off);
    RUN(render_factory_green_is_solid_green);
}
