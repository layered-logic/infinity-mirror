---
title: Stakeholder Map
type: hcde-artifact
phase: 1
week: 1
date: 2026-04-02
status: complete
tags: [hcde, stakeholders, research]
---

# Stakeholder Map — Layered Logic Infinity Mirror

**Prepared by:** William White, Founder & Lead Designer
**Date:** April 2, 2026
**Project Scope:** HCDE Independent Study / Pre-Production Venture
**Entity:** Layered Logic LLC (Washington)

---

## Executive Summary

The Infinity Mirror is an ESP32-powered ambient lighting system designed as a complete product system — not just hardware, but interaction design, sustainable manufacturing, made-to-order operations, and a venture-ready business model. This document maps every stakeholder who influences, is influenced by, or interacts with the product across its full lifecycle.

The stakeholder ecosystem is organized into five phases that trace the product from raw materials to end-of-life: **Creation**, **Distribution & Sales**, **Acquisition**, **The Lived Experience**, and **Support & Legacy**. Each phase identifies the stakeholders, their relationship to the product, their influence level, and the open questions that remain.

---

## Phase 1: Creation & Supply Chain

The people, organizations, and resources that make the product exist.

### 1.1 Founder (William White)

| | |
|---|---|
| **Role** | Sole designer, engineer, manufacturer, and compliance officer |
| **Influence** | Total — all decisions flow through the founder |
| **Risk** | Single point of failure. If the founder is unavailable, production and support stop. |

The founder bottleneck is the venture's primary internal risk. No single task is irreplaceable in isolation, but the founder is the only person driving the overall vision and managing the full stack (design, manufacturing, firmware, app, sales, compliance, customer support). This is acceptable during the independent study phase but becomes untenable at scale.

**Trigger for change:** When production consistently exceeds 1 unit/day, or when customer support volume interferes with manufacturing time.

### 1.2 Future Employees

| | |
|---|---|
| **Role** | Assembly technicians, customer support, repair/RMA processing |
| **Influence** | Low (do not exist yet) |
| **Trigger** | Volume exceeds what a sole proprietor can sustain (~20+ units/week) |

Identified early so that manufacturing processes, documentation, and training materials are designed with handoff in mind from day one.

### 1.3 Academic Advisors (UW HCDE)

| | |
|---|---|
| **Role** | Stakeholders in the pedagogical success and venture readiness of the project |
| **Influence** | Medium — shape the academic framing and deliverable quality |
| **Timeline** | Active through June 2027 (latest student status date) |

### 1.4 JLC PCB (Critical Manufacturing Partner)

| | |
|---|---|
| **Role** | Custom PCB fabrication and assembly |
| **Influence** | High — without JLC PCB, the ultra-small, high-power, rapid-flash custom boards are not possible |
| **Risk** | Lead times, MOQ changes, quality variance across batches |
| **Relationship** | Transactional (no formal partnership) |

JLC PCB is the most stable and critical external partner in the supply chain. Their reliability directly determines production throughput.

### 1.5 Commodity Suppliers (Amazon / AliExpress)

| | |
|---|---|
| **Role** | Source for LED strips (WS2812B), ESP32 modules, wire, connectors, and misc. components |
| **Influence** | Medium — product depends on component availability and consistency |
| **Risk** | Supply inconsistency (LED color temperature "binning" variance between batches), stock availability, no formal relationship or SLA |
| **Mitigation** | Binning risk is negligible for this application — color cycling means minor temp variance is imperceptible. Multiple supplier options tracked in pricing calculator. |

**Trigger for change:** At 100+ unit orders, transition to bulk distributors or direct manufacturer relationships to improve margins and consistency.

### 1.6 Resource Gatekeepers (UW Mill / Local Makerspaces)

| | |
|---|---|
| **Role** | Access to large-format laser cutters and 3D printers beyond the founder's home equipment |
| **Influence** | Low (current equipment handles all standard sizes) |
| **Risk** | University "no commercial use" policies. The independent study framing provides cover during the academic period, but a clear trigger point is needed for transitioning production to a private makerspace. |
| **Current capacity** | Home: 300x600mm laser, 300x300x300mm 3D printer. UW Mill: ~3x6 ft laser and large-format printers. |

---

## Phase 2: Distribution, Sales & Digital Platforms

The channels, platforms, and intermediaries that connect the product to buyers.

### 2.1 E-Commerce Platforms (Shopify / Etsy)

