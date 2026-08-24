---
title: STM8 Basic Firmware — board rev 2
type: engineering
status: RUNNING ON HARDWARE — flashed and confirmed 2026-08-24
---

# Layered Logic Basic — STM8 firmware, board rev 2

Firmware for the rev-2 STM8 controller (`D:\PCBs\Infinity_Mirror_STM8`,
25 × 47 mm, JLC order Y12). It replaces
[`../STM8/stm8_150mm.ino`](../STM8/stm8_150mm.ino), which stays in the tree as
the reference for the shipped 6" × 6" units.

Behaviour is unchanged product contract — 13 colours, 7 patterns,
single/double/triple/long on one button, the white auto-dim rule, EEPROM state
that survives an off→on cycle but not a power cycle. See
[`../STM8/spec.md`](../STM8/spec.md) and
[`../../docs/pattern-dictionary.md`](../../docs/pattern-dictionary.md).

What is new is everything under it.

## Why this is not an .ino any more

The old sketch built through **sduino**, an Arduino layer over SDCC. Two
things pushed this off that:

1. **sduino owns `main()`.** SDCC emits an interrupt vector-table entry only
   for handlers declared in the translation unit containing `main()`. Under
   sduino that is the core's `main.c`, which pulls in `stm8s_it.h`
   transitively through `Arduino.h` — and in that header
   `INTERRUPT_HANDLER(TIM2_CAP_COM_IRQHandler, 14)` is **commented out for the
   STM8S003**. So no vector-14 entry was ever declared, and a TIM2 capture ISR
   written in the sketch would have compiled, linked, and **never been
   called**. (It also would not have matched by name: the sketch spelled it
   `TIM2_CAPCOM_IRQHandler`.) Here `main()` and the handlers are in one file,
   and `build.sh` fails the build if the vector does not land.
2. **The Pi rig needs a scriptable build.** `sdcc` + `stm8flash` is two apt
   packages and a shell script. `arduino-cli` with sduino 0.5.0 is currently
   broken on the Windows box anyway (an empty sketch fails identically).

`vendor/stm8s.h` is ST's SPL header, byte-identical to the copy shipped with
sduino, included **for register definitions only** — no SPL `.c` file is
compiled or linked. `src/stm8s_conf.h` is an empty stub that satisfies the
header's own include without editing the vendored copy.

## Build

```bash
./build.sh
```

Windows (Git Bash): finds SDCC inside the sduino package automatically.
Raspberry Pi / Debian: `sudo apt install sdcc stm8flash`, then `make` or
`./build.sh` — both do the same thing.

Options, on either:

```bash
LEDS=60 ./build.sh        # strip length (make LEDS=60)
IR=1 ./build.sh           # NEC decoder — only once J8 has a receiver in it
UART=0 ./build.sh         # drop the debug trace
BRINGUP=1 ./build.sh      # bring-up image, ignores the button
```

**Defaults are `LEDS=32`, `IR=0`** — the 150 mm strip, and no IR receiver,
which is what the boards on the bench actually are. `IR=1` costs 421 bytes
and adds the only interrupt in the design, so it stays off until J8 is
populated.

Current sizes on an 8 KB part, SDCC 3.9.1:

| config | flash | static RAM |
|---|---|---|
| **default** (32 LEDs, UART, no IR) | 3517 / 8192 (43 %) | 121 / 1024 |
| `IR=1` | 3938 (48 %) | 133 |
| `BRINGUP=1` | 3769 (46 %) | 121 |
| `LEDS=100 IR=1` | 3903 (48 %) | 337 |
| `LEDS=60 UART=0` | 3111 (38 %) | 205 |

`build.sh` fails the build if either budget is exceeded, and — when IR is
enabled — if `TIM2_CAP_COM_IRQHandler` is missing from the map, which is the
exact failure sduino could not have caught.

## Test

```bash
./test.sh          # or: make test
```

Compiles `src/button_logic.h` — the real header, not a copy — with the host
compiler and drives the gesture machine a millisecond at a time. Ten cases
covering the four gestures, contact chatter, release bounce, and coarse
service granularity.

