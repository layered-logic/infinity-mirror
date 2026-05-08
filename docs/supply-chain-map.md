---
title: Supply Chain Map — Basic 6×6 Infinity Mirror
type: business
phase: 2
week: 6
date: 2026-05-07
status: complete (v1) — packaging supplier still TBD
tags: [bom, supply-chain, sourcing, risk, milestone-2]
---

# Supply Chain Map — Basic 6×6 Infinity Mirror

**Layered Logic — Spring 2026 | Week 6 / Milestone 2 Deliverable**

Builds on [bom-breakdown-basic-6x6.md](bom-breakdown-basic-6x6.md) (per-unit costs at 1 / 50 / 100 / 1,000 volume) and the JLCPCB analysis from [LL-011](../tasks.md#LL-011) (controller PCB at $0.90/board at qty 100). This doc adds the resilience layer — for each component: who's the backup if the primary stocks out, hikes prices, or disappears, and how exposed is the production schedule.

---

## Executive Risk Summary

Three risk axes, in order of severity:

1. **Tariff / import exposure — high.** Most components ship from China at the price points the BOM is built around. Tariffs are the single biggest exposure to unit COGS. Unlikely to *worsen* materially from current state, but unlikely to improve either. Mitigation is partial — fall back to US suppliers on critical components when tariff math goes sideways, accept a 30-50% unit cost bump on those lines.
2. **Lead time — medium, but graceful.** Bulk orders from China take 1-3 weeks; PCB fab takes 7-10 days. Etsy/handmade-channel customers self-grace long ship times (the bracket is 2-4 week ship-by norms), so the made-to-order workflow has built-in slack. Stockouts are recoverable inside one customer's expectation window.
3. **Single-source dependency — low.** Every BOM line has at least one fungible substitute. The components themselves are commodities (WS2812B LEDs, 5V PSUs, M3 hardware) or made-in-house (3D-printed parts). The closest thing to a sole-source risk is the 2-way mirror acrylic from TT Plastic Land — niche supplier, but other plastic-cutters carry equivalent stock.

The aggregate read: nothing here threatens the venture's go/no-go. Tariffs are the live variable to watch.

---

## Sourcing Chain — Primary / Backup Cascade

For commodity items, the standard cascade Bill uses across most projects:
**Amazon retail → AliExpress → Temu → direct Chinese manufacturers.** Each step trades lead time for unit cost. We pin the primary at the cheapest cost-per-unit that hits the volume tier; backups exist for redundancy and emergencies.

For PCB fab specifically:
**JLCPCB → PCBWay → OSH Park (US-domestic, slow + expensive but sovereign).**

For US-domestic alternatives in general: Bill would prefer them across the board but they don't exist at competitive cost for most lines. Documented per-row.

---

## Component Table

| Component | Primary | Lead time | Backup chain | Risk | Notes |
|---|---|---|---|---|---|
| **LED strip (WS2812B, 32 LEDs)** | Amazon retail (1–50 vol) | 2 days | AliExpress (1–2 weeks, qty 100) → Temu → factory direct (qty 1k) | Low | Highly fungible — same WS2812B die from many vendors. Quality varies; spec is a 60 LED/m strip, IP30. Reel-to-reel inspection at Station 1 catches duds. |
| **1-way mirror acrylic** (150×150×3.1 mm) | Makerstock | 5–7 days | TAP Plastics (US retail) → Inventables → factory laser-cut bulk (China, qty 100+) | Low | 4 panels per 12×12 sheet at $9.95. Pre-cut sheets a luxury — substitutable with bulk acrylic + in-house laser. |
| **2-way mirror acrylic** (same dims) | TT Plastic Land | ~1 week | Plastics 2000 → bulk acrylic import (search terms: "semi-transparent mirror", "see-through mirror") | Low | Multiple suppliers carry the reflective-coating stock; the constraint is cost, not availability. TT Plastic Land is currently the cheapest. Tariff-event mitigation is to absorb the price bump from a US specialty plastics supplier rather than re-source. |
| **Back panel (birch plywood)** | Makerstock | 5–7 days | Home Depot 12×12 craft sheets (lower quality, faster) → Inventables → bulk hardwood supplier | Low | Universally available; quality is the variable, not availability. |
| **Frame (2×4 lumber, 3-sided)** | Home Depot | Same day | Lowes → local lumber yards → big-box anywhere | Low | Pure commodity. Zero supplier risk. Sustainability story is local-and-renewable. |
| **LED controller PCB (fab)** | JLCPCB | 7–10 days | PCBWay (~12 days) → OSH Park (US, ~2 weeks, premium-priced) | Med — tariff | $0.90/board at qty 100 per [LL-011](../tasks.md#LL-011). PCBWay is essentially same-day equivalence on quality. OSH Park is the sovereign last-resort if tariff math turns hostile. |
| **Controller silicon (ESP32-C3-MINI-1)** | LCSC (China) | 2–3 weeks | Mouser → Digi-Key (US-domestic, ~+30% unit cost) → Arrow | Med — tariff | Espressif is the only chip vendor; the resilience is in the distributor layer. Historical shortages (2021-22) have receded but worth tracking. Per [LL-006](../tasks.md#LL-006), C6-MINI-1 is the future Pro+ variant — same distributor chain. |
| **PSU (USB-C PD, wattage TBD)** | TBD pending spec | TBD | Standard USB-C PD compliance means abundant supplier options once spec is set | Low | The current "5V brick" line in the BOM is a placeholder. Once a real PSU spec is pinned (USB-C PD wattage, voltage profile, certification reqs), supplier choice is wide-open commodity. |
| **3D-printed parts** (LED spacer + electronics enclosure, 44 g PETG) | **In-house** (Bill's printer) | 0 (4–6 hr print queue) | Backup printer (single point now — buy a 2nd Bambu when scale demands) | Low | Self-hosted manufacturing — Bill controls capacity. Easy to scale; lends itself to customization (different print files per SKU). |
| **PETG filament** | eSun / Polymaker via Amazon | 2 days | Hatchbox (Amazon) → AliExpress generic | Low | Many fungible brands. Stocking 2–3 spools at all times eliminates lead-time risk. |
| **LED holders / back reflector** | TBD | TBD | TBD | TBD | **Action item** — currently a placeholder line in the BOM. Spec these out (likely 3D-printed, in which case folds into the in-house printed-parts row above). |
| **Hardware (M3 screws, wire, JST connectors)** | McMaster-Carr (US) | 1–3 days | Home Depot retail → AliExpress hardware reels | Low | McMaster is the gold standard for fastener supply in US small-scale fabrication. Premium price, but lead time in days not weeks. |
| **Packaging (cardboard, ~8×8×3 in)** | TBD die-cut vendor | TBD | Laser-cut in-house (Bill — slower per unit, no setup cost) | Med | **Action item** — primary supplier still unknown. Laser-cut is feasible at low volume but die-cut is much faster per unit at scale. Need to source a die-cut packaging vendor. See [LL-051](../tasks.md#LL-051). |

---

## Open Actions

| # | Action | Owner | Driving |
|---|---|---|---|
| 1 | Source a US die-cut cardboard packaging vendor | Bill | [LL-051](../tasks.md#LL-051) packaging concept |
| 2 | Spec the LED-holder + back-reflector parts (likely 3D-printable; folds into in-house manufacturing) | Bill | This map's TBD line |
| 3 | Spec the PSU properly — USB-C PD wattage, voltage profile, certification reqs — so the supplier search converges | Bill | This map's TBD line; gates the cost-tier negotiation |

---

## Parked / Long-Term

- **Domestic manufacturing wishlist.** Bill would prefer US-sourced lines across the board for shipping speed + tariff insulation + sustainability narrative, but the price gap is currently ~30-50% across most categories. Document baseline US options on every row (above) so a future tariff event has the decision pre-made.

---

## Related

- [BOM Breakdown — Basic 6×6](bom-breakdown-basic-6x6.md) — per-unit material + labor costs at 4 volume tiers (this map's input)
- [LL-011](../tasks.md#LL-011) — JLCPCB cost analysis ($0.90/board at qty 100)
- [LL-051](../tasks.md#LL-051) — Packaging concept (will fill in the TBD packaging line)
- [LL-009](../tasks.md#LL-009) — Sustainability audit (frames the domestic-sourcing wishlist)
- [Sprint Plan §Phase 2 / Week 6](../sprint_plan.md) — Milestone 2 deliverable list
