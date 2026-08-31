---
title: STM8 Basic Firmware — Spec
type: engineering
phase: 2
week: 4
date: 2026-04-22
status: shipping — matches stm8_150mm.ino; canonical reference for pattern behavior
tags: [engineering, firmware, stm8, basic, shipping]
---

# STM8 Basic Firmware — Spec

> **This documents the 150 mm / 6"×6" firmware.** The rev-2 controller board
> (`D:\PCBs\Infinity_Mirror_STM8`, JLC order Y12) runs
> [`../STM8_rev2/`](../STM8_rev2/README.md) instead — a bare-metal SDCC
> rewrite with a real millisecond timebase, an interrupt vector table that
> actually contains the TIM2 handler, and an in-spec ADC clock. Behaviour is
> the same product contract; the sections below still describe it. Do not
> flash `stm8_150mm.ino` to a rev-2 board.

**Prepared by:** William White
**Date:** April 22, 2026
**Status:** Documents the firmware currently running on Layered Logic Basic (6"×6" STM8-based) units. This is the **canonical reference** for pattern visual behavior across the product line.

---

## 1. Purpose

This doc describes what the STM8 firmware does, why it does it that way, and what parts of its behavior are **locked as product contract** for all future Layered Logic firmwares.

Readers:
- Me-next-year re-flashing a unit or debugging a return
- ESP32 firmware implementers porting the patterns faithfully ([pattern-dictionary.md](../../docs/pattern-dictionary.md))
- Advisor / future contractor understanding the baseline

Source file: [stm8_150mm.ino](stm8_150mm.ino) (577 lines).

---

## 2. Hardware Target

| Item | Value |
|---|---|
| MCU | STM8S003F3P6 (TSSOP20, 8KB flash, 1KB RAM, 128B EEPROM) |
| Clock | 16 MHz internal HSI, no prescaler |
| LED strip | 32× WS2812B, arranged in one continuous loop around the 6"×6" infinity mirror |
| Button | 1× tactile momentary, SPST, internal pullup |
| LED data pin | **GPIOC pin 6 (TSSOP20 physical pin 16)** |
| Button pin | GPIOD pin 6 (active-low, internal pullup) |
| Power | 5V from USB-C **direct** to MCU + strip — there is no regulator |
| CC sense | PD2 = AIN3 (/CC1), PD3 = AIN4 (/CC2), each via a 10k series R |
| UART debug | TP1 = PD5 (UART1_TX) @115200 8N1, TP2 = GND, TP3 = PD4 spare |
| IR receiver | J8 (DNP) OUT → PA3 = TIM2_CH3, NEC |

**LED count: 32** — this is physical hardware, not a config. Changing it requires a new PCB.
(The rework-pass-2 board is specced to ≤100 LEDs; NUM_LEDS must match whatever
strip is actually fitted, and the brightness caps below assume 32.)

> **Corrected 2026-08-17.** This table said *GPIOC pin 7 / physical pin 17*.
> The PCB has routed `/LED_SIG` from **PC6 (pin 16)** since the `5bc9107`
> baseline — checked against all three board revisions — so the firmware was
> driving a pin that goes nowhere and the strip stayed dark. It also said the
> 5 V arrives *via regulator*; it does not, and that matters: **VDD is raw
> VBUS, so the ADC is ratiometric** and CC thresholds must be fractions of
> full scale rather than absolute volts.

### Rework pass 2 additions (2026-08-17)

- **CC sense / automatic current budget.** Both CC lines are read once a
  second; the higher one is the mated orientation. Codes below 130 mean a
  default-USB source, 130–254 a 1.5 A source, 255+ a 3.0 A source, and the
  brightness ceiling follows at 90 / 160 / 255. The cap is applied inside
  `showLEDs()` rather than in the button handlers so nothing can route around
  it. PD2/PD3 must stay input-floating (CR1 clear) — an internal pull-up
  corrupts the CC divider and misreads even a good 3 A source. The series R
  is **10k** (changed from 22k during the BOM pass): 15.1k source impedance
  settles in ~8.3 time constants, so a single conversion is good to ~0.3 LSB.