| | |
|---|---|
| **Role** | Primary online sales channels — storefronts, product listings, checkout, customer communication |
| **Influence** | High — they control the buying experience, search visibility, review systems, and payout schedules |
| **Risk** | Platform policy changes (fee increases, listing requirements, algorithm shifts) can directly impact revenue and discoverability. Account suspension would halt online sales entirely. |
| **Fees** | Etsy: ~6.5% transaction + 3% + $0.45 per sale. Shopify: ~2.9% + $0.30 per transaction. |

These platforms are not just tools — they are trust intermediaries. A buyer on Etsy trusts Etsy's buyer protection before they trust Layered Logic. Maintaining strong seller ratings, responsive communication, and accurate product descriptions is essential.

### 2.2 Crowdfunding Platforms (Kickstarter — Potential)

| | |
|---|---|
| **Role** | Potential launch channel for initial production run |
| **Influence** | High during campaign — backers expect transparency, regular updates, and on-time delivery |
| **Risk** | Overpromising on delivery timelines; manufacturing delays visible to a public audience |
| **Stakeholder note** | Kickstarter backers are not traditional customers — they are micro-investors buying into the founder and the process. They expect to be part of the HCDE journey, not just receive a product. |

### 2.3 Payment Processors (Stripe / PayPal / Shop Pay)

| | |
|---|---|
| **Role** | Process transactions, hold funds, mediate disputes |
| **Influence** | Medium — chargeback decisions can override seller judgment |
| **Risk** | If a shipment is lost or a buyer claims "not as advertised," the processor decides who keeps the money. Chargeback rates above ~1% can trigger account review or holds. |

### 2.4 Shipping Carriers (USPS / UPS / FedEx)

| | |
|---|---|
| **Role** | Physical delivery of the product to the customer |
| **Influence** | Medium — they are the last touchpoint before the customer's first impression |
| **Risk** | Damage in transit (acrylic mirrors are fragile), lost packages, delivery delays. A carrier dropping a box can undo weeks of manufacturing work. |
| **Mitigation** | Packaging design must account for fragility. Insurance on shipments above a cost threshold. |

### 2.5 Content Creators & Reviewers

| | |
|---|---|
| **Role** | YouTubers, TikTokers, Instagram creators, and bloggers who review or feature the product |
| **Influence** | Highly asymmetric — a single viral review (positive or negative) can dwarf all other marketing efforts combined |
| **Risk** | Negative reviews are permanent and public. A creator's audience trusts them more than they trust the brand. |
| **Opportunity** | The HCDE story (right-to-repair, sustainability, bespoke manufacturing, solo founder) is inherently compelling content. Creators who cover maker culture, home decor, or indie hardware are natural allies. |

These stakeholders exist whether or not you engage them. Cultivating relationships with aligned creators (maker community, home decor, indie hardware) is a high-leverage marketing activity. Sending a free unit to the right creator can be worth more than months of paid advertising.

### 2.6 Local Showroom Partners (Coffee Shops & Small Businesses)

| | |
|---|---|
| **Role** | Physical discovery channel — display mirrors in-store to generate foot traffic and brand awareness |
| **Influence** | Medium — they lend their reputation and trust to the product |
| **Risk** | If a mirror fails (flickers, dies) in a shop, it damages both the shop's atmosphere and Layered Logic's brand. |

Showroom units are simplified "basic" versions with button/rotary interfaces (no app required). They must be high-uptime, low-maintenance, and visually compelling.

**Installation:** Mirrors mount like a painting or framed print — standard wall hardware. The only additional requirement is proximity to a power outlet. The end user (or shop staff) can install without specialized tools or a professional installer. Corporate installations may optionally include installation guidance or on-site setup as a premium service.

**Open questions for shop owner conversations:**
- What is the space negotiation model? Flat rent, commission on QR-code sales, or free placement as "local art"?
- What is the expected SLA? Same-day replacement if a unit fails?
- Who is liable if a patron damages a mirror? Does the shop's insurance cover displayed art, or is a waiver needed?
- What are the shop's power outlet constraints? Can we offer a clean, concealed installation?
- What is the power draw impact on the shop's electric bill? (Power consumption testing planned — see Open Questions.)

---

## Phase 3: Acquisition & Installation

The people who decide to bring the product into a space.

### 3.1 The Self-Buyer

| | |
|---|---|
| **Role** | Primary customer — purchasing for personal use |
| **Archetypes** | **The Spec-Head** (cares about ESP32, refresh rates, flashable firmware), **The Interior Curator** (cares about cord management, finish quality, wall mounting), **The Vibe-Seeker** (cares about mood lighting, app UX, color customization) |
| **Influence** | High — their reviews and word-of-mouth drive organic growth |

