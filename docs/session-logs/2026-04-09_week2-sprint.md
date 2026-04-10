# Session Log — April 9, 2026
## Week 2 Sprint Session — Pricing, Sustainability & PCB Analysis

**Duration:** Full working session
**Sprint week:** Week 2 (Apr 7 – Apr 11): Pricing & The Sustainability Audit

---

### What Happened

**Business Banking**
- Completed business bank account application. Key inputs resolved during session:
  - Fiscal/tax year end: 12/31 (calendar year, standard for single-member LLC)
  - NAICS code: 335129 (Other Lighting Equipment Manufacturing) — primary; 541490 (Specialized Design Services) noted as secondary if consulting becomes significant
  - Business established: 03/2026, organized in Washington state
- Logged in sprint_log.md. Week 1 is now fully closed out — no remaining open items.

**Sustainability Audit**
- Measured full power profile via smart plug export (Home Assistant, 6,368 samples over ~48 hours):
  - Idle/standby: **0.1W**
  - Rainbow cycling (typical): **1.25W avg** (1.2–1.4W range)
  - Full white (firmware 80% cap): **2.8W**
  - Theoretical max (60mA/LED spec): 9.6W — never reached in practice
- Firmware drives LEDs at ~17.5mA (29% of rated 60mA). Thermal stress very low; real-world lifespan expected to exceed 50,000-hour spec (14+ years at typical use).
- Collected and documented physical specs: 150×150×3.1mm PMMA panels (169g total), 44g PETG printed parts (two pieces), wood frame (~300g), cardboard packaging.
- Computed embodied carbon across all lifecycle stages. Largest hotspots: PSU (~600g CO2), acrylic panels (~593g CO2), LEDs + PCB (~500g CO2 combined).
- Total cradle-to-door: **~2.5–2.7 kg CO2**. Annual use (typical): **1.89 kg CO2/yr**.
- Produced **`docs/sustainability-audit.md`** — full 7-stage lifecycle analysis, hotspot summary, and design commitments. Portfolio-ready artifact.
- Key EOL limitation identified: metallic-coated acrylic panels are non-recyclable as PMMA. Flagged for v2 (glass panel investigation). PSU oversizing (25W at 11% load) flagged for v2 right-sizing to 5–7W.

**BOM Breakdown**
- Built full material + labor + fixed cost model for the basic 6×6 at 1 / 50 / 100 / 1,000 units.
- Labor model: 50 min/unit at 1 unit (3 panels × 10 min laser + 10 min assembly + 10 min 3D printer setup), 30 min/unit at 1,000. Rate: $60/hr.
- Fixed capital total: $7,153.11 (laser $4,795.83 + 3D printer $1,500 + ancillary tooling).
- Variable COGS: $81.62 (1 unit) → $49.80 (1,000 units). Labor exceeds materials at every volume tier.
- **Pricing finding:** $50 retail only reaches $60/hr effective rate at 1,000 units. **$65 recommended as base price** for 50–100 unit early volumes ($57–70/hr effective).
- Produced **`docs/bom-breakdown-basic-6x6.md`**.

**JLCPCB PCB Cost Analysis**
- Analyzed `BOM-Infinity_Mirror_basic.csv` (STM8-based basic controller PCB):
  - 7 component lines, 9 total parts, 40 solder joints
  - Extended parts (add $3 setup each): STM8S003F3P (C52717), USB-C 6P (C456012)
  - Parts cost per board: $0.308
- JLCPCB assembled cost: **$3.61/board (qty 5)**, **$1.04/board (qty 50)**, **$0.90/board (qty 100)**
- Updated `Business/pricing_config.yaml`: basic controller from $5.00 placeholder → $1.00 at volume
- ESP32 PCB BOM pending refresh — analysis deferred

**TTP223 Capacitive Touch Sensor (evaluated)**
- Back-of-napkin add-on cost: **~$0.10/board all-in** (IC $0.08, sensitivity cap $0.01, 8 extra joints $0.014)
- JLCPCB basic part — no $3 extended setup fee
- Senses through PETG enclosure at 1–2mm without issue (copper pour pad, no discrete sensor component)
- Decision pending: basic model touch vs. physical button

---

### Key Decisions

| Decision | Rationale |
|---|---|
| $65 recommended base retail price | $50 only reaches $60/hr target rate at 1,000 units; $65 hits target at 50–100 units |
| NAICS 335129 for bank account | Manufacturer classification; consulting (541490) is secondary |
| PSU right-sizing flagged for v2 | 25W brick at 11% load — 5–7W brick reduces material, improves efficiency |
| Acrylic panel EOL gap accepted for v1 | Metallic coating prevents PMMA recycling; glass panel option deferred to v2 investigation |
| Basic controller placeholder updated | $5.00 → $1.00 in pricing_config based on JLCPCB analysis |

---

### Artifacts Produced

| Artifact | Path | Status |
|---|---|---|
| Sustainability audit (7-stage lifecycle) | `docs/sustainability-audit.md` | Complete |
| BOM breakdown (basic 6×6, 4 volume tiers) | `docs/bom-breakdown-basic-6x6.md` | Complete |
| pricing_config.yaml controller update | `Business/pricing_config.yaml` | Updated |
| Sprint log Week 1 + Week 2 updates | `sprint_log.md` | Updated |
| Session log | `docs/session-logs/2026-04-09_week2-sprint.md` | This file |

---

### Open Items Going into Rest of Week 2

- [ ] ESP32 PCB BOM — refresh and run JLCPCB analysis
- [ ] Packaging cost — get actual cardboard box quote
- [ ] Acrylic volume pricing — email Canal Plastic / TT Plastic Land for 50-sheet quote
- [ ] Basic controller — finalize part selection (STM8 vs. something simpler)
- [ ] TTP223 touch sensor — decide: include in basic PCB or use physical button
- [ ] Begin KiCad schematic (engineering track)

---

### Sprint Plan Alignment

| Sprint Plan Item | Status |
|---|---|
| Detailed BOM at 1 / 10 / 50 units | Done (ran at 1 / 50 / 100 / 1,000) |
| Margin analysis at each tier | Done |
| Sustainability audit (HCDE deliverable) | Done |
| Engineering: finalize schematic | Not started |
| Engineering: start PCB layout in KiCad | Not started |
| Business bank account | Done |
