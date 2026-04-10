# Sustainability Audit — Infinity Mirror v1.0
**HCDE Independent Study — Spring 2026**
*Lifecycle analysis covering all seven product lifecycle stages.*

---

## 1. Framing

This audit documents the full cradle-to-grave lifecycle of the v1.0 infinity mirror. It serves three purposes:
1. **Portfolio artifact** — demonstrates HCDE values applied to a real product
2. **Design input** — hotspot analysis surfaces decisions that reduce impact
3. **Compliance awareness** — anticipates EU right-to-repair directives and WA/CA repairability legislation

The made-to-order production model eliminates overproduction waste by design. Every unit built has a confirmed buyer before materials are cut or printed.

---

## 2. Product Snapshot

| Spec | Value |
|---|---|
| Dimensions | 150mm × 150mm × ~50mm deep |
| Panels | Two mirror-acrylic (PMMA): one one-way reflective, one two-way reflective |
| Frame | Wood (modified 2×4, domestic lumber) |
| LEDs | 32× WS2812B (from 5M/60-per-meter strip) |
| Controller | ESP32-C3-MINI-1 |
| PSU | 5V/5A brick (25W rated) |
| Printed parts | 22g PETG LED spacer + 22g PETG electronics enclosure |
| Packaged weight | < 2 lbs |
| Package dimensions | 8" × 8" × 3" |

---

## 3. Lifecycle Diagram

```
[1. Raw Materials] → [2. Component Mfg] → [3. Assembly]
                                                 ↓
                                          [4. Packaging & Shipping]
                                                 ↓
                                          [5. Customer Use]
                                                 ↓
                                     [6. Maintenance & Repair]
                                                 ↓
                                          [7. End of Life]
```

---

## 4. Stage-by-Stage Analysis

### Stage 1 — Raw Material Extraction

| Material | Source type | Notes |
|---|---|---|
| PMMA (acrylic panels) | Petroleum-derived | Energy-intensive monomer synthesis; no conflict minerals |
| PETG (printed parts) | Petroleum-derived | Thermoplastic, lower processing temp than ABS |
| Wood (frame) | Renewable — domestic lumber | 2×4 dimensional lumber; carbon-sequestering during growth phase |
| Copper (PCB, wiring) | Mined | Concentrated in PCB traces and LED bond wires; small quantity |
| Silicon (ESP32, LEDs) | Mined / refined | High-purity process; embedded in ICs, not recoverable |
| Ferrite / copper (PSU) | Mined | Transformer core + windings in AC-DC brick |
| Cardboard (packaging) | Renewable / recycled content | Likely partially post-consumer recycled fiber |

**Impact:** Medium. No conflict minerals in meaningful quantities. PMMA and PSU are the highest-carbon raw material inputs. Wood is the lowest.

**Design lever:** PSU is oversized (25W rated, 2.8W actual peak). A 5W brick would reduce raw material embodied carbon in the largest single component. Flagged for v2 BOM review.

---

### Stage 2 — Component Manufacturing

| Component | Manufacturing origin | Embedded carbon concern |
|---|---|---|
| Acrylic panels | Sheet stock, US/Asia | Cutting + vacuum coating process |
| WS2812B strip | China (Worldsemi) | IC fabrication + phosphor deposition |
| ESP32-C3-MINI-1 | China (Espressif/TSMC) | Semiconductor fab — high embedded energy |
| PCB (custom) | China (JLCPCB or equiv.) | FR4 laminate + copper etching + ENIG finish |
| PSU brick | China | Transformer winding, plastic housing |
| PETG filament | Varies | Filament extrusion from pellet |
| 2×4 lumber | Domestic US | Sawmill; low processing energy vs. engineered wood |

**Impact:** High — the majority of embedded CO2 is locked in at this stage, primarily in electronics manufacturing and acrylic production. This is typical for consumer electronics and largely outside direct control for a small-volume maker.

**Design lever:** Custom PCB consolidates ESP32, LED driver, and power regulation — eliminating dev-board redundancy and reducing total electronics mass. This is already planned for v1.0.

---

### Stage 3 — Assembly

*All assembly is made-to-order by a single assembler (founder). No factory overhead.*

| Operation | Detail | Environmental note |
|---|---|---|
| Wood cutting | ~2% waste by mass | Offcuts are small; compostable/combustible |
| Acrylic cutting | ~2% waste by mass (~3.4g) | PMMA offcuts are non-recyclable general waste |
| 3D printing — LED spacer | 22g PETG, 36.5 min | ~91 Wh, ~35g CO2 |
| 3D printing — electronics enclosure | 22g PETG, ~36.5 min | ~91 Wh, ~35g CO2 |
| Soldering / wiring | Hand assembly, soldering iron | Negligible energy; lead-free solder |
| QC / testing | Smart plug verification | Negligible energy |

