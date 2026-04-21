---
title: Spring 2026 Sprint Plan
type: planning
quarter: Spring 2026
status: active
tags: [sprint, plan, milestones]
---

# Infinity Mirror — Spring 2026 Sprint Plan
## Human-Centered Design Engineering Venture
**Quarter:** March 30 – June 12, 2026 (11 Weeks)

**Thesis:** Design a *system*, not just a gadget — an infinity mirror product where the human-centered design process, sustainability ethics, and venture readiness are as rigorous as the engineering.

**End State:** "Flip the switch" readiness. By June 12, every system (product, brand, operations, media, support) is in place to begin selling. The quarter's work is packaged as a professional portfolio case study.

---

## Phase 1: Research & Ethical Foundation (Weeks 1–3)
**Goal:** Move from "tinkering" to "design strategy" by defining the values and stakeholders.

### Week 1 (Mar 30 – Apr 4): Competitive Landscape & Stakeholders
- **Market Analysis:** Who is selling LED art? What are the price points and "vibes" (Etsy handmade vs. high-end gallery vs. Amazon commodity)?
- **HCDE Task: Stakeholder Mapping.** Beyond the buyer — who interacts with this product? Housemates, gift recipients, installers, the waste stream at end-of-life. Map every stakeholder and their relationship to the product.
- **LLC Setup:** Apply for EIN (IRS online, immediate). Open business bank account. Draft single-member operating agreement. File BOI Report with FinCEN (required within 90 days of formation — **deadline ~Jun 21**).
- **Engineering:** Select specific ESP32 module; confirm pinout for level-shifter (5V Neopixel logic). Set up dev board for functional prototyping.

### Week 2 (Apr 7 – Apr 11): Pricing & The Sustainability Audit
- **Business:** Detailed BOM breakdown at 1 / 10 / 50 units. Margin analysis at each tier.
- **HCDE Task: Sustainability Audit.** Document the full life cycle of the product. Can the LEDs be replaced? Is the frame recyclable? What's the end-of-life plan? This justifies the "Human-Centered Venture" angle and becomes a portfolio artifact.
- **Engineering:** Finalize schematic; start PCB layout in KiCad.

### Week 3 (Apr 14 – Apr 18): Operations & Critical Path
- **Operations:** Map the made-to-order workflow (Order → Build → QC → Package → Ship). Document assembly time per unit, tools required, workspace needs.
- **Engineering: Critical Path Milestone.** Order PCBs (aim for 10-day turnaround).

> **MILESTONE 1 (Apr 18):** Business foundation locked — competitive analysis, stakeholder map, pricing model, sustainability/ethics goals documented. PCBs on order.

---

## Phase 2: Interaction Design & Branding (Weeks 4–6)
**Goal:** Design the "Interface" (the App) and the "Identity" (the Brand). Ground design decisions in real user input.

### Week 4 (Apr 21 – Apr 25): Brand Identity & Repairability
- **Brand:** Finalize product name, logo, and visual language.
- **HCDE Task: Right-to-Repair Documentation.** Draft the "User Repair Guide." If a wire breaks or an LED burns out, how does the user fix it? This is a core HCDE artifact that demonstrates pro-consumer values.
- **User Research: Begin informal interviews** (3–5 people). Talk to target customers — what do they value in home decor? How do they feel about customization? What would they pay? Also begin conversations with local coffee shops to understand how they choose artists/makers to showcase.
- **Engineering:** Begin ESP32 firmware work — greenfield proprietary firmware rewrite on the ESP32-C3, with a companion app to pair with it. The existing V0 prototype's ESPHome YAML (`Firmware/ESP32/prototype_ESPHome_Version.yaml`) serves as a behavioral spec reference — feature parity target, not a port source. Prototype board will be fully reflashed.

**Wednesday (Apr 23) — Focus work day.** Compliance + engineering block:
- **BOI Report** filing with FinCEN (boiefiling.fincen.gov) — required within 90 days of LLC formation (deadline ~Jun 21; doing early)
- **Sales tax permit** — WA Dept. of Revenue registration, needed before first Etsy/Shopify sale
- **ESP32 firmware kickoff** — greenfield rewrite across three components: proprietary firmware + webapp + mobile app. The V0 prototype's ESPHome YAML is a feature-parity checklist only, not a port source or a reference architecture. Design principles locked Apr 20:
  - **Works out of the box with no required smart-home connection.** Smart-home integration is opt-in polish, not a gate on basic product function.
  - **App is the pattern editor.** Default patterns ship with firmware; users create and modify patterns through the app.
  - **Configurable LED count** — shipping product is the 6×6 (32 LEDs); the 12×12 (66 LEDs) is a test prototype used for firmware development, not a product SKU.
  - **Default color updates to locked brand palette** (Violet Signal `#A020D0` expected, confirm Wed).
  - **Closed-binary firmware** with self-hosted OTA + reflash-service backstop (per right-to-repair philosophy §3, §4).

  Parked for Wed Apr 23 architecture block: language/framework choice, provisioning UX pattern, smart-home integration path (ESP32-C6 Zigbee/Thread/Matter native vs. ESP32-C3 + individual integrations — not solving Wed, just flagging), OTA signed-binary architecture, app protocol.

