---
title: "Acrylic Panel — Crack or Deep Scratch"
type: failure-mode
component: acrylic-panel
severity: medium
probability: low
user-repairable: yes
tags: [failure-mode, acrylic, mirror, physical-damage]
---

# Acrylic Panel — Crack or Deep Scratch

One of the acrylic mirror panels (1-way or 2-way) is cracked, chipped, or deeply scratched from impact or mishandling.

---

## Symptoms

- Visible crack line in the mirror surface, catching light or breaking the infinity effect.
- A deep scratch that disrupts the reflective coating and creates a visible mark.
- A chip at the edge or corner, typically from a drop or impact.
- The infinity effect looks "broken" — light leaks through the crack instead of reflecting.

## Likely Causes

- **Drop impact** — acrylic is more impact-resistant than glass but still cracks under point loads or edge impacts.
- **Flex stress** — if the mirror was packed, shipped, or stored with pressure on the face, the panel can crack.
- **Thermal shock** — unlikely in normal home use, but a very hot object placed on the mirror could crack the acrylic.
- **Over-tightening during assembly or repair** — the frame screws can stress the panel if tightened beyond snug.

## Probability

**Low.** Acrylic is more forgiving than glass — it flexes before cracking and doesn't shatter. The panels are mounted inside a wood frame that absorbs most edge impacts. Cracking requires a significant event (drop, crush, point impact).

## Repair Path

The panels are mounted in the frame — not glued, not permanently bonded. Replacement requires opening the mirror and swapping the damaged panel.

1. Unplug the mirror from power.
2. Remove the back panel screws (Phillips-head).
3. Remove the back panel, then carefully lift the LED spacer assembly to access the panels.
4. Slide the damaged panel out of the frame channel.
5. Slide the replacement panel in (match orientation — the reflective coating faces inward toward the LED cavity).
6. Reassemble: LED spacer, back panel, screws.

### Which panel?

| Panel | Position | Description |
|---|---|---|
| **2-way mirror** | Front (viewer-facing) | The see-through half-mirror you look at. Partially reflective, partially transparent: it reflects most light back into the cavity while letting enough through for the infinity tunnel to be visible. |
| **1-way mirror** | Back (behind the LEDs) | The full mirror behind the LED cavity. Fully reflective: it bounces all the light forward toward the front panel. |

### Tools Required

- Phillips-head screwdriver

### Parts Required

- **1-way mirror acrylic, 150mm × 150mm × 3.1mm** — Makerstock. ~$2.50/panel (cut from $9.95 12×12" sheet, 4 panels per sheet).
- **2-way mirror acrylic, 150mm × 150mm × 3.1mm** — TT Plastic Land. ~$5.00/panel (cut from $20 12×12" sheet, 4 panels per sheet).
- Panels need to be cut to size. If you have a laser cutter, use the provided cut file. Otherwise, contact support for a pre-cut panel.

### Estimated Difficulty

Moderate — requires careful handling to avoid scratching the new panel or damaging the LED strip during disassembly.

### Estimated Time

20–30 minutes.

## When to Contact Support

- If you need a pre-cut replacement panel (no laser cutter access).
- If the crack caused secondary damage to the LED strip or frame.
- If you're unsure which panel (1-way vs. 2-way) needs replacement.

## Design Mitigations

- **Panels are mounted, not glued** — replacement is non-destructive.
- **Wood frame absorbs impact** — the frame protects the panel edges from direct impact.
- **Acrylic over glass** — chosen for impact resistance, weight, and ease of laser cutting. The tradeoff is that acrylic scratches more easily than glass (see [Coating Degradation](acrylic-coating-degradation.md)).
- **Packaging design** — shipping box includes foam or corrugated spacers to prevent flex pressure on the panels.

---

## Related

- [Acrylic Panel — Coating Degradation](acrylic-coating-degradation.md) — gradual coating issues vs. impact damage
- [Frame — Physical Damage](frame-physical-damage.md) — if the frame is also damaged
- [Right-to-Repair Philosophy](../docs/right-to-repair-philosophy.md) — §4 acrylic EOL limitation acknowledged
