---
title: Failure Mode Inventory
type: hcde-artifact
phase: 2
week: 4
date: 2026-04-27
status: complete
tags: [failure-mode, repair, hcde, reliability]
---

# Failure Mode Inventory — Infinity Mirror v1.0

Enumerated failure modes for every user-serviceable component in the v1.0 infinity mirror. Each file documents symptoms, causes, probability, repair path, parts sourcing, and design mitigations.

This inventory is the technical companion to the [Right-to-Repair Philosophy](../docs/right-to-repair-philosophy.md) — it proves the claim that "every failure mode I can imagine has a user-serviceable path."

---

## LED Strip

| Failure Mode | Severity | Probability | User-Repairable |
|---|---|---|---|
| [Dead Pixel(s)](led-dead-pixel.md) | Low | Very Low | Yes |
| [Total Failure](led-strip-total-failure.md) | High | Very Low | Yes |
| [Color Degradation](led-color-degradation.md) | Low | Very Low | Yes |

## Power Supply

| Failure Mode | Severity | Probability | User-Repairable |
|---|---|---|---|
| [No Output](psu-no-output.md) | Critical | Low | Yes |
| [Voltage Instability](psu-voltage-instability.md) | Medium | Low | Yes |

## Controller / PCB

| Failure Mode | Severity | Probability | User-Repairable |
|---|---|---|---|
| [Hardware Failure](controller-hardware-failure.md) | High | Very Low | Partial |
| [Firmware Corruption](controller-firmware-corruption.md) | High | Very Low | Partial |
| [Wi-Fi Connectivity Loss](controller-wifi-failure.md) | Medium | Low | Yes |
| [NVS Data Loss](controller-nvs-corruption.md) | Low | Very Low | Yes |

## Acrylic Panels

| Failure Mode | Severity | Probability | User-Repairable |
|---|---|---|---|
| [Crack or Deep Scratch](acrylic-crack-scratch.md) | Medium | Low | Yes |
| [Coating Degradation](acrylic-coating-degradation.md) | Low | Very Low | Yes |

## Wood Frame

| Failure Mode | Severity | Probability | User-Repairable |
|---|---|---|---|
| [Physical Damage](frame-physical-damage.md) | Low | Low | Yes |

## PETG Printed Parts

| Failure Mode | Severity | Probability | User-Repairable |
|---|---|---|---|
| [Breakage](petg-breakage.md) | Low | Low | Yes |

## Wiring & Connections

| Failure Mode | Severity | Probability | User-Repairable |
|---|---|---|---|
| [Solder Joint Failure](wiring-solder-joint-failure.md) | Medium | Low | Partial |
| [Barrel Jack — Intermittent](barrel-jack-intermittent.md) | Medium | Low | Yes |

## Buttons

| Failure Mode | Severity | Probability | User-Repairable |
|---|---|---|---|
| [Primary Unresponsive](button-primary-failure.md) | Medium | Very Low | Partial |
| [Recessed Unresponsive](button-recessed-failure.md) | Medium | Very Low | Partial |

---

## Summary

- **17 failure modes** enumerated across 8 component categories.
- **12 fully user-repairable**, 5 partially user-repairable (require soldering or support for firmware).
- **0 non-repairable** — every failure mode has a service path.
- All repair parts are either commodity (available from any electronics/hardware supplier) or printable (STL on request).
- The only component requiring Layered Logic involvement is the controller firmware (closed binary — reflash service available at cost of shipping).

---

## Related

- [Right-to-Repair Philosophy](../docs/right-to-repair-philosophy.md) — the values framework this inventory supports
- [Sustainability Audit](../docs/sustainability-audit.md) — Stage 6 repairability matrix
- [BOM Breakdown](../docs/bom-breakdown-basic-6x6.md) — component costs and sourcing
- [Button Interface Design](../docs/button-interface.md) — factory reset and pairing recovery paths
- [Sprint Plan](../sprint_plan.md) — Week 4 deliverables
