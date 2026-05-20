---
title: Mirror Assembly & Teardown Guide
type: operations
phase: 3
week: 8
date: 2026-05-20
status: draft — assembly times pending golden-sample measurement
tags: [operations, assembly, teardown, manufacturing, repair]
---

# Mirror Assembly & Teardown Guide — Basic 6×6 Infinity Mirror

**Prepared by:** William White, Layered Logic LLC
**Date:** May 20, 2026
**Applies to:** Basic 6"×6" Infinity Mirror v1.0

---

> **Why this document exists.** It is the canonical reference for how a unit goes together — and, read in reverse, how it comes apart. The manufacturing sequence (Sections 4–6) is the operations-track build process. The teardown procedure (Section 7) is the access path the customer-facing [User Repair Guide](user-repair-guide.md) depends on: every repair in that guide starts by getting into the unit, and this is the document that says how.

---

## 1. Scope & Audiences

This guide serves two readers:

- **The builder** — assembling units. Sections 2–6 are the build process: sub-assembly prep, station-by-station manufacturing, and final assembly.
- **The repairer** — a customer or Layered Logic opening a finished unit to service it. Section 7 is the teardown/access procedure, written as the reverse of final assembly.

The full manufacturing process is captured at station granularity in `Assembly_docs/LED_Mirror_Station_Details.csv` (20 stations, S01–S20). This guide is the narrative companion to that table.

---

## 2. Tools & Workspace

### Tools

- **Phillips-head screwdriver** — the only tool required to open or close a finished unit. No proprietary bits.
- Suction-cup handles (×2) — for handling mirror panels without touching the faces.
- Plastic spudger / pick tool — for the snap-fit controller housing and seating sub-assemblies.
- For manufacturing only: miter saw, router table + handheld router, wet saw, soldering iron, 3D printer, bench PSU. See the station CSV for the full per-station tool list.

### Workspace

- A rubber bench mat protects mirror faces during assembly.
- Pack-and-ship is kept physically separate from cutting and sanding — sawdust and loose hardware contaminate finished units.

---

## 3. Parts Inventory (Per Unit)

From the [BOM Breakdown](bom-breakdown-basic-6x6.md):

| Part | Qty | Notes |
|---|---|---|
| Wood frame, 3-sided | 1 set | Cut from 2×4 lumber; mitered + glued; mirror channels + electronics pocket milled in |
| 1-way mirror acrylic panel | 1 | 150×150×3.1 mm — the back panel |
| 2-way mirror acrylic panel | 1 | 150×150×3.1 mm — the front (viewing) panel |
| WS2812B LED strip | 1 | 32 LEDs, soldered to a connector |
| LED spacer (PETG, printed) | 1 | Holds the strip; part of the LED sub-assembly |
| Controller PCB (ESP32-C3 module) | 1 | Replaceable module on its own board |
| Controller housing (PETG, printed) | 1 set | Snap-fit tray + lid |
| 5 V power supply | 1 | Barrel-jack brick — commodity, replaceable |
| Back panel (birch plywood) | 1 | 150×150 mm |
| Back reflector | 1 | Behind the LED plane |
| LED holders | 1 set | |
| Hardware | 1 set | Screws, wire |

Printed parts total ≈ 44 g PETG (LED spacer + controller housing).

---

## 4. Sub-Assemblies

Three sub-assemblies are built ahead of final assembly and staged.

### 4.1 Frame (stations S01–S06)

2×4 lumber is inspected and rough-cut (S01), precision-mitered to length (S02), then run through the router table to mill the **mirror channels** that the acrylic panels seat into (S03). One designated segment gets the **electronics pocket** routed into it for the controller housing (S04) — this is the only non-repeatable frame step, jig-controlled and test-fit against a printed housing before production. The frame is glued at the miter joints and clamped square (S05), then sanded and finished (S06). Finish is kept out of the mirror channels and the electronics pocket — it changes fit tolerances.

> The frame is a **permanent glued assembly.** It is not taken apart for repair. All internal access happens through the back panel (Section 7).

### 4.2 LED sub-assembly (stations S10–S12)

LED strip rolls are pre-tested at receiving (S10) — every LED must light. The strip is cut to length and a connector soldered to the VIN/GND/DATA pads using a solder jig, then power-tested again (S11). The tested strip is pressed into the printed PETG **LED spacer**, with the connector cable routed out the spacer's exit port (S12). Result: a `LED + spacer` sub-assembly staged for final assembly.

### 4.3 Controller sub-assembly (stations S13–S16)

Controller PCBs are inspected on receipt (S13), batch-flashed with firmware on the Raspberry Pi rig (S14 — flashed and unflashed trays are never mixed), soldered and function-tested (S15), then seated into the printed PETG **controller housing** (S16). The housing is a two-piece snap-fit design — see Section 8. Result: a `controller + housing` sub-assembly staged for final assembly.

---

## 5. Mirror Panels (stations S07–S09)

Acrylic mirror sheets are received and stored **vertically** with foam separators — never stacked flat, which chips edges (S07). Sheets are wet-cut into 6×6" blanks on the tile saw (S08); dry cutting micro-fractures the acrylic. For **custom orders only**, blank panels are pulled from inventory and cut to the per-order design on the laser/CNC (S09) — standard units skip this station.

