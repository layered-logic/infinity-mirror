---
title: Task Registry
type: task-registry
next_id: LL-073
updated: 2026-05-07

---

# Task Registry

Machine-parseable source of truth for all tasks in this project. Format spec: [docs/task-format-v2.md](docs/task-format-v2.md). Every `[x]`/`[ ]` heading is a task; the heading is the completion flag. Sub-tasks use dash hierarchy (`LL-NNN-M`, `LL-NNN-M-K`).

`sprint:` is the sprint a task was **started in** — it does not change as work spans into later sprints. Active sprint is noted in Notes when relevant. Bump `last_engaged` whenever a task receives work.

---

<a id="LL-001"></a>
### [x] LL-001 — LLC Certificate of Formation

sprint: 1 (pre-quarter) | priority: high | deadline: 2026-03-23
added: 2026-03-21 | first_engaged: 2026-03-21 | last_engaged: 2026-03-23 | resolved: 2026-03-23
artifacts: Business/ folder (local, not committed)
dependencies: —

**Notes:** Filed pre-quarter for Layered Logic LLC, a WA-formed domestic single-member LLC. Foundation for all subsequent business tasks (EIN, bank account, operating agreement, BLS, BOI determination).

---

<a id="LL-002"></a>
### [x] LL-002 — EIN issuance

sprint: 1 | priority: high | deadline: 2026-04-04
added: 2026-03-30 | first_engaged: 2026-04-02 | last_engaged: 2026-04-02 | resolved: 2026-04-02
artifacts: —
dependencies: LL-001

**Notes:** EIN 41-5282028 issued by IRS online. Required for business banking and tax filings. Five-minute online process once LLC certificate confirmed.

---

<a id="LL-003"></a>
### [x] LL-003 — Pricing calculator MVP

sprint: 1 | priority: medium | deadline: 2026-04-04
added: 2026-03-30 | first_engaged: 2026-04-02 | last_engaged: 2026-04-02 | resolved: 2026-04-02
artifacts: tools/pricing/
dependencies: —

**Notes:** Streamlit dashboard with dynamic BOM, size scaling, tier/platform selection, and margin analysis. Foundation tool for all later pricing decisions.

---

<a id="LL-004"></a>
### [x] LL-004 — Stakeholder mapping

sprint: 1 | priority: high | deadline: 2026-04-04
added: 2026-03-30 | first_engaged: 2026-04-02 | last_engaged: 2026-04-02 | resolved: 2026-04-02
artifacts: [docs/stakeholder-map.md](docs/stakeholder-map.md)
dependencies: —

**Notes:** 45-min coached sprint with Gemini as investor/board member. 5-phase map covering 24 stakeholders across Creation, Distribution, Acquisition, Lived Experience, Support/Legacy. HCDE artifact.

---

<a id="LL-005"></a>
### [x] LL-005 — Operating agreement finalization

sprint: 1 | priority: medium | deadline: 2026-04-11
added: 2026-03-30 | first_engaged: 2026-04-02 | last_engaged: 2026-04-06 | resolved: 2026-04-06
artifacts: Business/ folder (local, not committed)
dependencies: LL-001, LL-002

**Notes:** Single-member operating agreement with Schedule A capital contributions. Drafted Apr 2, finalized + printed + signed Apr 6. Protects LLC liability shield.

---

<a id="LL-006"></a>
### [x] LL-006 — ESP32 module selection

sprint: 1 | priority: high | deadline: 2026-04-11
added: 2026-03-30 | first_engaged: 2026-04-02 | last_engaged: 2026-04-06 | resolved: 2026-04-06
artifacts: [docs/esp32-module-comparison.md](docs/esp32-module-comparison.md) · [docs/esp32-module-shortlist.md](docs/esp32-module-shortlist.md)
dependencies: —

**Notes:** ESP32-C3-MINI-1 selected. Smallest footprint, lowest cost (~$2 LCSC), built-in USB Serial/JTAG (no external UART needed). C6-MINI-1 (Zigbee/Thread/Matter) flagged as future Pro+ SKU variant.

---

<a id="LL-007"></a>
### [x] LL-007 — Market analysis and competitive landscape

sprint: 1 | priority: high | deadline: 2026-04-11
added: 2026-03-30 | first_engaged: 2026-04-06 | last_engaged: 2026-04-06 | resolved: 2026-04-06
artifacts: [docs/Market Analysis Report.md](docs/Market%20Analysis%20Report.md)
dependencies: —

**Notes:** Cross-platform analysis of 30+ listings (Etsy, Amazon, Pinterest, commercial signage). Bifurcated market: 4 segments (Bespoke Luxury $800–$5k+, B2B Branding $250–$1.2k, Sensory/Therapy $230–$4.3k, Lifestyle/Gaming $40–$250). $50–$120 Etsy bracket has 1,900+ listings — saturation risk if industrial design doesn't signal $200+ value.

---

<a id="LL-008"></a>
### [x] LL-008 — Business bank account opening

sprint: 1 | priority: high | deadline: 2026-04-25
added: 2026-03-30 | first_engaged: 2026-04-09 | last_engaged: 2026-04-28 | resolved: 2026-04-28
artifacts: Business/ folder (local, not committed)
dependencies: LL-001, LL-002

**Notes:** Application Apr 9, approved Apr 14 (NAICS 335129, calendar fiscal year). Activation Apr 28: external personal accounts linked, initial deposit cleared, account past inactivity threshold.

---

<a id="LL-009"></a>
### [x] LL-009 — Sustainability audit

sprint: 2 | priority: high | deadline: 2026-04-11
added: 2026-03-30 | first_engaged: 2026-04-07 | last_engaged: 2026-04-09 | resolved: 2026-04-09
artifacts: [docs/sustainability-audit.md](docs/sustainability-audit.md)
dependencies: —

**Notes:** Full 7-stage cradle-to-grave lifecycle. 6,368 smart-plug samples: idle 0.1W, rainbow 1.25W avg, full white 2.8W (firmware-capped at ~29% WS2812B max). LED lifespan 14+ yr. Hotspots: coated acrylic panels non-recyclable, PSU oversizing (25W brick at 11% load). Cardboard + PETG separability are strengths.

---

<a id="LL-010"></a>
### [x] LL-010 — BOM breakdown for basic 6×6 model

sprint: 2 | priority: high | deadline: 2026-04-11
added: 2026-03-30 | first_engaged: 2026-04-07 | last_engaged: 2026-04-09 | resolved: 2026-04-09
artifacts: [docs/bom-breakdown-basic-6x6.md](docs/bom-breakdown-basic-6x6.md)
dependencies: —

**Notes:** Material + labor + fixed cost analysis at 1/50/100/1,000 units. Variable COGS $81.62 (qty 1) → $49.80 (qty 1,000). Labor dominates at all volumes. $50 retail reaches $60/hr only at 1,000 units; $65 recommended base price for early volumes. Fixed capital (laser + printer) $7,153.11.

---

<a id="LL-011"></a>
### [x] LL-011 — JLCPCB PCB cost analysis

sprint: 2 | priority: medium | deadline: 2026-04-11
added: 2026-03-30 | first_engaged: 2026-04-09 | last_engaged: 2026-04-09 | resolved: 2026-04-09
artifacts: —
dependencies: —

**Notes:** STM8 PCB BOM (7 lines, 9 parts, 40 joints). $3.61/board at qty 5, $1.04 at qty 50, $0.90 at qty 100. Pricing config $5/controller placeholder is significantly overstated. TTP223 capacitive touch evaluated as ~$0.10 add-on (senses through PETG at 1–2mm). ESP32 BOM still pending.

---

<a id="LL-012"></a>
### [x] LL-012 — Factory operations planning

sprint: 3 | priority: high | deadline: 2026-04-18
added: 2026-03-30 | first_engaged: 2026-04-14 | last_engaged: 2026-04-14 | resolved: 2026-04-14
artifacts: Assembly_docs/LED_Mirror_Project_Brief.md · Assembly_docs/LED_Mirror_Station_Details.csv · Assembly_docs/LED_Mirror_Assembly_Calculator.xlsx
dependencies: —

**Notes:** Made-to-order workflow mapped. 3 parallel lanes (Frame, Mirrors, Electronics) feeding final assembly. 20 stations. Batch-by-operation strategy. Garage layout: wood dust on one end, electronics bench on other, pack & ship isolated. Phase 1 tooling list finalized. Solder ops flagged as first scaling bottleneck (first outsource candidate).

---

<a id="LL-013"></a>
### [x] LL-013 — Brand positioning worksheet

sprint: 4 | priority: high | deadline: 2026-04-25
added: 2026-04-20 | first_engaged: 2026-04-20 | last_engaged: 2026-04-21 | resolved: 2026-04-21
artifacts: [docs/brand-positioning.md](docs/brand-positioning.md) · [docs/brand-research-notes.md](docs/brand-research-notes.md)
dependencies: —

**Notes:** 9-section worksheet locked Apr 20. Three-tier architecture (Layered Logic parent → product mini-brand → product lines). Audience priority: Curator > Vibe-Seeker >> Spec-Head with "tech disappears" principle. Five adjectives: Ambient, Considered, Quiet, Crafted, Living. Italic-default typography (NHG ExtraLight Italic + Berkeley Mono). Hybrid palette. Tagline motif "[X] that layers." Mini-brand name still deferred.

---

<a id="LL-014"></a>
### [x] LL-014 — Initial palette swatches

sprint: 4 | priority: medium | deadline: 2026-04-25
added: 2026-04-20 | first_engaged: 2026-04-20 | last_engaged: 2026-04-20 | resolved: 2026-04-20
artifacts: [assets/brand/color/](assets/brand/color/)
dependencies: LL-013

**Notes:** SVG swatches for Option B (LED spectrum, reference), Option C (editorial warm, reference), and Hybrid (locked). Hybrid includes in-situ dark/light mode previews. (Subsequently adjusted via LL-021 for indigo shift and LL-023 for WCAG contrast.)

---

<a id="LL-015"></a>
### [x] LL-015 — Repository restructure as Obsidian vault

sprint: 4 | priority: medium | deadline: —
added: 2026-04-20 | first_engaged: 2026-04-20 | last_engaged: 2026-04-20 | resolved: 2026-04-20
artifacts: [index.md](index.md)
dependencies: —

