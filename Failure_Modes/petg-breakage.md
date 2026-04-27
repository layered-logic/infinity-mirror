---
title: "PETG Parts — Breakage"
type: failure-mode
component: petg-enclosure
severity: low
probability: low
user-repairable: yes
tags: [failure-mode, petg, 3d-print, enclosure, spacer]
---

# PETG Parts — Breakage

A 3D-printed PETG component (LED spacer or electronics enclosure) has cracked, broken, or deformed.

---

## Symptoms

- The LED spacer is cracked or broken — LED strip sags or shifts position, causing uneven light distribution in the infinity tunnel.
- The electronics enclosure is cracked — controller board or wiring is exposed or loose.
- A snap-fit tab has broken off — the part no longer holds friction-fit.
- Visible deformation (warping, bowing) of a printed part.

## Likely Causes

- **Impact or drop** — PETG is tough but can crack under sharp point loads, especially at thin-wall sections or layer boundaries.
- **Overtightening a screw** — if a screw is driven into a printed boss too hard, the boss splits along layer lines.
- **Heat exposure** — PETG glass transition is ~80°C. Placement near a heat source (radiator, south-facing window in direct sun, enclosed shelf with no airflow) can soften and deform the parts.
- **Layer adhesion failure** — a 3D printing quality issue where weak inter-layer bonding causes delamination under stress. Should be caught at production QC but can manifest as a latent defect.
- **UV degradation** — PETG is reasonably UV-stable, but years of direct sunlight can embrittle it.

## Probability

**Low.** The PETG parts are internal — protected by the wood frame and back panel from most impacts. They don't bear structural load. The LED spacer's job is to hold the strip in position; the enclosure's job is to cradle the controller board. Neither is under continuous mechanical stress.

## Repair Path

This is one of the simplest repairs — reprint the part.

1. Unplug the mirror from power.
2. Remove the back panel screws (Phillips-head).
3. Remove the broken part — LED spacer lifts out of the frame; enclosure is friction-fit or screwed in place.
4. Drop in the replacement part.
5. Reassemble.

### Getting the STL file

Per the [Right-to-Repair Philosophy §3](../docs/right-to-repair-philosophy.md#3-what-i-commit-to): STL files are supplied on request to any owner reporting a specific repair need. Contact support with your order number or serial, describe which part broke, and the file will be sent.

### If you don't have a 3D printer

Contact support for a pre-printed replacement part (cost of part + shipping). Local makerspaces and library 3D printing services are also an option — the parts are standard PETG with no special requirements.

### Tools Required

- Phillips-head screwdriver

### Parts Required

- **PETG filament** — if you're printing yourself. Any standard PETG filament works. The parts total ~44g per mirror. Cost: under $1 in material.
- **Pre-printed replacement** — available from support if you can't print.

### Estimated Difficulty

Easy — drop-in replacement, no soldering or wiring.

### Estimated Time

5–10 minutes (swap). 1–3 hours (printing, if you're doing it yourself — mostly unattended print time).

## When to Contact Support

- To request the STL file for the broken part.
- If you need a pre-printed replacement shipped to you.
- If the breakage caused secondary damage (e.g., broken spacer let the LED strip contact the acrylic and scratch it).

## Design Mitigations

- **Friction-fit or screwed — never glued** — parts remove and replace without damage to surrounding components.
- **PETG over PLA** — PETG was chosen specifically for its higher heat resistance (~80°C vs. ~60°C glass transition), better impact toughness, and superior UV stability compared to PLA.
- **STL-on-request commitment** — any owner can get the file to reprint. Not published by default (per philosophy doc), but never denied for a repair.
- **Standard material** — no specialty filament required. Any PETG filament from any brand works.

---

## Related

- [Frame — Physical Damage](frame-physical-damage.md) — if the frame is also damaged
- [LED Strip — Dead Pixel(s)](led-dead-pixel.md) — if a broken spacer shifted the strip and caused LED issues
- [Right-to-Repair Philosophy](../docs/right-to-repair-philosophy.md) — §3 STL file commitment