**Total print energy:** ~183 Wh → **~70g CO2**
**Total assembly waste:** ~3.4g acrylic offcuts + trace wood dust

**Impact:** Low. Made-to-order eliminates overproduction. No solvents or adhesives in the assembly process. Printed parts are sized to exact-use quantities.

**Design lever:** Print-on-demand for PETG parts means zero inventory waste. If a design revision is needed, no obsolete stock exists.

---

### Stage 4 — Packaging & Shipping

| Item | Detail |
|---|---|
| Packaging material | Laser-cut folded cardboard — no foam, no plastic |
| Packaging recyclability | Fully recyclable curbside (cardboard) |
| Package dimensions | 8" × 8" × 3" |
| Packaged weight | < 2 lbs (~907g) |
| Shipping method | Domestic US (USPS Priority / UPS Ground) |
| Estimated shipping CO2 | ~0.3–0.5 kg CO2 (domestic avg; varies by distance) |

**Impact:** Low. Laser-cut folded cardboard is a deliberate material choice that eliminates foam inserts and poly bags common in comparable products. Fully curbside recyclable at destination.

**Design lever:** Box geometry is cut-and-fold from a single cardboard sheet — no adhesive assembly required. Customer can flatten and recycle immediately after unboxing.

---

### Stage 5 — Customer Use

*All power data measured via smart plug (Home Assistant, 6,368 samples over 48 hours).*

#### Power Profile

| Mode | Measured draw |
|---|---|
| Idle / standby (display off) | **0.1W** |
| Rainbow cycling (typical) | **1.25W avg** (1.2–1.4W range) |
| Full white (firmware 80% cap) | **2.8W** |
| Theoretical absolute max (60mA/LED spec) | 9.6W — never reached |

**Firmware design note:** The ESP32 firmware caps LED drive current at approximately 36% of the WS2812B rated maximum (60mA per channel). At full white with the 80% user cap applied, each LED draws ~17.5mA — well below the thermal knee of the device. This is a conservative thermal design decision with a direct longevity benefit.

#### Annual Energy Consumption

*Worst-case usage model: device on 40% of the day (9.6 hrs), idle 60% (14.4 hrs).*

| Scenario | Annual energy | Annual CO2 (US avg grid) |
|---|---|---|
| Typical use (rainbow cycling) | **4.91 kWh** | **1.89 kg CO2** |
| Peak use (full white all on-time) | **10.34 kWh** | **3.99 kg CO2** |
| Idle contribution only | 0.53 kWh | 203g CO2 |

**Context:** At typical use, the infinity mirror consumes approximately the same annual energy as **half of a single 10W LED bulb** running 3 hours per day. The 0.1W idle draw is comparable to the best-in-class smart home devices (many draw 0.5–2W at standby).

#### LED Lifespan

WS2812B rated lifespan: **50,000 hours**. At worst-case 9.6 hrs/day on-time:

> **50,000 ÷ 9.6 ÷ 365 = 14.3 years**

Operating at ~29% of rated current further reduces thermal degradation. Real-world lifespan is likely to **exceed** the 50,000-hour specification.

**Impact:** Very low. This is an exceptional energy profile for an ambient LED product. Low idle draw eliminates the "always-on vampire load" concern common to smart home devices.

---

### Stage 6 — Maintenance & Repair

The product is designed for user serviceability. This is both a pro-consumer value and a compliance consideration under emerging EU right-to-repair directives and anticipated WA/CA legislation.

| Component | Replaceable by user? | Method |
|---|---|---|
| LED strip | Yes | Strip is accessible; standard WS2812B 5M/60 replacement from any electronics supplier |
| ESP32 controller | Yes | Module is socketed / enclosure-accessible |
| PSU brick | Yes | Standard 5V/5A barrel jack; commodity part |
| PETG enclosure | Yes | Friction-fit or screwed; no adhesive bonding |
| PETG LED spacer | Yes | Removable from frame |
| Acrylic panels | Yes (with care) | Panels are mounted, not glued; replaceable if cracked |
| Wood frame | Repairable | Standard wood repair; replaceable components |
| Firmware | Yes — OTA | ESP32 supports over-the-air firmware updates; no physical access required for software updates |

**Repairability Manual** (in development — see Week 4/8 sprint): A formal step-by-step repair guide will document LED strip replacement, controller swap, and connectivity troubleshooting as a standalone portfolio artifact.

**Impact:** Low to positive. No planned obsolescence. Every component is either a commodity part (LED strip, PSU) or a user-printable/user-machinable part (PETG, wood). Firmware is updateable without hardware replacement.