**Notes:** YAML frontmatter + ## Related cross-linking added across README, sprint_plan, sprint_log, all docs/. MOC at index.md. Standard markdown links (not wikilinks) chosen for GitHub compatibility.

---

<a id="LL-016"></a>
### [x] LL-016 — Design assets folder scaffold

sprint: 4 | priority: low | deadline: —
added: 2026-04-20 | first_engaged: 2026-04-20 | last_engaged: 2026-04-20 | resolved: 2026-04-20
artifacts: [assets/](assets/) · [assets/README.md](assets/README.md)
dependencies: —

**Notes:** Subfolders for brand (color/logo/type/tokens), moodboards, product-photos, icons, figma, source-files. Conventions documented.

---

<a id="LL-017"></a>
### [x] LL-017 — Logo P5.js repo first-pass review

sprint: 4 | priority: low | deadline: —
added: 2026-04-20 | first_engaged: 2026-04-20 | last_engaged: 2026-04-20 | resolved: 2026-04-20
artifacts: [docs/brand-positioning.md](docs/brand-positioning.md) §7
dependencies: LL-013

**Notes:** First-pass review of Bill's separate logo repo (layered-logic/layered-logic-logo) captured. 5 parked items: color unification with brand palette, canonical rest pose, favicon variant, horizontal-arm asymmetry, optional indigo bloom layer. Out of scope this sprint.

---

<a id="LL-018"></a>
### [x] LL-018 — Firmware architecture scoping doc

sprint: 4 | priority: high | deadline: 2026-04-22
added: 2026-04-20 | first_engaged: 2026-04-20 | last_engaged: 2026-04-20 | resolved: 2026-04-20
artifacts: [docs/firmware-architecture-scoping.md](docs/firmware-architecture-scoping.md)
dependencies: LL-006

**Notes:** Locked 5 design principles (standalone out-of-box, app is pattern editor, configurable LED count, brand-palette default, closed-binary + self-hosted OTA). Staged 5 open decisions for Wed Apr 22 architecture block (LL-027). Captured feature-parity checklist from V0 ESPHome YAML, success criteria, explicit out-of-scope items.

---

<a id="LL-019"></a>
### [x] LL-019 — Advisor meeting Apr 21 doc

sprint: 4 | priority: medium | deadline: 2026-04-21
added: 2026-04-21 | first_engaged: 2026-04-21 | last_engaged: 2026-04-21 | resolved: 2026-04-21
artifacts: [docs/Advisor-Meetings/advisor-meeting-2026-04-21.md](docs/Advisor-Meetings/advisor-meeting-2026-04-21.md)
dependencies: —

**Notes:** M1 close-out, Week 4 brand/engineering deliverables summary, 5 open questions for advisor (brand scope, greenfield firmware scope, RtR liability stress-test, user research rigor, PCB production timing). Subsequently condensed in LL-023.

---

<a id="LL-020"></a>
### [x] LL-020 — Logo color decision and SVG export set

sprint: 4 | priority: medium | deadline: —
added: 2026-04-21 | first_engaged: 2026-04-21 | last_engaged: 2026-04-21 | resolved: 2026-04-21
artifacts: [assets/brand/logo/](assets/brand/logo/) (logo-primary-dark.svg · logo-primary-light.svg · logo-primary-mono.svg)
dependencies: LL-017

**Notes:** Closed parked logo item #1 (color unification). Final values: outer L `#4A25FF` (Indigo Reference), inner L `#3214FF` (Indigo Signal). Initial magenta-violet `#BF00FF`/`#A020D0` rendered too pink, triggered LL-021 indigo shift. P5 source still on `#420AFF` outer — minor sync parked.

---

<a id="LL-021"></a>
### [x] LL-021 — Palette shift to indigo

sprint: 4 | priority: high | deadline: —
added: 2026-04-21 | first_engaged: 2026-04-21 | last_engaged: 2026-04-21 | resolved: 2026-04-21
artifacts: [docs/brand-positioning.md](docs/brand-positioning.md) · [assets/brand/color/](assets/brand/color/)
dependencies: LL-013, LL-014

**Notes:** Whole vault re-anchored magenta-violet → indigo: Reference `#4A25FF`, Signal `#3214FF`, Stone `#4F3FB0`, Deep `#150A4A`. Cascade across brand-positioning, brand-research-notes, palette SVGs, logo SVGs, firmware-architecture-scoping default-color, advisor doc, project memory. Option-B SVG deleted as redundant.

---

<a id="LL-022"></a>
### [x] LL-022 — External chat archive markdown conversion

sprint: 4 | priority: low | deadline: —
added: 2026-04-21 | first_engaged: 2026-04-21 | last_engaged: 2026-04-21 | resolved: 2026-04-21
artifacts: [external_chats/](external_chats/) · [external_chats/convert_chats.py](external_chats/convert_chats.py)
dependencies: —

**Notes:** 3 Gemini JSON exports (market research, repo sharing, bank account) converted to paired .md with YAML frontmatter, summaries, tags. Apr 2 "Sharing GitHub Repos" flagged superseded by Apr 6 "Market_Research". Reusable converter script committed.

---

<a id="LL-023"></a>
### [x] LL-023 — Palette swatch WCAG contrast fixes and advisor doc condensation

sprint: 4 | priority: medium | deadline: —
added: 2026-04-21 | first_engaged: 2026-04-21 | last_engaged: 2026-04-21 | resolved: 2026-04-21
artifacts: [assets/brand/color/](assets/brand/color/) · [docs/Advisor-Meetings/advisor-meeting-2026-04-21.md](docs/Advisor-Meetings/advisor-meeting-2026-04-21.md)
dependencies: LL-014, LL-019, LL-021

**Notes:** WCAG audit caught `#3214FF` on `#0B0A0F` failing AA at 2.5:1. Section headers swapped to cream `#F4EFE6` (~14:1 AAA); mono labels on dark ground swapped to `#8E7FFF` (~6.2:1 AA-large). Hybrid swatch: background extended to full viewBox, light-mode bloom opacity boosted 0.5→0.6. Advisor meeting doc trimmed ~140→85 lines preserving all content.

---

<a id="LL-024"></a>
### [x] LL-024 — Brand vault interlink and typography/logo doc spinout

sprint: 4 | priority: medium | deadline: —
added: 2026-04-21 | first_engaged: 2026-04-21 | last_engaged: 2026-04-21 | resolved: 2026-04-21
artifacts: [docs/typography-decisions.md](docs/typography-decisions.md) · [assets/brand/logo/logo-spec.md](assets/brand/logo/logo-spec.md) · [assets/brand/logo/logo-code-notes.md](assets/brand/logo/logo-code-notes.md)
dependencies: LL-013

**Notes:** Typography rationale extracted from brand-positioning §5 to its own doc. Logo §7 split into logo-spec.md (branding/constraints/parked items) + logo-code-notes.md (P5 params/architecture/dev recipes) + deterministic SVG rest pose. Moodboards + type folder READMEs scaffolded. Cross-links tightened.

---

<a id="LL-025"></a>
### [x] LL-025 — Button interface design

sprint: 4 | priority: high | deadline: 2026-04-25
added: 2026-04-22 | first_engaged: 2026-04-22 | last_engaged: 2026-04-22 | resolved: 2026-04-22
artifacts: [docs/button-interface.md](docs/button-interface.md)
dependencies: —

**Notes:** Two-button hardware model locked. Primary exposed: single = on/advance color, double = pattern, triple = brightness, hold = off. Recessed pinhole: short-hold 3s = BLE pairing, long-hold 10s = factory reset. 17 colors (16 HSV + white), 4 brightness steps (25/50/75/100%), recessed = hold-only. Soft-off resumes prior state; hard-off resumes default. State machine, LED feedback cues, GPIO assignment all locked.

---

<a id="LL-026"></a>
### [x] LL-026 — WA Business Licensing Service registration

sprint: 4 | priority: high | deadline: 2026-04-25
added: 2026-03-30 | first_engaged: 2026-04-22 | last_engaged: 2026-04-22 | resolved: 2026-04-22
artifacts: —
dependencies: LL-001, LL-002

**Notes:** Submitted with WA Dept. of Revenue. Activities: Retailing (primary, 0.471% B&O) + Manufacturing (secondary, 0.484% B&O). NAICS 335129. $90 fee paid. UBI issued at submission; physical license arrives ~10 business days. Reseller permit follow-up tracked separately as LL-068.

---

<a id="LL-027"></a>
### [x] LL-027 — Firmware architecture decisions block

sprint: 4 | priority: high | deadline: 2026-04-22
added: 2026-04-20 | first_engaged: 2026-04-22 | last_engaged: 2026-04-22 | resolved: 2026-04-22
artifacts: [docs/firmware-architecture-scoping.md](docs/firmware-architecture-scoping.md) · [docs/firmware-security.md](docs/firmware-security.md) · [docs/control-protocol-spec.md](docs/control-protocol-spec.md) · [docs/firmware-spec.md](docs/firmware-spec.md) · [docs/app-spec.md](docs/app-spec.md) · [docs/webapp-spec.md](docs/webapp-spec.md)
dependencies: LL-018

**Notes:** 5 architecture decisions resolved: §5.1 ESP-IDF + C; §5.2 BLE primary + SoftAP fallback (radios dark at rest, user-initiated only); §5.3 two firmware variants (Pro / Pro+ Matter) with shared core lib; §5.4 self-hosted HTTPS OTA on `ota.layeredlogic.cc` with ECDSA P-256 + A/B partitions + anti-rollback + staged rollout; §5.5 BLE GATT + WebSocket + HTTP REST with JSON envelope, mDNS discovery, user-selectable auth at provisioning. Three-tier product line locked: Basic (STM8) / Pro (ESP32 + LL app) / Pro+ (Pro + Matter).

---

<a id="LL-028"></a>
### [x] LL-028 — BOI Report filing with FinCEN

sprint: 1 | priority: medium | deadline: 2026-06-21
added: 2026-03-30 | first_engaged: 2026-04-22 | last_engaged: 2026-04-22 | resolved: 2026-04-22
artifacts: https://fincen.gov/boi
dependencies: LL-001

