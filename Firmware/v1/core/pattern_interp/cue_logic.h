#pragma once

/*
 * Cue overlay state machine.
 *
 * Cues are short visual signals that temporarily take over the LED frame
 * when the user does something irreversible-adjacent: holding the recessed
 * button (blue blink), triggering factory reset (red flash → green hold).
 *
 * Priority: factory > hold > none. A factory-reset event during a hold
 * kicks the machine straight into the red-flash state; the subsequent
 * HOLD_END is ignored until the factory sequence finishes.
 *
 * Split from pattern_interp.c as pure C so the transition logic is
 * host-testable without pulling in FreeRTOS / esp_event.
 */

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    CUE_STATE_NONE = 0,            /* no overlay — pattern passes through */
    CUE_STATE_HOLD_BLINK,          /* recessed held: blue 0.5s on / 0.5s off */
    CUE_STATE_FACTORY_RED,         /* resetting: red 0.1s on / 0.1s off */
    CUE_STATE_FACTORY_GREEN,       /* reset complete: solid green hold */
} cue_state_e;

typedef enum {
    CUE_EVENT_TICK = 0,            /* time-only step; call every frame */
    CUE_EVENT_HOLD_BEGIN,          /* recessed button pressed */
    CUE_EVENT_HOLD_END,            /* recessed button released */
    CUE_EVENT_FACTORY_RESET,       /* factory-reset gesture fired */
} cue_event_e;

typedef struct {
    cue_state_e state;
    uint32_t entered_ms;           /* tick_ms when we entered `state` */
} cue_t;

/* Blink / flash timings. Public so tests can reference them. */
#define CUE_HOLD_BLINK_HALF_MS    500u    /* blue on/off half-period */
#define CUE_FACTORY_RED_HALF_MS   100u    /* red on/off half-period */
#define CUE_FACTORY_RED_MS        1000u   /* total red-flash duration */
#define CUE_FACTORY_GREEN_MS      1000u   /* green hold after reset */

/* Advance the state machine and return the new state. Use CUE_EVENT_TICK
 * as the once-per-frame step; other events fire in response to button
 * input. Passing the absolute monotonic time lets the renderer know
 * how far into a timed state (RED/GREEN) we are without holding a
 * separate clock. */
cue_state_e cue_step(cue_t *c, cue_event_e ev, uint32_t tick_ms);

/* True when a cue is overlaying the pattern. Callers should render the
 * pattern normally and replace the frame with the cue output when this
 * returns true. */
bool cue_is_active(const cue_t *c);

/* Write led_count*3 bytes of R,G,B into out_rgb representing the current
 * cue overlay. Results are unspecified if cue_is_active() is false; caller
 * is expected to gate. */
void cue_render(const cue_t *c, uint32_t tick_ms,
                uint16_t led_count, uint8_t *out_rgb);
