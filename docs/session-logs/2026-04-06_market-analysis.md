# Session Log — April 6, 2026
## Week 1 Close-Out: Operating Agreement, ESP32 Selection, Market Analysis

**Sprint week:** Week 1 (Mar 30 – Apr 4, carried into Apr 6)

---

### What Happened

**Legal**
- Operating agreement finalized, printed, and signed. Business bank account is now fully unblocked.

**ESP32 Module Selection**
- Researched all current Espressif production modules (21 variants). Full comparison documented in `docs/esp32-module-comparison.md`.
- Selected **ESP32-C3-MINI-1** for v1.0 hardware. Rationale: smallest footprint, cheapest (~$2 LCSC), wide availability, built-in USB Serial/JTAG (no external UART chip needed on PCB). Wi-Fi + BLE 5.0 is sufficient for app control and provisioning. Single-core 160 MHz is not a constraint for LED animation workloads.
- **ESP32-C6-MINI-1 deferred** as a potential v2 SKU (adds Zigbee/Thread/Matter in the same footprint for ~$0.80 more). Decision gated on early customer survey data — survey first buyers on smart home ownership and Zigbee usage before investing in a second SKU.
- Focused strengths/weaknesses analysis documented in `docs/esp32-module-shortlist.md`.

**Market Analysis**
- Conducted competitive landscape research via Gemini (`external_chats/Gemini-Market_Research.json`), cross-referencing 30+ listings across Etsy, Amazon, Pinterest, and commercial signage vendors.
- Output: `docs/Market Analysis Report.md`

---

### Key Findings — Market Analysis

**Market Segmentation (4 Quadrants)**

| Segment | Est. Price Range | Notes |
|---|---|---|
| Bespoke Luxury | $800 – $5,000+ | Glass, aluminum, wood. Gallery/high-income residential. |
| B2B Branding | $250 – $1,200 | Custom firmware, SLA reliability. Bars, restaurants, retail. |
| Sensory/Therapy | $230 – $4,300 | Clinical and special needs. Simplified UI, durability focus. |
| Lifestyle/Gaming | $40 – $250 | 3D-printed/plastic. App-heavy. Crowded. |

**The $50–$120 Bracket Problem**
Over 1,900 Etsy listings for "Infinity Mirror Logos" — from $3.75 (digital files) to $7,860 (large installations). The $50–$120 range is flooded with hobbyist-grade 3D-printed units. The v1.0 base model ($50 6x6) sits at the bottom of this saturated range. Industrial design and finish quality must immediately signal a $200+ value proposition or risk "brand bleed" downward.

**Protocol Fragmentation Risk**
Matter-over-Thread has reached maturity as of 2026. Apple and Google have largely withdrawn native support for non-Matter Wi-Fi accessories in newest OS updates. A Wi-Fi-only app is now flagged as "technical debt" by market analysis. This strengthens the case for the C6 variant (with Zigbee/Thread/Matter) as a future SKU.

**Repairability as Legislation, Not Just Feature**
2026/2027 Right to Repair legislation (EU Directive + WA/CA bills) mandates repairability using standard tools and parts for 7–10 years. The venture's modular/toolless design is a legislative necessity in major markets, not just an HCDE talking point.

**FCC Part 15 — EMI**
High-frequency LED driver switching creates electromagnetic interference even without a radio. Flagged for attention during PCB design. ESP32-C3-MINI-1 module itself is FCC-certified; the custom PCB as a system still needs consideration.

---

### Key Decisions

| Decision | Rationale |
|---|---|
| ESP32-C3-MINI-1 for v1.0 | Size + cost + availability + built-in USB/JTAG |
| C6 Zigbee variant deferred | Validate demand via customer survey before second SKU |
| Target positioning above $120 bracket | $50–$120 is saturated; material quality must signal premium |
| Repairability is non-negotiable | Legislative requirement in key markets, not just portfolio feature |

---

### Artifacts Produced

| Artifact | Path | Committed |
|---|---|---|
| ESP32 module comparison | `docs/esp32-module-comparison.md` | Yes (PR #1) |
| ESP32 module shortlist + decision | `docs/esp32-module-shortlist.md` | Yes (PR #1) |
| Market Analysis Report | `docs/Market Analysis Report.md` | Pending |
| Gemini market research chat | `external_chats/Gemini-Market_Research.json` | Pending |
| Session log | `docs/session-logs/2026-04-06_market-analysis.md` | Pending |

---

### Open Items Carried into Week 2

- [ ] Open business bank account (fully unblocked)
- [ ] ESP32-C3-MINI-1 dev board setup + level-shifter pinout confirmation
- [ ] Week 2: Sustainability audit (HCDE deliverable)
- [ ] Week 2: Detailed BOM at 1 / 10 / 50 units
- [ ] Week 2: PCB schematic start (KiCad)
- [ ] Design: industrial design and finish quality must differentiate from $50–$120 commodity bracket

---

### Sprint Plan Alignment

| Sprint Plan Item | Status |
|---|---|
| Market Analysis / Competitive Landscape | Done |
| LLC Setup: Operating agreement | Done |
| LLC Setup: Business bank account | In progress (unblocked) |
| Engineering: ESP32 module selection | Done |
| Engineering: Dev board setup | Not started |
| Pricing calculator (pulled forward from Week 2) | Done (Apr 2) |