### Week 5 (Apr 28 – May 2): App UX/UI Design
- **HCDE Task: Wireframes.** Design the app experience in Figma. How do users pick colors? Set schedules/timers? Request a custom design? Use the dev board to validate that planned features are technically feasible as you design.
- **User Research:** Continue interviews. Test wireframe concepts informally — show people the app flow and get reactions.
- **Portfolio:** Document the "Design Rationale" — why an app vs. physical buttons? What did users tell you that shaped the interface?

### Week 6 (May 5 – May 9): Hardware Integration & Supply Chain
- **Business:** Full supply chain map — every component, lead time, cost, backup supplier. Packaging concept.
- **Engineering:** PCBs arrive. Solder and test the new board. Validate that the dev board prototyping translates to the custom PCB.

> **MILESTONE 2 (May 9):** Brand identity finalized. App UI designed and informed by user research. PCB hardware validated. Supply chain mapped.

---

## Phase 3: System Hardening & Value Strategy (Weeks 7–9)
**Goal:** Ensure the "Venture" part of the project is as robust as the "Hardware" part.

### Week 7 (May 12 – May 16): Service Blueprinting & The Custom Experience
- **HCDE Task: Service Blueprint.** Map the complete user journey: Seeing an ad → Browsing → Choosing standard or custom → Purchasing → Unboxing → Setup → Daily use → Troubleshooting/Repair. Identify friction points in every stage.
- **Custom Order Focus:** The bespoke design process is the primary value differentiator. Explicitly map: How does a customer communicate their custom vision? How do you translate that into a buildable design? What's the approval flow? What are the upsell tiers (size, complexity, frame material)?
- **Academic Value:** This demonstrates you are designing a *system of interactions*, not just a product. The service blueprint is the proof.
- **Engineering:** Stress-test ESP32 firmware. Does it handle Wi-Fi drops gracefully? BLE reconnection? This is technical "usability."

### Week 8 (May 19 – May 23): Sustainability Deep Dive & Legal
- **Venture Task:** Finalize the Repairability Manual — an actual PDF or web page that tells a user how to replace a burnt-out LED, swap a controller, or troubleshoot connectivity.
- **Business:** Set Terms of Service and Warranty policy. Balance pro-consumer values with business liability. Define return/refund policy. Verify all LLC compliance filings are current (see LLC Compliance Checklist below). *(Note: LLC entity formation completed pre-quarter on Mar 23 — ToS/warranty/compliance are the remaining legal items.)*
- **Engineering:** App ↔ hardware integration. Mobile app communicating with ESP32 over BLE/Wi-Fi.

### Week 9 (May 26 – May 30): Marketing Storytelling & Assets
- **Creative:** Product photography and video. Focus on high-quality demo content that works for portfolio, Etsy listings, personal website, *and* a potential Kickstarter page. Shoot for versatile media assets.
- **Documentation:** Finalize the Bill of Materials (BOM) for the v1.0 release.
- **Engineering:** Pre-production mirror assembly — the "golden sample."

> **MILESTONE 3 (May 30):** Service blueprint complete (with custom order flow). Repairability manual done. Media assets shot. Legal/warranty set. Pre-production mirror built.

---

## Phase 4: Synthesis & The "Go/No-Go" Decision (Weeks 10–11)
**Goal:** Package the quarter's work into a professional-grade case study.

### Week 10 (Jun 2 – Jun 6): Portfolio Construction
- **Deliverable:** Build a comprehensive case study that highlights the full Design Engineering process.
- **Highlight:** Show the transition from the single-button STM8 prototype to the ESP32/app-ready version, justified through the lens of user customization and human-centered design.
- **Include:** Stakeholder map, sustainability audit, user research findings, service blueprint, repairability manual, brand identity, business model, and technical architecture.

### Week 11 (Jun 9 – Jun 12): Launch Readiness Audit
- **The Final Report:** A "Venture Readiness" document. If you were to start selling on June 13th, what are the remaining risks? (Shipping costs, component lead times, app store approval, support capacity, etc.)
- **Kickstarter Readiness:** All media, copy, reward tiers, and funding goals documented — ready to launch a campaign if/when the decision is made.
- **Engineering:** Final "Golden Sample" mirror finished and boxed as if being shipped to a customer.

> **MILESTONE 4 (Jun 12):** Portfolio case study complete. Venture readiness document delivered. Golden sample boxed. One switch flip from live.

