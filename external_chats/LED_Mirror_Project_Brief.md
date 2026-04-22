---
title: LED Mirror — Factory Operations Project Brief
type: external-chat-log
source: claude
exported: 2026-04-14
status: archived
tags: [external-chat, claude, factory-operations, assembly, week-3]
---

# LED Mirror — Factory Operations Project Brief

## Summary

Summary brief produced from a full factory/assembly planning session (external Claude chat, Apr 14) covering the product's three sub-assemblies, 20-station production line, garage workshop layout, Phase 1 tooling list, and all key operational decisions. This brief plus the companion CSV and XLSX artifacts in `Assembly_docs/` were the basis for the Week 3 operations entry in [sprint_log.md](../sprint_log.md). Key decisions captured: 3 parallel production lanes (Frame / Mirrors / Electronics) feeding final assembly, batch-by-operation strategy, milled mirror channels (not spacer layup), and solder operations flagged as the first scale bottleneck / first outsource candidate.

---

> **Context:** This document summarizes a full factory/assembly planning session for an LED mirror product. It covers the product architecture, assembly flow, 20-station production line, garage workshop layout, tooling list, and all key operational decisions made. Use this as context for continuing work in Claude Code or any new session.

---

## Product Overview

An LED mirror product consisting of three core sub-assemblies:

- **Frame** — Wood construction. 2x4s cut, milled, and mitered together. All segments are repeatable except one, which has a pocket milled for the electronics bay.
- **Mirrors** — Two 6"x6" mirror panels per unit, cut from 12"x12" sheets (4 blanks per sheet). Can be blank or custom-cut to a per-order design.
- **Electronics** — LED strip, connector, 3D printed spacer, controller PCB, and 3D printed controller housing.

**Target volume:** 1 order per day (low volume, single-operator assumed).

---

## Assembly Flow — High Level

Three production lanes run in parallel and feed into final assembly:

```
LANE 1: Frame
  Lumber receiving → Miter saw cuts → Router table profile mill →
  Handheld router (electronics pocket) → Glue-up & clamp → Sand & finish → Frame QC

LANE 2: Mirrors
  Sheet receiving → Wet saw batch cut (12x12 → 4x 6x6) → Blank inventory →
  [Custom design cut per order, if applicable] → Mirror QC

LANE 3A: LED Strip Prep
  Roll receiving & pre-test → Cut to length → Solder connector (VIN/GND/DATA) →
  Post-solder power test → Seat in 3D printed spacer → Staging bin

LANE 3B: Controller Prep
  Receive from China (5-day lead time) → Inspect → Flash firmware (Raspberry Pi jig) →
  Solder required pads → Function test → Seat in 3D printed housing → Staging bin

FINAL ASSEMBLY
  Kit check (traveler card) → Install controller housing into pocket →
  Mirror #1 into milled channels → LED strip assembly → Mirror #2 →
  Backing → Cable management → Final power-on QC → Box & ship
```

---

## Key Operational Decisions Made

| Decision | Choice | Rationale |
|---|---|---|
| Mirror channel method | Milled channels (not spacer layup) | More repeatable, less per-unit variability |
| Mirror blank strategy | Store pre-cut 6x6 blanks; custom cut per order | Decouples cutting cadence from order intake |
| Controller stock | Bulk order with conservative reorder point | 5-day China lead time; must never be the constraint |
| Build strategy | Batch by operation, not one unit start-to-finish | Setup time paid once per session, not per unit |
| LED pre-test timing | At receiving (roll level), not at assembly | Discovering dead strips mid-build is a major disruption |
| 3D printed parts supply | Print a full week's buffer in one overnight run | Reactive per-order printing creates daily bottlenecks |
| Traveler card | One physical card per unit through the entire line | Cheapest way to catch skipped steps before they ship |

---

## 20-Station Production Line

| Station | Name | Lane | Key Input → Output |
|---|---|---|---|
| S01 | Lumber Receiving & Staging | Frame | Raw 2x4s → Rough-cut segments |
| S02 | Miter Saw — Precision Cut | Frame | Rough segments → Mitered segments |
| S03 | Router Table — Profile & Channel Mill | Frame | Mitered segments → Profiled segments with mirror channels |
| S04 | Handheld Router — Electronics Pocket | Frame | Pocket segment → Segment with routed electronics bay |
| S05 | Frame Glue-Up & Assembly | Frame | All frame segments → Clamped assembled frame |
| S06 | Frame Sanding & Finish | Frame | Cured frame → Finished frame, QC passed |
| S07 | Mirror Sheet Receiving & Storage | Mirrors | Raw 12x12 sheets → Stored, inspected sheets (vertical A-frame rack) |
| S08 | Wet Saw — Mirror Batch Cut | Mirrors | 12x12 sheets → 4x 6x6 blanks per sheet |
| S09 | Custom Design Cut (Per-Order) | Mirrors | 6x6 blanks → Custom-cut mirror panels (skip for blank units) |
| S10 | LED Strip Receiving & Pre-Test | LED Prep | Raw roll → Tested and approved roll stock |
| S11 | LED Strip Cut & Solder | LED Prep | Roll stock → Cut strip with soldered connector, tested |
| S12 | LED Spacer Sub-Assembly | LED Prep | Strip + spacer → LED sub-assembly, ready for final |
| S13 | Controller Receiving & Inspection | Controller Prep | Inbound PCBs → Inspected, sorted into ESD trays |
| S14 | Firmware Flash Station | Controller Prep | Uninflashed controllers → Flashed controllers (Pi jig, batch 5-10) |
| S15 | Controller Solder & Function Test | Controller Prep | Flashed PCBs → Fully soldered, tested controllers |
| S16 | Controller Housing Assembly | Controller Prep | Controller + housing → Controller sub-assembly, ready for final |
| S17 | Kitting Station | Final Assembly | All sub-assemblies → Complete kitted order tray with traveler card |
| S18 | Final Assembly | Final Assembly | Kitted tray → Fully assembled LED mirror unit |
| S19 | Final Quality Control | Final Assembly | Assembled unit → QC-passed unit with signed traveler card |
| S20 | Pack & Ship | Final Assembly | QC-passed unit → Boxed, labeled, logged, outbound |