**Notes:** Resolved N/A, Apr 22. FinCEN interim final rule exempts all US-formed domestic entities and their beneficial owners from BOI reporting. Layered Logic LLC qualifies. No filing required. **Re-check before 2026-06-21** in case rule is reversed; if reinstated needs LLC legal name, EIN, beneficial owner DOB/address/gov ID image. Marked `[x]` because definitively resolved under current rule.

---

<a id="LL-029"></a>
### [x] LL-029 — Firmware V1 bring-up: 4 core modules + host test harness

sprint: 4 | priority: high | deadline: —
added: 2026-04-22 | first_engaged: 2026-04-22 | last_engaged: 2026-04-22 | resolved: 2026-04-22
artifacts: Firmware/v1/core/{state_bus,button,led_driver,pattern_interp}/ · Firmware/v1/tests/ · [docs/pattern-dictionary.md](docs/pattern-dictionary.md) · Firmware/STM8/spec.md
dependencies: LL-027

**Notes:** Greenfield ESP-IDF firmware from scaffold to runnable end-to-end loop. state_bus (single-writer event loop, LL_EV_STATE_CHANGED), button (debounced ISR + dual gesture state machines, primary + recessed), led_driver (RMT-backed WS2812B with custom encoder + GRB reorder + brightness scaling), pattern_interp (60Hz render task pinned to core 1, 7 stateless renderers, cue overlay state machine for recessed-hold + factory-reset). Pure-C logic split for host testing (~50 tests across 8 suites). ESP-IDF 5.5.4 + MSYS2 MinGW64 toolchain end-to-end. Not yet flashed at write time; first flash landed in LL-035-0.

---

<a id="LL-030"></a>
### [x] LL-030 — Right-to-Repair philosophy doc

sprint: 4 | priority: high | deadline: 2026-04-25
added: 2026-04-20 | first_engaged: 2026-04-20 | last_engaged: 2026-04-27 | resolved: 2026-04-27
artifacts: [docs/right-to-repair-philosophy.md](docs/right-to-repair-philosophy.md)
dependencies: —

**Notes:** Status flipped draft → complete on Apr 27. Covers: core principle, three-part rationale (pro-consumer → environmental → legal), 8 concrete repair commitments, honest limits (acrylic EOL, ESP32 obsolescence, closed firmware, solo support), compliance reinforcement (EU RtR, WA HB-1933, CA SB-244), buyer ask. Cross-linked to failure-mode inventory (LL-031).

---

<a id="LL-031"></a>
### [x] LL-031 — Failure-mode inventory

sprint: 4 | priority: high | deadline: 2026-04-25
added: 2026-04-22 | first_engaged: 2026-04-22 | last_engaged: 2026-04-27 | resolved: 2026-04-27
artifacts: [Failure_Modes/](Failure_Modes/) · [Failure_Modes/README.md](Failure_Modes/README.md)
dependencies: LL-030

**Notes:** 17 individual failure-mode files across 8 categories: LED strip (3), PSU (2), controller/PCB (4), acrylic panels (2), wood frame (1), PETG (1), wiring (2), buttons (2). Each documents symptoms, causes, probability, repair path, tools/parts, difficulty, support contact, mitigations. 12 fully user-repairable, 5 partial (soldering or firmware reflash), 0 non-repairable. Validates philosophy doc's claim.

---

<a id="LL-032"></a>
### [x] LL-032 — User interview outlines

sprint: 5 | priority: medium | deadline: 2026-05-05
added: 2026-04-28 | first_engaged: 2026-04-28 | last_engaged: 2026-04-28 | resolved: 2026-04-28
artifacts: [docs/user-interview-outline.md](docs/user-interview-outline.md)
dependencies: —

**Notes:** Two outlines (~22 min each): end-buyer + showcase partner / coffee shops. Captures data concepts to collect rather than verbatim script (Bill conducts impromptu). 7-row "data we want" table tying answers back to specific open decisions (price brackets, native/webapp, Pro vs Pro+, repair messaging value). Actual interviews still pending under LL-048.

---

<a id="LL-033"></a>
### [x] LL-033 — Button Design Rationale

sprint: 5 | priority: medium | deadline: 2026-05-05
added: 2026-04-28 | first_engaged: 2026-04-28 | last_engaged: 2026-04-28 | resolved: 2026-04-28
artifacts: [docs/button-design-rationale.md](docs/button-design-rationale.md)
dependencies: LL-025

**Notes:** HCDE portfolio companion to button-interface.md. 10 sections: question (light vs smart device), audience priority, "light first" tension, 4 rejected alternatives (app-only, single-button, capacitive, encoder), why two buttons + Eero precedent, V0 gesture grammar continuity, accepted trade-offs, 6 testable hypotheses tied to interview questions, parked items. Status: draft, pre-user-research — revises after interviews land.

---

<a id="LL-034"></a>
### [x] LL-034 — V1 firmware pre-flash review and critical fixes

sprint: 5 | priority: high | deadline: —
added: 2026-04-28 | first_engaged: 2026-04-28 | last_engaged: 2026-04-28 | resolved: 2026-04-28
artifacts: Firmware/v1/core/button/button.c
dependencies: LL-029

**Notes:** Read-through of 4 V1 modules + nvs/provisioning before Session 0 smoke flash. **Critical bug found**: `ll_button_init` unconditionally accessed `LL_PIN_BUTTON_RESET=-1` on c3_devkit (would have panicked on first boot). Patched with `if (LL_PIN_BUTTON_RESET >= 0)` guards (compiler dead-code-eliminates on c3_devkit). Also fixed c3_devkit.h LED_COUNT_DEFAULT 32→66 to match V0 prototype hardware. Pin assignments verified against V0 ESPHome YAML.

---

<a id="LL-035"></a>
### [x] LL-035 — App Demo Mini-Sprint

sprint: 5 | priority: high | deadline: 2026-05-05
added: 2026-04-28 | first_engaged: 2026-04-28 | last_engaged: 2026-04-30 | resolved: 2026-04-30
artifacts: [docs/mini-sprint-app-demo.md](docs/mini-sprint-app-demo.md) · commits `5216633`, `e48c9dd`, `f1db077`, `1b86d5e`
dependencies: LL-027, LL-029, LL-034

**Notes:** 8-session push Apr 28→May 5 targeting faculty advisor demo of webapp + RN app controlling a real ESP32 mirror. Path A+B+C committed (transport + webapp + RN app). Stack: React Native bare (Android only — iOS deferred), Preact + Vite for webapp, esp_http_server + cJSON for transport. Demo board = XIAO ESP32-C3 (V0 prototype). Demo network = phone hotspot. Auth = open mode for demo (HMAC stays specced). RN skips BLE provisioning (webapp captive portal handles first-time pairing, RN discovers via mDNS). Pattern editor confirmed OUT (V2). Hardware safety rules locked: no eFuse burns. **Demo gate hit Apr 30 — 5 days early.** Sessions 0–8 tracked as children below; closed during cleanup once all children resolved.

---

<a id="LL-035-0"></a>
#### [x] LL-035-0 — Session 0: First flash on real silicon

parent: LL-035 | sprint: 5
added: 2026-04-28 | first_engaged: 2026-04-28 | last_engaged: 2026-04-28 | resolved: 2026-04-28
artifacts: sprint_log.md · commit `50035df`

**Notes:** V1 firmware booted end-to-end on XIAO ESP32-C3 after fixing two pre-flash-review-missed bugs in-session: TASK_CORE 1→0 (C3 is Unicore — `xTaskCreatePinnedToCore` asserted on `xCoreID < 1`); state_bus_defaults led_count 32→66 (was overriding board header). Boot log clean: app version `50035df`, all 4 modules init with `leds=66`, button gestures + LED response confirmed. Build: 983KB / 1MB partition (94% — flagged for ab_with_factory in next session).

---

<a id="LL-035-1"></a>
#### [x] LL-035-1 — Session 1: Transport scaffold and dev SoftAP

parent: LL-035 | sprint: 5
added: 2026-04-29 | first_engaged: 2026-04-29 | last_engaged: 2026-04-29 | resolved: 2026-04-29
artifacts: Firmware/v1/core/transport/ · Firmware/v1/core/ll_mdns/ · sprint_log.md · commits `37be211`, `9874b74-dirty`

**Notes:** Three intra-session sub-milestones tracked as grandchildren below. End state: device advertises on `_layeredlogic._tcp.local`, listens on /ws, responds to `ping` op. Session 1 DoD ("transport reachable; wscat ping/pong works") satisfied.

---

<a id="LL-035-1-1"></a>
##### [x] LL-035-1-1 — Session 1 foundation: 4MB ab_with_factory partition table

parent: LL-035-1 | sprint: 5
added: 2026-04-29 | first_engaged: 2026-04-29 | last_engaged: 2026-04-29 | resolved: 2026-04-29
artifacts: Firmware/v1/partitions.csv · Firmware/v1/sdkconfig.defaults · commit `37be211`

**Notes:** Layout: nvs 24KB at 0x9000 (default offset preserved so existing NVS survives), otadata 8KB, factory 1.25MB, ota_0 1.25MB, ota_1 1.25MB; 128KB headroom. Build green: 983KB / 1.25MB slot, 25% free. Boot green: bootloader read partitions correctly, NVS state from prior session survived. Production hardening flags (secure boot, flash encryption, anti-rollback) explicitly commented as forbidden this sprint per demo-build safety rules. No source code touched.

---

<a id="LL-035-1-2"></a>
##### [x] LL-035-1-2 — Session 1: ll_mdns module

parent: LL-035-1 | sprint: 5
added: 2026-04-29 | first_engaged: 2026-04-29 | last_engaged: 2026-04-29 | resolved: 2026-04-29
artifacts: Firmware/v1/core/ll_mdns/ll_mdns.h · Firmware/v1/core/ll_mdns/ll_mdns.c

**Notes:** Wraps espressif/mdns managed component. Service `_layeredlogic._tcp.local` on port 80 with TXT records `variant`, `version`, `id`, `auth`. Hostname `layered-logic-mirror-XXXXXX.local` (lower 3 bytes of WiFi STA MAC). Compile-time `LL_VARIANT_NAME` injected via top-level CMakeLists. **Lesson learned**: component name folder rename `core/mdns/` → `core/ll_mdns/` to avoid collision with espressif/mdns in build graph. Boot green: mDNS task announced, `service deferred until wifi up` correctly fired in wifi-down state.