---

## Stretch: Product Line Expansion (Post-Quarter / What's Next)
This is explicitly scoped as "what comes after June 12" — explored during Week 11's readiness audit but not a deliverable for this quarter.

- **Form factors:** What other shapes and sizes can the platform support? Wall-mounted panels, desk pieces, large-format art?
- **Product types:** Beyond infinity mirrors — what other LED display products can use the same ESP32 platform, app, and brand? Light boxes, edge-lit displays, signage?
- **Series roadmap:** If the infinity mirror is "Series 1," what's Series 2?
- **Smart home integrations:** Google Home, Apple HomeKit, Matter protocol — architecture plan for future firmware updates.
- **Partnership expansion:** Coffee shops, maker spaces, local galleries as showcase/retail partners.

---

## Pre-Quarter Progress
| Date | Event |
|------|-------|
| Mar 21 | Sprint planning, repo setup, GitHub org created |
| Mar 23 | **LLC Certificate of Formation filed** — legal entity established for the venture |
| Apr 2 | **EIN received** (41-5282028). Operating agreement outline drafted. Pricing calculator MVP built (`tools/pricing/`). |

---

## Key Dates & Deadlines
| Date | Event |
|------|-------|
| Mar 30 | Quarter begins |
| Apr 18 | **Milestone 1:** Business foundation + ethics docs |
| ~Apr 18 | PCBs ordered (critical path) |
| May 9 | **Milestone 2:** Brand + app UI + hardware validated |
| May 30 | **Milestone 3:** Go-to-market ready + golden sample |
| Jun 12 | **Milestone 4:** Portfolio + venture readiness + launch-ready |

## Engineering Timeline (Parallel Track)
| Weeks | Focus |
|-------|-------|
| 1–2 | ESP32 module selection, dev board setup, schematic |
| 3 | PCB order |
| 4–5 | Firmware dev on dev board (Wi-Fi, BLE) |
| 6 | PCBs arrive, solder & validate |
| 7–8 | Firmware hardening, app integration |
| 9–11 | Pre-production build, golden sample |

---

## LLC Compliance Checklist
Tasks required after filing the Certificate of Formation. Items marked with a week target are integrated into the sprint plan above.

### Immediate (Week 1)
- [x] **EIN** — Apply via IRS online (irs.gov) once Certificate of Formation is confirmed. Free, takes 5 minutes, issued immediately. Required for business banking, tax filings, and hiring.
- [ ] **Business bank account** — Open a dedicated account under the LLC + EIN. Keeps personal and business finances separated (critical for maintaining LLC liability protection).
- [ ] **Operating agreement** — Draft and sign a single-member operating agreement. Not always required by state law, but strongly recommended — it documents ownership structure, profit distribution, and decision-making authority. Protects the LLC's legal standing. *(Outline drafted Apr 2 with Schedule A capital contributions — needs finalization and signature.)*

### Within 90 Days of Formation (~by Jun 21)
- [ ] **BOI Report (Beneficial Ownership Information)** — File with FinCEN at boiefiling.fincen.gov. Required by the Corporate Transparency Act for most new LLCs. Reports who owns/controls the company. Free to file, but **penalties for non-compliance are steep** ($591/day). Needs: legal name of LLC, EIN, each beneficial owner's full name, DOB, address, and a government-issued ID image.

### Ongoing / Annual
- [ ] **State annual report** — Check your state's Secretary of State website for filing requirements, deadlines, and fees. Some states require this annually, others biennially.
- [ ] **Registered agent** — Confirm you have a registered agent on file (can be yourself at a physical address, or a registered agent service). Required to maintain good standing.
- [ ] **Business license** — Check city/county requirements. Selling physical products (especially online) may require a general business license and/or sales tax permit.
- [ ] **Sales tax permit** — If selling taxable goods (physical products), register with your state's Department of Revenue. Needed before first sale on Etsy/Shopify.
- [ ] **Estimated quarterly taxes** — As an LLC, income passes through to your personal return. If you expect to owe >$1,000 in taxes, IRS expects quarterly estimated payments (Form 1040-ES). Deadlines: Apr 15, Jun 15, Sep 15, Jan 15.
- [ ] **Bookkeeping** — Track all income and expenses from day one. A simple spreadsheet works to start; consider Wave or QuickBooks Self-Employed as volume grows.

---

## Related

- [Index / MOC](index.md) — vault home
- [Sprint Log](sprint_log.md) — progress tracker this plan is measured against
- [Stakeholder Map](docs/stakeholder-map.md) — who the venture is designed around
- [Market Analysis Report](docs/Market%20Analysis%20Report.md) — market positioning the plan is built on
- [Advisor Meeting — Apr 14](docs/advisor-meeting-2026-04-14.md) — most recent Milestone 1 review