- **UART debug on TP1.** 115200 8N1, transmit only (RX would be PD6, which is
  the button). Prints the CC tier at boot and whenever it changes, plus every
  IR code received.
- **IR (NEC) on PA3**, behind `#define ENABLE_IR`. The command bytes are
  placeholders — capture your remote's real codes off TP1 first. Leave option
  byte **AFR1 unprogrammed**, or TIM2_CH3 moves to PD2, which is now AIN3.
- **Flash cost**, measured with sdcc on the sketch translation unit:
  2302 → 3217 bytes. Dropping IR (`ENABLE_IR 0`) gives back 443 of that.
  RAM grew ~13 bytes. On an 8 KB part, check the linker total before flashing.

**Why STM8 for Basic:** unit BOM cost. The STM8S003F3P6 is ~$0.30 at volume vs $3+ for an ESP32 module. Basic is the entry SKU; there's no network stack, no app, no OTA — it's a light that a button turns on and changes.

---

## 3. What It Does (User-Facing)

### 3.1 Power & daily controls

| Gesture | When OFF | When ON |
|---|---|---|
| Single press | Turn on — resume last state if saved this power session, otherwise defaults (white, solid, 50% brightness) | Advance color (13 positions) |
| Double press | — | Advance pattern (7 positions) |
| Triple press | — | Reduce brightness one step (wraps 25% → 100%) |
| Long press (≥600 ms) | — | Turn off + save state to EEPROM |

### 3.2 Persistence

**Within a power session** (unit stays plugged in):
- Long-press-off writes `(color, pattern, brightness)` to EEPROM with magic byte `0xA5`.
- Next single-press-on reads and applies that state, then clears the magic byte.
- Means: **one save/restore per off→on cycle**, not unbounded restore.

**Across power loss** (unplug / outage):
- EEPROM magic byte gets cleared on boot (line [505](stm8_150mm.ino)).
- On boot, `loadState()` finds no magic → uses compile-time defaults: white, solid, `DEFAULT_BRIGHTNESS` (128 = 50%).

**Why clear magic on boot:** users expect unplug-and-move to reset the state. Saved state is a convenience within a single installation session, not a permanent preference.

### 3.3 White auto-dim

When color = white (index 0) **and** pattern = solid, effective brightness is scaled by `WHITE_DIM_FACTOR = 217/255 ≈ 85%`. Every other combination uses brightness verbatim.