---

<a id="LL-035-1-3"></a>
##### [x] LL-035-1-3 — Session 1 milestone: transport + wscat ping/pong

parent: LL-035-1 | sprint: 5
added: 2026-04-29 | first_engaged: 2026-04-29 | last_engaged: 2026-04-29 | resolved: 2026-04-29
artifacts: Firmware/v1/core/transport/transport.c · Firmware/v1/core/provisioning/provisioning.c · commit `9874b74-dirty`

**Notes:** core/transport wraps esp_http_server, /ws handler parses JSON envelope via cJSON, dispatches by `op`. Session 1 implements only `ping`; unknown ops return `unknown_op`. Dev SoftAP path under `LL_DEV_OPEN_SOFTAP=1` opens `LL-Mirror-XXXXXX` (open, no password) at boot when no creds. **Two in-session bugs caught**: (1) sdkconfig doesn't pick up sdkconfig.defaults once it exists — required `Remove-Item Firmware/v1/sdkconfig` then rebuild for `CONFIG_HTTPD_WS_SUPPORT=y`; (2) AP fast path raced subscribe order — split start_dev_softap into init + kick (publicly callable) so main.c can sequence `*_subscribe` before the wifi-up event fires. End-to-end verified: `wscat -c ws://192.168.4.1/ws` ping/pong round-trips from dev PC.

---

<a id="LL-035-2"></a>
#### [x] LL-035-2 — Session 2: Transport ↔ state-bus control surface

parent: LL-035 | sprint: 5
added: 2026-04-29 | first_engaged: 2026-04-29 | last_engaged: 2026-04-29 | resolved: 2026-04-29
artifacts: Firmware/v1/core/transport/transport.c

**Notes:** Three intra-session sub-milestones (2a/2b/2c) as grandchildren. End state: any WS client can read or write state, every other client receives the change within ~65ms. Session 2 DoD satisfied; foundation for cross-client demo.

---

<a id="LL-035-2-1"></a>
##### [x] LL-035-2-1 — Session 2a: get_state op live

parent: LL-035-2 | sprint: 5
added: 2026-04-29 | first_engaged: 2026-04-29 | last_engaged: 2026-04-29 | resolved: 2026-04-29
artifacts: Firmware/v1/core/transport/transport.c

**Notes:** Read-only op. `state_to_json()` helper serializes 7 fields (`on`, `pattern_id`, `base_color` as `#RRGGBB`, `brightness`, `led_count`, `auth_mode`, `telemetry_enabled`). State_bus exposes `const ll_state_t *` directly — read races writer task by design (eventual consistency). Verified end-to-end with NVS-persisted state.

---

<a id="LL-035-2-2"></a>
##### [x] LL-035-2-2 — Session 2b: set_state op live

parent: LL-035-2 | sprint: 5
added: 2026-04-29 | first_engaged: 2026-04-29 | last_engaged: 2026-04-29 | resolved: 2026-04-29
artifacts: Firmware/v1/core/transport/transport.c

**Notes:** Pivotal milestone — full write surface via WS. `parse_hex_color()` helper. 4 mutable fields (`on`, `base_color`, `brightness`, `pattern_id`) post matching state-bus events. Sensitive fields rejected via set_state: `auth_mode` (use set_auth_mode), `telemetry_enabled` (use set_telemetry), `led_count` (read-only). Async response design documented: result is best-effort (likely stale); broadcasts are authoritative. Visual confirmation: LEDs went bright green, then breathing/blue/50%.

---

<a id="LL-035-2-3"></a>
##### [x] LL-035-2-3 — Session 2c: cross-client state-change broadcasts

parent: LL-035-2 | sprint: 5
added: 2026-04-29 | first_engaged: 2026-04-29 | last_engaged: 2026-04-29 | resolved: 2026-04-29
artifacts: Firmware/v1/core/transport/transport.c

**Notes:** `broadcast_state()` builds `{op:"state", ts, state:{...}}` envelope, enumerates open WS sockets via httpd_get_client_list, sends async. Subscribed via `LL_EV_STATE_CHANGED`. No-op when no server up. Two-watcher triangle verified at ~65ms latency. Multi-field set_state sends N broadcasts all carrying final state — coalescing flagged as future optimization. `ts` is uptime-since-boot, not Unix time (no NTP yet).

---

<a id="LL-035-3"></a>
#### [x] LL-035-3 — Session 3: Webapp scaffold + first click changes color

parent: LL-035 | sprint: 5
added: 2026-04-29 | first_engaged: 2026-04-29 | last_engaged: 2026-04-29 | resolved: 2026-04-29
artifacts: Firmware/v1/webapp/ · commit `37be211`

**Notes:** C → TS pivot. Vite 5.4 + Preact 10.22 + TypeScript 5.5. Standalone npm project, no monorepo hoisting. Output renamed `app.js`/`app.css` with `cssCodeSplit:false` for stable embed filenames. ws-client.ts deliberately Preact-free for Session 7 RN port. UI: connection panel + color (large swatch + picker + 7 presets) + state kv table. Snapshot-on-open pattern (fires get_state immediately on connect). Color sends use `onChange` not `onInput` to avoid flooding. **Bundle: 8.6KB gzipped — 11% of 80KB budget.** Bill: "works great, change takes effect instantaneously."

---

<a id="LL-035-4"></a>
#### [x] LL-035-4 — Session 4: All 5 webapp capabilities + bundle budget gate

parent: LL-035 | sprint: 5
added: 2026-04-29 | first_engaged: 2026-04-29 | last_engaged: 2026-04-29 | resolved: 2026-04-29
artifacts: Firmware/v1/webapp/src/app.tsx · Firmware/v1/webapp/src/ws-client.ts

**Notes:** Brightness slider (commit-on-release, full integer 0-100), pattern dropdown (7 builtins), on/off toggle (`brightness 0` / restore-last-active), factory reset (UI mockup, wire op deferred to Session 5 sub-4). WS client gained reconnect-with-backoff (1s→2s→4s→8s). Local-state-during-drag pattern documented (RN slider will need same shape). Layout: connection → [power|pattern] → color → brightness → state → danger zone. **Bundle: 9.87KB gzipped — 12% of 80KB budget.** Parked: transient `socket closed` errors under user load (esp_http_server WS handler is single-task, fanout backpressure) — captured as bug #1.

---

<a id="LL-035-5"></a>
#### [x] LL-035-5 — Session 5: Captive portal flow + provisioning

parent: LL-035 | sprint: 5
added: 2026-04-29 | first_engaged: 2026-04-29 | last_engaged: 2026-04-29 | resolved: 2026-04-29
artifacts: Firmware/v1/core/{webapp_assets,captive_dns}/ · Firmware/v1/webapp/

**Notes:** Five sub-milestones tracked as grandchildren. End state: webapp end-to-end demo works (captive portal + Wi-Fi entry + fall-through to home Wi-Fi). Session 5 DoD satisfied; first demo half closed.

---

<a id="LL-035-5-1"></a>
##### [x] LL-035-5-1 — Session 5 sub-1: Webapp bundle embedded in firmware

parent: LL-035-5 | sprint: 5
added: 2026-04-29 | first_engaged: 2026-04-29 | last_engaged: 2026-04-29 | resolved: 2026-04-29
artifacts: Firmware/v1/core/webapp_assets/ · Firmware/v1/webapp/scripts/gzip-dist.mjs · commit `93f686c-dirty`

**Notes:** New component `core/webapp_assets/` owns embed + static URI handlers at `/`, `/app.js`, `/app.css`. Pre-gzip in npm build (Z_BEST_COMPRESSION) not CMake — keeps firmware build pure C/asm. Component CMakeLists checks for .gz files at configure with friendly fatal-error pointer to `npm run build`. transport.c integration is two lines + bumped `max_uri_handlers` 4→8. **One in-session bug**: `-Werror=comment` flagged nested `/*` in path glob inside outer block comment; reworded. Bonus side-quest: mDNS-over-SoftAP works on Windows (`Resolve-DnsName` resolved hostname in 120ms).

---

<a id="LL-035-5-2"></a>
##### [x] LL-035-5-2 — Session 5 sub-2: mDNS-over-hotspot Plan A confirmation

parent: LL-035-5 | sprint: 5
added: 2026-04-29 | first_engaged: 2026-04-29 | last_engaged: 2026-04-29 | resolved: 2026-04-29
artifacts: sprint_log.md (no committed code — throwaway spike, reverted)

**Notes:** Tier 1 risk validation: does Pixel 9 hotspot forward mDNS? **Yes for PC + browser via Win32 getaddrinfo path** (curl, Python urllib, ping all resolve `layered-logic-mirror-b2332c.local` to STA IP). Single failure: PowerShell's `Invoke-WebRequest` (.NET HttpClient quirk on .local). Two surprises: SSID typo from chat, Windows scan lying about band availability (C3 found 2.4GHz BSSID for same SSID even though `netsh` only showed 5GHz). Phone-side mDNS deferred to Sessions 6-8. Spike reverted clean — no creds in version control.

---

<a id="LL-035-5-3"></a>
##### [x] LL-035-5-3 — Session 5 sub-3: Captive-portal DNS hijack + HTTP 404 redirect

parent: LL-035-5 | sprint: 5
added: 2026-04-29 | first_engaged: 2026-04-29 | last_engaged: 2026-04-29 | resolved: 2026-04-29
artifacts: Firmware/v1/core/captive_dns/ · Firmware/v1/core/webapp_assets/webapp_assets.c

**Notes:** New `core/captive_dns/` (~250 lines) UDP responder gated on AP mode. Every A query returns gateway IP `192.168.4.1` (RFC 1035 compliant, name-compressed). HTTP 404 wildcard handler in webapp_assets returns `302 Location: /` for Android `/generate_204`, iOS `/hotspot-detect.html`, Windows `/connecttest.txt` etc. **Weird bug**: Unicode `→` (U+2192) in runtime format strings silently broke `vsnprintf` emission — comments fine, format strings broken. Defensive rule documented: ASCII-only in runtime format strings. End-to-end: captive-portal sheet pops automatically on phone OS join.

---

