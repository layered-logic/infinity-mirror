# Session Log — March 21, 2026
## Sprint Planning & Project Setup

**Duration:** Initial planning session (pre-quarter)
**Quarter start:** March 30, 2026

---

### Purpose
Establish the 11-week sprint plan for the Spring 2026 independent study quarter and set up project infrastructure.

### Context
The project builds on several existing prototypes and experiments:
- STM8 LED controller with button-driven animations
- React + Three.js infinity mirror 3D visualizer
- ESP32 strip tester (PlatformIO)
- KiCad PCB designs (ESP32-C6 based) with fabricated Gerbers

The independent study takes these building blocks and asks what it would take to turn this into a real, sellable, supportable product.

---

### Key Decisions Made

**1. Project Framing: Human-Centered Design Engineering Venture**
- The project is framed as a *system*, not just a gadget
- Primary emphasis is on business model, HCDE process, and portfolio — not pure engineering
- Engineering runs in parallel but is not the academic focus
- This framing was a deliberate pivot from an earlier ops-heavy draft toward something more human-centered and academically grounded

**2. Product Definition**
- ESP32-based infinity mirror with BLE/Wi-Fi app connectivity
- Base model: 6" x 6" at ~$50
- Primary value differentiator: custom/bespoke designs (upsell on size, complexity, frame material)
- Made-to-order model — no inventory to start, small batch only for proven sellers
- Mobile native app for color/pattern control
- Smart home integrations (HomeKit, Google Home) scoped as future/post-quarter

**3. Sales & Launch Strategy**
- Etsy + personal website (Shopify or similar)
- Kickstarter is on the table but not committed — focus is on having media/assets ready so the option exists
- Coffee shop and maker space partnerships are post-launch, though informal conversations will happen during user research in Weeks 4–5

**4. Sprint Structure (4 Phases, 4 Milestones)**
- Phase 1 (Weeks 1–3): Research & Ethical Foundation
- Phase 2 (Weeks 4–6): Interaction Design & Branding
- Phase 3 (Weeks 7–9): System Hardening & Value Strategy
- Phase 4 (Weeks 10–11): Synthesis & Go/No-Go Decision
- Stretch (post-quarter): Product line expansion

**5. HCDE Deliverables Identified**
- Stakeholder map
- Sustainability / life cycle audit
- Right-to-repair / repairability manual
- Informal user interviews (target customers + coffee shops)
- App wireframes with documented design rationale
- Service blueprint with custom order flow
- Full portfolio case study
- Venture readiness report

**6. Engineering Approach**
- Will use a dev board to prototype app/firmware functionality before custom PCBs arrive
- This allows UX wireframes (Week 5) to be grounded in tested capabilities
- Custom PCBs ordered in Week 3 with ~10-day lead time, arriving around Week 6

---

### Artifacts Produced
| Artifact | Location |
|----------|----------|
| 11-week sprint plan | `sprint_plan.md` |
| Project README | `README.md` |
| GitHub repo | [layered-logic/infinity-mirror](https://github.com/layered-logic/infinity-mirror) |
| This session log | `docs/session-logs/2026-03-21_sprint-planning.md` |

### Infrastructure Set Up
- Created GitHub organization: **layered-logic**
- Created public repo: **layered-logic/infinity-mirror**
- Initial commit with sprint plan and project config
- Stylized README with project overview, milestones, and tech stack

---

### Open Items / To Revisit
- **Faculty advisor** — needs to be added as a collaborator on the repo
- **Mobile app framework** — React Native, Flutter, or native Swift/Kotlin not yet decided
- **Brand name** — "Layered Logic" is the org/personal brand, but the product name is TBD (scheduled for Week 4)
- **Kickstarter** — decision deferred; assets will be built regardless
- **License** — TBD in repo

---

### Next Steps
- Quarter begins March 30
- Week 1 kicks off with competitive landscape analysis and stakeholder mapping
- Select ESP32 module and set up dev board for prototyping
