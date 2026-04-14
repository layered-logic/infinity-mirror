# Session Log — April 14, 2026
## Week 3 Kickoff — Factory Operations Planning

**Duration:** Full working session (external Claude session + sync)
**Sprint week:** Week 3 (Apr 14 – Apr 18): Operations & Critical Path

---

### What Happened

**Factory Operations Planning**
- Completed a full assembly and factory planning session (external Claude session). All outputs landed in `Assembly_docs/`.
- Mapped the made-to-order workflow end-to-end: 3 parallel production lanes feeding into final assembly, 20 numbered stations (S01–S20), from lumber receiving through pack & ship.
- Produced `Assembly_docs/LED_Mirror_Station_Details.csv` — one row per station, fully populated with tools, fixtures, consumables, inputs, outputs, QC criteria, and operational notes.
- Produced `Assembly_docs/LED_Mirror_Assembly_Calculator.xlsx` — three-sheet time model: Time Standards, Unit Calculator, Batch Planner. All values are estimates pending real unit timing.
- Produced `Assembly_docs/LED_Mirror_Project_Brief.md` — full context summary of the session (decisions, layout, tool list, open questions).

**Key Decisions Locked**

| Decision | Choice | Rationale |
|---|---|---|
| Lane structure | 3 parallel lanes (Frame, Mirrors, Electronics) | Sub-assemblies can progress independently; no serial blocking |
| Mirror retention method | Milled channels in frame | More repeatable than spacer layup; reduces per-unit variability |
| Mirror blank strategy | Pre-cut 6x6 blanks in inventory; custom cut per order | Decouples batch cutting cadence from order intake |
| Build strategy | Batch by operation, not one unit at a time | Pays setup cost once per session, not per unit |
| LED pre-test timing | At receiving (roll level) | Catching dead strips mid-build is a major disruption |
| 3D printed parts supply | One overnight batch run per week | Reactive per-order printing creates daily bottlenecks |
| Traveler card | One physical card per unit through all 20 stations | Cheapest way to catch skipped steps before they ship |
| Controller stock | Bulk order with conservative reorder point | 5-day China lead time must never be the constraint |

**Garage Workshop Layout Defined**
- Single-car garage (~20×20ft)
- Wood dust zone (miter saw, router table) on one end; electronics bench on the opposite end
- Pack & ship fully isolated from all production — dust contamination of finished goods is a real failure mode
- Mirror flat storage is most awkward constraint — vertical, padded, away from cutting zone
- Electronics bench is the cleanest, best-lit zone; anti-static mat and fume extractor always on

**Phase 1 Tool List Confirmed**
- Miter saw: Dewalt 12" sliding
- Router table: Kreg or Bosch benchtop
- Handheld router: Dewalt 1-3/4hp
- Wet tile saw: 7" benchtop diamond blade
- Soldering iron: Hakko FX-888D (non-negotiable for daily solder work)
- Bench power supply: 0–30V, 5A
- Multimeter: Fluke 115
- Fume extractor with filter
- Shop vac with auto-on

**Solder Bottleneck Flagged**
- Two distinct solder operations per unit: (1) LED connector pads on flex strip (S11), (2) controller PCB through-hole and pad work (S15)
- At low volume: manageable for one operator
- At scale: first hire or first outsource — PCB assembly house for controller solder is a viable option

**Sprint Log Updated**
- Week 3 Done section added with full assembly planning summary.

---

### Key Artifacts Produced

| Artifact | Path | Status |
|---|---|---|
| 20-station production line detail | `Assembly_docs/LED_Mirror_Station_Details.csv` | Complete |
| Per-unit time calculator | `Assembly_docs/LED_Mirror_Assembly_Calculator.xlsx` | Complete (estimates) |
| Operations planning context brief | `Assembly_docs/LED_Mirror_Project_Brief.md` | Complete |
| Sprint log Week 3 update | `sprint_log.md` | Updated |
| Session log | `docs/session-logs/2026-04-14_week3-sprint.md` | This file |

---

### Open Items Going into Rest of Week 3

**Carried over from Week 2:**
- [ ] ESP32 PCB BOM — refresh and run JLCPCB analysis
- [ ] Packaging cost — get actual cardboard box quote
- [ ] Acrylic volume pricing — email Canal Plastic / TT Plastic Land for 50-sheet quote
- [ ] Basic controller — finalize (STM8 vs. simpler alternative)
- [ ] TTP223 touch sensor — decide: include in basic PCB or use physical button
- [ ] Begin KiCad schematic (engineering track)

**New from assembly planning:**
- [ ] Confirm LED strip spec (density, voltage, chipset) — affects cut length and firmware flash config
- [ ] Confirm controller PCB BOM — determines exact solder operations at S15
- [ ] Define packaging spec — box dimensions, foam insert, unboxing experience
- [ ] Define reorder point formula for controllers based on actual daily run rate
- [ ] **PCB order — critical path. Must be placed by Apr 18 (Milestone 1 deadline)**

---

### Sprint Plan Alignment

| Sprint Plan Item | Status |
|---|---|
| Map made-to-order workflow | Done |
| Document assembly time per unit | Done (estimates; real timing pending) |
| Document tools required and workspace needs | Done |
| Engineering: order PCBs | Not yet — must happen by Apr 18 |