### 3.2 The Gift Giver

| | |
|---|---|
| **Role** | Purchases the product for someone else |
| **Influence** | Medium — they care about unboxing experience, perceived quality, and delivery timing |
| **Design implication** | Packaging and first-run experience must be "gift-ready" — no complex setup required before the recipient can enjoy it |

### 3.3 The Gift Recipient

| | |
|---|---|
| **Role** | Receives the product without choosing it — the ultimate test of intuitive design |
| **Influence** | High on long-term brand perception — if setup is frustrating, they blame the product, not the giver |
| **Design implication** | Out-of-box experience must be zero-friction. The mirror should work (basic mode) without downloading an app or reading a manual. |

### 3.4 The Corporate / Organizational Buyer

| | |
|---|---|
| **Role** | Purchases mirrors for lobbies, offices, retail spaces, or branded installations (e.g., company logo as infinity mirror) |
| **Influence** | High-value, low-volume. Willing to pay premium prices but expects premium reliability. |
| **Risk** | If a branded logo mirror flickers in a lobby, it's a professional embarrassment. These buyers expect an SLA. |
| **Opportunity** | The bespoke manufacturing model (laser-cut custom shapes, 3D-printed custom frames) makes corporate customization a natural extension with no technical re-tooling required. |

---

## Phase 4: The Lived Experience

Everyone who interacts with the product after it's been placed in a space — including those who didn't choose it.

### 4.1 The Primary User

| | |
|---|---|
| **Role** | The person with the app/controls — sets colors, patterns, schedules |
| **Influence** | Direct — their satisfaction determines reviews, referrals, and repeat purchases |

### 4.2 Passive Stakeholders (Co-habitants, Guests, Coworkers)

| | |
|---|---|
| **Role** | People who share a space with the mirror without having purchased or configured it — roommates, family members, house guests, office workers |
| **Influence** | Indirect but significant — if they find the light annoying, distracting, or disruptive, the product becomes a source of friction in shared spaces |
| **Design implication** | A physical global off switch is essential (currently implemented). Brightness/mode defaults should err toward ambient rather than attention-grabbing. |

This is one of the most overlooked stakeholder groups in consumer electronics. The mirror is not just a personal gadget — it's an architectural intervention in a shared environment. Designing for the passive stakeholder means designing for coexistence.

