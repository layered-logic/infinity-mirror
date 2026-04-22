#include "cue_logic.h"

static void enter(cue_t *c, cue_state_e s, uint32_t tick_ms)
{
    c->state = s;
    c->entered_ms = tick_ms;
}

cue_state_e cue_step(cue_t *c, cue_event_e ev, uint32_t tick_ms)
{
    /* Factory reset takes priority over anything else in flight. */
    if (ev == CUE_EVENT_FACTORY_RESET) {
        enter(c, CUE_STATE_FACTORY_RED, tick_ms);
        return c->state;
    }

    switch (c->state) {
    case CUE_STATE_NONE:
        if (ev == CUE_EVENT_HOLD_BEGIN) {
            enter(c, CUE_STATE_HOLD_BLINK, tick_ms);
        }
        break;

    case CUE_STATE_HOLD_BLINK:
        if (ev == CUE_EVENT_HOLD_END) {
            enter(c, CUE_STATE_NONE, tick_ms);
        }
        /* HOLD_BEGIN while already holding: ignore (shouldn't happen but
         * harmless if the button module double-fires). */
        break;

    case CUE_STATE_FACTORY_RED:
        /* Ignore HOLD_END — the user probably released when factory fired
         * at the 10s mark, and we want the red flash to play through. */
        if (ev == CUE_EVENT_TICK &&
            (tick_ms - c->entered_ms) >= CUE_FACTORY_RED_MS) {
            enter(c, CUE_STATE_FACTORY_GREEN, tick_ms);
        }
        break;

    case CUE_STATE_FACTORY_GREEN:
        if (ev == CUE_EVENT_TICK &&
            (tick_ms - c->entered_ms) >= CUE_FACTORY_GREEN_MS) {
            enter(c, CUE_STATE_NONE, tick_ms);
        }
        break;
    }

    return c->state;
}

bool cue_is_active(const cue_t *c)
{
    return c->state != CUE_STATE_NONE;
}

static void fill(uint8_t *out, uint16_t n, uint8_t r, uint8_t g, uint8_t b)
{
    for (uint16_t i = 0; i < n; i++) {
        out[i * 3 + 0] = r;
        out[i * 3 + 1] = g;
        out[i * 3 + 2] = b;
    }
}

void cue_render(const cue_t *c, uint32_t tick_ms,
                uint16_t led_count, uint8_t *out_rgb)
{
    uint32_t elapsed = tick_ms - c->entered_ms;

    switch (c->state) {
    case CUE_STATE_NONE:
        fill(out_rgb, led_count, 0, 0, 0);
        break;

    case CUE_STATE_HOLD_BLINK: {
        bool on_phase = ((elapsed / CUE_HOLD_BLINK_HALF_MS) & 1u) == 0u;
        if (on_phase) {
            fill(out_rgb, led_count, 0x00, 0x00, 0xFF);  /* pure blue */
        } else {
            fill(out_rgb, led_count, 0, 0, 0);
        }
        break;
    }

    case CUE_STATE_FACTORY_RED: {
        bool on_phase = ((elapsed / CUE_FACTORY_RED_HALF_MS) & 1u) == 0u;
        if (on_phase) {
            fill(out_rgb, led_count, 0xFF, 0x00, 0x00);  /* red */
        } else {
            fill(out_rgb, led_count, 0, 0, 0);
        }
        break;
    }

    case CUE_STATE_FACTORY_GREEN:
        fill(out_rgb, led_count, 0x00, 0xFF, 0x00);      /* green */
        break;
    }
}