It exists because of one of them. Delete the `!wasLong` guard in
`button_logic.h` and `5 s hold fires LONG exactly once` reports **2190**
long-press events for a single five-second hold — each of which writes four
EEPROM bytes. That is the old firmware's behaviour, throttled only by its
slow loop to roughly fifty events per hold.

## Flash

Two programmers, two paths. Both write **PROGRAM MEMORY only**.

### This PC — ST-LINK/V2 through ST Visual Programmer

Nothing needs recompiling in the Arduino IDE, or anywhere else. `./build.sh`
already produced a finished image; `build/ll_basic_rev2.hex` is plain Intel
HEX spanning 0x8000–0x8DBC, which STVP loads directly. (`.ihx` is the same
bytes, but STVP's file dialog filters on the extension and will not offer
it — that is the only reason both names exist.)

Scripted, with the safety flags baked in:

```bash
./flash_stvp.sh
```

Or in the GUI: pick the device, `File ▸ Open` the `.hex` into the **PROGRAM
MEMORY** tab, then `Program ▸ Current tab`.

> **Do not program the OPTION BYTE tab.** Option bit `AFR1` remaps
> `TIM2_CH3` from PA3 to PD2 — and PD2 is the CC-sense ADC input on this
> board — so writing option bytes blind can take out the current budget (and
> IR later). STVP programs option bytes by *default* from whatever
> `Option.hex` it last held, which is why `flash_stvp.sh` passes
> `-no_progOption`. Leave the **DATA MEMORY** tab alone too: that is the
> EEPROM the firmware uses for saved state.

If `-Device=STM8S003F3` is rejected, read the exact string off STVP's device
dropdown — its device database is an obfuscated `.cnf`, so it cannot be
grepped.

### The Pi rig — stm8flash

```bash
./flash.sh          # PROG=stlinkv21 for a V2-1
```

`sudo apt install sdcc stm8flash` and the whole build-and-flash is two
commands. Note `stm8flash` speaks libusb, so **on Windows** it may want the
ST-Link rebound to WinUSB with Zadig — which would stop STVP working. Don't:
use STVP here and keep `stm8flash` for the Pi.

### SWIM pads

Relative geometry is frozen because the programming jig mates to it:

```
    J2 GND  ----  J4 NRST
    J3 SWIM ----  J5 +5V        columns 3.75 mm, rows 2.50 mm
```

Powering the board from the jig's +5 V pad is fine, but with no USB-C cable
attached both CC lines read ~0, so the firmware reports tier 0 and holds the
dim default cap. That is correct, not a fault — test the tiers with a real
charger.

## Bringing up a fresh board

```bash
BRINGUP=1 ./build.sh && ./flash_stvp.sh
```

No button and no remote needed. It loops: red, green, blue, dim white, then a
single white pixel walking the strip. In order that proves the MCU runs, PC6
reaches J7, the strip's colour order, and its length — a wrong `LEDS=` shows
up immediately as dark tail pixels or a chase that wraps early. The CC tier
and the derived brightness caps are printed on TP1 each cycle.

Then rebuild without `BRINGUP` and reflash.

## Debug trace (TP1)

115 200 8N1, transmit only. TP1 = PD5, TP2 = GND, TP3 = PD4 spare. RX is
impossible on this board: UART1_RX is physically PD6, which is the Mode
button.

```
LL Basic rev2 | leds=32 caps=92/159/255 cc=0
CC tier=2 cap=255
IR addr=0x00 cmd=0x45
```

### Teaching it a remote

The `IR_CMD_*` values in `src/config.h` are placeholders. Fit J8, point a
terminal at TP1, press each button on the remote, and copy the `cmd=0x..`
values in. Codes that do not match anything are printed and otherwise
ignored, so an unknown remote does nothing rather than firing a wrong action.

## What changed from the 150 mm firmware

Behavioural, in the order they matter:

1. **Debounce existed only on paper.** Every button threshold was
   `CONSTANT_MS / PATTERN_STEP_MS` in integer maths against a loop that
   counted busy-wait iterations — and `DEBOUNCE_MS / PATTERN_STEP_MS` is
   `20/50` = **0**, so both debounce comparisons were always true. Timing now
   comes from TIM2's free-running counter at 8 µs/tick and the constants mean
   milliseconds. The counter keeps running while interrupts are masked, which
   is why it is polled rather than serviced by a tick interrupt: the WS2812
   frame masks for ~1.6 ms at 32 LEDs.
