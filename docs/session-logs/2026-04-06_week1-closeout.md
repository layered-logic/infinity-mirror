# Session Log — April 6, 2026
## Week 1 Close-Out Session

**Duration:** Full working session
**Sprint week:** Week 1 (Mar 30 – Apr 4, carried into Apr 6)

---

### What Happened

**Legal**
- Operating agreement finalized, printed, and signed. Logged in sprint_log.md. Business bank account is now fully unblocked — no remaining blockers.

**ESP32 Module Research & Selection**
- Researched all 21 current Espressif production modules. Full feature-by-feature comparison built across 4 tables (identity/cost/dimensions, radio features, peripherals, use cases/availability). Documented in `docs/esp32-module-comparison.md`.
- Narrowed to 6 shortlisted modules relevant to the project constraints (small, cheap, available, Wi-Fi + BLE, Zigbee as bonus). Strengths/weaknesses analysis for each documented in `docs/esp32-module-shortlist.md`.
- **Selected: ESP32-C3-MINI-1** for v1.0. Rationale: smallest footprint (13.2 × 16.6 mm), lowest cost (~$2 LCSC), wide availability, built-in USB Serial/JTAG (eliminates discrete UART chip from PCB design). Wi-Fi + BLE 5.0 is sufficient for app control and provisioning.
- **ESP32-C6-MINI-1 deferred** as a potential v2 SKU — adds Zigbee/Thread/Matter in the same footprint for ~$0.80 more. Decision gated on early customer survey data (smart home ownership, Zigbee device usage). Noted as an HCDE-aligned decision: validate demand before speculative feature investment.

**Market Analysis**
- Reviewed Gemini market research session export (`external_chats/Gemini-Market_Research.json`). Session covered competitive landscape across Etsy, Amazon, Pinterest, and commercial signage vendors (30+ listings).
- Synthesized into `docs/Market Analysis Report.md`.
- Key findings logged in sprint_log.md and session log.

**Advisor Meeting Prep**
- Built `docs/advisor-meeting-2026-04-07.md` — serves as meeting addenda, update log, and open questions document for Week 2 advisor meeting.

**Git / Repo**
- PR #1 merged (ESP32 research, operating agreement update).
- Local master synced via `git pull`.
- Worktree rebased onto updated master.
- Market analysis files committed and pushed as PR #2.
- Advisor meeting doc and this session log are staged for next commit.

---

### Key Decisions

| Decision | Rationale |
|---|---|
| ESP32-C3-MINI-1 for v1.0 | Best size/cost/availability balance; built-in USB/JTAG simplifies PCB |
| C6 Zigbee variant deferred | Validate demand via early customer survey before committing to second SKU |
| $50 base price flagged for review | Market analysis shows $50–$120 bracket has 1,900+ Etsy listings; need to discuss with advisor |
| Repairability as compliance, not just HCDE feature | EU + WA/CA Right to Repair legislation makes modular design a legal requirement in key markets |

---

### Artifacts Produced

| Artifact | Path | Status |
|---|---|---|
| ESP32 full module comparison | `docs/esp32-module-comparison.md` | Committed (PR #1) |
| ESP32 shortlist + decision | `docs/esp32-module-shortlist.md` | Committed (PR #1) |
| Market Analysis Report | `docs/Market Analysis Report.md` | Committed (PR #2) |
| Gemini market research export | `external_chats/Gemini-Market_Research.json` | Committed (PR #2) |
| Market analysis session log | `docs/session-logs/2026-04-06_market-analysis.md` | Committed (PR #2) |
| Advisor meeting doc | `docs/advisor-meeting-2026-04-07.md` | Pending |
| Week 1 close-out session log | `docs/session-logs/2026-04-06_week1-closeout.md` | Pending |

---

### Open Items Going into Week 2

- [ ] Open business bank account
- [ ] Advisor meeting Apr 7 — get input on pricing floor, sustainability scope, FCC timing, UW Mill policy, Kickstarter framing
- [ ] Sustainability audit (HCDE deliverable — Week 2 primary task)
- [ ] BOM at 1 / 10 / 50 units with margin analysis
- [ ] ESP32-C3-MINI-1 dev board setup; confirm level-shifter pinout for 5V NeoPixel logic
- [ ] Begin KiCad schematic

---

### Sprint Plan Alignment

| Sprint Plan Item | Status |
|---|---|
| Market Analysis / Competitive Landscape | Done |
| Stakeholder Mapping | Done |
| LLC: EIN | Done |
| LLC: Operating agreement | Done |
| LLC: Business bank account | In progress (unblocked) |
| Engineering: ESP32 module selection | Done |
| Engineering: Dev board setup | Not started |
| Pricing calculator | Done (pulled forward from Week 2) |
