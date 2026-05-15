---
title: Index — Infinity Mirror Independent Study
type: moc
status: active
tags: [moc, home]
---

# Infinity Mirror — Map of Content

> Home note. Start here for any session. Links fan out to every other doc.

**Company:** Layered Logic LLC (WA) · **Quarter:** Spring 2026 (Mar 30 – Jun 12) · **Today:** 2026-05-14 · **Current week:** 7 of 11

---

## Right Now (Week 7: May 12 – May 16)

**Service Blueprinting & The Custom Experience.** Anchor deliverables landed on May 13; firmware resilience work + user-research outreach run in parallel through the end of the week.

**This week's deliverables:**
- ✅ [Service Blueprint](docs/service-blueprint.md) — 10-stage customer journey with custom-order deep dive ([LL-053](tasks.md#LL-053) [LL-054](tasks.md#LL-054)) landed May 13
- ✅ User-flow authoring methodology + Stage 1 (Discover) rendered ([LL-074](tasks.md#LL-074), [docs/user-flow-authoring.md](docs/user-flow-authoring.md))
- 🚧 Cold outreach campaign engaged ([LL-043](tasks.md#LL-043) [LL-043-1](tasks.md#LL-043-1)) — 5 sends today (Reed Kleinman, Matt Perks, Robert Tait, Lewis, Joe Barnard), 5/day cadence going forward; tracked in [docs/outreach-list.md](docs/outreach-list.md)
- *(pending)* [LL-055](tasks.md#LL-055) firmware Wi-Fi/BLE resilience stress-test + transient-socket bug fix — Sat May 16 deadline

**Carryover (status needs Bill's review):**
- *(pending)* User Repair Guide ([LL-042](tasks.md#LL-042))
- *(blocked)* Packaging QR code ([LL-044](tasks.md#LL-044)) — awaiting packaging concept
- *(pending)* Logo P5 source sync, Type specimen SVGs, Moodboard captures

**Compliance follow-ups:**
- *(reminder)* Re-check FinCEN BOI guidance before Jun 21 in case the interim rule reverses
- *(active)* Reseller permit follow-up tracked as [LL-068](tasks.md#LL-068)

**Next-up after Week 7:** Week 8 (May 19-23) — Sustainability Deep Dive + Legal (ToS, warranty, LLC compliance verification) + App↔hardware integration ([LL-057](tasks.md#LL-057)).

---

## Planning

- [Sprint Plan](sprint_plan.md) — 11-week plan, milestones, LLC compliance checklist
- [Sprint Log](sprint_log.md) — daily narrative, done / in-progress / blocked by week
- [Task Registry](tasks.md) — structured `LL-NNN` task index with status, dates, dependencies
- [Task Event Log](task_log.md) — append-only event history
- [Task Format Spec](docs/task-format-v2.md) — ID scheme, schema, migration plan
- [README](README.md) — public project overview

---

## HCDE Artifacts

- [Stakeholder Map](docs/stakeholder-map.md) — 24 stakeholders across 5 phases · *Week 1*
- [Sustainability Audit](docs/sustainability-audit.md) — 7-stage lifecycle, power measurements · *Week 2*
- [Brand Positioning](docs/brand-positioning.md) — product-line identity worksheet · *Week 4*
- [Brand Research Notes](docs/brand-research-notes.md) — five-reference visual grammar analysis · *Week 4*
- [Typography Decisions](docs/typography-decisions.md) — italic-default + Berkeley Mono rationale · *Week 4*
- [Repair Ecosystem — Index](docs/repair-index.md) — sub-MOC tying the philosophy + inventory + design decisions + scorecard into one navigable artifact · *Week 5*
- [Right-to-Repair Philosophy](docs/right-to-repair-philosophy.md) — pro-consumer values, repair commitments, honest limits · *Week 4*
- [Failure Mode Inventory](Failure_Modes/README.md) — 17 failure modes across 8 categories with user-facing repair paths · *Week 4*
- [Repair-Driven Design Decisions](docs/repair-design-decisions.md) — 20-row matrix mapping design choices to repair values + failure modes · *Week 5*
- [Repairability Scorecard](docs/repairability-scorecard.md) — quantitative companion to the philosophy (12/17 fully user-repairable, 1-tool, 7-yr parts commitment) · *Week 5*
- [User Interview Outlines](docs/user-interview-outline.md) — end-buyer + showcase-partner interview outlines, 20–25 min each · *Week 5*
- [Button Design Rationale](docs/button-design-rationale.md) — *why an app + physical buttons* portfolio artifact (draft, pre-user-research) · *Week 5*
- *(pending)* User Repair Guide · *Week 4 → Week 8 final*
- [Service Blueprint](docs/service-blueprint.md) — 10-stage customer journey + custom-order deep dive · *Week 7*
- [User Flow Authoring](docs/user-flow-authoring.md) — methodology + Graphviz rendering recipe for HCDE user-flow diagrams · *Week 7*

## Business & Venture

- [Market Analysis Report](docs/Market%20Analysis%20Report.md) — 4-segment bifurcation, pricing brackets, regulatory risks · *Week 1*
- [BOM Breakdown — Basic 6x6](docs/bom-breakdown-basic-6x6.md) — COGS at 1/50/100/1000 units · *Week 2*
- [Pricing Calculator](tools/pricing/) — Streamlit COGS/margin dashboard
- *(pending)* Supply Chain Map · *Week 6*
- *(pending)* Warranty & ToS · *Week 8*

## Engineering

- [ESP32 Module Comparison](docs/esp32-module-comparison.md) — full Espressif lineup review
- [ESP32 Module Shortlist](docs/esp32-module-shortlist.md) — final selection (ESP32-C3-MINI-1)
- [Firmware Architecture Scoping](docs/firmware-architecture-scoping.md) — **locked Apr 22** — 5 architecture decisions + variant split + board parameterization
- [Button Interface Design](docs/button-interface.md) — two-button model (primary + recessed), gesture grammar, state machine
- [Firmware Spec](docs/firmware-spec.md) — component-level ESP32 firmware spec across both variants
- [Firmware Security Model](docs/firmware-security.md) — signing, OTA, auth modes, telemetry, privacy
- [Control Protocol Spec](docs/control-protocol-spec.md) — shared JSON wire protocol (BLE + WebSocket + HTTP)
- [Pattern Dictionary](docs/pattern-dictionary.md) — **cross-firmware visual contract** — 7 patterns, 13 colors, tick cadence
- [STM8 Basic Firmware Spec](Firmware/STM8/spec.md) — canonical reference implementation (shipping on Basic SKU)
- [App Spec](docs/app-spec.md) — LL mobile app (iOS + Android)
- [Webapp Spec](docs/webapp-spec.md) — device-hosted browser UI (+ V2 hosted plan)
- [Firmware/v1/ scaffold](Firmware/v1/README.md) — greenfield ESP-IDF repo, blink-level bring-up
- *(pending)* Schematic update for ESP32 production PCB
- *(pending)* KiCad PCB layout (Pro + Pro+)
- *(pending)* Firmware — real module implementations beyond scaffold

## Operations

- [Assembly Station Details](Assembly_docs/LED_Mirror_Station_Details.csv) — 20-station production line
- [Assembly Time Calculator](Assembly_docs/LED_Mirror_Assembly_Calculator.xlsx) — per-unit time by operation
- *LED Mirror Project Brief* — see `external_chats/` (factory ops planning session)

## Design Assets

- [assets/](assets/) — logos, color, type, moodboards, product photos, Figma exports (see [assets/README.md](assets/README.md) for conventions)
- [assets/brand/logo/](assets/brand/logo/) — [spec](assets/brand/logo/logo-spec.md) · [code notes](assets/brand/logo/logo-code-notes.md) · rest-pose SVGs ([dark](assets/brand/logo/logo-primary-dark.svg) / [light](assets/brand/logo/logo-primary-light.svg) / [mono](assets/brand/logo/logo-primary-mono.svg))
- [assets/brand/type/](assets/brand/type/) — specimens and license (scaffolded, contents pending)
- [assets/brand/color/](assets/brand/color/) — palette swatches (hybrid locked, B/C references)
- [assets/moodboards/](assets/moodboards/) — five-reference index (image captures pending)

## Meeting Logs

- [Advisor — Apr 7, 2026](docs/Advisor-Meetings/advisor-meeting-2026-04-07.md) — Week 1 check-in
- [Advisor — Apr 14, 2026](docs/Advisor-Meetings/advisor-meeting-2026-04-14.md) — Week 2+3, Milestone 1 status
- [Advisor — Apr 21, 2026](docs/Advisor-Meetings/advisor-meeting-2026-04-21.md) — Week 4 kickoff, Milestone 1 close-out
- [Advisor — Apr 30, 2026](docs/Advisor-Meetings/advisor-meeting-2026-04-30.md) — Week 5 update: app mini-sprint delivery + repair portfolio + brand §1 lock + OTA proven
- [Session logs](docs/session-logs/) — working session records

---

## Milestones

| Target | Milestone | Status |
|---|---|---|
| Apr 18 | **M1:** Business foundation + ethics docs + PCBs ordered | Partial — PCB order deferred, rest complete |
| May 9 | **M2:** Brand + app UI + hardware validated + supply chain | Not started |
| May 30 | **M3:** Go-to-market ready + golden sample | Not started |
| Jun 12 | **M4:** Portfolio + venture readiness + launch-ready | Not started |

---

## Open Questions (Rolling)

*Pulled from across docs. Answer here or link to where they get answered.*

**Business / legal**
- ~~BOI Report filing~~ — **Resolved Apr 22:** US-domestic entities exempt under current FinCEN interim final rule. Recheck before Jun 21 in case of rule reversal.
- Product liability insurance — cost/coverage research pending
- ~~Sales tax permit~~ — **Resolved Apr 22:** WA BLS filed (Retailing + Manufacturing), UBI issued. Reseller permit follow-up after UBI confirmed (~10 biz days).

**Product / design**
- ~~Brand architecture: monolithic, endorsed sub-brand, or standalone?~~ — **Resolved Apr 20:** three-tier (Layered Logic → product mini-brand → product lines), with consulting arm also under the parent. See [Brand Positioning §0](docs/brand-positioning.md).
- Mini-brand name (Tier 2) — deferred until visual identity lands
- Coffee shop SLA + space negotiation model — awaiting user interviews
- Gift-recipient "First 5 Minutes" unboxing flow

**Engineering**
- ESP32 PCB BOM refresh (superseding STM8 analysis)
- ~~Matter/Thread support — second-SKU decision deferred to customer research~~ — **Resolved Apr 22:** two firmware variants (Pro / Pro+) with Matter on Pro+ only; Pro+ SKU commitment vs demo-only remains open pending customer research
- Pro+ shipping SKU decision — whether to ship or keep as demo-only
- Final Matter commissioning gesture timing on recessed button (tentative 6s hold)
- EMC testing lab engagement trigger for wireless version

---

## Conventions

- Docs use YAML frontmatter with `type`, `phase`, `week`, `status`, `tags`.
- Cross-doc links are standard markdown (work in both Obsidian and GitHub).
- Each major doc ends with a `## Related` section linking to adjacent work.
- Tasks have stable `LL-NNN` IDs. The plan and log reference them inline as `[LL-NNN](tasks.md#LL-NNN)`. IDs are immutable — children that grow spawn grandchildren rather than being promoted. See [docs/task-format-v2.md](docs/task-format-v2.md) for the full schema.
- `MEMORY.md` (in `~/.claude/projects/.../memory/`) holds Claude's persistent context across sessions — not part of the vault.
