#include "patterns.h"

#include <string.h>

/* ---- Shared helpers ---- */

static inline uint8_t rgb_r(uint32_t c) { return (uint8_t)((c >> 16) & 0xFFu); }
static inline uint8_t rgb_g(uint32_t c) { return (uint8_t)((c >>  8) & 0xFFu); }
static inline uint8_t rgb_b(uint32_t c) { return (uint8_t)( c        & 0xFFu); }

/* Integer HSV → RGB.
 *   h: 0..1535 (256 steps per sector × 6 sectors — finer than degrees,
 *      avoids float and keeps hue transitions smooth on 8-bit channels)
 *   s,v: 0..255
 *
 * Reference: standard 6-sector HSV decomposition. Matches the output of
 * typical float implementations to ±1 per channel. */
static void hsv_to_rgb(uint16_t h, uint8_t s, uint8_t v,
                       uint8_t *r, uint8_t *g, uint8_t *b)
{
    h %= 1536;
    uint8_t sector = (uint8_t)(h / 256);
    uint8_t frac   = (uint8_t)(h % 256);

    uint8_t p = (uint8_t)(((uint16_t)v * (255 - s)) / 255);
    uint8_t q = (uint8_t)(((uint16_t)v * (255 - ((uint16_t)s * frac) / 255)) / 255);
    uint8_t t = (uint8_t)(((uint16_t)v * (255 - ((uint16_t)s * (255 - frac)) / 255)) / 255);

    switch (sector) {
    case 0:  *r = v; *g = t; *b = p; break;
    case 1:  *r = q; *g = v; *b = p; break;
    case 2:  *r = p; *g = v; *b = t; break;
    case 3:  *r = p; *g = q; *b = v; break;
    case 4:  *r = t; *g = p; *b = v; break;
    case 5:  *r = v; *g = p; *b = q; break;
    default: *r = 0; *g = 0; *b = 0; break;  /* unreachable: h%1536 < 6*256 */
    }
}

/* 32-bit integer hash. Good-enough mixing for animation seeds — not a
 * cryptographic hash. Source: splitmix-ish constants, widely used for
 * deterministic visual noise. */
static uint32_t hash32(uint32_t x)
{
    x ^= x >> 16;
    x *= 0x7feb352dU;
    x ^= x >> 15;
    x *= 0x846ca68bU;
    x ^= x >> 16;
    return x;
}

/* Triangle wave 0..255..0 over `period` ms. Stateless; derived purely
 * from the phase within the period. */
static uint8_t triangle_u8(uint32_t t_ms, uint32_t period_ms)
{
    if (period_ms == 0) return 0;
    uint32_t phase = t_ms % period_ms;
    uint32_t half = period_ms / 2;
    if (phase < half) {
        return (uint8_t)((phase * 255u) / half);
    } else {
        return (uint8_t)(((period_ms - phase) * 255u) / half);
    }
}

static inline uint8_t scale_u8(uint8_t v, uint8_t scale)
{
    return (uint8_t)(((uint16_t)v * scale) / 255u);
}

static void fill_rgb(uint8_t *out, uint16_t n, uint8_t r, uint8_t g, uint8_t b)
{
    for (uint16_t i = 0; i < n; i++) {
        out[i * 3 + 0] = r;
        out[i * 3 + 1] = g;
        out[i * 3 + 2] = b;
    }
}

static void write_pixel(uint8_t *out, uint16_t i,
                        uint8_t r, uint8_t g, uint8_t b)
{
    out[i * 3 + 0] = r;
    out[i * 3 + 1] = g;
    out[i * 3 + 2] = b;
}

/* ---- Patterns ---- */

static void render_solid(const pattern_ctx_t *ctx, uint8_t *out_rgb)
{
    fill_rgb(out_rgb, ctx->led_count,
             rgb_r(ctx->base_color_rgb),
             rgb_g(ctx->base_color_rgb),
             rgb_b(ctx->base_color_rgb));
}

static void render_rainbow(const pattern_ctx_t *ctx, uint8_t *out_rgb)
{
    if (ctx->led_count == 0) return;

    uint32_t elapsed = ctx->tick_ms - ctx->pattern_start_ms;
    /* ~12s per full hue rotation: 1536 hue units / (elapsed / 8ms). */
    uint16_t base_hue = (uint16_t)((elapsed / 8u) % 1536u);
    uint16_t step = (uint16_t)(1536u / ctx->led_count);

    for (uint16_t i = 0; i < ctx->led_count; i++) {
        uint16_t h = (uint16_t)((base_hue + i * step) % 1536u);
        uint8_t r, g, b;
        hsv_to_rgb(h, 255, 255, &r, &g, &b);
        write_pixel(out_rgb, i, r, g, b);
    }
}

