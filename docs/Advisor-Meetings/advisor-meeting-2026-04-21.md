---
title: Advisor Meeting — April 21, 2026
type: meeting-log
phase: 2
week: 4
date: 2026-04-21
status: draft
tags: [meeting, advisor, milestone-1, week-4]
---

# Advisor Meeting — April 21, 2026
**Project:** Layered Logic / Infinity Mirror Venture
**Quarter:** Spring 2026 — Week 4 of 11
**Purpose:** M1 close-out, Week 4 check-in, 3 open questions

---

## Milestone 1 Close-out (Apr 18)

M1 is substantively complete. Competitive analysis, stakeholder map, pricing model, sustainability/ethics goals, and business foundation (LLC, EIN, bank) all delivered on schedule. **"PCBs on order" reframed** — 5 STM8 test boards on hand cover bring-up; production ESP32 PCB defers to post-firmware-architecture.

## Week 4 Delivered

**Brand (locked Apr 20–21)** — [brand-positioning.md](../brand-positioning.md)
- Three-tier architecture (Layered Logic parent → product mini-brand deferred → product lines; consulting lives under parent)
- Audience priority: Interior Curator > Vibe-Seeker >> Spec-Head
- Typography: NHG ExtraLight **Italic** as default posture; Berkeley Mono accent
- Palette: hybrid three-tier anchored on Indigo Reference `#4A25FF` / Signal `#3214FF` *(originally magenta-violet Apr 20; shifted to indigo Apr 21 after rendered artifacts read too pink)*
- Tagline motif: `"[X] that layers."` — infinity mirror gets *"Light that layers."*, consulting gets *"data that layers."*
- Companion research doc distills 5 signature visual devices from 5 moodboard references

**Engineering** — [right-to-repair-philosophy.md](../right-to-repair-philosophy.md) · [firmware-architecture-scoping.md](../firmware-architecture-scoping.md)
- Right-to-repair philosophy: STLs on-request, firmware closed-binary with reflash service, "every failure has a service path"
- Firmware scoping locks 5 principles (works out-of-box, app is pattern editor, configurable LED count, brand-palette default, closed-binary + self-hosted OTA) and stages 5 decisions for Wed Apr 23
- **Scope note:** V0 runs ESPHome; Phase 2 is greenfield rewrite. YAML is behavioral spec, not port source.

**Infrastructure** — repo restructured as Obsidian vault with MOC at [index.md](../../index.md); design assets scaffolded at [assets/](../../assets/).

## Week 4 Remaining (Apr 21–25)

| Task | Priority |
|---|---|
| Failure-mode inventory + User Repair Guide | High |
| User interview script + recruit 3–5 participants | High |
| Wed Apr 23: BOI filing + sales tax permit + ESP32 firmware kickoff | Critical |
| Packaging QR code concept | Medium |
| ESP32 schematic kickoff (gates ~May 1 PCB order) | Medium |

---

## Open Questions

**1. Brand scope overreach?** Three-tier architecture + parent-brand tagline motif + consulting arm — appropriate for an HCDE quarter on a single product, or premature over-architecting?
- is it a studio/consultancy/fine art/mass market/ livelihood/ 
	- where does the business lie, what's the vibe

**2. Right-to-repair — HCDE differentiator vs. liability.** Published repair docs, on-request STLs, reflash service. How to think about the tradeoffs between supporting access and parts vs. IP and design protection?

**3. User research rigor.** 3–5 informal interviews — defensible HCDE portfolio artifact, or should I budget for a structured protocol (script, recording, coded themes)? What would you want to see for this independent study?  - informal interviews okay -
- user interactions
- cafes: Fremont Coffee Co., Broadcast Coffee

---

## Milestones Ahead

| Date | Milestone |
|---|---|
| ✅ Apr 18 | M1 — Business foundation locked |
| May 9 | M2 — Brand + app UI + hardware validated |
| May 30 | M3 — Go-to-market ready + golden sample |
| Jun 12 | M4 — Portfolio complete + venture readiness |

---

## Related

- [Index / MOC](../../index.md)
- [Advisor Meeting — Apr 14](advisor-meeting-2026-04-14.md)
- [Brand Positioning](../brand-positioning.md) · [Brand Research Notes](../brand-research-notes.md)
- [Right-to-Repair Philosophy](../right-to-repair-philosophy.md) · [Firmware Architecture Scoping](../firmware-architecture-scoping.md)
- [Sprint Plan](../../sprint_plan.md) · [Sprint Log](../../sprint_log.md)
