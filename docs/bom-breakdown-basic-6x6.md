# BOM Breakdown — Basic 6×6 Infinity Mirror
**Layered Logic — Spring 2026 | Week 2 Deliverable**
*Volume tiers: 1 / 50 / 100 / 1,000 units*

---

## Product Configuration

| Spec | Value |
|---|---|
| Model | Basic 6"×6" |
| Mirror panels | 150mm×150mm×3.1mm acrylic (1-way + 2-way reflective) |
| LED strip | WS2812B, 32 LEDs per unit (60 LEDs/m strip) |
| Controller | Basic LED controller |
| PSU | 5V brick (basic) |
| Frame | 3-sided, cut from 2×4 lumber |
| Printed parts | 44g PETG (LED spacer + electronics enclosure) |
| Packaging | Laser-cut folded cardboard, 8"×8"×3", <2 lbs |

---

## Fixed Capital Equipment

Capital required before first unit ships. Dominated by the laser cutter.

| Item | Cost | Notes |
|---|---|---|
| Laser cutter (300×600mm) | $4,795.83 | Primary cutting tool for acrylic + back panel |
| 3D printer (300³mm) | $1,500.00 | PETG parts |
| Mini PC (laser control) | $314.00 | |
| Laser table + stabilizer | $200.96 | |
| LightBurn (lifetime license) | $189.21 | Laser CAM software |
| Soldering iron | $60.36 | |
| Raspberry Pi | $50.00 | Flashing/testing |
| Fume extractor | $33.33 | |
| ST-Link v2 | $9.42 | |
| **Total** | **$7,153.11** | |

> **Note:** This equipment serves the full product line and general fabrication — not exclusively infinity mirrors. True per-product amortization would be lower if other products share the tooling.

---

## Material BOM — Per Unit at Each Volume

*Prices based on current sourcing (retail → wholesale → bulk direct as volume increases).*
*LED count: 32 confirmed measured (model computes 37 from perimeter; actual frame geometry uses 32).*

| Component | Qty | Unit | 1 unit | 50 units | 100 units | 1,000 units | Source notes |
|---|---|---|---|---|---|---|---|
| LED strip (WS2812B) | 32 | LED | $1.63 | $1.63 | $1.28 | $0.80 | 1–50: Amazon reel ($0.051/LED); 100: AliExpress ~$12/reel; 1k: bulk ~$7.50/reel |
| 1-way mirror acrylic | 1 | panel | $2.49 | $2.24 | $2.12 | $1.87 | Makerstock $9.95/12×12 sheet → bulk discount at volume; 4 panels/sheet |
| 2-way mirror acrylic | 1 | panel | $5.00 | $4.50 | $4.25 | $3.75 | TT Plastic Land $20/12×12 sheet (best current price); 4 panels/sheet |
| Back panel (birch ply) | 1 | panel | $1.25 | $1.13 | $1.06 | $0.94 | Makerstock $5/12×12 sheet; 4 panels/sheet |
| Frame sides (3× basic) | 1 | set | $2.00 | $1.75 | $1.50 | $1.25 | Cut from 2×4 lumber |
| LED controller (basic) | 1 | unit | $5.00 | $4.00 | $3.50 | $2.50 | |
| PSU (basic, 5V) | 1 | unit | $5.00 | $3.50 | $3.00 | $2.00 | AliExpress bulk at 50+ |
| PETG parts (44g total) | 1 | set | $1.50 | $1.50 | $1.25 | $1.00 | Material + print wear; $14.75/kg filament |
| LED holders | 1 | set | $0.50 | $0.50 | $0.50 | $0.40 | |
| Back reflector | 1 | piece | $0.50 | $0.50 | $0.40 | $0.30 | |
| Hardware (screws, wire) | 1 | set | $0.75 | $0.60 | $0.55 | $0.40 | |
| Packaging (cardboard) | 1 | unit | $2.00 | $1.25 | $1.00 | $0.60 | ⚠️ Estimate — actual cost TBD pending box design |
| **Materials subtotal** | | | **$27.62** | **$23.10** | **$20.41** | **$15.80** | |

