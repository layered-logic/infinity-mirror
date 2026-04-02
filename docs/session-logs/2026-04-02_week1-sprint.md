# Session Log — April 2, 2026
## Week 1 Sprint: LLC Finalization, Pricing Tool, & Stakeholder Map

**Duration:** Full working session
**Sprint week:** Week 1 (Mar 30 – Apr 4)

---

### What Happened

**LLC & Legal**
- EIN confirmation received from IRS (41-5282028, CP-575 G letter). Marked complete in sprint plan.
- Organized business documents into gitignored `Business/` folder (certificate of formation, EIN letter, expense tracking CSVs, cost estimate CSVs).
- Drafted full operating agreement outline with all details populated from Certificate of Formation and EIN letter — member name, address, EIN, UBI number, registered agent, formation date, etc.
- Created Schedule A (initial capital contribution) itemizing all pre-formation expenses with conservative business-use allocations for dual-use equipment. Total contributed value: $5,959.21.
- Decisions made: cash basis accounting, successor designation via will.

**Pricing Calculator**
- Built a Streamlit-based pricing dashboard (`tools/pricing/`) as the first piece of software in the repo.
- Features: rectangular mirror sizing (W x H sliders), basic/pro tier toggle, Etsy/Shopify platform fee calculation, configurable markup, full BOM breakdown table with per-component costs, sheet utilization optimization across multiple suppliers, waterfall cost chart.
- Data model supports three scaling types (fixed, perimeter, area) and automatically finds the cheapest supplier option for each component.
- Validated against existing CSV cost estimates — 6x6 basic materials ~$24, 6x6 pro materials ~$73 (close to CSV targets of $26/$75).
- Created `Business/pricing_config.yaml` as the single source of truth for all component data, supplier URLs, and platform fees.

**Stakeholder Mapping**
- Reviewed exported Gemini chat (`external_chats/`) from a 45-minute stakeholder mapping exercise where Gemini acted as an investor/board member.
- Synthesized the session into a formal stakeholder map document (`docs/stakeholder-map.md`) with 5 lifecycle phases and 24 identified stakeholders.
- Added stakeholders not covered in the original session: digital platform stakeholders (Shopify/Etsy, payment processors, shipping carriers), content creators/reviewers, product liability insurance, and the dissatisfied customer/returns pathway.
- Documented safety & accessibility considerations (no sharp edges, no hot surfaces, photosensitivity controls, color vision independence).
- Included a stakeholder influence matrix and a phased open questions/research agenda.

---

### Key Decisions

| Decision | Rationale |
|----------|-----------|
| Cash basis accounting | Simpler for a solo founder; can switch to accrual later if needed |
| Successor via will | Keeps operating agreement clean; actual designation lives in estate planning |
| Streamlit for pricing tool | Python already available, instant dashboard UI, no frontend build pipeline |
| 30-60 day return policy | Standard for DTC; rework strategy minimizes loss on custom returns |
| Strobing disabled on showroom units | Photosensitivity safety for public spaces |
| Open-source commitment (future) | All files/firmware to be open-sourced before founder steps away, but not prematurely |

---

### Artifacts Produced

| Artifact | Path | Committed |
|----------|------|-----------|
| Operating agreement outline | `Business/operating_agreement_outline.md` | No (gitignored) |
| Pricing config | `Business/pricing_config.yaml` | No (gitignored) |
| Pricing calculator | `tools/pricing/` | Yes |
| Stakeholder map | `docs/stakeholder-map.md` | Pending |
| Session log | `docs/session-logs/2026-04-02_week1-sprint.md` | Pending |

---

### Open Items Carried Forward

- [ ] Finalize and sign operating agreement
- [ ] Open business bank account (EIN now available)
- [ ] Market analysis / competitive landscape (Week 1 sprint plan item)
- [ ] ESP32 module selection and dev board setup (Week 1 engineering item)
- [ ] Power consumption testing with monitoring plugs (added to research agenda)
- [ ] Product liability insurance research (before first sale or public placement)
- [ ] Phase 2 of pricing calculator: live web scraping for supplier prices

---

### Sprint Plan Alignment

| Sprint Plan Item | Status |
|-----------------|--------|
| Stakeholder Mapping | Done |
| LLC Setup: EIN | Done |
| LLC Setup: Operating agreement | In progress (outline complete, needs signing) |
| LLC Setup: Business bank account | Not started (unblocked by EIN) |
| Market Analysis | Not started |
| Engineering: ESP32 selection | Not started |
| Pricing calculator (pulled forward from Week 2) | Done |
