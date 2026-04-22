---
title: Pattern Dictionary — Shared Firmware Spec
type: engineering
phase: 2
week: 4
date: 2026-04-22
status: v1 — seeded from STM8 canonical implementation (stm8_150mm.ino)
tags: [engineering, firmware, patterns, cross-firmware, spec]
---

# Pattern Dictionary — Shared Firmware Spec

**Prepared by:** William White
**Date:** April 22, 2026
**Status:** v1 — authoritative pattern contract for all Layered Logic infinity-mirror firmwares (STM8 Basic, ESP32 Pro, ESP32 Pro+)

---

## 1. Purpose

One source of truth for **what a pattern does**, independent of the microcontroller running it. A user who flips their Basic STM8 mirror from "Scanner" to "Scanner" on a Pro ESP32 mirror should see the same visual behavior.

Patterns are algorithmic data (colors, fade rates, probabilities, tick cadence), not hardware. This doc lives between the product and any specific firmware implementation.

**Readers:**
- Firmware implementers (STM8, ESP32 standard, ESP32 matter) — use this to port patterns faithfully
- App + webapp builders — know what pattern IDs exist and what they do visually
- Future-me debugging pattern drift across hardware generations

**Not in scope:**
- LED driver timing (WS2812B bit-bang on STM8, RMT on ESP32) — lives in firmware
- Pattern **upload grammar** for user-defined patterns — deferred to V2 app spec
- Exact byte layout of stored pattern data — firmware internal

---

## 2. Canonical Constants

These constants apply to every firmware that implements this dictionary. Ported verbatim from STM8 — any change requires a new revision of this doc.

### 2.1 Palette

13 colors, addressed by index 0..12. Single-press (when on) cycles through this list.

| Index | Name | RGB | Hex |
|---|---|---|---|
| 0 | White | (255, 255, 255) | `#FFFFFF` |
| 1 | Red | (255, 0, 0) | `#FF0000` |
| 2 | Red-Orange | (255, 64, 0) | `#FF4000` |
| 3 | Orange | (255, 128, 0) | `#FF8000` |
| 4 | Yellow-Orange | (255, 192, 0) | `#FFC000` |
| 5 | Yellow | (255, 255, 0) | `#FFFF00` |
| 6 | Yellow-Green | (128, 255, 0) | `#80FF00` |
| 7 | Green | (0, 255, 0) | `#00FF00` |
| 8 | Blue-Green | (0, 255, 128) | `#00FF80` |
| 9 | Blue | (0, 0, 255) | `#0000FF` |
| 10 | Blue-Purple | (64, 0, 255) | `#4000FF` |
| 11 | Purple | (128, 0, 255) | `#8000FF` |
| 12 | Red-Purple | (255, 0, 128) | `#FF0080` |

**Note on ESP32 extension:** The ESP32 firmware's primary button exposes a finer *base color* cycle (16 spectrum + 1 white = 17 positions, see [button-interface §4.3](button-interface.md#43-base-color-cycle-single-press-semantics)). This 13-color palette remains the **canonical pattern-reference palette** — patterns reference "base color" as an abstract token; the actual color comes from whatever cycle the current UI supports. STM8 resolves to this table; ESP32 resolves to its 17-position cycle. Visual behavior of the pattern is identical either way.

### 2.2 White-Only Dimming Rule

**Solid pattern + White color → apply WHITE_DIM_FACTOR = 217/255 (~85%) on top of user brightness.**

Reason: direct white at full brightness is uncomfortably bright and saps lifespan. Dimming is scoped to solid-on-white only — every other pattern and every other color uses user brightness verbatim. This rule is a permanent product decision, not a per-firmware tweak.

Pseudocode:
```
if pattern == SOLID and color_index == 0:
    effective_brightness = scale8(user_brightness, 217)
else:
    effective_brightness = user_brightness
```

### 2.3 Brightness Semantics

| Firmware | Brightness values |
|---|---|
| STM8 | 25–255, step 25 (`DEFAULT=128`, `MIN=25`, `STEP=25`); triple-press decrements, wraps to 128 when would go below `MIN+STEP` |
| ESP32 | 4 discrete steps: 25 / 50 / 75 / 100 (%) — triple-press cycles |

The **cycling semantics differ by firmware** (legacy STM8 behavior preserved for Basic SKU), but both apply uniformly after pattern rendering (no pattern computes against brightness).

### 2.4 Tick Rate

**50 ms per frame (20 Hz).** Every pattern's animation advances one step per tick. This is the canonical cadence for the STM8 implementation and the reference for ESP32 ports.