2. **A long press fired repeatedly for as long as it was held.** After
   firing, the machine went to `DEBOUNCE_REL`, saw the button still down, and
   returned to `PRESSED` with the hold timer untouched — so it re-fired every
   couple of loop iterations, and each `BTN_LONG` does a `saveState()` of four
   EEPROM bytes. A one-second hold was tens of unnecessary EEPROM writes on a
   100 k-cycle part. Fixed with a latch that clears on release.
3. **Long-press while already off** used to save state and clear the strip.
   Now it is a no-op.
4. **fADC was out of spec.** `fmaster/2` = 8 MHz; the STM8S003F3 datasheet
   caps fADC at 4 MHz below VDDA 4.5 V, and VDD here is raw VBUS. Now
   `fmaster/6` = 2.67 MHz, which also widens the sample window to ~1.1 µs
   against the 15 kΩ CC source impedance.
5. **An EEPROM write on every boot.** `setup()` cleared the magic byte
   unconditionally. `eeprom_write()` now skips when the byte already holds
   the value.
6. **IR codes were printed in decimal behind an `0x` prefix** — a good way to
   fill in the wrong `IR_CMD_*` constants. Hex is hex now, and the address
   byte is printed too.
7. **Current caps are derived from `NUM_LEDS`** instead of three constants
   hardcoded for a 32-LED strip. At 32 the formula reproduces the shipped
   90/160/255 as 92/159/255. The top tier is **2400 mA**, per REWORK_SPEC
   decision 9 — not 3000.
8. **Floating pins tied off** — PA3, PD4, PB4/PB5 and the six unconnected
   pins. PD1 (SWIM) and PD2/PD3 (CC sense, which must stay input-floating)
   are excluded by construction; see the comments in `src/board.h`.
9. `scale8()` is a multiply instead of a 16-bit divide, so the per-pixel work
   inside the interrupts-off window dropped from ~19 µs to about 1 µs.

Deliberately unchanged: the WS2812 NOP timing, and every pattern algorithm.
The bit-bang is field-proven on shipped units and retuning it belongs behind
a scope on J7 pin 2, not behind a code review.

## Verified on hardware — 2026-08-24

Flashed to a rev-2 board with an ST-LINK/V2 through ST Visual Programmer on
the Windows box. Bring-up image first, then the normal firmware. Both ran.

That closes the things that mattered most: the board really is `/LED_SIG` on
**PC6** (the old firmware's PC7 would have left the strip dark), the
bare-metal SDCC image boots and keeps time, and STVP takes the `.hex`
directly with no Arduino toolchain anywhere in the path.

Still unconfirmed, because they need specific conditions rather than a
power-up:

- **CC tier response.** Confirm `cap=` moves on TP1 when the board is
  powered from a 1.5 A and then a 3 A USB-C charger rather than the jig's
  +5 V pad. Tier 0 with no cable attached is correct behaviour.
- **EEPROM save/restore.** Long-press off, single-press on, and check the
  colour/pattern/brightness came back; then unplug and check it did *not*.
- **IR.** Never built into a flashed image (`IR=0` by default) — J8 has no
  receiver in any board yet, and `IR_CMD_*` are still placeholders.
- **`NUM_LEDS` is 32**, confirmed with Bill 2026-08-24.

## Layout

```
src/main.c         the firmware
src/board.h        pin map, transcribed from the board netlist
src/config.h       build-time options and the derived current budget
src/button_logic.h the gesture machine as pure logic, host-testable
src/stm8s_it.h     ISR declarations -- read the comment, it is the whole
                   reason the old build's IR could not work
src/stm8s_conf.h   empty stub so vendor/stm8s.h stays ST's original
vendor/stm8s.h     ST SPL register definitions (headers only)
build.sh           build anywhere, no make needed
test.sh            host tests
tools/test_button_logic.c
flash_stvp.sh      ST Visual Programmer, this PC
flash.sh           stm8flash, the Pi rig
Makefile           same build, for the Pi
```
