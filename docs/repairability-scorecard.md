---
title: Repairability Scorecard — Infinity Mirror v1.0
type: hcde
phase: 2
week: 5
status: draft
tags: [hcde, repair, scorecard, metrics, portfolio]
---

# Repairability Scorecard — Infinity Mirror v1.0

**Author:** William White
**Date:** April 30, 2026
**Status:** Draft — quantitative companion to [right-to-repair-philosophy.md](right-to-repair-philosophy.md). Several cells use placeholder values pending measurement on the golden sample; flagged with **TBD** and an estimation source.

---

## 1. Top-line numbers

Pulled from the [failure mode inventory](../Failure_Modes/README.md) and the [BOM breakdown](bom-breakdown-basic-6x6.md):

| Metric | Value | Source |
|---|---|---|
| Failure modes enumerated | **17** | [Failure_Modes/](../Failure_Modes/) — counted across 8 component categories |
| Fully user-repairable | **12 (71%)** | [Failure_Modes/README.md §Summary](../Failure_Modes/README.md) |
| Partially user-repairable (needs soldering or LL involvement) | **5 (29%)** | same |
| Non-repairable | **0** | same |
| Component classes that are 100% commodity-sourceable | **6 of 8** (LED strip, PSU, screws, acrylic, wood frame, wiring) | [BOM breakdown](bom-breakdown-basic-6x6.md) |
| Component classes that depend on Layered Logic | **2 of 8** (PCB, controller firmware) | [right-to-repair-philosophy §4](right-to-repair-philosophy.md#4-the-honest-limits) |
| Tools required for full disassembly | **1** (Phillips screwdriver) | [right-to-repair-philosophy §2](right-to-repair-philosophy.md) |
| Replacement-part availability commitment | **7 years from ship date** | [right-to-repair-philosophy §3](right-to-repair-philosophy.md) |

---

## 2. Per-failure-mode scorecard

The 17 modes from [Failure_Modes/](../Failure_Modes/) scored on five axes. Repair time is wall-clock end-to-end, including disassembly, replacement, and reassembly — **TBD** values are estimates pending golden-sample measurement.

| Failure mode | Severity | Repair tier | Tools | Parts source | Est. repair time |
|---|---|---|---|---|---|
| [LED dead pixel](../Failure_Modes/led-dead-pixel.md) | Low | User | Phillips, soldering iron | Commodity (WS2812B 5M/60) | **TBD ~25 min** (estimate) |
| [LED strip total failure](../Failure_Modes/led-strip-total-failure.md) | High | User | Phillips, soldering iron | Commodity | **TBD ~40 min** (estimate; covers full-strip swap on a 6×6) |
| [LED color degradation](../Failure_Modes/led-color-degradation.md) | Low | User | Phillips, soldering iron | Commodity | **TBD ~40 min** (treats as full strip replacement) |
| [PSU no output](../Failure_Modes/psu-no-output.md) | Critical | User | None | Commodity (5V barrel-jack) | **TBD ~2 min** (unplug + plug) |
| [PSU voltage instability](../Failure_Modes/psu-voltage-instability.md) | Medium | User | None | Commodity | **TBD ~2 min** |
| [Controller hardware failure](../Failure_Modes/controller-hardware-failure.md) | High | Partial — needs LL reflash | Phillips | LL (controller module) + commodity | **TBD ~15 min user-side, plus ship round-trip** |
| [Controller firmware corruption](../Failure_Modes/controller-firmware-corruption.md) | High | Partial — OTA-resolvable in most cases | None (OTA) or Phillips + LL reflash | LL (firmware) | OTA: < 5 min unattended. Reflash: ship round-trip |
| [Wi-Fi connectivity loss](../Failure_Modes/controller-wifi-failure.md) | Medium | User | None | None (recovery path only) | **TBD ~3 min** (recessed-button factory reset + re-pair) |
| [NVS data loss](../Failure_Modes/controller-nvs-corruption.md) | Low | User | None | None (graceful default) | < 1 min (auto-recovery on next boot) |
| [Acrylic crack/scratch](../Failure_Modes/acrylic-crack-scratch.md) | Medium | User | Phillips | Commodity (cut to spec) or LL replacement panel | **TBD ~20 min** + acquisition lead time |
| [Acrylic coating degradation](../Failure_Modes/acrylic-coating-degradation.md) | Low | User | Phillips | LL or specialty supplier | **TBD ~20 min** + lead time |
| [Frame physical damage](../Failure_Modes/frame-physical-damage.md) | Low | User | Phillips, woodworking tools | Commodity (1×2 hardwood, milled) or LL reorder | Variable — **TBD** depending on damage extent |
| [PETG breakage](../Failure_Modes/petg-breakage.md) | Low | User | Phillips, FDM printer | Commodity (filament) + LL STL on request | **TBD ~30 min** + print time (~1–3 hr per part) |
| [Solder joint failure](../Failure_Modes/wiring-solder-joint-failure.md) | Medium | Partial — soldering required | Phillips, soldering iron | None (re-flow existing) | **TBD ~15 min** |
| [Barrel jack intermittent](../Failure_Modes/barrel-jack-intermittent.md) | Medium | User | Phillips, soldering iron (sometimes) | Commodity (panel-mount jack) | **TBD ~20 min** |
| [Primary button unresponsive](../Failure_Modes/button-primary-failure.md) | Medium | Partial — soldering required | Phillips, soldering iron | LL (PCB-mount switch) | **TBD ~25 min** |
| [Recessed button unresponsive](../Failure_Modes/button-recessed-failure.md) | Medium | Partial — soldering required | Phillips, soldering iron | LL (PCB-mount switch) | **TBD ~25 min** |

---

## 3. Tooling accessibility

Distribution of repairs by required tooling — proxy for "how technical does the buyer need to be":

| Tools required | Count | % of failure modes | Examples |
|---|---|---|---|
| **No tools at all** | 4 | 24% | PSU swap, Wi-Fi reset, NVS auto-recovery, OTA |
| **Phillips only** | 0 | 0% | (Phillips alone doesn't resolve any failure mode without an inner action; tracking for completeness) |
| **Phillips + acquisition** | 4 | 24% | Acrylic panel swap, frame replacement, controller module swap, PETG reprint |
| **Phillips + soldering iron** | 9 | 53% | All LED-strip and PCB-side electrical repairs |
| Specialty equipment | 0 | 0% | None require equipment beyond a standard hobbyist electronics bench |

The soldering-iron threshold is the main accessibility line. **TBD: target measurement** — what fraction of buyers in the target audience are comfortable soldering, vs. comfortable shipping the unit back for service. The reflash-service pathway and the "send the controller module to LL for replacement" pathway exist precisely because soldering-comfort is not universal.

---

## 4. Parts independence

Counted as: of the parts needed to complete each repair, how many depend on Layered Logic continuing to operate?

| Independence tier | Count | Examples |
|---|---|---|
| **Fully independent** (commodity parts only — buyer can source from any electronics retailer or hardware store) | 12 | LED strip, PSU, barrel jack, screws, frame wood, acrylic panel (cut to spec), filament for PETG reprints |
| **STL-on-request** (LL provides file, buyer prints) | 1 | PETG breakage |
| **LL-supplied part** (LL ships a replacement) | 4 | PCB, controller module, custom replacement acrylic, custom-cut frame from original supplier |
| **LL-supplied service** (LL performs the work) | 1 | Firmware reflash |

The 5 LL-dependent paths are exactly what [philosophy §4](right-to-repair-philosophy.md#4-the-honest-limits) and the [§3 commitments](right-to-repair-philosophy.md#3-what-i-commit-to) name explicitly. The 7-year availability commitment is the contractual mitigation; the public schematic publication and the architecture-generic PCB design are the long-tail mitigations against LL-shutdown risk.

---

## 5. Comparative context

Repairability scores from comparable consumer-LED products. **TBD: source these from public iFixit teardowns, manufacturer disclosures, or the EU Right-to-Repair scoring framework as it phases in through 2026–2027.** Categories below are placeholders illustrating the framework — replace with real data before publishing externally.

| Product class | Tools to access | Common failure path | Replacement availability | Repair-friendly? |
|---|---|---|---|---|
| **LL Infinity Mirror v1.0** | Phillips screwdriver | User-serviceable for 12 of 17 modes | 7-year LL commitment + commodity supply | Yes by design |
| Mass-market LED art (Etsy / Amazon, $50–120 segment) | **TBD** | **TBD** — typically replace-the-whole-unit | **TBD** — typically none | **TBD** |
| Mid-tier sensory-therapy LED ($230–4,300) | **TBD** | **TBD** | **TBD** | **TBD** |
| Smart-home lighting (Hue / Nanoleaf) | Often non-opening (glued) | Replace whole unit | Vendor-controlled, multi-year | Typically not by design |

The above table is the most explicitly TBD section of this scorecard — comparative data needs sourcing before the scorecard is portfolio-final. The framework is right; the cells need filling.

---

## 6. Lifetime carbon implications

From [sustainability-audit.md](sustainability-audit.md) §6 (Repairability) and §7 (End-of-life): the v1.0 unit's embodied carbon is ~2.5–2.7 kg CO₂ cradle-to-door. Repairable design extends the in-service window — every additional year a unit stays in service amortizes that embodied carbon over a longer use phase. The [LED strip's 14+ year service life](sustainability-audit.md) is the longest-tenured component; the expected dominant constraint is one of the partially-repairable PCB-side failure modes.

| Hypothetical timeline | Embodied carbon amortization | Notes |
|---|---|---|
| 1 year (worst case — early controller failure, owner doesn't pursue repair) | ~2.5 kg CO₂ / yr | Repair philosophy specifically designed to prevent this scenario |
| 7 years (LL replacement-availability floor) | ~0.36 kg CO₂ / yr | Aligns with §3 commitment window |
| 14 years (LED service life ceiling) | ~0.18 kg CO₂ / yr | Implies all electronic components have been replaced at least once during service |

These figures are from the audit and are the most direct answer to "why repair *matters*" beyond the values argument.

---

## 7. Open methodology questions

Items the scorecard would benefit from once the golden sample is built:

- **Stopwatch the actual repair times** for each documented failure mode on a real unit. Replace every **TBD** repair-time estimate with measured data.
- **Soldering-required repairs** could be revisited if the v2 PCB uses a swappable connector for the LED strip rather than direct solder pads. That alone would push 3 modes from "Partial" to "User" tier.
- **Comparative repairability data** — see §5. Sourcing options: iFixit teardowns where available, EU scoring once disclosure rules ship, manufacturer warranty terms as proxy.
- **Publish the scorecard with each unit shipment** as a sticker-style summary card? Open question on whether this is over-promising. Tabling pending Week 7 service-blueprint work on the unboxing flow.

---

## Related

- [Right-to-Repair Philosophy](right-to-repair-philosophy.md) — the values doc this scorecard quantifies
- [Failure Mode Inventory](../Failure_Modes/README.md) — every row in §2 sources from a file here
- [Repair-Driven Design Decisions](repair-design-decisions.md) — how the design was shaped by the repair values measured here
- [Repair Index](repair-index.md) — entry point for the repair-ecosystem docs
- [Sustainability Audit](sustainability-audit.md) — lifecycle data behind §6
- [BOM Breakdown](bom-breakdown-basic-6x6.md) — component-source data behind §4
- [Sprint Plan](../sprint_plan.md) — Week 4 / Week 8 deliverable arc