static void render_scanner(const pattern_ctx_t *ctx, uint8_t *out_rgb)
{
    uint16_t n = ctx->led_count;
    if (n == 0) return;

    uint32_t elapsed = ctx->tick_ms - ctx->pattern_start_ms;
    const uint32_t period = 2000;   /* ms for one back-and-forth sweep */

    /* Head position as triangle wave mapped onto [0, n-1]. */
    uint32_t phase = elapsed % period;
    uint32_t half = period / 2;
    int16_t head;
    if (phase < half) {
        head = (int16_t)((phase * (n > 1 ? (n - 1) : 0)) / half);
    } else {
        head = (int16_t)(((period - phase) * (n > 1 ? (n - 1) : 0)) / half);
    }

    uint8_t r = rgb_r(ctx->base_color_rgb);
    uint8_t g = rgb_g(ctx->base_color_rgb);
    uint8_t b = rgb_b(ctx->base_color_rgb);

    /* Head + two-pixel fading tail on each side. */
    for (uint16_t i = 0; i < n; i++) {
        int16_t d = (int16_t)i - head;
        if (d < 0) d = (int16_t)-d;
        uint8_t scale;
        switch (d) {
        case 0:  scale = 255; break;
        case 1:  scale = 96;  break;
        case 2:  scale = 24;  break;
        default: scale = 0;   break;
        }
        write_pixel(out_rgb, i, scale_u8(r, scale),
                                scale_u8(g, scale),
                                scale_u8(b, scale));
    }
}

static void render_spinner(const pattern_ctx_t *ctx, uint8_t *out_rgb)
{
    uint16_t n = ctx->led_count;
    if (n == 0) return;

    uint32_t elapsed = ctx->tick_ms - ctx->pattern_start_ms;
    uint16_t pos = (uint16_t)((elapsed / 60u) % n);  /* 60ms per LED step */

    fill_rgb(out_rgb, n, 0, 0, 0);
    write_pixel(out_rgb, pos,
                rgb_r(ctx->base_color_rgb),
                rgb_g(ctx->base_color_rgb),
                rgb_b(ctx->base_color_rgb));
}

static void render_random(const pattern_ctx_t *ctx, uint8_t *out_rgb)
{
    uint32_t elapsed = ctx->tick_ms - ctx->pattern_start_ms;
    uint32_t epoch = elapsed / 100u;   /* 10 Hz refresh */

    uint8_t r = rgb_r(ctx->base_color_rgb);
    uint8_t g = rgb_g(ctx->base_color_rgb);
    uint8_t b = rgb_b(ctx->base_color_rgb);

    for (uint16_t i = 0; i < ctx->led_count; i++) {
        /* Combine epoch and index into a single hash input so each
         * (epoch, led) pair has an independent draw. */
        uint32_t h = hash32(epoch * 0x9E3779B9u + i);
        if ((h & 0xFFu) < 64u) {   /* ~25% duty */
            write_pixel(out_rgb, i, r, g, b);
        } else {
            write_pixel(out_rgb, i, 0, 0, 0);
        }
    }
}

static void render_breathing(const pattern_ctx_t *ctx, uint8_t *out_rgb)
{
    uint32_t elapsed = ctx->tick_ms - ctx->pattern_start_ms;
    uint8_t scale = triangle_u8(elapsed, 3000);  /* 3s breath cycle */
    /* Floor so the pattern never goes fully dark — a user setting brightness
     * already handles "off". Breathing to black reads as broken. */
    if (scale < 20) scale = 20;

    uint8_t r = scale_u8(rgb_r(ctx->base_color_rgb), scale);
    uint8_t g = scale_u8(rgb_g(ctx->base_color_rgb), scale);
    uint8_t b = scale_u8(rgb_b(ctx->base_color_rgb), scale);

    fill_rgb(out_rgb, ctx->led_count, r, g, b);
}

static void render_twinkle(const pattern_ctx_t *ctx, uint8_t *out_rgb)
{
    const uint32_t period = 4000;
    uint32_t elapsed = ctx->tick_ms - ctx->pattern_start_ms;

    uint8_t r = rgb_r(ctx->base_color_rgb);
    uint8_t g = rgb_g(ctx->base_color_rgb);
    uint8_t b = rgb_b(ctx->base_color_rgb);

    for (uint16_t i = 0; i < ctx->led_count; i++) {
        /* Per-LED constant phase offset → each LED breathes on its own
         * schedule, so the ensemble looks like uncorrelated twinkling. */
        uint32_t offset = hash32((uint32_t)i * 0x9E3779B9u) % period;
        uint8_t scale = triangle_u8(elapsed + offset, period);
        write_pixel(out_rgb, i, scale_u8(r, scale),
                                scale_u8(g, scale),
                                scale_u8(b, scale));
    }
}

/* ---- Registry ---- */

static const pattern_def_t k_patterns[] = {
    { "solid",     render_solid     },
    { "rainbow",   render_rainbow   },
    { "scanner",   render_scanner   },
    { "spinner",   render_spinner   },
    { "random",    render_random    },
    { "breathing", render_breathing },
    { "twinkle",   render_twinkle   },
};
#define PATTERN_COUNT (sizeof(k_patterns) / sizeof(k_patterns[0]))

const pattern_def_t *patterns_lookup(const char *id)
{
    if (id) {
        for (size_t i = 0; i < PATTERN_COUNT; i++) {
            if (strcmp(id, k_patterns[i].id) == 0) {
                return &k_patterns[i];
            }
        }
    }
    /* Solid is index 0 and the spec-defined fallback. */
    return &k_patterns[0];
}

void patterns_render(const char *id, const pattern_ctx_t *ctx,
                     uint8_t *out_rgb)
{
    patterns_lookup(id)->render(ctx, out_rgb);
}
