#include "button_logic.h"

#include <stdint.h>
#include <string.h>

/* ---- Cycle tables ----
 *
 * COLOR_CYCLE: 16 HSV hue steps at 22.5° increments (full S, full V) plus
 * one white step. See button-interface.md §4.3. Brightness scaling happens
 * downstream in led_driver, not here.
 */
const uint32_t LL_COLOR_CYCLE[LL_COLOR_CYCLE_LEN] = {
    0xFF0000u,  /*   0.0° red          */
    0xFF6000u,  /*  22.5° red-orange   */
    0xFFBF00u,  /*  45.0° orange       */
    0xDFFF00u,  /*  67.5° yellow       */
    0x80FF00u,  /*  90.0° yellow-green */
    0x20FF00u,  /* 112.5° green        */
    0x00FF40u,  /* 135.0° green-cyan   */
    0x00FF9Fu,  /* 157.5° cyan-green   */
    0x00FFFFu,  /* 180.0° cyan         */
    0x009FFFu,  /* 202.5° cyan-blue    */
    0x0040FFu,  /* 225.0° blue         */
    0x2000FFu,  /* 247.5° blue-violet  */
    0x8000FFu,  /* 270.0° violet       */
    0xDF00FFu,  /* 292.5° magenta      */
    0xFF00BFu,  /* 315.0° magenta-pink */
    0xFF0060u,  /* 337.5° pink         */
    0xFFFFFFu,  /* white               */
};

const char *const LL_PATTERN_CYCLE[LL_PATTERN_CYCLE_LEN] = {
    "solid", "rainbow", "scanner", "spinner",
    "random", "breathing", "twinkle",
};

const uint8_t LL_BRIGHTNESS_CYCLE[LL_BRIGHTNESS_CYCLE_LEN] = {
    25, 50, 75, 100,
};

/* ---- Cycle helpers ---- */

uint8_t color_cycle_index_of(uint32_t rgb)
{
    int32_t r = (int32_t)((rgb >> 16) & 0xFF);
    int32_t g = (int32_t)((rgb >> 8) & 0xFF);
    int32_t b = (int32_t)(rgb & 0xFF);

    uint8_t best = 0;
    uint32_t best_dist = UINT32_MAX;
    for (uint8_t i = 0; i < LL_COLOR_CYCLE_LEN; i++) {
        int32_t cr = (int32_t)((LL_COLOR_CYCLE[i] >> 16) & 0xFF);
        int32_t cg = (int32_t)((LL_COLOR_CYCLE[i] >> 8) & 0xFF);
        int32_t cb = (int32_t)(LL_COLOR_CYCLE[i] & 0xFF);
        int32_t dr = cr - r, dg = cg - g, db = cb - b;
        uint32_t d = (uint32_t)(dr * dr + dg * dg + db * db);
        if (d < best_dist) {
            best_dist = d;
            best = i;
        }
    }
    return best;
}

uint32_t color_cycle_advance(uint32_t current_rgb)
{
    uint8_t i = color_cycle_index_of(current_rgb);
    return LL_COLOR_CYCLE[(i + 1) % LL_COLOR_CYCLE_LEN];
}

uint8_t pattern_cycle_index_of(const char *id)
{
    if (id == NULL) return 0;
    for (uint8_t i = 0; i < LL_PATTERN_CYCLE_LEN; i++) {
        if (strcmp(LL_PATTERN_CYCLE[i], id) == 0) return i;
    }
    return 0;
}

const char *pattern_cycle_advance(const char *current_id)
{
    uint8_t i = pattern_cycle_index_of(current_id);
    return LL_PATTERN_CYCLE[(i + 1) % LL_PATTERN_CYCLE_LEN];
}

