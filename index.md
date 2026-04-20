---
title: Index — Infinity Mirror Independent Study
type: moc
status: active
tags: [moc, home]
---

# Infinity Mirror — Map of Content

> Home note. Start here for any session. Links fan out to every other doc.

**Company:** Layered Logic LLC (WA) · **Quarter:** Spring 2026 (Mar 30 – Jun 12) · **Today:** 2026-04-20 · **Current week:** 4 of 11

---

## Right Now (Week 4: Apr 21 – Apr 25)

**Phase 2 begins — Interaction Design & Branding.** Week 4 focus: brand identity, right-to-repair, user interviews, ESP32 firmware kickoff.

**Brand track — substantially locked (Apr 20):**
- [Brand Positioning Worksheet](docs/brand-positioning.md) — three-tier architecture, audience priority, voice, typography inversion (italic default), Berkeley Mono, hybrid palette, *"Light that layers"* tagline motif
- [Brand Research Notes](docs/brand-research-notes.md) — five moodboard references analyzed, five signature visual devices distilled
- [Palette swatches](assets/brand/color/) — B (reference), C (reference), Hybrid (locked)
- Logo P5.js repo at [layered-logic/layered-logic-logo](https://github.com/layered-logic/layered-logic-logo) — reviewed, parked until Bill reopens

**Still to do this week:**
- *(pending)* Right-to-Repair Philosophy — HCDE artifact (inherits voice from brand §4)
- *(pending)* User Repair Guide — customer-facing doc
- *(pending)* User interview script + contact list
- *(pending)* Type specimen — Berkeley Mono + Neue Haas Grotesk ExtraLight Italic in-situ
- *(pending)* Moodboard reference images → `assets/moodboards/`

Deferred from Week 3:
- PCB schematic update and production order — unblocked by 5 test boards in hand; not critical path (see [project_pcb_status memory](../../../.claude/projects/C--Users-bowhi-Desktop-Independent-Study/memory/project_pcb_status.md))

---

## Planning

- [Sprint Plan](sprint_plan.md) — 11-week plan, milestones, LLC compliance checklist
- [Sprint Log](sprint_log.md) — done / in-progress / blocked by week
- [README](README.md) — public project overview

---

## HCDE Artifacts

- [Stakeholder Map](docs/stakeholder-map.md) — 24 stakeholders across 5 phases · *Week 1*
- [Sustainability Audit](docs/sustainability-audit.md) — 7-stage lifecycle, power measurements · *Week 2*
- [Brand Positioning](docs/brand-positioning.md) — product-line identity worksheet · *Week 4*
- *(pending)* Right-to-Repair Philosophy · *Week 4*
- *(pending)* User Repair Guide · *Week 4 → Week 8 final*
- *(pending)* Service Blueprint · *Week 7*

## Business & Venture

- [Market Analysis Report](docs/Market%20Analysis%20Report.md) — 4-segment bifurcation, pricing brackets, regulatory risks · *Week 1*
- [BOM Breakdown — Basic 6x6](docs/bom-breakdown-basic-6x6.md) — COGS at 1/50/100/1000 units · *Week 2*
- [Pricing Calculator](tools/pricing/) — Streamlit COGS/margin dashboard
- *(pending)* Supply Chain Map · *Week 6*
- *(pending)* Warranty & ToS · *Week 8*

## Engineering

- [ESP32 Module Comparison](docs/esp32-module-comparison.md) — full Espressif lineup review
- [ESP32 Module Shortlist](docs/esp32-module-shortlist.md) — final selection (ESP32-C3-MINI-1)
- *(pending)* Schematic update for ESP32-C3
- *(pending)* KiCad PCB layout
- *(pending)* Firmware — Wi-Fi/BLE provisioning

## Operations

- [Assembly Station Details](Assembly_docs/LED_Mirror_Station_Details.csv) — 20-station production line
- [Assembly Time Calculator](Assembly_docs/LED_Mirror_Assembly_Calculator.xlsx) — per-unit time by operation
- *LED Mirror Project Brief* — see `external_chats/` (factory ops planning session)

## Design Assets

- [assets/](assets/) — logos, color, type, moodboards, product photos, Figma exports (see [assets/README.md](assets/README.md) for conventions)

## Meeting Logs

- [Advisor — Apr 7, 2026](docs/advisor-meeting-2026-04-07.md) — Week 1 check-in
- [Advisor — Apr 14, 2026](docs/advisor-meeting-2026-04-14.md) — Week 2+3, Milestone 1 status
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
- BOI Report filing (~Jun 21 deadline) — not yet filed
- Product liability insurance — cost/coverage research pending
- Sales tax permit — needed before first Etsy/Shopify sale

**Product / design**
- ~~Brand architecture: monolithic, endorsed sub-brand, or standalone?~~ — **Resolved Apr 20:** three-tier (Layered Logic → product mini-brand → product lines), with consulting arm also under the parent. See [Brand Positioning §0](docs/brand-positioning.md).
- Mini-brand name (Tier 2) — deferred until visual identity lands
- Coffee shop SLA + space negotiation model — awaiting user interviews
- Gift-recipient "First 5 Minutes" unboxing flow

**Engineering**
- ESP32 PCB BOM refresh (superseding STM8 analysis)
- Matter/Thread support — second-SKU decision deferred to customer research
- EMC testing lab engagement trigger for wireless version

---

## Conventions

- Docs use YAML frontmatter with `type`, `phase`, `week`, `status`, `tags`.
- Cross-doc links are standard markdown (work in both Obsidian and GitHub).
- Each major doc ends with a `## Related` section linking to adjacent work.
- `MEMORY.md` (in `~/.claude/projects/.../memory/`) holds Claude's persistent context across sessions — not part of the vault.
