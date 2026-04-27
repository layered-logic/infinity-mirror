---
title: "Button — Primary Unresponsive"
type: failure-mode
component: button
severity: medium
probability: very-low
user-repairable: partial
tags: [failure-mode, button, input, gpio]
---

# Button — Primary Unresponsive

The main exposed button stops registering presses — single, double, triple, and hold gestures all produce no response.

---

## Symptoms

- Pressing the primary button does nothing — no color change, no pattern change, no on/off.
- The mirror continues running its current pattern and color normally (if it was on) — just can't be controlled.
- (ESP32 variant) The mirror still responds to app commands over WebSocket — only the physical button is dead.
- The recessed button still works (if applicable).

## Likely Causes

- **Button switch failure** — the tactile switch has worn out or its internal contact has failed. Typical tactile switches are rated for 100,000–1,000,000 cycles. At normal use (10–20 presses/day), this is years to decades of life.
- **Solder joint failure at the button** — the button's leads have cracked loose from the PCB. See [Wiring — Solder Joint Failure](wiring-solder-joint-failure.md).
- **GPIO failure on the MCU** — the input pin (GPIO9) has been damaged by ESD. Rare.
- **Firmware bug** — the button task or ISR has crashed. A power cycle or factory reset may resolve this. See [Firmware Corruption](controller-firmware-corruption.md).
- **Mechanical obstruction** — something is preventing the button from actuating (debris in the button well, enclosure misalignment after a repair).

## Probability

**Very low.** Tactile switches are among the most reliable components in the BOM. The primary button on the v1.0 mirror is used moderately — not a gaming controller or TV remote level of abuse. Mechanical failure before year 5 would be unusual.

## Repair Path

### Step 1: Rule out firmware (no tools needed)

1. Unplug the mirror, wait 5 seconds, plug back in. This power-cycles the controller and restarts the button task.
2. Try all gestures (single, double, triple, hold).
3. If it works now, it was a firmware glitch — monitor for recurrence. If it happens again, factory reset (recessed button 10s hold).

### Step 2: Physical inspection

1. Unplug from power.
2. Open the back panel (Phillips-head screws).
3. Inspect the button:
   - Is the button physically intact? Press it by hand — you should feel a tactile click.
   - Check the solder joints at the button's leads.
   - Check that the button can actuate freely through the enclosure hole.

### Step 3: Button replacement (if the switch is dead)

1. Desolder the failed button from the PCB (2 or 4 leads depending on switch type).
2. Solder in a replacement tactile switch — generic 6mm × 6mm through-hole tactile momentary switch, available from any electronics supplier.
3. Reassemble and test.

### Tools Required

- Phillips-head screwdriver
- Soldering iron + solder (for replacement)
- (Optional) Multimeter to test button continuity

### Parts Required

- **6mm × 6mm tactile momentary switch (through-hole, SPST, normally open)** — commodity part, pennies each. Available from Amazon, Adafruit, DigiKey, Mouser, or any electronics supplier.

### Estimated Difficulty

Easy (firmware reset) to Moderate (button replacement requires desoldering and resoldering).

### Estimated Time

1 minute (power cycle) to 20 minutes (physical button replacement).

## When to Contact Support

- If you're not comfortable with soldering.
- If the power cycle and factory reset don't help and the button appears physically fine — may be a GPIO issue on the MCU.
- If you'd like a replacement board with a working button pre-soldered.

## Design Mitigations

- **Internal pullup** — no external resistor needed. The GPIO's built-in pullup keeps the input clean, reducing false-trigger or stuck-low issues.
- **Firmware debouncing** — 20ms debounce window filters contact bounce, protecting the gesture state machine from noisy input.
- **App as fallback** — on ESP32 variants, all button functions are also accessible via the app/webapp. A dead button degrades the experience but doesn't brick the product.

---

## Related

- [Button — Recessed Failure](button-recessed-failure.md) — if the recessed button is the one that's dead
- [Wiring — Solder Joint Failure](wiring-solder-joint-failure.md) — if the issue is at the joint, not the switch
- [Controller — Firmware Corruption](controller-firmware-corruption.md) — if a firmware glitch is killing the button task