uint8_t brightness_cycle_index_of(uint8_t value)
{
    uint8_t best = 0;
    uint8_t best_dist = 255;
    for (uint8_t i = 0; i < LL_BRIGHTNESS_CYCLE_LEN; i++) {
        uint8_t d = value > LL_BRIGHTNESS_CYCLE[i]
                  ? (uint8_t)(value - LL_BRIGHTNESS_CYCLE[i])
                  : (uint8_t)(LL_BRIGHTNESS_CYCLE[i] - value);
        if (d < best_dist) {
            best_dist = d;
            best = i;
        }
    }
    return best;
}

uint8_t brightness_cycle_advance(uint8_t current)
{
    uint8_t i = brightness_cycle_index_of(current);
    return LL_BRIGHTNESS_CYCLE[(i + 1) % LL_BRIGHTNESS_CYCLE_LEN];
}

/* ---- Primary gesture state machine ----
 *
 * Tracks press/release edges + tick. Decides:
 *   - HOLD fires while still pressed past LONG_PRESS_MS (and short-circuits
 *     any pending click count — clicks before a hold are dropped).
 *   - SINGLE/DOUBLE/TRIPLE fire on tick after DOUBLE_CLICK_MS of release
 *     idle.
 */
primary_gesture_t primary_step(primary_state_t *s,
                               gesture_input_t in,
                               uint32_t now_ms)
{
    switch (in) {
    case GESTURE_INPUT_PRESS:
        s->pressed = true;
        s->press_start_ms = now_ms;
        s->hold_fired = false;
        return PRIMARY_GESTURE_NONE;

    case GESTURE_INPUT_RELEASE:
        s->pressed = false;
        if (!s->hold_fired) {
            s->click_count++;
            s->last_release_ms = now_ms;
        }
        return PRIMARY_GESTURE_NONE;

    case GESTURE_INPUT_TICK:
        if (s->pressed && !s->hold_fired
            && (now_ms - s->press_start_ms) >= LL_BUTTON_LONG_PRESS_MS) {
            s->hold_fired = true;
            s->click_count = 0;
            return PRIMARY_GESTURE_HOLD;
        }
        if (!s->pressed && s->click_count > 0
            && (now_ms - s->last_release_ms) >= LL_BUTTON_DOUBLE_CLICK_MS) {
            primary_gesture_t g;
            switch (s->click_count) {
            case 1:  g = PRIMARY_GESTURE_SINGLE; break;
            case 2:  g = PRIMARY_GESTURE_DOUBLE; break;
            default: g = PRIMARY_GESTURE_TRIPLE; break;  /* 3+ clamps */
            }
            s->click_count = 0;
            return g;
        }
        return PRIMARY_GESTURE_NONE;
    }
    return PRIMARY_GESTURE_NONE;
}

/* ---- Recessed gesture state machine ----
 *
 * Hold-only — no tap semantics. Release between PROVISION and FACTORY
 * thresholds fires PROVISION; holding past FACTORY threshold fires
 * FACTORY_RESET immediately (don't wait for release, since the user
 * needs a clear "yes it triggered" cue before letting go).
 */
recessed_gesture_t recessed_step(recessed_state_t *s,
                                 gesture_input_t in,
                                 uint32_t now_ms)
{
    switch (in) {
    case GESTURE_INPUT_PRESS:
        s->pressed = true;
        s->press_start_ms = now_ms;
        s->factory_fired = false;
        return RECESSED_GESTURE_NONE;

    case GESTURE_INPUT_RELEASE:
        s->pressed = false;
        if (!s->factory_fired
            && (now_ms - s->press_start_ms) >= LL_RECESSED_PROVISION_MS) {
            return RECESSED_GESTURE_PROVISION;
        }
        return RECESSED_GESTURE_NONE;

    case GESTURE_INPUT_TICK:
        if (s->pressed && !s->factory_fired
            && (now_ms - s->press_start_ms) >= LL_RECESSED_FACTORY_MS) {
            s->factory_fired = true;
            return RECESSED_GESTURE_FACTORY_RESET;
        }
        return RECESSED_GESTURE_NONE;
    }
    return RECESSED_GESTURE_NONE;
}