ESP32 implementations **may** render at a higher frame rate (up to 60 Hz) by re-rendering the same pattern state between logical steps, but the logical pattern state must advance at 20 Hz to preserve identical visual pacing across hardware.

### 2.5 State Variables

Every pattern has access to:
- `base_color` — RGB of the currently selected palette color
- `animStep` — uint8, incremented by each pattern's own rule, wraps at 256
- `breathVal` + `breathDir` — uint8 + bool, used by Breathing only, persists between ticks
- `ledBuffer[NUM_LEDS]` — RGB8 output buffer the pattern writes to

---

## 3. Patterns

Each pattern has a canonical **ID** (used on the wire in [control-protocol-spec §5](control-protocol-spec.md#5-state-object) `pattern_id`), an STM8 enum (historical, preserved), and a full algorithmic spec.

| ID | Name | STM8 enum | Uses base color | Fade/tail |
|---|---|---|---|---|
| `solid` | Solid | `PATTERN_SOLID` | yes | — |
| `rainbow` | Rainbow | `PATTERN_RAINBOW` | no (self-palette) | — |
| `scanner` | Scanner | `PATTERN_SCANNER` | yes | per-tick fade 20 |
| `spinner` | Spinner | `PATTERN_SPINNER` | yes | 1/3-brightness tail (no fade) |
| `random` | Random Twinkle | `PATTERN_RANDOM` | yes | per-tick fade 15 |
| `breathing` | Breathing | `PATTERN_BREATHING` | yes | — |
| `twinkle` | Twinkle | `PATTERN_TWINKLE` | yes | per-tick fade 10 |

### 3.1 `solid`

**Description:** All LEDs show `base_color` at current brightness. The White case applies the WHITE_DIM_FACTOR rule (§2.2).

**Algorithm:**
```
for each LED: ledBuffer[i] = base_color
```

**Constants:** none beyond the dim rule.

**Tick rate:** redraws every tick but output is static.

**Canonical implementation:** [stm8_150mm.ino:221-226](../Firmware/STM8/stm8_150mm.ino)

---

### 3.2 `rainbow`

**Description:** Ignores `base_color`. Every LED shows a different hue from a rainbow wheel; the wheel rotates forward at a constant rate.

**Algorithm:**
```
for i in 0..NUM_LEDS-1:
    pos = (i * 256 / NUM_LEDS + animStep) mod 256
    ledBuffer[i] = wheel(pos)
animStep = (animStep + 3) mod 256
```

`wheel(pos)` is a three-segment RGB wheel (0–85 red→green, 85–170 green→blue, 170–255 blue→red), each channel computed as `pos * 3` or `255 - pos * 3` within its segment. See [stm8_150mm.ino:214-218](../Firmware/STM8/stm8_150mm.ino).

**Constants:** `ROTATION_STEP = 3` per tick.

**Tick rate:** 50 ms.

**Note:** Single-press still advances the base color in firmware state during rainbow — the user's "base color" is sticky but invisible until they switch to a base-color pattern. See [button-interface §4.3](button-interface.md#43-base-color-cycle-single-press-semantics).

**Canonical implementation:** [stm8_150mm.ino:228-235](../Firmware/STM8/stm8_150mm.ino)

---

### 3.3 `scanner`

**Description:** A bright dot of `base_color` travels back and forth across the strip (a "KITT" / Cylon sweep). Trails fade behind it.

**Algorithm:**
```
pos_raw = animStep mod (NUM_LEDS * 2)
pos = pos_raw if pos_raw < NUM_LEDS else (NUM_LEDS * 2 - pos_raw - 1)

for each LED:
    ledBuffer[i] = max(0, ledBuffer[i] - 20) per channel  // fade

ledBuffer[pos] = base_color
animStep = (animStep + 1) mod 256
```

**Constants:** `FADE_PER_TICK = 20` per channel.

**Tick rate:** 50 ms. Full sweep (32 LEDs both directions) = 64 ticks = 3.2 s.

**Canonical implementation:** [stm8_150mm.ino:237-258](../Firmware/STM8/stm8_150mm.ino)

---

### 3.4 `spinner`

**Description:** Four `base_color` dots evenly spaced around the ring, rotating together like a fan. Each leading dot has a dim (1/3-brightness) trailing dot.

**Algorithm:**
```
spacing = NUM_LEDS / 4              // 8 for 32 LEDs
offset = animStep mod spacing       // 0..7

clear buffer (all black)

for arm in 0..3:
    pos = (arm * spacing + offset) mod NUM_LEDS
    ledBuffer[pos] = base_color
    tail_pos = (pos + NUM_LEDS - 1) mod NUM_LEDS
    ledBuffer[tail_pos] = base_color / 3   // per channel

animStep = (animStep + 1) mod 256
```

**Constants:** `NUM_ARMS = 4`, `TAIL_BRIGHTNESS = 1/3`.

**Tick rate:** 50 ms. Full rotation (8 steps) = 400 ms.

**Notes:**
- Spinner **clears the buffer each tick** (no persistent fade). This is intentional — the tail is explicit, not a fade artifact.
- Assumes `NUM_LEDS` is divisible by 4. For non-multiples-of-4, firmware implementers should adjust `NUM_ARMS` or round spacing down.

**Canonical implementation:** [stm8_150mm.ino:260-285](../Firmware/STM8/stm8_150mm.ino)

---

### 3.5 `random`

**Description:** Sparkles of `base_color` pop on at random LED positions and fade out. Only one LED lights per sparkle trigger.

**Algorithm:**
```
for each LED:
    ledBuffer[i] = max(0, ledBuffer[i] - 15) per channel  // fade

if animStep mod 3 == 0:
    pos = random() mod NUM_LEDS
    ledBuffer[pos] = base_color

animStep = (animStep + 1) mod 256
```

**Constants:** `FADE_PER_TICK = 15` per channel, `SPARKLE_EVERY_N_TICKS = 3`.

**Tick rate:** 50 ms. Sparkle triggers every 150 ms.

**RNG:** STM8 uses a 16-bit LFSR (taps `0xB400`), seeded `12345`. ESP32 implementations should use their own PRNG but do **not** need to match the seed (visual behavior is statistical, not deterministic).

**Canonical implementation:** [stm8_150mm.ino:287-309](../Firmware/STM8/stm8_150mm.ino)

---

### 3.6 `breathing`

**Description:** All LEDs show `base_color` at a brightness that rises and falls in a triangular wave (not sinusoidal — this is deliberate; the STM8 can't afford trig and the visual difference is small).

**Algorithm:**
```
// Update breath value (triangular wave)
if breathDir == 1 (up):
    if breathVal < 250: breathVal += 5
    else: breathDir = 0 (down)
else:
    if breathVal > 5: breathVal -= 5
    else: breathDir = 1 (up)

// Apply to all LEDs
for each LED:
    ledBuffer[i].r = scale8(base_color.r, breathVal)
    ledBuffer[i].g = scale8(base_color.g, breathVal)
    ledBuffer[i].b = scale8(base_color.b, breathVal)
```

`scale8(v, sc) = v * sc / 255`.

**Constants:** `STEP = 5`, `RANGE = [5, 250]`.

**Tick rate:** 50 ms. Full cycle = 2 × (245 / 5) × 50 ms = 4.9 s.

**Notes:**
- `breathVal` and `breathDir` persist between ticks. They must be reset to `0` and `1` (up) on pattern entry / power-on.
- Apparent minimum is `breathVal=5` → barely visible, never fully dark. Deliberate — fully-dark breathing looks broken.

**Canonical implementation:** [stm8_150mm.ino:311-332](../Firmware/STM8/stm8_150mm.ino)

---

### 3.7 `twinkle`

**Description:** Stars. Random LEDs briefly light up at full `base_color` and slowly fade. Multiple LEDs can sparkle simultaneously.

**Algorithm:**
```
for each LED:
    rnd = random()
    if rnd < 30:
        ledBuffer[i] = base_color  // sparkle on
    else:
        ledBuffer[i] -= 10 per channel (clamped at 0)  // fade
```

**Constants:** `SPARKLE_PROB = 30/255 ≈ 11.8%` per LED per tick, `FADE_PER_TICK = 10` per channel.

**Tick rate:** 50 ms.

**Notes:** Unlike `random`, every LED rolls a sparkle check each tick, so multiple simultaneous sparkles are expected. Fade is slower (10/tick vs 15/tick) giving a lingering glow.

**Canonical implementation:** [stm8_150mm.ino:334-355](../Firmware/STM8/stm8_150mm.ino)

---

## 4. Pattern Entry / Exit Rules

When the user switches patterns (double-press), firmware must:

1. Clear `ledBuffer` to all-black (prevents old pattern's residual fade state from bleeding into the new one).
2. Reset `animStep = 0`.
3. Reset `breathVal = 0`, `breathDir = 1` (relevant only if entering breathing, harmless otherwise).
4. Render one frame of the new pattern before next tick.

See [stm8_150mm.ino:533-546](../Firmware/STM8/stm8_150mm.ino) for the canonical entry sequence.

---

## 5. Persistence Semantics

Defined here because it's part of the **pattern contract**, not the transport spec.

| Event | STM8 behavior | ESP32 behavior |
|---|---|---|
| **Long-press off** (user holds primary button) | Save `(color, pattern, brightness)` to EEPROM with magic byte `0xA5` | Same fields saved to NVS |
| **Single-press on** (after long-press off, same power session) | Load saved state, then invalidate magic so next power loss = defaults | Load saved state, no invalidation needed (distinguished by soft vs hard off flag) |
| **Hard power loss / unplug** | On boot, magic byte is cleared → defaults (color=0 white, pattern=SOLID, brightness=128) | Defaults to brand-configured default (Indigo Signal `#3214FF`, 80% brightness) |

**Key invariant across firmwares:** *Unplugging always resets to defaults. Soft-off preserves state within a power session.*

See [button-interface §4.1](button-interface.md#41-persistence) and [stm8_150mm.ino:126-153](../Firmware/STM8/stm8_150mm.ino).

---

## 6. Firmware Coverage Matrix

Which firmware must implement which patterns.

| Pattern | STM8 Basic | ESP32 Pro (standard) | ESP32 Pro+ (matter) |
|---|---|---|---|
| `solid` | ✅ shipping | required V1 | required V1 |
| `rainbow` | ✅ shipping | required V1 | required V1 |
| `scanner` | ✅ shipping | required V1 | required V1 |
| `spinner` | ✅ shipping | required V1 | required V1 |
| `random` | ✅ shipping | required V1 | required V1 |
| `breathing` | ✅ shipping | required V1 | required V1 |
| `twinkle` | ✅ shipping | required V1 | required V1 |
| *user-uploaded* | — (no uplink) | V2 — when app-defined grammar ships | V2 |
| *pairing cue* (slow indigo pulse) | — | internal (not user-selectable) | internal |
| *factory-reset cue* (red flash) | — | internal | internal |

Pattern ID on the wire is lowercase_snake (`solid`, `scanner`, `random_twinkle` → use `random` to match STM8 enum). The `random_twinkle` name was an earlier draft in firmware-spec.md; canonical is `random`.

---

## 7. V2 — Declarative Pattern Grammar

Out of scope for V1. When the mobile app ships with a pattern-editor UI, we'll define a declarative JSON grammar for user-uploaded patterns. Placeholder shape in [control-protocol-spec §6](control-protocol-spec.md#6-pattern-definition-format).

V1 approach: **all 7 patterns are hardcoded C functions** on both STM8 and ESP32, matching the STM8 implementation 1:1. No pattern uploads in V1. The app exposes the 7 built-ins as fixed choices with human names + preview thumbnails.

This keeps V1 simple and guaranteed-matching across SKUs. V2 can add user patterns without breaking V1's visual contract.

---

## 8. How to Port

When writing a new firmware (e.g., the ESP32 pattern_interp module):

1. Start from §3 algorithms — they're expressed in hardware-agnostic pseudocode.
2. Match constants exactly: fades, probabilities, steps, tick rate.
3. Cross-check against [stm8_150mm.ino](../Firmware/STM8/stm8_150mm.ino) — this is the reference; if pseudocode and code disagree, code wins and this doc gets updated.
4. Port §2.2 (white dimming) and §4 (pattern entry) — both easy to miss.
5. Side-by-side visual check: run Basic and Pro mirrors on the same color + same pattern; they should be indistinguishable at arm's length.

---

## 9. Revision Policy

Changing a pattern's visual behavior (fade rate, tick rate, color reference) is a **breaking change** to the shared pattern contract. Any such change:

1. Bumps this doc's version.
2. Requires OTA to all ESP32 fleet (STM8 is firmware-frozen in shipping units; a change would be cosmetic-only for new production runs).
3. Gets logged in the sprint log + Advisor meeting notes.

Bug fixes (e.g., off-by-one on wrap) don't bump version but do get a note here.

---

## Related

- [Firmware Spec](firmware-spec.md) — ESP32 core modules; `pattern_interp` implements this dictionary
- [Control Protocol Spec](control-protocol-spec.md) — wire representation of pattern state
- [Button Interface Design](button-interface.md) — user-facing gestures that cycle patterns
- [Firmware Architecture Scoping](firmware-architecture-scoping.md) — higher-level context
- [Firmware/STM8/spec.md](../Firmware/STM8/spec.md) — STM8-specific implementation notes
- [Firmware/STM8/stm8_150mm.ino](../Firmware/STM8/stm8_150mm.ino) — canonical reference implementation
