#pragma once

/*
 * Pure-C button logic: gesture state machines + cycle helpers.
 *
 * Split from button.c so it builds on the host (no ESP-IDF, no FreeRTOS)
 * for unit tests. button.c owns the GPIO ISR / task glue and feeds inputs
 * into these functions with real timestamps.
 *
 * Timings are inherited from the STM8 reference implementation
 * (Firmware/STM8/stm8_150mm.ino) — proven on shipping units. Don't
 * change without user testing.
 */

#include <stdbool.h>
#include <stdint.h>

/* ---- Timings (canonical, do not change) ---- */
#define LL_BUTTON_DEBOUNCE_MS        20
#define LL_BUTTON_DOUBLE_CLICK_MS    200
#define LL_BUTTON_LONG_PRESS_MS      600
#define LL_RECESSED_PROVISION_MS     3000
#define LL_RECESSED_FACTORY_MS       10000

/* ---- Cycle table sizes ---- */
#define LL_COLOR_CYCLE_LEN     17
#define LL_PATTERN_CYCLE_LEN   7
#define LL_BRIGHTNESS_CYCLE_LEN 4

/* ---- Inputs ---- */

typedef enum {
    GESTURE_INPUT_TICK,      /* periodic check; deadline-driven */
    GESTURE_INPUT_PRESS,     /* debounced press edge */
    GESTURE_INPUT_RELEASE,   /* debounced release edge */
} gesture_input_t;

/* ---- Primary button ---- */

typedef enum {
    PRIMARY_GESTURE_NONE = 0,
    PRIMARY_GESTURE_SINGLE,
    PRIMARY_GESTURE_DOUBLE,
    PRIMARY_GESTURE_TRIPLE,
    PRIMARY_GESTURE_HOLD,
} primary_gesture_t;

typedef struct {
    bool pressed;
    bool hold_fired;
    uint8_t click_count;
    uint32_t press_start_ms;
    uint32_t last_release_ms;
} primary_state_t;

primary_gesture_t primary_step(primary_state_t *s,
                               gesture_input_t in,
                               uint32_t now_ms);

/* ---- Recessed button ---- */

typedef enum {
    RECESSED_GESTURE_NONE = 0,
    RECESSED_GESTURE_PROVISION,
    RECESSED_GESTURE_FACTORY_RESET,
} recessed_gesture_t;

typedef struct {
    bool pressed;
    bool factory_fired;
    uint32_t press_start_ms;
} recessed_state_t;

recessed_gesture_t recessed_step(recessed_state_t *s,
                                 gesture_input_t in,
                                 uint32_t now_ms);

/* ---- Cycle tables (extern; defined in button_logic.c) ---- */

extern const uint32_t LL_COLOR_CYCLE[LL_COLOR_CYCLE_LEN];
extern const char *const LL_PATTERN_CYCLE[LL_PATTERN_CYCLE_LEN];
extern const uint8_t LL_BRIGHTNESS_CYCLE[LL_BRIGHTNESS_CYCLE_LEN];

/* ---- Cycle helpers ----
 *
 * The "_index_of" variants snap to the nearest cycle slot: if the current
 * state value isn't in the cycle (e.g., the app set a custom color via
 * WebSocket), the next press resumes from the closest known position.
 * This loses the user's custom value on next press — accepted UX trade
 * per design discussion (Apr 22). */

uint8_t color_cycle_index_of(uint32_t rgb);
uint32_t color_cycle_advance(uint32_t current_rgb);

uint8_t pattern_cycle_index_of(const char *id);
const char *pattern_cycle_advance(const char *current_id);

uint8_t brightness_cycle_index_of(uint8_t value);
uint8_t brightness_cycle_advance(uint8_t current);