---

## Full COGS Breakdown

*Labor model: assembly 10 min + laser cutting 30 min (3 panels × 10 min) + 3D printer setup 10 min = 50 min at 1 unit. Efficiency gains modeled at scale (batching). Rate: $60/hr.*

| Cost element | 1 unit | 50 units | 100 units | 1,000 units |
|---|---|---|---|---|
| Materials | $27.62 | $23.10 | $20.41 | $15.80 |
| Labor (active time) | $50.00 (50 min) | $40.00 (40 min) | $35.00 (35 min) | $30.00 (30 min) |
| Energy | $1.00 | $1.00 | $1.00 | $1.00 |
| Overhead | $3.00 | $3.00 | $3.00 | $3.00 |
| **Variable COGS** | **$81.62** | **$67.10** | **$59.41** | **$49.80** |
| Fixed cost amortized | $7,153.11 | $143.06 | $71.53 | $7.15 |
| **Total COGS (inc. fixed)** | **$7,234.73** | **$210.16** | **$130.94** | **$56.96** |

---

## Margin Analysis

Two ways to read this, both valid depending on context:

**View A — Labor as owner earnings** (recommended for sole-owner maker business)
True cash cost = materials + energy + overhead. Labor is what you earn.

| Retail price | 1 unit | 50 units | 100 units | 1,000 units |
|---|---|---|---|---|
| True cash cost | $31.62 | $27.10 | $24.41 | $19.80 |
| **$50 retail** | $18.38/unit → **$22/hr** | $22.90/unit → **$34/hr** | $25.59/unit → **$44/hr** | $30.20/unit → **$60/hr** |
| **$65 retail** | $33.38/unit → **$40/hr** | $37.90/unit → **$57/hr** | $40.59/unit → **$70/hr** | $45.20/unit → **$90/hr** |
| **$80 retail** | $48.38/unit → **$58/hr** | $52.90/unit → **$79/hr** | $55.59/unit → **$95/hr** | $60.20/unit → **$120/hr** |

**View B — Full COGS margin** (labor counted as a cost)

| Retail price | 1 unit | 50 units | 100 units | 1,000 units |
|---|---|---|---|---|
| $50 | -$31.62 | -$17.10 | -$9.41 | **+$0.20** |
| $65 | -$16.62 | -$2.10 | **+$5.59** | **+$15.20** |
| $80 | -$1.62 | **+$12.90** | **+$20.59** | **+$30.20** |

*(Fixed amortization excluded from View B — treated as sunk cost / shared infrastructure)*

---

## Key Findings

### 1. Labor is the dominant cost driver
At all volume tiers, labor exceeds material cost. Even at 1,000 units (30 min/unit × $60/hr = $30 labor vs. $15.80 materials), labor is nearly 2× materials. Any investment in reducing active assembly time — jigs, fixtures, better workflow — has an outsized impact on effective hourly rate.

### 2. The $50 retail price is tight
At $50, the effective hourly rate ranges from **$22/hr (1 unit)** to **$60/hr (1,000 units)**. The $50 price point only reaches your $60/hr target at very high volume. Consider:
- **$65 as the base price** — reaches $57–90/hr across realistic early volumes (50–100 units)
- **$50 as a promotional/Etsy entry price only** if driving initial reviews, understanding the margin compression

### 3. The 2-way mirror is the highest-cost material
At $5.00/panel at 1 unit, it's the single most expensive material line item. Supplier negotiation or bulk ordering has the most leverage here. At 1,000 units it drops to $3.75 — worth pursuing a direct quote from Canal Plastic or TT Plastic Land at ~100+ sheet volume.

### 4. Fixed equipment is not the pricing constraint
At 100 units the laser + equipment is only $71.53/unit amortized — significant but not prohibitive. At 1,000 units it's $7.15. These tools serve the full product line; pricing should not be held hostage to 100% equipment cost recovery from this SKU alone.