<a id="LL-035-5-4"></a>
##### [x] LL-035-5-4 — Session 5 sub-4: Setup screen + set_wifi_creds wire op

parent: LL-035-5 | sprint: 5
added: 2026-04-29 | first_engaged: 2026-04-29 | last_engaged: 2026-04-29 | resolved: 2026-04-29
artifacts: Firmware/v1/core/transport/transport.c · Firmware/v1/core/provisioning/provisioning.c · Firmware/v1/webapp/src/{hooks.ts,components/Header.tsx,pages/{control,settings,setup}.tsx,app.tsx}

**Notes:** Closes Session 5 DoD. Architectural call: option B (JSON-WS set_wifi_creds delegating into esp_wifi/NVS) over option C (full protocomm-HTTP via wifi_prov_mgr). Webapp speaks one JSON envelope; production HA support is separate concern via Matter variant. New ops: `set_wifi_creds`, `factory_reset` (wire op replacing UI mockup). State extension: `provisioning_active`, `wifi_ssid`. Webapp refactored to hash router (3 pages: /, /settings, /setup) with shared chrome. **15s fallback timer** (the demo-saver): typo'd password no longer strands the device — falls back to SoftAP automatically. Three coupled UX bug fixes (power button reads `state.on` directly; power-on patches restore brightness; color/pattern auto-power-on). Bug log: closed #3 (auto-power-on), opened #5 (Windows captive sheet → MSN), opened #6 (failed-cred error flow ergonomics). Real Pixel 9 hotspot creds verified on silicon.

---

<a id="LL-035-5-5"></a>
##### [x] LL-035-5-5 — Session 5 sub-5: DEV_ prefix cleanup

parent: LL-035-5 | sprint: 5
added: 2026-04-29 | first_engaged: 2026-04-29 | last_engaged: 2026-04-29 | resolved: 2026-04-29
artifacts: Firmware/v1/core/provisioning/provisioning.{c,h} · Firmware/v1/variants/standard/main.c · Firmware/v1/core/transport/transport.h

**Notes:** Pure rename + comment refresh; no functional/behavior/wire changes. `LL_DEV_OPEN_SOFTAP` → `LL_SOFTAP_PROVISIONING`, `LL_DEV_AP_*` → `LL_AP_*`, `init_dev_softap` → `init_softap`, etc. Public `ll_provisioning_kick_dev_softap()` → `ll_provisioning_kick_softap()`. Comment + log strings refreshed (dropped "DEV-ONLY:" / "Replace before prod" framing). transport.h top-of-file doc updated to reflect actual V1 op surface. Older sprint_log entries deliberately not retro-edited.

---

<a id="LL-035-6"></a>
#### [x] LL-035-6 — Sessions 6-8 collapsed: Bare RN app + 5 controls end-to-end

parent: LL-035 | sprint: 5
added: 2026-04-30 | first_engaged: 2026-04-30 | last_engaged: 2026-04-30 | resolved: 2026-04-30
artifacts: App/v1/ · App/v1/src/{protocol.ts,ws-client.ts} · App/v1/App.tsx

**Notes:** Three scheduled sessions landed in a single ~2.5-hour morning push. Bare RN (not Expo), Android-only this sprint, RN 0.85.2 + React 19.2.3 + TS 5.5. **Three Windows wedges hit and fixed**: gradlew.bat path, foojay-resolver-convention 0.5.0 → 1.0.0 (Gradle 9 compatibility), Windows MAX_PATH (dropped `react-native-safe-area-context` + `@react-native/new-app-screen`). Phase 2 source ports: `protocol.ts` verbatim from webapp; `ws-client.ts` with only `defaultDeviceUrl` removed (Metro has no `import.meta.env`). Single-screen UI wires all 5 capabilities. End-to-end verified on AVD against live mirror over phone hotspot: connect+ping, set_state via swatch, on/off toggle. Cross-client sync implicitly verified (same wire protocol as webapp).

---

<a id="LL-035-7"></a>
#### [x] LL-035-7 — RN app provisioning + Find Mirror + brand pass + Pixel 9 install

parent: LL-035 | sprint: 5
added: 2026-04-30 | first_engaged: 2026-04-30 | last_engaged: 2026-04-30 | resolved: 2026-04-30
artifacts: App/v1/App.tsx · App/v1/src/find-mirror.ts · App/v1/android/app/src/main/java/com/v1/{WifiInfoModule,WifiInfoPackage}.kt · App/v1/scripts/gen_app_icon.py

**Notes:** Crosses original Decision #7 from mini-sprint plan (RN does SoftAP-based provisioning end-to-end, no BLE). Setup screen + auto-route on `provisioning_active=true`. Subnet-scan discovery via WifiInfo native Kotlin module + 253 parallel fetch probes for `<title>Layered Logic Mirror</title>` (mDNS deferred to LL-040 due to Windows MAX_PATH wedge on `react-native-zeroconf`). Brand pass: app label `v1` → `LLogic`; launcher icon generated from canonical SVG via PIL (no inkscape dep); StatusBar overlap fix. **First install on physical Pixel 9 Pro** via USB tethering with `adb reverse`. Three legs verified: control over existing network, factory-reset → SoftAP provisioning round-trip → discovery + control on new network. Bill: "now I can control the mirror from my phone through the app!"

---

<a id="LL-035-8"></a>
#### [x] LL-035-8 — Cross-client sync demo verified

parent: LL-035 | sprint: 5
added: 2026-04-30 | first_engaged: 2026-04-30 | last_engaged: 2026-04-30 | resolved: 2026-04-30
artifacts: sprint_log.md
dependencies: LL-035-2-3, LL-035-7

**Notes:** Final demo-day DoD line. Bench test: webapp on dev PC + LLogic app on Pixel 9, both pointed at the same mirror on `LayeredLogicDemo` hotspot. State changes from either surface propagate to other within broadcast latency. Bill: "I can make changes from the webapp on the dev pc that are seen in the mirror and in the mobile app." **Mini-sprint DoD fully green five days ahead of May 5 advisor meeting.** Minor follow-up parked: webapp/RN color-swatch presets inconsistent (resolved later in LL-038-adjacent work, see commit `cf43899`).

---

<a id="LL-036"></a>
### [x] LL-036 — Right-to-Repair portfolio rationale (decisions matrix + scorecard + index)

sprint: 5 | priority: high | deadline: —
added: 2026-04-30 | first_engaged: 2026-04-30 | last_engaged: 2026-04-30 | resolved: 2026-04-30
artifacts: [docs/repair-design-decisions.md](docs/repair-design-decisions.md) · [docs/repairability-scorecard.md](docs/repairability-scorecard.md) · [docs/repair-index.md](docs/repair-index.md)
dependencies: LL-030, LL-031

**Notes:** Closes Week 4 carryover for HCDE portfolio repair story. Three new docs add the analytical / quantitative / navigational layer over existing philosophy + failure-mode inventory. Decisions matrix: 20 rows mapping design choices → 5 repair values + failure modes pre-empted. Scorecard: 17 modes / 12 fully repairable (71%) / 1-tool requirement / 7-yr parts commitment. Repair-index sub-MOC tells different audiences (buyer/advisor/regulator/engineer/repair shop) which path to take. Linked from index.md home note. §5 comparative table TBD pending iFixit / EU framework data.

---

<a id="LL-037"></a>
### [x] LL-037 — Brand-positioning §1 sentence locked

sprint: 5 | priority: medium | deadline: —
added: 2026-04-30 | first_engaged: 2026-04-30 | last_engaged: 2026-04-30 | resolved: 2026-04-30
artifacts: [docs/brand-positioning.md](docs/brand-positioning.md) §1
dependencies: LL-013

**Notes:** Last template-Mad-Libs slot in brand-positioning §1. Locked text: *"For design-conscious adults who want atmosphere over gadgetry, [product name] is a made-to-order LED light line that blends into a living space as ambient presence and that you can still repair with a screwdriver ten years later — designed, built, and documented as a system to keep, not a novelty to throw away."* Mini-brand `[product name]` slot remains bracketed (Tier 2 naming deferred). Apr 20 seed kept in `<details>` for traceability.

---

<a id="LL-038"></a>
### [x] LL-038 — OTA infrastructure end-to-end validation V1→V2

sprint: 5 | priority: high | deadline: —
added: 2026-04-30 | first_engaged: 2026-04-30 | last_engaged: 2026-04-30 | resolved: 2026-04-30
artifacts: Firmware/v1/core/ota/ · App/v1/src/ws-client.ts (startOta) · commit `1b86d5e`
dependencies: LL-027 (§5.4 OTA decisions)

**Notes:** Throwaway dev-test path proves A/B partition boot + esp_https_ota integration before investing in production plumbing. Welcome sequence at boot doubles as OTA-success canary (V1 white → V2 cyan). New `core/ota/` minimal HTTP-pull module (header explicitly calls out throwaway scope). New `start_ota` WS op. **Bug fixed in-session**: `CONFIG_ESP_HTTPS_OTA_ALLOW_HTTP=y` was missing — first OTA returned `ESP_ERR_INVALID_ARG`. LLogic app gains Settings page with Firmware update panel. End-to-end verified on silicon, USB unplugged: cyan welcome on reboot, ping confirmed `1b86d5e-dirty`. Production OTA (signed binaries + Cloudflare Worker + R2 + anti-rollback) parked as separate workstream LL-NNN-tbd post-demo. **Binary at 91% utilization — getting tight.**

---

<a id="LL-039"></a>
### [x] LL-039 — User-definable mirror name + multi-mirror discovery

sprint: 5 | priority: medium | deadline: —
added: 2026-04-30 | first_engaged: 2026-04-30 | last_engaged: 2026-04-30 | resolved: 2026-04-30
artifacts: Firmware/v1/core/{state_bus,nvs,transport}/ · App/v1/src/find-mirror.ts · App/v1/App.tsx · Firmware/v1/webapp/src/pages/settings.tsx · commit `f1db077-dirty`
dependencies: LL-035-7

