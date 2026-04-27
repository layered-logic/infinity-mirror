---
title: "PSU — No Output"
type: failure-mode
component: power-supply
severity: critical
probability: low
user-repairable: yes
tags: [failure-mode, psu, power]
---

# PSU — No Output

The 5V power supply brick has stopped providing power entirely. The mirror is completely dark and unresponsive.

---

## Symptoms

- Mirror plugged in but completely dark — no LEDs, no controller activity.
- No warmth from the PSU brick (it's normally slightly warm during operation).
- Pressing any button produces no response at all.
- Unplugging and replugging does not help.

## Likely Causes

- **Internal component failure** — electrolytic capacitor aging, transformer failure, or regulator burnout. Common in commodity switch-mode PSUs after several years.
- **Surge damage** — a power spike (lightning, grid surge) destroyed the PSU's input stage.
- **Cord damage** — the AC cord or DC barrel cable was pinched, cut, or stressed at a bend point.
- **Thermal shutdown** — some PSUs have thermal protection. If the PSU is in an enclosed space with no airflow, it may shut down. This is recoverable — let it cool and try again.

## Probability

**Low.** The PSU is significantly oversized for the load — a 25W brick driving a ~2.8W max load (11% utilization per the sustainability audit). Running at 11% of rated capacity puts minimal stress on internal components, extending lifespan well beyond typical. That said, commodity PSUs have the widest quality variance of any component in the BOM.

## Repair Path

This is the simplest hardware repair — it's an external plug-in component.

1. Unplug the dead PSU from the wall and from the mirror.
2. Plug in a replacement.
3. That's it.

### Tools Required

None.

### Parts Required

- **5V DC power supply with barrel jack connector** — match the barrel size of the original (typically 5.5mm × 2.1mm). 5V at 2A or higher is sufficient (the mirror draws under 1A at full white). Available from Amazon, AliExpress, Adafruit, or any electronics supplier. Cost: ~$3–$8.

### Estimated Difficulty

Trivial — plug swap, no tools, no opening the mirror.

### Estimated Time

Under 1 minute.

## When to Contact Support

- If the replacement PSU also doesn't power the mirror — the issue is inside the mirror (barrel jack, wiring, or controller). See [Barrel Jack — Intermittent Connection](barrel-jack-intermittent.md) or [Controller — Hardware Failure](controller-hardware-failure.md).
- If you're unsure which PSU specs to match (voltage, amperage, barrel size).

## Design Mitigations

- **Standard barrel jack connector** — not a proprietary plug. Any 5V barrel-jack PSU works.
- **External PSU** — the power supply is completely outside the mirror enclosure, so replacing it never requires opening the product.
- **PSU oversizing** — running at 11% of rated load reduces thermal stress on the PSU's internals. (The sustainability audit flags this as an efficiency concern for v2 right-sizing, but it's a reliability benefit.)

---

## Related

- [PSU — Voltage Instability](psu-voltage-instability.md) — partial failure (flickering) vs. total failure
- [Barrel Jack — Intermittent Connection](barrel-jack-intermittent.md) — if the PSU is fine but the connector is loose
- [LED Strip — Total Failure](led-strip-total-failure.md) — if LEDs are dark but the controller has power