---

### Stage 7 — End of Life

| Component | EOL pathway | Notes |
|---|---|---|
| Wood frame | Compost / general recycling / biomass | Untreated/unstained domestic lumber |
| PETG parts (44g total) | PETG recyclable — check local drop-off | PETG (#1 in some classifications) accepted at some specialty recyclers; user-separable from frame |
| Acrylic panels (166g) | General waste — limited recycling options | PMMA with vacuum-deposited metallic coating; not accepted in curbside PMMA recycling streams |
| WS2812B LEDs + PCB | E-waste — electronics recycler | Contains copper, silicon, trace metals; should not go to landfill |
| ESP32 module | E-waste — electronics recycler | Semiconductor device |
| PSU brick | E-waste — electronics recycler | Contains transformer, capacitors |
| Cardboard packaging | Curbside recyclable | Fully recyclable at delivery destination |

**EOL limitation:** The acrylic mirror panels are the primary end-of-life gap. The vacuum-deposited metallic mirror coating makes them non-recyclable as pure PMMA. At ~166g per unit, these will likely go to general waste at end of product life. This is the honest limitation of the material choice.

**Design consideration for v2:** Investigate whether glass mirror panels (more established EOL recycling pathways) could replace acrylic without compromising safety or weight targets. Alternatively, explore panel take-back / refurbishment program for custom orders.

**Impact:** Medium. Electronics (LEDs, PCB, PSU) constitute e-waste and require intentional disposal. The product's long lifespan (14+ years at typical use) significantly reduces the frequency of EOL events compared to lower-quality alternatives.

---

## 5. Embodied Carbon Summary

*Estimates based on published material intensity factors. PSU, PCB, and LED values are approximate.*

| Component | Est. mass | Embodied CO2 |
|---|---|---|
| Acrylic panels (PMMA) | 169g | ~590g |
| PSU brick | ~150g | ~600g |
| WS2812B LEDs (32×) | ~64g | ~320g |
| PCB + ESP32 | ~30g | ~180g |
| Cardboard packaging | ~200g | ~200g |
| PETG parts | 44g | ~154g |
| 3D print energy | — | ~70g |
| Wood frame | ~300g | ~120g |
| **Total (ex-shipping)** | | **~2.24 kg CO2** |
| Domestic shipping (avg) | | ~0.3–0.5 kg CO2 |
| **Cradle-to-door total** | | **~2.5–2.7 kg CO2** |

*For context: a single Amazon package delivery averages ~0.4 kg CO2. A 500mL plastic water bottle embodies ~0.2 kg CO2.*

---

## 6. Hotspot Summary

Three stages carry the majority of impact:

1. **Component manufacturing (Stage 2)** — Electronics fabrication (PCB, ESP32, LEDs, PSU) accounts for the largest share of embodied carbon. This is largely outside direct control at current volume but should inform supplier selection and BOM decisions at scale.

2. **Acrylic panels (Stage 1/7)** — Coated PMMA is both energy-intensive to produce and non-recyclable at end of life. At 169g per unit and ~590g CO2 embodied, it's the largest single material hotspot. The EOL limitation (non-recyclable metallic coating) is the product's most honest sustainability gap.

3. **PSU oversizing (Stage 1/5)** — A 25W brick powering a 2.8W peak load runs at ~11% of rated capacity. At low load, AC-DC brick efficiency drops to ~60–70%. A 5–7W brick would reduce both raw material use and idle conversion losses. Flagged for v2 BOM.

---

## 7. Design Commitments Derived from This Audit

The following specific decisions come directly out of this analysis:

| Commitment | Rationale |
|---|---|
| Made-to-order only (no inventory) | Eliminates overproduction waste by design |
| Laser-cut folded cardboard packaging | Zero foam or plastic; fully curbside recyclable |
| PETG parts friction-fit, not bonded | User-separable for EOL recycling |
| LED current capped at ~29% of spec max | Thermal protection; expected lifespan exceeds 14 years |
| OTA firmware update support | Software improvements without hardware replacement |
| Repairability manual (in development) | Documents user repair paths for all serviceable components |
| PSU right-sizing flagged for v2 | 5–7W brick reduces material use and improves efficiency at actual load |
| Glass panel option for v2 | Investigate to address acrylic EOL gap |

---

*Last updated: 2026-04-09*
*Data sources: direct measurement (Home Assistant smart plug, 6,368 samples), manufacturer specs (WS2812B datasheet), published material intensity factors (PMMA ~3.5 kg CO2/kg, PETG ~3.5 kg CO2/kg, FR4 PCB ~6 kg CO2/kg, dimensional lumber ~0.4 kg CO2/kg).*