Full station detail (tools, fixtures, consumables, QC criteria, notes) is in `LED_Mirror_Station_Details.csv`.

---

## Garage Workshop Layout

Single-car garage (~20x20ft). Key principle: **wood dust and electronics are on opposite ends**, door between them.

```
[ LUMBER STORAGE  |  MITER SAW STATION  |  ROUTER TABLE    ]
[                                                            ]
[ ASSEMBLY BENCH  |  ELECTRONICS BENCH  |  3D PRINT CORNER ]
[                                                            ]
[ SANDING/FINISH  |  MIRROR FLAT STORE  |  PACK & SHIP     ]
```

- **Mirror storage** is the most awkward constraint — must be vertical, padded, away from the cutting zone
- **Electronics bench** is the cleanest, best-lit zone — anti-static mat, fume extractor always on
- **Pack & ship** is fully separated from all production — dust contamination of finished goods is a real failure mode
- **8ft assembly bench** is the largest surface — build it from 2x4 and plywood, stronger than anything you can buy

---

## Critical Tool List

### Phase 1 — Must-Have to Operate
| Tool | Spec / Model |
|---|---|
| Miter saw | Dewalt 12" sliding |
| Router table | Kreg or Bosch benchtop |
| Handheld router | Dewalt 1-3/4hp |
| Glass/tile wet saw | 7" benchtop diamond blade |
| Soldering iron | Hakko FX-888D (non-negotiable for daily solder work) |
| Bench power supply | 0-30V, 5A |
| Multimeter | Fluke 115 |
| Fume extractor | Proper extractor with filter — not a fan |
| Shop vac | Ridgid or Festool with auto-on |
| Main workbench | 8ft × 36" deep, built from 2x4 + plywood |

### Phase 2 — Quality and Speed Improvements
| Item | Purpose |
|---|---|
| LED strip solder jig | Holds strip flat and positions connector — build it yourself |
| Miter saw dead-stop jig | One stop per cut length; eliminates measuring per piece |
| Router sled jig | Controls electronics pocket depth and position |
| Frame glue-up square jig | L-shaped clamping fixture; ensures square glue-up every time |
| Pogo pin flash fixture | Batch flash 5-10 controllers per session |
| USB microscope / loupe | Solder joint inspection on small pads |
| Label printer | Dymo or Rollo |
| Rubber bench mat | Protects mirrors during final assembly |
| Suction cup handles | Mirror handling — no bare hands on mirror faces |

### Phase 3 — Optimization
| Item | Purpose |
|---|---|
| Second 3D printer | Redundancy — one printer failure kills housing/spacer supply |
| Ceiling air filtration unit | Passive fine dust capture |
| Mini-split HVAC | Resin prints, glue, and electronics all care about temperature |
| Upgraded dust collection | Wall separator + central vac |

---

## Infrastructure Notes

- **Dedicated 20A circuit** to electronics bench — shop vac must not trip breakers mid-solder
- **LED shop lights** at 5000K / 4000+ lumens per fixture — one per zone minimum
- **Interlocking rubber floor tiles** — protects dropped components, easier on feet
- **Pegboard back wall** on main bench with tool outlines — you know instantly if something is missing

---

## Solder Operations (Bottleneck Flag)

There are **two separate solder operations** per unit:
1. LED connector — VIN, GND, DATA pads on flex strip (S11)
2. Controller PCB — through-hole and pad work (S15)

At low volume this is manageable for one operator. As volume grows, **this is the first hire or the first outsource** (PCB assembly house for the controller solder work is a reasonable option at scale).

---

## Files in This Project

| File | Contents |
|---|---|
| `LED_Mirror_Project_Brief.md` | This document — full context summary |
| `LED_Mirror_Assembly_Calculator.xlsx` | Per-unit time calculator with 3 sheets: Time Standards, Unit Calculator, Batch Planner |
| `LED_Mirror_Station_Details.csv` | Full 20-station detail: tools, fixtures, consumables, inputs, outputs, QC checks, notes |

---

## Open Questions / Next Steps

- [ ] Confirm exact LED strip spec (density, voltage, chipset) — affects cut length and controller flash config
- [ ] Confirm controller PCB BOM — determines exact solder operations at S15
- [ ] Design and build miter jig, router sled, glue-up square jig, and LED solder fixture
- [ ] Time 5 real units with a stopwatch and replace estimates in the xlsx with actual numbers
- [ ] Define reorder point formula for controllers based on actual daily run rate
- [ ] Establish firmware version control process for Pi flashing rig
- [ ] Define packaging spec — box dimensions, foam insert design, unboxing experience