### 5. Material cost reduction path is clear
Materials drop from $27.62 → $15.80 (43%) from 1 to 1,000 units. The biggest levers in order:
1. PSU: $5.00 → $2.00 (-$3.00)
2. LED controller: $5.00 → $2.50 (-$2.50)
3. 2-way mirror: $5.00 → $3.75 (-$1.25)
4. LED strip: $1.63 → $0.80 (-$0.83)

---

## PCB Cost Analysis — JLCPCB (Basic STM8 Board)

*BOM: `Claude_PCB_Files/jlcpcb/production_files/BOM-Infinity_Mirror_basic.csv`*
*7 component lines | 9 total parts | 40 solder joints | 2 extended parts (STM8, USB-C)*

| Component | LCSC # | Qty | Unit price | Subtotal |
|---|---|---|---|---|
| 0.1uF 0603 cap | C14663 | 2 | $0.010 | $0.020 |
| 10k 0603 resistor | C25804 | 1 | $0.008 | $0.008 |
| 1uF 0603 cap | C15849 | 2 | $0.010 | $0.020 |
| 330R 0603 resistor | C23138 | 1 | $0.008 | $0.008 |
| 47uF 0805 cap | C16780 | 1 | $0.015 | $0.015 |
| STM8S003F3P TSSOP-20 | C52717 | 1 | $0.217 | $0.217 |
| USB-C receptacle 6P | C456012 | 1 | $0.020 | $0.020 |
| **Parts total/board** | | | | **$0.308** |

| Order qty | PCB fab | Setup + ext. | Joints | Parts (+10%) | **Per board** |
|---|---|---|---|---|---|
| 5 | $2.00 | $14.00 | $0.34 | $1.69 | **$3.61** |
| 10 | $4.00 | $14.00 | $0.68 | $3.39 | **$2.21** |
| 50 | $17.50 | $14.00 | $3.40 | $16.94 | **$1.04** |
| 100 | $35.00 | $14.00 | $6.80 | $33.88 | **$0.90** |

> **Implication:** The $5.00 basic controller placeholder in `pricing_config.yaml` is overstated by ~5×. At production volume (50–100 units), the assembled PCB is ~$1/board. The `pricing_config.yaml` has been updated to reflect $1.00 at volume with a note.

### TTP223 Touch Sensor Add-On (evaluated Apr 9)

Adding a TTP223 capacitive touch IC to the basic PCB adds approximately **$0.10/board** all-in:
- IC (C42422127, SOT-23-6): ~$0.08
- 1× sensitivity cap (0603): ~$0.01
- Assembly (8 extra joints): ~$0.014
- **No extended part fee** — TTP223 is a JLCPCB basic part

Touch pad is a copper pour on the PCB (no additional component). Senses through PETG enclosure at 1–2mm thickness without issue. Decision pending on whether basic model needs touch vs. physical button.

---

## Assumptions & Caveats

| Item | Assumption | Flag |
|---|---|---|
| Packaging | $2.00 estimate at 1 unit | ⚠️ TBD — get actual quote for laser-cut cardboard box |
| LED count | 32 (measured) | Model computes 37 from perimeter math — verify frame geometry |
| Labor | 50 min at 1 unit, 30 min at 1,000 | Based on: 3 panels × 10 min laser + 10 min assembly + 10 min 3D setup |
| Volume discounts | Estimated 10–25% for acrylic | No formal quotes obtained yet |
| Basic controller | ~$1.00 at 50–100 units (JLCPCB) | Updated from $5 placeholder; ESP32 PCB BOM pending |
| Platform fees | Not included | Etsy adds ~9.5% + $0.20 listing; Shopify ~2.9% + $0.30 |
| Shipping | Not included | ~$8–12 domestic; typically passed to customer |

---

*Last updated: 2026-04-09*
*Source data: `Business/pricing_config.yaml`, `Business/infinity mirror pricing - Spent(as of 2_22).csv`, `Business/infinity mirror pricing - cost estimates.csv`, direct hardware measurements, JLCPCB/LCSC pricing (Apr 2026).*
