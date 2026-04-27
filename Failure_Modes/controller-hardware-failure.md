---
title: "Controller — Hardware Failure"
type: failure-mode
component: controller-pcb
severity: high
probability: very-low
user-repairable: partial
tags: [failure-mode, controller, pcb, esp32, stm8]
---

# Controller — Hardware Failure

The MCU or controller PCB has failed — the board is non-functional and the mirror doesn't respond to any input.

---

## Symptoms

- Mirror has power (PSU is warm, barrel jack seated) but no LEDs light and no button responds.
- **Distinguishing from PSU failure:** if you have a multimeter, measure 5V at the barrel jack input on the board. If 5V is present but the board is unresponsive, the controller is the problem.
- Controller was previously working — this isn't a setup/provisioning issue.
- (ESP32 Pro/Pro+ variant) The device disappears from the app and from the network entirely.

## Likely Causes

- **ESD damage** — static discharge to an exposed GPIO or connector. Most likely during handling or repair of other components.
- **Power surge** — a spike that the PSU passed through to the board. The bulk capacitor provides some protection but can't absorb everything.
- **Manufacturing defect** — cold solder joint on the MCU, marginal crystal/oscillator, or defective IC. Pre-ship QC at Stations S14–S15 catches most of these, but latent defects can appear weeks to months after initial power-on.
- **Water or condensation damage** — if the mirror was placed in a humid environment (bathroom) without adequate protection.
- **Physical damage** — drop impact cracking the PCB or breaking a trace.

## Probability

**Very low.** The STM8 and ESP32 are industrial-grade ICs designed for reliability. Running within spec with conservative power draw, these controllers should outlast every other component. Firmware flash and function test at Stations S14–S15 catch manufacturing defects before shipping.

## Repair Path

The controller is a board-level replacement — you swap the entire PCB module.

1. Unplug the mirror from power.
2. Remove the back panel screws (Phillips-head).
3. Disconnect the LED strip from the controller (3-pin connection).
4. Disconnect the barrel jack input wiring.
5. Remove the controller from its mounting (friction-fit in PETG enclosure or screwed depending on revision).
6. Seat the replacement controller in the same position.
7. Reconnect the LED strip (match 5V, GND, Data).
8. Reconnect barrel jack input.
9. Reassemble the back panel and test.

**Note:** A replacement controller board needs firmware flashed before it will operate. Pre-flashed replacement boards are available through support. If you have a bare board, see "When to Contact Support" below.

### Tools Required

- Phillips-head screwdriver
- (Optional) Soldering iron if connections are soldered rather than connectorized

### Parts Required

- **Replacement controller PCB (pre-flashed)** — contact Layered Logic support. The PCB is custom but inexpensive (~$1–$4 at production cost). A pre-flashed board is the simplest path.
- For STM8 Basic variant: STM8S003F3P6 board per the published schematic.
- For ESP32 Pro variant: ESP32-C3-MINI-1 board per the published schematic.

### Estimated Difficulty

Moderate — straightforward physically, but requires a pre-flashed replacement board.

### Estimated Time

15–30 minutes for the physical swap.

## When to Contact Support

- **Always recommended for this failure mode.** The replacement board needs firmware, and the firmware is a closed binary. Contact support for a pre-flashed replacement board (cost of board + shipping).
- The [reflash service commitment](../docs/right-to-repair-philosophy.md#3-what-i-commit-to) means you can also send your existing board in for diagnosis and reflash at cost of shipping.
- Published schematics allow you to verify the board-level issue, but the firmware must come from Layered Logic.

## Design Mitigations

- **Published schematics** for every shipped PCB revision — the hardware is transparent even though the firmware is closed.
- **Versioned PCB silkscreen** — the revision letter is visible when the enclosure is opened, so you and support are always looking at the same schematic.
- **Firmware flash + function test** at Station S14–S15 before shipping catches manufacturing defects.
- **Bulk capacitor** on the power input provides basic surge protection.
- **Reflash service** — if the controller is physically fine but firmware-corrupted, it can be reflashed. See [Firmware Corruption](controller-firmware-corruption.md).

---

## Related

- [Controller — Firmware Corruption](controller-firmware-corruption.md) — if the hardware is fine but firmware is broken
- [Controller — Wi-Fi Failure](controller-wifi-failure.md) — network-specific issues (ESP32 only)
- [LED Strip — Total Failure](led-strip-total-failure.md) — if LEDs are dark, rule out the strip before blaming the controller
- [Right-to-Repair Philosophy](../docs/right-to-repair-philosophy.md) — §3 schematic publication, §4 closed-binary rationale