**Notes:** Self-contained subset of V2 multi-mirror story. `ll_state_t.name` (32-char user string, default empty), read-only `id` (MAC suffix), `LL_EV_NAME_CHANGE` event. NVS schema bumped 1→2 (existing test mirror reset to defaults — by design, no migration path until shipped users exist). New `GET /api/info` HTTP endpoint returns `{product, id, name, fw_version}` with CORS. RN findMirrors plural with picker (rendering name → falls back to id). Settings rename input in both webapp + RN. **OTA cycle V2→V3 verified end-to-end on silicon**, USB unplugged. Diagnostic gotcha: WS client kept TCP socket across reboot, returned cached app-desc on first ping — always re-probe via HTTP first then WS.

---

<a id="LL-040"></a>
### [x] LL-040 — App connectivity hardening: cleartext + native mDNS + IP-in-state

sprint: 5 | priority: high | deadline: —
added: 2026-05-01 | first_engaged: 2026-05-01 | last_engaged: 2026-05-01 | resolved: 2026-05-01
artifacts: App/v1/android/app/src/main/AndroidManifest.xml · App/v1/android/app/src/main/java/com/v1/MdnsModule.kt · App/v1/src/find-mirror.ts · Firmware/v1/core/{provisioning,transport}/ · [Firmware/v1/README.md](Firmware/v1/README.md) · commit `e48c9dd`
dependencies: LL-035-7, LL-039

**Notes:** Three independent fixes shipped end-to-end on silicon + Pixel 9. (1) **Cleartext-traffic block on release builds**: RN gradle plugin's `finalizeDsl` clobbers user override; fixed by hardcoding `android:usesCleartextTraffic="true"` in AndroidManifest, nuking the placeholder. (2) **Native mDNS module replaces 254-parallel subnet scan**: OkHttp dispatcher caps concurrent at 64 — first 64 slots stuck on ARP timeouts blocked the actual mirror IP. New Kotlin `MdnsModule` wrapping `NsdManager` (single-flight resolves, IPv4-only, 2.5s window). Resolves in 2-3s vs scan that couldn't finish. (3) **`ip` field in DeviceState**: mirrored existing `g_sta_ssid` pattern; populated via IPSTR on wifi-up; Settings page in app shows IP under Network. **OTA + flash-config rabbit hole** captured in new "Building" section of Firmware/v1/README.md (webapp prereq, set-target verification, PROJECT_VER bump, chip-target-switching gotcha). Two new feedback memories saved (`feedback_rf_minimal_unless_asked`, `feedback_respect_explicit_actions`). Follow-up bug not fixed: app OTA button stuck on "Mirror downloading + rebooting" forever after success — needs `useEffect` reset on `conn==='open'` after `otaState==='rebooting'`.

---

<a id="LL-041"></a>
### [x] LL-041 — Multi-network design doc

sprint: 5 | priority: medium | deadline: —
added: 2026-05-01 | first_engaged: 2026-05-01 | last_engaged: 2026-05-01 | resolved: 2026-05-01
artifacts: [docs/multi-network-design.md](docs/multi-network-design.md)
dependencies: —

