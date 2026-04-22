#pragma once

/*
 * Pure-C pattern renderers.
 *
 * Each pattern is a function from (context, time, base color, LED count)
 * to an RGB frame buffer. No allocations, no ESP-IDF, no FreeRTOS —
 * everything the renderer needs lives in the context struct. This keeps
 * the animation code trivially host-testable: feed in tick_ms values,
 * inspect the output bytes.
 *
 * Patterns are stateless with respect to the renderer: all animation is
 * derived from (tick_ms - pattern_start_ms). This trades a bit of math
 * per frame for no per-pattern allocation, no state migration on pattern
 * change, and deterministic output for a given input (→ testable).
 *
 * The registry below maps the seven IDs in the button cycle
 * (solid / rainbow / scanner / spinner / random / breathing / twinkle)
 * to their renderers. Unknown IDs fall back to solid.
 */

#include <stdint.h>

/* Frame geometry limits. Renderers work with any LED count up to this. */
#define PATTERNS_MAX_LEDS 256

typedef struct {
    uint16_t led_count;
    uint32_t base_color_rgb;   /* 0xRRGGBB */
    uint32_t tick_ms;          /* monotonic time, absolute */
    uint32_t pattern_start_ms; /* when the current pattern became active */
} pattern_ctx_t;

/* Signature for a pattern renderer. Writes led_count*3 bytes into out_rgb,
 * ordered R,G,B per LED. Caller owns the buffer. */
typedef void (*pattern_render_fn)(const pattern_ctx_t *ctx, uint8_t *out_rgb);

typedef struct {
    const char *id;
    pattern_render_fn render;
} pattern_def_t;

/* Look up a renderer by id. Returns a pointer to the solid renderer if
 * the id is unknown or NULL — callers never have to null-check the result. */
const pattern_def_t *patterns_lookup(const char *id);

/* Convenience: render whichever pattern matches ctx's id lookup. If you
 * already have a pattern_def_t*, call def->render(ctx, out) directly. */
void patterns_render(const char *id, const pattern_ctx_t *ctx,
                     uint8_t *out_rgb);
