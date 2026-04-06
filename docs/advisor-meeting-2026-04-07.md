# Advisor Meeting — April 7, 2026
**Project:** Layered Logic / Infinity Mirror Venture
**Student:** William White (Bill)
**Quarter:** Spring 2026 — Week 2 of 11
**Purpose:** Week 1 update, open questions, feedback

---

## Week 1 Accomplishments (Mar 30 – Apr 6)

### Legal & Business Foundation
- **LLC fully operational.** Certificate of Formation filed pre-quarter (Mar 23). EIN received Apr 2. Operating agreement finalized, printed, and signed Apr 6.
- **Next:** Open business bank account (fully unblocked as of today).

### HCDE Deliverables
- **Stakeholder Map complete.** 45-minute coached exercise produced a 5-phase stakeholder map covering 24 stakeholders across Creation, Distribution, Acquisition, Lived Experience, and Support/Legacy. Documented in `docs/stakeholder-map.md`.
- **Market Analysis complete.** Cross-platform competitive landscape across 30+ listings (Etsy, Amazon, Pinterest, commercial signage). Documented in `docs/Market Analysis Report.md`.

### Engineering
- **ESP32 module selected: ESP32-C3-MINI-1.** Chosen for smallest footprint, lowest cost (~$2/unit at volume), wide availability, and built-in USB/JTAG (eliminates a discrete chip from the PCB design). Full module comparison documented.
- **Pricing calculator built.** Streamlit dashboard with dynamic BOM, size scaling, platform fee calculation, and margin analysis at 1/10/50 unit tiers. (`tools/pricing/`)

---

## Key Decisions Made This Week

| Decision | Rationale |
|---|---|
| ESP32-C3-MINI-1 for v1.0 | Best size/cost/availability balance. Wi-Fi + BLE sufficient for app control. |
| Zigbee variant deferred | Survey first customers on smart home usage before committing to a second SKU. |
| Cash basis accounting | Simpler for solo founder; switch to accrual if/when needed. |
| Made-to-order only (no inventory) | Minimizes financial risk before product-market fit is validated. |
| 30–60 day return policy | Standard DTC; rework strategy limits loss on custom returns. |

---

## Market Analysis — Key Findings

The market is bifurcated into four segments:

| Segment | Price Range | Opportunity |
|---|---|---|
| Bespoke Luxury | $800 – $5,000+ | High margin, low volume, gallery/residential |
| B2B Branding | $250 – $1,200 | Coffee shops, bars, retail — recurring relationship |
| Sensory/Therapy | $230 – $4,300 | Clinical, special needs — specialized but underserved |
| Lifestyle/Gaming | $40 – $250 | Saturated. Commodity race. |

**The critical finding:** The $50–$120 bracket has over 1,900 Etsy listings. The v1.0 base model at $50 sits directly in the most crowded tier. Industrial design and material quality must immediately signal a $200+ value proposition — otherwise the product risks being perceived as another hobbyist listing.

**Regulatory flags identified:**
- FCC Part 15 — LED driver switching creates EMI even without a radio. Will need EMC consideration during PCB design.
- Right to Repair legislation (EU Directive + WA/CA bills) — mandates repairability using standard tools/parts for 7–10 years. The modular/toolless design approach is a legislative necessity in key markets, not just an HCDE talking point.
- Matter/Thread protocol maturity — Apple and Google have largely withdrawn native support for non-Matter Wi-Fi accessories. A Wi-Fi-only app may face ecosystem headwinds. This informs the deferred C6 (Zigbee/Thread/Matter) SKU decision.

---

## Week 2 Plan (Apr 7 – Apr 11)

| Task | Type | Notes |
|---|---|---|
| **Sustainability Audit** | HCDE Deliverable | Full product life cycle — materials, end-of-life, repairability, recyclability. Portfolio artifact. |
| **BOM at 1 / 10 / 50 units** | Business | Detailed cost breakdown with margin analysis at each volume tier. |
| **PCB schematic (start)** | Engineering | Begin KiCad schematic. Finalize level-shifter approach for 5V NeoPixel logic from 3.3V ESP32. |
| **Open business bank account** | Legal/Ops | Carry-over from Week 1. Fully unblocked. |

---

## Open Questions — Advisor Input Requested

### 1. Pricing & Positioning
The market analysis suggests the $50 base model sits in a dangerously saturated bracket. The data points toward positioning at $120+ as the floor for a "serious" product.
- **Q:** Should the base model price be reconsidered now, before brand and marketing assets are built around $50? Or is the $50 entry point a deliberate customer acquisition strategy?
- **Q:** Is there value in explicitly targeting the B2B branding segment (coffee shops, bars) as the primary channel, rather than DTC? The margin and relationship model looks more defensible there.

### 2. HCDE Deliverable Scope
The sprint plan has a sustainability audit in Week 2, but the market analysis revealed that repairability legislation makes this more than an academic exercise — it's a compliance and product design requirement.
- **Q:** Should the sustainability audit and repairability documentation be treated as a single integrated deliverable, or kept separate? The Right-to-Repair documentation is currently planned for Week 4.
- **Q:** Are there specific sustainability frameworks or methodologies you'd recommend for the life cycle audit? (LCA, cradle-to-gate, etc.)

### 3. FCC Compliance Path
The venture uses a pre-certified ESP32 module, which simplifies the end-product certification path. But the custom PCB as a complete system may still require testing.
- **Q:** At what stage in the project should FCC compliance testing be formally scoped? Is this a Week 3 (pre-PCB-order) conversation, or post-prototype?
- **Q:** Is there UW HCDE or CoE guidance on navigating regulatory compliance for student venture projects?

### 4. UW Resources & Commercial Use
The project may eventually need access to larger fabrication equipment (The Mill's laser cutters, large-format 3D printers) for prototype builds or larger custom orders.
- **Q:** What is the UW policy on using The Mill for a project that is simultaneously an independent study and a real business entity (Layered Logic LLC)? Is there a threshold where "student project" becomes "commercial use"?

### 5. Kickstarter / Launch Timing
The sprint plan includes Kickstarter readiness as a Week 11 deliverable — assets and copy ready, but not committed to launching.
- **Q:** From an HCDE portfolio standpoint, does a "launch-ready but not launched" state satisfy the venture readiness deliverable? Or is there value in doing a limited soft launch during the quarter?

---

## Upcoming Milestones

| Date | Milestone |
|---|---|
| **Apr 18** | Milestone 1 — Business foundation locked, PCBs on order |
| **May 9** | Milestone 2 — Brand + app UI + hardware validated |
| **May 30** | Milestone 3 — Go-to-market ready + golden sample |
| **Jun 12** | Milestone 4 — Portfolio complete + venture readiness |

**Milestone 1 is 11 days out.** Remaining items: BOM finalized, sustainability/ethics goals documented, competitive analysis done (✓), PCBs ordered.

---

## Repository
All project documentation, sprint plan, and code are tracked at:
`https://github.com/layered-logic/infinity-mirror`

Sprint plan: `sprint_plan.md` | Progress log: `sprint_log.md`