**Notes:** Full spec for storing N saved Wi-Fi networks in NVS, scan-and-pick reconnect, protocol additions (`list/add/remove_wifi_network`, `set_wifi_creds` kept as deprecated shim), app UX, ~6 days of implementation. §10 Locked Decisions: Q1 priority = last-used wins; Q2 cap N_MAX=4 (Bill's realistic personal-use ceiling); Q3 boot-no-network = stay in BACKOFF indefinitely, **no auto-SoftAP** (per `feedback_rf_minimal_unless_asked`); Q4 remove-while-connected = proactive disconnect + app-side confirm dialog (per `feedback_respect_explicit_actions`); Q5 set_wifi_creds deprecation = keep through V2 lifetime, remove with next firmware major. Status: spec — implementation-ready. Implementation tracked separately as LL-046.

---

<a id="LL-072"></a>
### [x] LL-072 — Migrate admin_workflow GitHub feed to LL-NNN stable IDs

sprint: 6 | priority: medium | deadline: —
added: 2026-05-06 | first_engaged: 2026-05-06 | last_engaged: 2026-05-06 | resolved: 2026-05-06
artifacts: external — `admin_workflow/n8n-nodes/infinity_mirror_{task_collector,accomplishments,reconcile,validation}.js` · `admin_workflow/n8n-nodes/task_collector_sql_builder.js`
dependencies: —

**Notes:** Retooled the four n8n flows in `admin_workflow` that consume this repo's tasks.md so the planning + reconciliation pipeline operates on stable LL-NNN IDs instead of slug-fuzzy-matching `sprint_plan.md` through GPT-5-nano. Four structural-parse Code nodes shipped: morning task_collector (one item per open `[ ]`), accomplishments scraper (`resolved` or `last_engaged` = today), reconcile (task_log.md events → outcome_events), validation (structural invariants — every `[x]` has a `done` log row, sprint_log `[LL-NNN]` tags resolve, `next_id` counter consistency, no top-level ID gaps). Output shape conformed to the PascalCase `{Id, Source, Title, Notes, Importance, Deadline, Meta}` envelope used by the existing Trello / Canvas / TrainerRoad collectors; `task_collector_sql_builder.js` gained a `normalizeTask` shim so PascalCase and legacy lowercase coexist without a forked code path. Postgres scrubbed of 9 legacy `github:sprint:*` rows + 22 volatile rows; downstream namespace is now `github:task:LL-NNN`, immutable across renames — task identity no longer requires an LLM hop.

---

<a id="LL-042"></a>
### [ ] LL-042 — User Repair Guide / Repairability Manual

sprint: 4 | priority: high | deadline: 2026-05-23
added: 2026-04-22 | first_engaged: 2026-04-22 | last_engaged: 2026-04-30
artifacts: —
dependencies: LL-030, LL-031, LL-036

**Notes:** Customer-facing PDF or web page covering common failures + fixes (LED replacement, controller swap, connectivity troubleshooting). Sprint 4 plan called for draft; sprint 8 plan calls for finalization. Listed in Week 4 In-Progress section. Largely blocked on assembly guide which lives in operations track. Status updated 2026-04-30 — RtR portfolio docs (LL-036) covered the analytical layer; the user-facing guide itself is still pending. Active sprint: 5–8.

---

<a id="LL-043"></a>
### [ ] LL-043 — User interview contact list and outreach

sprint: 4 | priority: medium | deadline: 2026-05-09
added: 2026-04-22 | first_engaged: 2026-04-28
artifacts: —
dependencies: LL-032

**Notes:** Interview outline drafted via LL-032 (Apr 28); compiling the contact list and reaching out to 3-5 end-buyers + a few coffee-shop showcase partners still pending. In-progress per Week 4 sprint_log. Active sprint: 5.

---

<a id="LL-044"></a>
### [ ] LL-044 — Packaging QR code

sprint: 4 | priority: low | deadline: —
added: 2026-04-27 | first_engaged: 2026-04-27 | last_engaged: 2026-04-28
artifacts: —
dependencies: LL-051
block_reason: Needs final packaging design (cardboard stock, dimensions, print/laser process) decided first; concept stays parked until packaging is far enough along to know what surface the QR lives on.

**Notes:** Laser-etch or perforate into outer cardboard in the same cutting pass; zero extra material, surfaces the repair philosophy at unboxing rather than hiding it on a marketing page. Captured in [docs/right-to-repair-philosophy.md](docs/right-to-repair-philosophy.md). Blocked Apr 28 awaiting LL-051 (packaging concept).

---

<a id="LL-045"></a>
### [ ] LL-045 — Production PCB order

sprint: 3 | priority: medium | deadline: —
added: 2026-03-30
artifacts: —
dependencies: —
sprint_target: post-quarter

**Notes:** Originally scoped as Week 3 critical path. Silently slipped — no Done entry, no explicit cancel. Per [project_pcb_status memory](.claude/projects/C--Users-bowhi-Desktop-Independent-Study/memory/project_pcb_status.md), 5 STM8 test boards already on hand from Apr 20; schematic update + production order deferred, not blocking near-term firmware work. Multi-network firmware work (LL-046) replaces this on the Week 6 critical path. Re-enter scope when production-volume hardware is needed.

---

<a id="LL-046"></a>
### [x] LL-046 — Multi-network firmware implementation

sprint: 6 | priority: high | deadline: 2026-05-09
added: 2026-05-01 | first_engaged: 2026-05-07 | last_engaged: 2026-05-07 | resolved: 2026-05-07
artifacts: [Firmware/v1/core/ll_wifi/](Firmware/v1/core/ll_wifi/) · [Firmware/v1/core/provisioning/provisioning.c](Firmware/v1/core/provisioning/provisioning.c) · [App/v1/App.tsx](App/v1/App.tsx)
dependencies: LL-041

**Notes:** Implements the spec locked in LL-041 (multi-network design doc). NVS schema bump for N saved networks (N_MAX=4), scan-and-pick reconnect, protocol additions (`list/add/remove_wifi_network`), `set_wifi_creds` retained as deprecated shim through V2 lifetime. App UX: settings page network list with last-used-wins ordering, proactive disconnect on remove with confirm dialog. ~6 days estimated implementation. Proposed Week 6 engineering replacement for the no-longer-real PCB-arrival item.

Step 1/6 (NVS layer + migration shim) shipped May 7: new `core/ll_wifi/` module with `wifi_entry_t`, list ops (find/add/remove/pick_next/sanitize), per-entry NVS persistence, mutex-guarded singleton, and a separate `ll_wifi_migrate_from_esp_wifi(ssid, password)` hook so the dependency direction stays one-way (provisioning depends on ll_wifi, not vice versa). 27 host tests cover sizing, find, add (insert/update/full/invalid), remove (compaction + active_idx fixup), pick_next priority + recently-failed mask + tiebreak, and sanitize.

Step 2/6 (provisioning refactor) shipped May 7: `provisioning.c` now drives STA from `ll_wifi`. Boot-time legacy cred migration via `migrate_legacy_cred_if_needed()`; `esp_wifi_set_storage(WIFI_STORAGE_RAM)` so `esp_wifi`'s NVS becomes vestigial; new helpers `find_idx_by_ssid` / `apply_entry_to_esp_wifi`; `on_wifi_apply_creds` writes to `ll_wifi` first then drives the SoftAP→STA handoff; apply-creds fallback removes the failed entry from `ll_wifi` (was previously `esp_wifi_restore` only); `on_factory_reset` calls `ll_wifi_erase_all` alongside `esp_wifi_restore`; `post_wifi_connected` stamps `ll_wifi_set_active(idx, last_used_us)`. **OTA-flashed and verified on live mirror b2332c** — `fw_version` flipped `ip-state-v3` → `4c1f888-dirty`, STA auto-reconnected to "IoT", all 7 `ll_settings` fields preserved, migration shim real-hardware-confirmed.

Step 3/6 (SCANNING/PICKING/BACKOFF state machine) shipped May 7. New SM in `provisioning.c`: 5 states (IDLE/SCANNING/CONNECTING/ONLINE/BACKOFF), backoff 5s/15s/60s, 8s connect-watchdog, recently-failed bitmask cleared on BACKOFF→SCANNING. PICKING is sub-logic in `sm_handle_scan_done()` — builds a not-visible mask from scan results, ORs with recently-failed, picks via `ll_wifi_list_pick_next`. STA_START kicks the SM (boot path) or calls `esp_wifi_connect` directly (apply-creds path); STA_DISCONNECTED branches on SM state. Factory reset and apply-creds fallback both tear the SM down to IDLE. Boot path simplified — no more `pick_boot_idx`/`apply_entry_to_esp_wifi` at init; the SM kicks itself from STA_START. **First OTA bricked the live mirror** via stack overflow in `sm_handle_scan_done` (1.6 KB `wifi_ap_record_t` array on ~2.3 KB system event task stack); fix heap-allocates the records buffer. USB recovery flashed the stack-fixed build; mirror booted clean, SM connected to "IoT" autonomously through SCANNING→PICKING→CONNECTING→ONLINE.

OTA rollback safety net added during Step 3 (May 7 — out of LL-046's original scope, surfaced by the brick). `CONFIG_BOOTLOADER_APP_ROLLBACK_ENABLE=y` in `sdkconfig.defaults`; new `ll_ota_subscribe()` calls `esp_ota_mark_app_valid_cancel_rollback()` on first `LL_EV_WIFI_CONNECTED` after boot. Future OTAs that panic before getting online (or never connect) will auto-rollback to the previous slot. Wired into `main.c` after captive-DNS. Orthogonal to LL-071's eFuse anti-rollback (downgrade defense vs. boot-loop recovery). Bootloader change deployed via USB flash.

Step 4/6 (wire protocol ops) shipped + OTA-verified May 7. Three new ops in `transport.c`: `list_wifi_networks` (sorted by last_used desc, includes is_active), `add_wifi_network` (insert-or-update, never switches active), `remove_wifi_network` (forget by SSID; proactively disconnects via new `ll_provisioning_drop_active()` if removing the active SSID, the SM picks the next eligible). `DeviceState.wifi_saved_count` added per design-doc §7.5 (count only — full list on demand). add/remove trigger `broadcast_state()` so connected clients refresh. `set_wifi_creds` kept as legacy first-cred entry point per §7.4. Smoke-tested 9 cases on live mirror — insert / update / full / invalid / remove / not_found / wifi_saved_count round-trip all green.

Step 5/6 (RN app Settings UI) shipped May 7. `App/v1/` Settings page replaces the single "Reconfigure Wi-Fi" block with a saved-networks list (rendered from `list_wifi_networks`, active-checkmark + per-row Forget button) and an inline "+ Add a network" form. Forget on the active SSID surfaces a confirm dialog (per design-doc §4.3); non-active forgets silently. `protocol.ts` gained `WifiNetwork` / `Add*` / `Remove*` types + `wifi_saved_count` on DeviceState; `ws-client.ts` gained the matching thin wrappers. List refetches on Settings open and on `wifi_saved_count` broadcasts. Setup-flow form (`provisioning_active=true`) unchanged — still uses `set_wifi_creds` per §8.2. Hardware-on-device verification deferred to Step 6 / cross-network E2E.

Step 6 hardware test (May 7) surfaced two issues. (1) The release APK installed and the multi-network UI worked — Bill exercised `set_wifi_creds`, `add_wifi_network`, `remove_wifi_network` paths cleanly. But he hit a recovery gap: bad creds on a second network + Forget on the first (good) network ⇒ mirror stuck in BACKOFF with no auto-SoftAP fallback (per design-doc Q3 / `feedback_rf_minimal_unless_asked`). Recovery via USB `esptool erase_region 0x9000 0x6000` (NVS partition only — preserves firmware, wipes ll_settings + ll_wifi + esp_wifi cred). (2) Bill flagged the bigger feature gap: the spec deliberately omits a "switch to this network" op (§4.2 Q1, "Adding ≠ joining"), but the failure mode he hit proves it's needed. Added a new `connect_wifi_network(ssid)` wire op + `ll_wifi_bump_priority` (last_used only) + `ll_provisioning_request_switch` (handles all SM states); RN app gains a "Connect" button per non-active row. Firmware USB-flashed; APK rebuilt.

Status-bar overlap fix landed in the same session (out of LL-046's original scope but Bill flagged it during testing): module-load read of `StatusBar.currentHeight` returned 0 in cold-start races on Android 15 / Pixel 9, leaving the Settings nav icon untappable under the system bar. Tried `react-native-safe-area-context` first; the deep worktree path blew the C++ codegen filenames past Windows MAX_PATH and a `subst`-aliased drive broke `path.relative` cross-drive in `@react-native/codegen`. Shipped a smaller render-time read floored at 32px instead — no native deps, no Windows-path risk, same outcome.

connect_wifi_network UX polish (May 7): first hardware exercise of the Connect button had no haptic/visual feedback on tap and the WS op was timing out client-side because `ll_provisioning_request_switch()` was tearing the STA down synchronously before transport could flush the response. Refactored to post a new `LL_EV_WIFI_REQUEST_SWITCH` event; provisioning's handler runs on the state-bus task with a 250ms response-flush yield (same pattern as `on_wifi_apply_creds`). App side: optimistic-clear of `is_active` on tap (per Bill's UX call), "Switching to X…" banner, 15ms `Vibration.vibrate` haptic, and the Settings list-refetch `useEffect` now keys on `state.wifi_ssid` so the post-switch settle replaces the optimistic state. Three follow-on fixes after a second hardware pass: VIBRATE permission added to AndroidManifest (was crashing the app on tap), banner copy reworded to tell the user to match their phone's Wi-Fi and re-run Find mirror (auto-reconnect is a lie unless the phone is on the same network), and the Find-mirror "no mirror found" error now mentions disabling VPN as the most common cause.

---

<a id="LL-047"></a>
### [ ] LL-047 — Wireframes in Figma

sprint: 5 | priority: medium | deadline: 2026-05-09
added: 2026-04-22
artifacts: —
dependencies: —

**Notes:** App UX/UI wireframes informed by user research and informing implementation. Per Week 5 plan: "wireframes inform implementation, working builds inform design rationale." Working builds shipped first via LL-035 mini-sprint (5 days early); wireframes still owed for the portfolio retrospective view of the design process.

---

<a id="LL-048"></a>
### [ ] LL-048 — Informal end-buyer interviews (3-5 people)

sprint: 4 | priority: high | deadline: 2026-05-09
added: 2026-04-22
artifacts: —
dependencies: LL-032, LL-043

**Notes:** Test wireframe concepts informally; gather data on price brackets, customization appetite, control preferences, values alignment, discovery patterns. Per Week 4 plan + Week 5 continuation. Outline ready (LL-032); contact list pending (LL-043). 6 testable hypotheses from button-design rationale (LL-033) tied to specific interview questions.

---

<a id="LL-049"></a>
### [ ] LL-049 — Coffee shop / showcase partner interviews

sprint: 4 | priority: medium | deadline: 2026-05-16
added: 2026-04-22
artifacts: —
dependencies: LL-032, LL-043

**Notes:** Conversations with local coffee shops to understand how they choose artists/makers to showcase. Outline ready (LL-032); contact list pending (LL-043). Distinct audience from end-buyer interviews — different incentives, different decision criteria.

---

<a id="LL-050"></a>
### [x] LL-050 — Supply chain map

sprint: 6 | priority: high | deadline: 2026-05-09
added: 2026-03-30 | first_engaged: 2026-05-07 | last_engaged: 2026-05-07 | resolved: 2026-05-07
artifacts: [docs/supply-chain-map.md](docs/supply-chain-map.md)
dependencies: LL-010, LL-011

**Notes:** Every component, lead time, cost, backup supplier. Per Week 6 plan. Builds on BOM (LL-010) and JLCPCB analysis (LL-011); adds supplier diversity and lead-time risk exposure. Required input for Milestone 2. Resolved 2026-05-07: 13-line component table with primary + backup cascade per row, executive risk summary (tariff = high, lead time = medium-but-graceful, single-source = low), and 3 open actions (find die-cut packaging vendor, spec LED-holder/back-reflector parts, finalize PSU spec to USB-C PD). Standard sourcing cascades documented: Amazon → AliExpress → Temu → factory direct for commodity items; JLCPCB → PCBWay → OSH Park for PCB fab.

---

<a id="LL-051"></a>
### [ ] LL-051 — Packaging concept

sprint: 6 | priority: medium | deadline: 2026-05-09
added: 2026-03-30
artifacts: —
dependencies: —

**Notes:** Cardboard stock, dimensions, print/laser process. Per Week 6 plan. Blocking dependency for LL-044 (packaging QR code).

---

<a id="LL-052"></a>
### [ ] LL-052 — PCB hardware integration and validation

sprint: 6 | priority: medium | deadline: 2026-05-09
added: 2026-03-30
artifacts: —
dependencies: LL-045

**Notes:** Per Week 6 plan: "PCBs arrive. Solder and test the new board. Validate that the dev board prototyping translates to the custom PCB." Blocked on LL-045 (PCB order slipped). Demo build path is staying on XIAO ESP32-C3 dev kit per [project_pcb_status memory](.claude/projects/C--Users-bowhi-Desktop-Independent-Study/memory/project_pcb_status.md).

---

<a id="LL-053"></a>
### [ ] LL-053 — Service blueprint

sprint: 7 | priority: high | deadline: 2026-05-16
added: 2026-03-30
artifacts: —
dependencies: LL-004, LL-007

**Notes:** Map the complete user journey: ad → browse → choose standard or custom → purchase → unbox → setup → daily use → troubleshoot/repair. HCDE artifact demonstrating "system of interactions" rather than just product. Per Week 7 plan.

---

<a id="LL-054"></a>
### [ ] LL-054 — Custom order flow mapping

sprint: 7 | priority: high | deadline: 2026-05-16
added: 2026-03-30
artifacts: —
dependencies: LL-053

**Notes:** Bespoke design process is the primary value differentiator. Map: how customer communicates custom vision → translation into buildable design → approval flow → upsell tiers (size, complexity, frame material). Per Week 7 plan. Lives inside LL-053's broader blueprint.

---

<a id="LL-055"></a>
### [ ] LL-055 — Firmware Wi-Fi/BLE resilience stress-test

sprint: 7 | priority: medium | deadline: 2026-05-16
added: 2026-03-30
artifacts: —
dependencies: LL-029, LL-046

**Notes:** Wi-Fi drop handling, BLE reconnection, network jitter under load. Technical "usability" — closes spec §7 of [docs/firmware-spec.md](docs/firmware-spec.md). Includes addressing the parked transient-socket-closed bug from LL-035-4 ([post-mini-sprint-bugs.md #1](docs/post-mini-sprint-bugs.md)). Per Week 7 plan.

---

<a id="LL-056"></a>
### [ ] LL-056 — Terms of Service and Warranty policy

sprint: 8 | priority: high | deadline: 2026-05-23
added: 2026-03-30
artifacts: —
dependencies: LL-001, LL-030

**Notes:** Balance pro-consumer values with business liability. Define return/refund policy. ToS for online sales. Warranty terms (length, what's covered, what's not). Per Week 8 plan. RtR philosophy (LL-030) shapes the warranty ergonomics; legal entity (LL-001) provides the contracting party.

---

<a id="LL-057"></a>
### [ ] LL-057 — App ↔ hardware integration polish

sprint: 8 | priority: medium | deadline: 2026-05-23
added: 2026-03-30
artifacts: —
dependencies: LL-035, LL-040

**Notes:** Per Week 8 plan. Most engineering integration was front-loaded into LL-035 mini-sprint and LL-040 hardening. Remaining scope: stuck-OTA-button bug (open from LL-040 follow-ups, in [post-mini-sprint-bugs.md](docs/post-mini-sprint-bugs.md)), open bugs #1, #2, #6 from the bug log, BLE provisioning stretch goal, telemetry/auth surfaces, OTA UI polish.

---

<a id="LL-058"></a>
### [ ] LL-058 — Product photography and video

sprint: 9 | priority: high | deadline: 2026-05-30
added: 2026-03-30
artifacts: —
dependencies: LL-060

**Notes:** High-quality demo content reusable for portfolio, Etsy listings, personal website, and a potential Kickstarter page. Per Week 9 plan. Depends on golden-sample mirror (LL-060) being build-complete and presentable.

---

<a id="LL-059"></a>
### [ ] LL-059 — BOM finalization for v1.0

sprint: 9 | priority: high | deadline: 2026-05-30
added: 2026-03-30
artifacts: —
dependencies: LL-010, LL-050

**Notes:** Final v1.0 release BOM with locked suppliers + lead times. Builds on prelim BOM (LL-010) and supply chain map (LL-050). Per Week 9 plan.

---

<a id="LL-060"></a>
### [ ] LL-060 — Pre-production "golden sample" mirror build

sprint: 9 | priority: high | deadline: 2026-05-30
added: 2026-03-30
artifacts: —
dependencies: LL-052, LL-059

**Notes:** The pre-production mirror — proof that the system can be built end-to-end as documented. Per Week 9 plan ("the 'golden sample.'"). Subsequent build (LL-064) produces the boxed-as-shipped final.

---

<a id="LL-061"></a>
### [ ] LL-061 — Portfolio case study

sprint: 10 | priority: high | deadline: 2026-06-06
added: 2026-03-30
artifacts: —
dependencies: LL-004, LL-009, LL-013, LL-030, LL-031, LL-036, LL-048, LL-053

**Notes:** Comprehensive case study of the full Design Engineering process. Highlights transition from single-button STM8 prototype to ESP32/app-ready version, justified through user customization + HCD lens. Includes stakeholder map, sustainability audit, user research findings, service blueprint, repairability manual, brand identity, business model, technical architecture. Per Week 10 plan.

---

<a id="LL-062"></a>
### [ ] LL-062 — Venture readiness document

sprint: 11 | priority: high | deadline: 2026-06-12
added: 2026-03-30
artifacts: —
dependencies: LL-050, LL-056, LL-061

**Notes:** "If you started selling June 13, what are the remaining risks?" Shipping costs, component lead times, app store approval, support capacity. Final report for the quarter. Per Week 11 plan.

---

<a id="LL-063"></a>
### [ ] LL-063 — Kickstarter readiness assets

sprint: 11 | priority: medium | deadline: 2026-06-12
added: 2026-03-30
artifacts: —
dependencies: LL-058, LL-061

**Notes:** All media, copy, reward tiers, funding goals documented — ready to launch a campaign if/when the decision is made. Per Week 11 plan.

---

<a id="LL-064"></a>
### [ ] LL-064 — Final golden sample assembly + boxed

sprint: 11 | priority: high | deadline: 2026-06-12
added: 2026-03-30
artifacts: —
dependencies: LL-060

**Notes:** Final mirror finished and boxed as if being shipped to a customer. Per Week 11 plan. The "flip the switch" demonstration.

---

<a id="LL-065"></a>
### [ ] LL-065 — WA state annual report

sprint: 1 | priority: medium | deadline: 2027-03-31
added: 2026-03-30
artifacts: —
dependencies: LL-001

**Notes:** Recurring obligation. Check WA Secretary of State website for filing requirements, deadlines, fees. First filing due ~12 months after LLC formation date.

---

<a id="LL-066"></a>
### [ ] LL-066 — Registered agent confirmation

sprint: 1 | priority: low | deadline: —
added: 2026-03-30
artifacts: —
dependencies: LL-001

**Notes:** Confirm registered agent on file (self at physical address, or registered agent service). Required to maintain LLC good standing. Likely already set during LL-001 filing — needs verification.

---

<a id="LL-067"></a>
### [ ] LL-067 — City/county business license check

sprint: 1 | priority: medium | deadline: —
added: 2026-03-30
artifacts: —
dependencies: LL-026

**Notes:** Verify whether business address requires a city or county endorsement on top of WA BLS state registration. Selling physical products online may require general business license.

---

<a id="LL-068"></a>
### [ ] LL-068 — Reseller permit follow-up

sprint: 4 | priority: medium | deadline: —
added: 2026-04-22
artifacts: —
dependencies: LL-026

**Notes:** Separate application from BLS, free, 4-yr validity. Saves sales tax on component purchases from WA suppliers. File once UBI is confirmed (physical license arrives ~10 business days after BLS submission).

---

<a id="LL-069"></a>
### [ ] LL-069 — Estimated quarterly taxes setup

sprint: 1 | priority: medium | deadline: 2026-04-15
added: 2026-03-30
artifacts: —
dependencies: LL-002

**Notes:** Single-member LLC income passes through to personal return. If expecting to owe >$1,000, IRS expects quarterly estimated payments via Form 1040-ES. Deadlines: Apr 15, Jun 15, Sep 15, Jan 15. First deadline already passed for 2026; recurring obligation.

---

<a id="LL-070"></a>
### [ ] LL-070 — Bookkeeping setup

sprint: 1 | priority: medium | deadline: —
added: 2026-03-30
artifacts: —
dependencies: LL-008

**Notes:** Track all income and expenses from day one. Spreadsheet acceptable to start; Wave or QuickBooks Self-Employed as volume grows. Becomes load-bearing once first revenue lands and quarterly estimated tax math depends on it.

---

<a id="LL-071"></a>
### [ ] LL-071 — Production OTA infrastructure

sprint: 5 | priority: medium | deadline: —
added: 2026-04-30
artifacts: [docs/firmware-architecture-scoping.md](docs/firmware-architecture-scoping.md) §5.4
dependencies: LL-038

**Notes:** Production-grade OTA: signed binaries (ECDSA P-256, key in offline password manager), self-hosted on `ota.layeredlogic.cc` (Cloudflare Worker + R2), `CONFIG_BOOTLOADER_APP_ANTI_ROLLBACK` (eFuse burn — out of scope this sprint per demo-build hardware safety rules), auto-revert on health-check failure within 60s, staged rollout via device-ID hash buckets. Replaces the throwaway dev-test path from LL-038. Post-demo workstream — eFuse burns are irreversible, only safe to enable after secure-boot strategy is fully locked.

**Architecture decisions added 2026-05-07** (in conversation while OTA-flashing LL-046 Step 2):
- **Pull-based, manifest-then-binary.** Device polls a small JSON manifest at `ota.layeredlogic.cc` carrying `{version, url, sha256, signature}`. Only fetches the (~1.2 MB) binary if `version` is newer than what it's running. Manifest is ~200 bytes; binaries can be GitHub Releases-hosted with the manifest pointing at them — keeps storage/bandwidth on GitHub while keeping the routing/staged-rollout/observability surface on Bill's domain.
- **Poll interval: 48 hours, configurable via NVS-backed setting.** Bill's expected update cadence is "rarely urgent," so 48h gives a reasonable freshness floor without thrashing the device. Surface as a hidden/dev setting (e.g., `ota_poll_interval_h`) so it can be lowered for staged rollout investigations or raised for power-sensitive deployments without a firmware push.
- **UX is prompt-and-confirm via the LL app.** App surfaces "update available" when `current < manifest.version`; user taps "install" to actually trigger the download + reboot. Auto-install would mean unexpected mid-use reboots — explicitly avoided per consumer-hardware UX norms.
- **Cohort routing via Cloudflare Worker.** Worker hashes `device_id` from the request + maps to a rollout bucket (e.g., 5% canary → 50% → 100%). Bad release stays contained until the canary catches it.

---

## Related

- [Sprint Plan](sprint_plan.md) — narrative roadmap; tasks here carry inline `[LL-NNN]` links into this registry
- [Sprint Log](sprint_log.md) — daily progress narrative
- [Task Event Log](task_log.md) — append-only event history
- [Task Format Spec](docs/task-format-v2.md) — schema, ID rules, migration plan