See [pattern-dictionary §2.2](../../docs/pattern-dictionary.md#22-white-only-dimming-rule) — this rule propagates to every Layered Logic firmware.

---

## 4. Colors

13 entries, indexed 0..12. Single-press (when on) cycles `currentColor = (currentColor + 1) % NUM_COLORS`.

Full table in [pattern-dictionary §2.1](../../docs/pattern-dictionary.md#21-palette). Canonical RGBs live at [stm8_150mm.ino:41-55](stm8_150mm.ino).

---

## 5. Patterns

7 patterns, indexed 0..6. Double-press (when on) cycles `currentPattern = (currentPattern + 1) % NUM_PATTERNS`.

Each pattern's algorithm, constants, and entry rules are defined in [pattern-dictionary §3](../../docs/pattern-dictionary.md#3-patterns). **This STM8 code is the canonical reference for those definitions** — if the dictionary and the .ino ever disagree, the .ino wins.

Quick index:

| Index | ID | Name | Line |
|---|---|---|---|
| 0 | `solid` | Solid | [221-226](stm8_150mm.ino) |
| 1 | `rainbow` | Rainbow | [228-235](stm8_150mm.ino) |
| 2 | `scanner` | Scanner | [237-258](stm8_150mm.ino) |
| 3 | `spinner` | Spinner | [260-285](stm8_150mm.ino) |
| 4 | `random` | Random Twinkle | [287-309](stm8_150mm.ino) |
| 5 | `breathing` | Breathing | [311-332](stm8_150mm.ino) |
| 6 | `twinkle` | Twinkle | [334-355](stm8_150mm.ino) |

All patterns run at 50 ms tick (`PATTERN_STEP_MS`).

---

## 6. Button State Machine

Implemented in [`checkButton()`](stm8_150mm.ino) (lines 392-483). Five states, timing constants from §14.

```
    ┌────────┐  pressed       ┌──────────────────┐
    │  IDLE  │───────────────►│ DEBOUNCE_PRESS   │
    └────▲───┘                └────┬──────────┬──┘
         │                         │ stable   │ released <20ms
         │                         ▼          │ (bounce)
         │                 ┌───────────────┐  │
         │                 │   PRESSED     │  │
         │                 │ (holding)     │  │
         │                 └───┬───────┬───┘  │
         │                     │       │      │
         │            release  │       │ held │
         │            < 600ms  │       │ ≥600ms
         │                     ▼       ▼      │
         │             ┌───────────────────┐  │
         │             │   DEBOUNCE_REL    │◄─┘
         │             └─────┬────────┬────┘
         │      long-press   │        │  short-click
         │  event fired      │        │  count++
         └───────────────────┘        ▼
                               ┌──────────────┐
                               │  WAIT_MULTI  │ 200ms window
                               └──┬────────┬──┘
                   another press  │        │ timeout
                                  │        ▼
                                  │  fire (single/double/
                                  │        triple)
                                  ▼
                         back to DEBOUNCE_PRESS
```

**Click counting:** `clickCount` accumulates across a single multi-click sequence. When `clickCount == 3`, `BTN_TRIPLE` fires immediately (no need to wait for timeout). Single and double wait for the 200 ms `DOUBLE_CLICK_MS` window to expire to disambiguate.

**Long-press firing:** fires *during* the hold, not on release (line 430). User gets immediate feedback — light turns off while still holding the button.

---

## 7. WS2812B Driver

Bit-banged on GPIOC7 with NOP-counted timing. 16 MHz clock, so 1 NOP = 62.5 ns.

| Bit | High time | Low time |
|---|---|---|
| `1` | 12 NOPs (~750 ns, target 800 ns) | 4 NOPs (~250 ns, target 450 ns) |
| `0` | 4 NOPs (~250 ns, target 400 ns) | 12 NOPs (~750 ns, target 850 ns) |

Tolerances on WS2812B are loose enough (~±150 ns) that this works reliably. [stm8_150mm.ino:161-177](stm8_150mm.ino).

**Interrupts disabled during frame send** (line 183) — any interrupt preemption longer than ~5 μs would desynchronize the strip. Re-enabled immediately after, followed by a ≥50 μs latch delay (line 196).

**Brightness application is per-pixel in `showLEDs()`** (line 182-194). Pattern functions write raw RGB to `ledBuffer`; the driver scales by `currentBrightness` (and by `WHITE_DIM_FACTOR` when applicable) just before bit-banging. Keeps pattern code clean.

---

## 8. EEPROM Layout

128 bytes at `0x4000–0x407F`. Only 4 bytes used:

| Offset | Field | Values |
|---|---|---|
| 0 | Magic | `0xA5` (valid) / anything else (invalid, use defaults) |
| 1 | Color index | 0..12 |
| 2 | Pattern index | 0..6 |
| 3 | Brightness | 25..255 |

Writes happen **only on long-press-off** (1 write per user turn-off). EEPROM endurance is 100k cycles — even at 100 off-cycles per day, that's >2.7 years before wear-out.

Unlock sequence: DUKR = 0xAE then 0x56, per STM8 reference manual. See [stm8_150mm.ino:105-120](stm8_150mm.ino).

---

## 9. RNG

16-bit LFSR (lines 93-102), taps `0xB400`, seeded `12345`. Returns low 8 bits. Used by `pattern_random` and `pattern_twinkle`.

Not cryptographically secure. Not seeded from hardware noise. This is fine — statistical sparkle distribution is the only requirement.

---

## 10. Timing Budget

One loop iteration:

| Work | Time |
|---|---|
| `checkButton()` | <10 μs |
| `updatePattern()` (render to ledBuffer) | <100 μs (varies by pattern; Spinner does an extra clear) |
| `showLEDs()` (bit-bang 32 × 24 bits + latch) | ~960 μs data + 156 μs latch ≈ 1.1 ms |
| `delay_ms_soft(50)` | 50.0 ms (blocking soft delay) |
| **Total** | ~51 ms, so effective tick ~19.6 Hz |

The ~20 Hz tick rate is the reference cadence for every Layered Logic firmware ([pattern-dictionary §2.4](../../docs/pattern-dictionary.md#24-tick-rate)).

---

## 11. What's Deliberately Absent

Called out because someone might wonder why:

- **No network.** No Wi-Fi, no BLE, no app integration. Basic SKU is offline by definition.
- **No OTA.** Flash via SWIM/ST-Link only. No field update path; this is acceptable because shipping units have mature firmware and the SKU is positioned as "the light that just works."
- **No button-combo gestures.** Single, double, triple, long — nothing more. Keeps state machine simple and users non-confused.
- **No "pattern preview" mode.** Cycle lands on a pattern and stays there.
- **No factory-reset gesture.** EEPROM clears on unplug anyway; nothing else to reset.

---

## 12. Build

```
arduino-cli compile --fqbn sduino:stm8:stm8sblue stm8_150mm.ino
```

Toolchain: [sduino](https://github.com/tenbaht/sduino) — Arduino-compatible layer over SDCC targeting STM8. Output: Intel HEX flashed via ST-Link V2 using `stm8flash -c stlinkv2 -p stm8s003f3 -w build/.../*.ihx`.

Build artifacts live in [build/](build/) (gitignored beyond this spec — no binaries committed).

---

## 13. Known Issues / Future Work

None critical. Possible improvements (none scheduled):

- **Breathing wave shape:** triangular rather than sinusoidal. Sin lookup table would eat ~256 bytes of flash; current look is acceptable.
- **Spinner wraparound on NUM_LEDS % 4 ≠ 0:** would break if ring size ever changes. 32 LEDs is hardware-fixed; non-issue in practice.
- **Single button only.** No Wi-Fi → no need for a pairing button. If a future Basic variant ever needs one (unlikely), this would require a PCB change.
- **`delay_ms_soft()` is blocking.** Fine for single-purpose firmware; would be first thing to rip out if we ever added peripherals.

---

## 14. Constants Summary

| Constant | Value | Meaning |
|---|---|---|
| `NUM_LEDS` | 32 | Physical LED count |
| `NUM_COLORS` | 13 | Palette entries (see pattern-dictionary §2.1) |
| `NUM_PATTERNS` | 7 | Pattern entries (see pattern-dictionary §3) |
| `DEFAULT_BRIGHTNESS` | 128 | ~50% — default on boot |
| `MIN_BRIGHTNESS` | 25 | ~10% — brightness floor before wrap |
| `BRIGHTNESS_STEP` | 25 | Triple-press decrement |
| `DEBOUNCE_MS` | 20 | Hardware cap handles most bounce |
| `DOUBLE_CLICK_MS` | 200 | Max gap between clicks in a multi-click |
| `LONG_PRESS_MS` | 600 | Hold threshold for long-press |
| `PATTERN_STEP_MS` | 50 | Tick cadence (20 Hz) |
| `WHITE_DIM_FACTOR` | 217 | 85% — applied to solid-on-white only |
| `EEPROM_MAGIC_VALUE` | `0xA5` | State-validity marker |

Values from [stm8_150mm.ino:4-30](stm8_150mm.ino).

---

## Related

- [stm8_150mm.ino](stm8_150mm.ino) — the code this doc describes
- [docs/pattern-dictionary.md](../../docs/pattern-dictionary.md) — visual behavior spec (cross-firmware)
- [docs/firmware-architecture-scoping.md](../../docs/firmware-architecture-scoping.md) — how STM8 Basic sits in the product line
- [docs/button-interface.md](../../docs/button-interface.md) — UX contract (STM8 = single-button subset)
- [docs/bom-breakdown-basic-6x6.md](../../docs/bom-breakdown-basic-6x6.md) — cost model for this SKU