**Safety & accessibility considerations:**
- **Physical safety:** No sharp edges (acrylic, not glass — won't shatter if dropped). All electronics are internal and far from any exterior surface — no hot spots even at peak brightness. Child-safe by default.
- **Photosensitivity:** Strobing and rapid pattern transitions will be disabled on showroom/public-facing units. Home versions will include a user-configurable option to limit transition speed.
- **Color vision:** The infinity effect is compelling regardless of color perception — the depth illusion is geometric, not color-dependent. Users with color vision deficiency will still enjoy the core product experience.

### 4.3 Shop Patrons (Public Discovery Users)

| | |
|---|---|
| **Role** | Encounter the mirror in coffee shops or businesses — the "in-the-wild" discovery audience |
| **Influence** | Low individually, high collectively — they are the top of the marketing funnel |
| **Design implication** | Showroom mirrors must be visually compelling without interaction. The product sells itself through ambient presence, not feature demonstrations. |

### 4.4 Community & Pattern Sharing (Future)

| | |
|---|---|
| **Role** | Users who create and share custom patterns, creating a network effect around the product |
| **Influence** | Not yet active — this is a post-MVP feature |
| **Opportunity** | A "Gallery" feature where users share patterns turns buyers into contributors and creates a reason to revisit the app. Users become stakeholders in each other's experience. |
| **Status** | Noted for future development. Not a ship blocker or current priority. The product must be proven out before investing in community features. Will be considered for app/webapp roadmap once core functionality is validated and there is an active user base to build on. |

---

## Phase 5: Support, Compliance & Legacy

The stakeholders involved in keeping the product running, keeping it legal, and handling its end of life.

### 5.1 The Amateur Repairer (End User as Maintainer)

| | |
|---|---|
| **Role** | Customers performing modular repairs using replacement parts and video guides |
| **Influence** | High — successful self-repair builds brand loyalty; failed self-repair destroys it |

The product is engineered for toolless disassembly using directional JST connectors and modular 3D-printed components. The four core serviceable modules are:

1. **Mirrors** (acrylic, easy to ship and swap)
2. **Microcontroller** (JST connector — unplug old, plug in new)
3. **LED strip** (JST connector — swap and reconnect)
4. **Spacers/frame** (3D printed — snap-fit replacement)

**Support model:** For each RMA, the repair is performed in-house first and filmed. This builds a growing video repository of common repairs — a "scalable employee" that provides 24/7 customer support at zero marginal cost.

**Warranty boundary:** Self-repair without filing an RMA voids warranty. Repairs using RMA-supplied parts remain covered.

### 5.2 The Dissatisfied Customer / Returns

| | |
|---|---|
| **Role** | Buyers who want to return the product — whether due to buyer's remorse, unmet expectations, or defects |
| **Influence** | Medium — a bad return experience generates negative reviews; a generous return policy builds trust |

**Return policy:** 30–60 day no-questions-asked money-back guarantee. This is standard for DTC products and signals confidence in the product.

**Rework strategy for returned custom units:** The only component that differs between custom orders is the one-way mirror middle plane (the laser-cut design layer). All other components (frame, LEDs, controller, PSU, back panel) are reusable. Returned standard units can be resold directly. Returned custom units can be reworked by swapping the custom plane, or — if the custom design is non-proprietary — resold at a steep discount as a one-of-a-kind piece.

**Open question:** What is the true restocking/rework cost per returned unit? This needs to be modeled once assembly is finalized — factor in labor time, shipping both directions, component inspection, and the likelihood of resale.

### 5.3 Regulatory & Compliance Bodies

| | |
|---|---|
| **Role** | FCC (electromagnetic compliance), state/local business licensing, consumer product safety |
| **Influence** | Binary — pass or fail. Non-compliance can halt sales entirely. |
| **Current status** | The founder serves as compliance officer. The current product version (no wireless connectivity) has a simpler regulatory profile. |

**For the wireless-enabled future version (ESP32 with Wi-Fi/BLE):**
- FCC Part 15 (Unintentional Radiators) applies — even pre-certified ESP32 modules require the final assembled product to meet emissions standards
- A Seattle-based EMC testing lab / consultancy will be engaged when the wireless version approaches production
- This is tracked as a future requirement, not a current blocker

### 5.3 Product Liability Insurance

| | |
|---|---|
| **Role** | Financial protection against claims of property damage, injury, or product defects |
| **Influence** | High in the event of an incident — without coverage, a single claim could pierce the LLC's liability shield |
| **Current status** | Not yet in place. To be explored alongside other compliance items. |

Even with safe materials (no glass, no battery, no hot surfaces), selling a consumer electronic product exposes the venture to liability. General liability and product liability insurance are standard for any hardware company shipping to consumers. This becomes especially important before placing units in public spaces (coffee shops, lobbies) where the product is unattended and accessible to anyone.

**Open question:** What is the cost and coverage level of a basic product liability policy for a low-volume consumer electronics venture? Research this before first public placement or first sale.

### 5.4 App Platform Gatekeepers (Apple / Google / W3C)

| | |
|---|---|
| **Role** | Control distribution of the mobile app and set the rules for Bluetooth/Wi-Fi permissions |
| **Influence** | High for the app-enabled version — App Store rejection or permission changes can break functionality |
| **Mitigation** | The mirror always has a standalone offline mode (button/rotary interface) that provides core functionality without any app. A web app (Chrome, Safari) provides full feature parity as a fallback if app store distribution is disrupted. The product is never a "brick." |

### 5.4 Open Source Community (Future)

| | |
|---|---|
| **Role** | Potential stewards of the firmware and design files if the founder steps away |
| **Influence** | Not yet active |
| **Commitment** | The founder commits to open-sourcing all design files and firmware before ever stepping away from the product. The project is already documented on a public GitHub repository, building a foundation for future community contribution. Open-sourcing will not be premature — it will happen at the right time to ensure quality and completeness, but it will happen. |

### 5.5 End-of-Life & Sustainability

| | |
|---|---|
| **Role** | Waste management facilities, recycling programs, the environment |
| **Influence** | Indirect but ethically significant — a core HCDE commitment |
| **Current status** | No hazardous chemicals. No battery included. Components are standard PCBs, acrylic, PLA/PETG plastic, and copper wire. Full life cycle analysis is planned as a deeper independent study deliverable. |

---

## Stakeholder Influence Matrix

A summary view of all stakeholders by influence level and lifecycle phase.

| Stakeholder | Phase | Influence | Current Status |
|---|---|---|---|
| Founder (William White) | 1 - Creation | Critical | Active — sole operator |
| JLC PCB | 1 - Creation | High | Active — transactional relationship |
| Amazon / AliExpress | 1 - Creation | Medium | Active — commodity purchasing |
| UW Mill / Makerspaces | 1 - Creation | Low | Available — not yet needed |
| Future Employees | 1 - Creation | Low | Not yet triggered |
| Shopify / Etsy | 2 - Distribution | High | Shopify active; Etsy planned |
| Payment Processors | 2 - Distribution | Medium | Active via Shopify |
| Shipping Carriers | 2 - Distribution | Medium | Not yet shipping product |
| Kickstarter | 2 - Distribution | High (if used) | Under consideration |
| Content Creators | 2 - Distribution | High (asymmetric) | Not yet engaged |
| Coffee Shop Owners | 2 - Distribution | Medium | Planned — not yet approached |
| Self-Buyers | 3 - Acquisition | High | Target customer — no sales yet |
| Gift Giver / Recipient | 3 - Acquisition | Medium / High | Secondary customer segment |
| Corporate / Org Buyers | 3 - Acquisition | High (value) | Opportunistic — not actively pursued |
| Primary User | 4 - Experience | High | Designing for |
| Passive Stakeholders | 4 - Experience | Medium | Designing for |
| Shop Patrons | 4 - Experience | Low | Designing for |
| User Community | 4 - Experience | Low (future) | Post-MVP consideration |
| Amateur Repairer | 5 - Support | High | Designing for — RMA process planned |
| Dissatisfied Customer | 5 - Support | Medium | Return policy drafted (30-60 day) |
| FCC / Regulatory | 5 - Support | Binary | Future — wireless version trigger |
| Product Liability Insurance | 5 - Support | High (if incident) | Not yet in place — to be explored |
| Apple / Google | 5 - Support | High | Future — app development phase |
| Open Source Community | 5 - Legacy | Medium (future) | Committed to — timing TBD |
| Waste / Sustainability | 5 - Legacy | Ethical | LCA planned as study deliverable |

---

## Open Questions & Research Agenda

These questions emerged from the stakeholder mapping exercise and will guide primary research (interviews, usability studies) and technical validation throughout the independent study.

### Immediate (Weeks 1-3)
- What do coffee shop owners expect in terms of SLA, space negotiation, and liability for displayed art?
- What does the "First 5 Minutes" unboxing experience look like for a gift recipient who didn't choose this product?
- At current component costs, what is the true labor time per unit including QA and packaging?
- **Power consumption testing:** Measure actual draw (watts) at full white, rotating rainbow, single color, and "efficient" ambient patterns using power monitoring plugs. Produce a simple reference card for coffee shop owners showing worst/normal/best case energy cost impact. *(Add to a testing sprint.)*

### Near-Term (Weeks 4-8)
- At what volume does buying components off Amazon become a margin liability vs. bulk suppliers?
- What is the UW Mill's policy on commercial use for student projects in the independent study context?
- What does the corporate buyer actually need beyond the standard product? (Custom firmware? Extended warranty? Installation service?)
- What is the true restocking/rework cost per returned unit? Model labor, two-way shipping, component inspection, and resale likelihood.
- What does a basic product liability insurance policy cost for a low-volume consumer electronics venture? Research before first public placement or first sale.
- Which content creators (maker community, home decor, indie hardware) are aligned with the Layered Logic story? Begin identifying and building relationships.

### Longer-Term (Weeks 9-11 and Beyond)
- What is the specific trigger point for engaging an EMC testing lab for the wireless version?
- How should the open-source transition be structured to ensure quality and community adoption?
- What does the full life cycle analysis reveal about end-of-life recyclability?
- What is the optimal return policy window (30 vs. 60 days) based on actual return rates and customer feedback?

---

*This document is a living artifact of the HCDE Independent Study. It will be updated as stakeholder relationships are validated through primary research and as the product moves from prototype to pre-production.*

---

## Related

- [Index / MOC](../index.md)
- [Market Analysis Report](Market%20Analysis%20Report.md) — segment and competitor context
- [Sustainability Audit](sustainability-audit.md) — end-of-life stakeholder analysis
- [Right-to-Repair Philosophy](right-to-repair-philosophy.md) — operationalizes the end-of-life and repair stakeholders mapped here
- [BOM Breakdown](bom-breakdown-basic-6x6.md) — cost structure referenced by Self-Buyer pricing
- [Brand Positioning](brand-positioning.md) — audience ranking draws from §3 archetypes here
- [Sprint Plan](../sprint_plan.md)

---

<p align="center"><em>Layered Logic LLC — Spring 2026</em></p>
