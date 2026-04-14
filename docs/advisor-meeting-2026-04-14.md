# Advisor Meeting — April 14, 2026
**Project:** Layered Logic / Infinity Mirror Venture
**Student:** William White (Bill)
**Quarter:** Spring 2026 — Week 3 of 11
**Purpose:** Week 2 + 3 update, open questions, Milestone 1 check-in

---

## Since Last Meeting (Apr 7 → Apr 14)

### Week 2 Completed Deliverables

**Sustainability Audit** (`docs/sustainability-audit.md`)
- Full 7-stage cradle-to-grave lifecycle analysis using 6,368 smart plug samples (48 hrs)
- Power profile: idle 0.1W | rainbow cycling 1.25W avg | full white 2.8W
- Embodied carbon ~2.5–2.7 kg CO2 cradle-to-door; annual use ~1.89 kg CO2/yr
- LED lifespan projected 14+ years at actual drive levels (29% of rated max)
- Key v1 limitation: metallic-coated acrylic panels are non-recyclable as PMMA — flagged for v2 (glass investigation)
- PSU oversizing flagged (25W brick at 11% load) — v2 right-sizing to 5–7W

**BOM Breakdown** (`docs/bom-breakdown-basic-6x6.md`)
- Full material + labor + fixed cost model at 1 / 50 / 100 / 1,000 units
- Variable COGS: $81.62 (1 unit) → $49.80 (1,000 units); labor exceeds materials at every volume
- **Pricing decision: $65 recommended base retail** (hits $60/hr target at 50–100 units; $50 only hits it at 1,000)
- Fixed capital: $7,153.11 (laser + printer + tooling)

**PCB Cost Analysis**
- JLCPCB assembled estimate for STM8 basic controller: $3.61 (qty 5) → $0.90/board (qty 100)
- TTP223 capacitive touch sensor evaluated as PCB add-on: ~$0.10/board all-in
- ESP32 PCB BOM analysis deferred (still pending)

**Business Banking**
- Business bank account application submitted (NAICS 335129, calendar fiscal year)

### Week 3 — Operations Planning (Today)

- Completed full factory/assembly planning session: 3 production lanes, 20-station production line (S01–S20), garage workshop layout, Phase 1 tool list
- Artifacts produced: `Assembly_docs/LED_Mirror_Station_Details.csv`, `LED_Mirror_Assembly_Calculator.xlsx`, `LED_Mirror_Project_Brief.md`
- **Key finding:** Two solder operations per unit (LED connector + controller PCB) are the first scale bottleneck; PCB assembly outsourcing flagged as the first transition point at volume

---

## Milestone 1 Status — Due Apr 18 (4 days)

| Milestone 1 Checklist | Status |
|---|---|
| Competitive analysis | Done (Apr 6) |
| Stakeholder map | Done (Apr 2) |
| Pricing model | Done (Apr 9) |
| Sustainability / ethics goals documented | Done (Apr 9) |
| Business foundation (LLC, EIN, bank) | Done — bank account approved Apr 14 |
| **PCBs on order** | **Not yet — critical path** |

PCB order is the only remaining Milestone 1 item. Blocker: basic controller schematic exists in KiCad but PCB layout review hasn't happened. ESP32 schematic not started.

---

## Open Questions — Advisor Input Requested

### 1. Solder Outsourcing — When Does It Make Sense?
Two solder operations per unit were flagged as the first bottleneck at scale. A PCB assembly (PCBA) house could take over the controller solder work. This changes the BOM structure (labor moves from in-house to COGS line item).

**Q:** At what volume or stage should I formally evaluate the PCBA outsourcing option? Is this a Week 6 (supply chain) question, or does it affect the PCB design decisions I'm making now (e.g., all-SMD vs. mixed SMD/through-hole)?

### 2. Assembly Time Estimates vs. Reality
All time estimates in the assembly calculator are guesses. The BOM's 50 min/unit labor model was built before the 20-station line existed — they may not be consistent.

**Q:** Is there a recommended approach for getting real time data before volume ramps? (e.g., time 3 builds with a stopwatch and use the worst case, or use standard industrial time-study methods?) How much does this matter for the portfolio vs. just for my own planning?

### 3. Operations Docs as HCDE Portfolio Artifacts
The assembly planning produced detailed operational documents (station-level detail, garage layout, tool specs) that feel more "manufacturing operations" than "human-centered design."

**Q:** Are these appropriate as HCDE portfolio artifacts, or should they stay in the business/ops layer? What lens (if any) reframes a 20-station production line as an HCDE deliverable?

### 4. Packaging Spec
Packaging is still an open line item — no dimensions, no insert design, no supplier quote. Affects BOM accuracy and the "unboxing experience" as a UX touchpoint.

**Q:** For a $65 product at 1–100 units, what's your take on the right packaging investment level? (Minimal/sustainable kraft box vs. premium insert vs. custom mailer?) This feels like a brand decision as much as a cost decision, and brand work is still two weeks out.

---

## Week 3 Remaining Plan (Apr 14 – Apr 18)

| Task | Priority | Notes |
|---|---|---|
| Place PCB order | Critical path — Milestone 1 | STM8 basic controller; review layout before submitting |
| ESP32 PCB BOM refresh | High | Needed for Milestone 1 pricing accuracy |
| Packaging cost quote | Medium | Needed for BOM completeness |
| Acrylic volume pricing | Medium | Carry-over from Week 2 |
| Begin KiCad schematic (ESP32) | Medium | Can't order until this is done |
| TTP223 touch sensor decision | Low | Include in basic PCB or use physical button |

---

## Upcoming Milestones

| Date | Milestone |
|---|---|
| **Apr 18** | Milestone 1 — Business foundation locked, PCBs on order |
| **May 9** | Milestone 2 — Brand + app UI + hardware validated |
| **May 30** | Milestone 3 — Go-to-market ready + golden sample |
| **Jun 12** | Milestone 4 — Portfolio complete + venture readiness |