Each unit uses one 1-way panel (back) and one 2-way panel (front).

---

## 6. Final Assembly (stations S17–S20)

### 6.1 Kitting (S17)

All sub-assemblies and hardware for one unit are pulled and checked against the kit checklist into a labeled order tray with a traveler card. A missing component sends the tray back — partial assembly never starts.

### 6.2 Assembly (S18)

Build order, into the finished frame:

1. **Controller housing** into the electronics pocket — seats flush.
2. **Mirror #1 (1-way / back panel)** laid into the milled channels.
3. **LED + spacer sub-assembly** seated against mirror #1.
4. **Mirror #2 (2-way / front panel)** laid into the channels — the infinity effect lives between the two panels.
5. **Back panel** installed and secured with screws.
6. **Cables** routed and managed before the back panel closes; the power connector is left accessible and strain-relieved.

Mirrors are handled with suction cups only — no bare hands on the faces.

### 6.3 QC (S19)

Full power-on test (all LEDs fire, no dead zones, no flicker), mirror inspection (no scratches/chips/misalignment), structural check (no rattles, backing flush), connector engagement check. Zero tolerance for dead LEDs or cracked mirrors — automatic rework. The traveler card is signed off.

### 6.4 Pack & Ship (S20)

The unit is wrapped, boxed with void fill, labeled, and logged. No unit is boxed without a completed traveler card.

---

## 7. Teardown / Access Procedure

This is final assembly (Section 6.2) run in reverse. It is the access path the [User Repair Guide](user-repair-guide.md) builds on — every component repair begins here.

**Before you start:** unplug the power supply. Work on a soft, clean surface. Have a Phillips screwdriver and suction-cup handles ready.

1. **Unplug the unit** and lay it face-down on a rubber mat or towel.
2. **Remove the back panel.** Back out the Phillips screws securing the birch-plywood back panel and set them aside (they are reused). Lift the panel off.
3. **Note the cable routing** before disturbing anything — a quick photo helps on reassembly. Free any cable ties as needed.
4. **Lift out the back reflector.**
5. **Lift out mirror #2 (front 2-way panel)** from its channels using suction cups. Set it face-up on a clean cloth.
6. **Lift out the LED + spacer sub-assembly.** The connector cable unplugs from the controller.
7. **Lift out mirror #1 (back 1-way panel)** the same way.
8. **Remove the controller housing** from the electronics pocket. To reach the PCB itself, open the snap-fit housing — see Section 8.

To reassemble, reverse the steps: controller housing → mirror #1 → LED sub-assembly → mirror #2 → back reflector → back panel. Re-route cables before the back panel goes on, and confirm the power connector is accessible.

> The wood frame is never disassembled — it is a glued structural unit. Every serviceable part is reached through the back panel.

---

## 8. The Controller Housing (Snap-Fit)

The controller PCB lives in a two-piece printed PETG enclosure: a **tray** (floor + walls, with compression snap pegs that retain the PCB through its mounting holes) and a **lid** with snap tabs that engage through-slots in the tray walls.

- **To open:** push each lid barb inward through its wall slot from the outside, then lift the lid free.
- **The PCB** is held by Y-split compression snap pegs through its mounting holes — flex the prongs to release.
- **No glue, no screws** in the housing itself; it is designed to open and reclose indefinitely.

The parametric housing model and its design history are in `Assembly_docs/basic_housing/` (`basic_housing.py`, Fusion 360 script; `sketch_housing_layout.py`, the layout sign-off).

> **Open item:** the housing model in `Assembly_docs/basic_housing/` is dimensioned for the earlier STM8 test board (25.83 × 21.7 mm). The shipping product uses the ESP32-C3 controller module — the housing footprint needs re-confirming against the production controller PCB before this section is final.

---

## 9. Open Items

- **Assembly times.** Per-station and per-unit assembly times are not yet measured. The [BOM Breakdown](bom-breakdown-basic-6x6.md) models ~50 min active labor at single-unit volume (10 min assembly + 30 min cutting + 10 min print setup), dropping toward 30 min at scale — these are estimates pending the golden-sample build (Weeks 9–11).
- **Controller housing footprint** — STM8 vs. ESP32-C3, see Section 8.
- **Back-panel fastener count and type** — to be confirmed against the production frame.

---

## Related

- `Assembly_docs/LED_Mirror_Station_Details.csv` — the full 20-station manufacturing process
- [User Repair Guide](user-repair-guide.md) — the customer-facing repair doc that consumes Section 7
- [BOM Breakdown — Basic 6×6](bom-breakdown-basic-6x6.md) — the parts inventory
- [Repair-Driven Design Decisions](repair-design-decisions.md) — why the unit is reopenable (Phillips-only, no bonded panels)
- [Failure Mode Inventory](../Failure_Modes/README.md) — what can break, and which teardown steps reach it
- [Sprint Plan](../sprint_plan.md) — Week 8 deliverable [LL-077](../tasks.md#LL-077)
