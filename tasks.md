---
title: Task Registry
type: task-registry
next_id: LL-088
updated: 2026-06-05

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

**Notes:** 8-session push Apr 28→May 5 for a faculty-advisor demo of the webapp + RN app controlling a real ESP32 mirror. Stack: React Native bare (Android-only), Preact+Vite webapp, esp_http_server+cJSON transport; demo board XIAO ESP32-C3, phone-hotspot network, open-mode auth, no BLE provisioning. Pattern editor confirmed out (V2); no eFuse burns. **Demo gate hit Apr 30, 5 days early.** Sessions 0–8 tracked as children below; closed during cleanup once all resolved. Session narratives in sprint_log_archive.md (Week 5).

---

<a id="LL-035-0"></a>
#### [x] LL-035-0 — Session 0: First flash on real silicon

parent: LL-035 | sprint: 5
added: 2026-04-28 | first_engaged: 2026-04-28 | last_engaged: 2026-04-28 | resolved: 2026-04-28
artifacts: sprint_log.md · commit `50035df`

**Notes:** V1 firmware booted end-to-end on the XIAO ESP32-C3 after two in-session fixes (TASK_CORE 1→0 for the Unicore C3; state_bus led_count default 32→66). Boot log clean, all 4 modules init, button + LED response confirmed. Build 983 KB / 1 MB.

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

**Notes:** 4MB ab_with_factory partition table (nvs 24KB at 0x9000, otadata 8KB, factory + ota_0 + ota_1 at 1.25MB each, 128KB headroom). Build 983 KB / 1.25 MB slot; prior-session NVS survived. Production hardening flags left off per demo-build safety rules. No source touched.

---

<a id="LL-035-1-2"></a>
##### [x] LL-035-1-2 — Session 1: ll_mdns module

parent: LL-035-1 | sprint: 5
added: 2026-04-29 | first_engaged: 2026-04-29 | last_engaged: 2026-04-29 | resolved: 2026-04-29
artifacts: Firmware/v1/core/ll_mdns/ll_mdns.h · Firmware/v1/core/ll_mdns/ll_mdns.c

**Notes:** `core/ll_mdns/` wraps the espressif/mdns component — service `_layeredlogic._tcp.local:80` with `variant`/`version`/`id`/`auth` TXT records, hostname from the WiFi STA MAC suffix. Lesson: folder renamed `core/mdns/`→`core/ll_mdns/` to avoid a build-graph collision with espressif/mdns.

---

<a id="LL-035-1-3"></a>
##### [x] LL-035-1-3 — Session 1 milestone: transport + wscat ping/pong

parent: LL-035-1 | sprint: 5
added: 2026-04-29 | first_engaged: 2026-04-29 | last_engaged: 2026-04-29 | resolved: 2026-04-29
artifacts: Firmware/v1/core/transport/transport.c · Firmware/v1/core/provisioning/provisioning.c · commit `9874b74-dirty`

**Notes:** `core/transport` wraps esp_http_server; the /ws handler parses a JSON envelope via cJSON and dispatches by `op` (Session 1: `ping` only). Dev SoftAP opens `LL-Mirror-XXXXXX` when no creds. End-to-end `wscat` ping/pong verified. Two bugs caught: sdkconfig.defaults needs a clean rebuild to take effect; the AP fast-path raced subscribe order (split into init + kick).

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

**Notes:** `get_state` op. `state_to_json()` serializes 7 fields (`base_color` as `#RRGGBB`). Reads race the writer task by design (eventual consistency).

---

<a id="LL-035-2-2"></a>
##### [x] LL-035-2-2 — Session 2b: set_state op live

parent: LL-035-2 | sprint: 5
added: 2026-04-29 | first_engaged: 2026-04-29 | last_engaged: 2026-04-29 | resolved: 2026-04-29
artifacts: Firmware/v1/core/transport/transport.c

**Notes:** `set_state` op — full WS write surface. `parse_hex_color()` helper; 4 mutable fields post state-bus events; `auth_mode`/`telemetry_enabled`/`led_count` rejected (own ops / read-only). Result is best-effort; broadcasts are authoritative.

---

<a id="LL-035-2-3"></a>
##### [x] LL-035-2-3 — Session 2c: cross-client state-change broadcasts

parent: LL-035-2 | sprint: 5
added: 2026-04-29 | first_engaged: 2026-04-29 | last_engaged: 2026-04-29 | resolved: 2026-04-29
artifacts: Firmware/v1/core/transport/transport.c

**Notes:** `broadcast_state()` builds a `{op:"state",ts,state}` envelope and async-sends to all open WS sockets on `LL_EV_STATE_CHANGED`. ~65ms two-watcher latency. Multi-field set_state sends N broadcasts (coalescing deferred); `ts` is uptime, not Unix time.

---

<a id="LL-035-3"></a>
#### [x] LL-035-3 — Session 3: Webapp scaffold + first click changes color

parent: LL-035 | sprint: 5
added: 2026-04-29 | first_engaged: 2026-04-29 | last_engaged: 2026-04-29 | resolved: 2026-04-29
artifacts: Firmware/v1/webapp/ · commit `37be211`

**Notes:** Webapp scaffold — C→TS pivot, Vite + Preact + TypeScript, standalone npm project. `ws-client.ts` kept Preact-free for the later RN port. Snapshot-on-open; color sends on `onChange`. Bundle 8.6 KB gzipped (11% of the 80 KB budget). First click changes color on hardware.

---

<a id="LL-035-4"></a>
#### [x] LL-035-4 — Session 4: All 5 webapp capabilities + bundle budget gate

parent: LL-035 | sprint: 5
added: 2026-04-29 | first_engaged: 2026-04-29 | last_engaged: 2026-04-29 | resolved: 2026-04-29
artifacts: Firmware/v1/webapp/src/app.tsx · Firmware/v1/webapp/src/ws-client.ts

**Notes:** All 5 webapp capabilities — brightness slider, pattern dropdown, on/off, factory reset (UI mockup; wire op in sub-5-4) — plus WS reconnect-with-backoff. Bundle 9.87 KB gzipped (12% of budget). Parked transient `socket closed` under load as bug #1 (single-task WS fanout backpressure).

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

**Notes:** `core/webapp_assets/` embeds the pre-gzipped webapp bundle and serves `/`, `/app.js`, `/app.css`. Pre-gzip in the npm build keeps the firmware build pure C. transport.c integration is 2 lines + `max_uri_handlers` 4→8.

---

<a id="LL-035-5-2"></a>
##### [x] LL-035-5-2 — Session 5 sub-2: mDNS-over-hotspot Plan A confirmation

parent: LL-035-5 | sprint: 5
added: 2026-04-29 | first_engaged: 2026-04-29 | last_engaged: 2026-04-29 | resolved: 2026-04-29
artifacts: sprint_log.md (no committed code — throwaway spike, reverted)

**Notes:** Tier-1 risk check: does the Pixel 9 hotspot forward mDNS? Yes for PC/browser (Win32 getaddrinfo resolves the mirror hostname); only PowerShell `Invoke-WebRequest` fails (.NET quirk on `.local`). Phone-side mDNS deferred to Sessions 6-8. Throwaway spike, reverted clean.

---

<a id="LL-035-5-3"></a>
##### [x] LL-035-5-3 — Session 5 sub-3: Captive-portal DNS hijack + HTTP 404 redirect

parent: LL-035-5 | sprint: 5
added: 2026-04-29 | first_engaged: 2026-04-29 | last_engaged: 2026-04-29 | resolved: 2026-04-29
artifacts: Firmware/v1/core/captive_dns/ · Firmware/v1/core/webapp_assets/webapp_assets.c

**Notes:** `core/captive_dns/` UDP responder (AP-mode only) answers every A query with the gateway IP; an HTTP 404 handler 302-redirects the OS captive-check URLs to `/`. Captive sheet now pops automatically on join. Bug: a Unicode `→` in a runtime format string silently broke `vsnprintf` — ASCII-only in format strings.

---

<a id="LL-035-5-4"></a>
##### [x] LL-035-5-4 — Session 5 sub-4: Setup screen + set_wifi_creds wire op

parent: LL-035-5 | sprint: 5
added: 2026-04-29 | first_engaged: 2026-04-29 | last_engaged: 2026-04-29 | resolved: 2026-04-29
artifacts: Firmware/v1/core/transport/transport.c · Firmware/v1/core/provisioning/provisioning.c · Firmware/v1/webapp/src/{hooks.ts,components/Header.tsx,pages/{control,settings,setup}.tsx,app.tsx}

**Notes:** Setup screen + `set_wifi_creds` and `factory_reset` wire ops. Architectural call: JSON-WS provisioning (option B) over protocomm-HTTP. State gained `provisioning_active`/`wifi_ssid`; webapp refactored to a 3-page hash router. **15s fallback timer** — a typo'd password falls back to SoftAP instead of stranding the device. Closes Session 5 DoD; closed bug #3, opened #5 and #6.

---

<a id="LL-035-5-5"></a>
##### [x] LL-035-5-5 — Session 5 sub-5: DEV_ prefix cleanup

parent: LL-035-5 | sprint: 5
added: 2026-04-29 | first_engaged: 2026-04-29 | last_engaged: 2026-04-29 | resolved: 2026-04-29
artifacts: Firmware/v1/core/provisioning/provisioning.{c,h} · Firmware/v1/variants/standard/main.c · Firmware/v1/core/transport/transport.h

**Notes:** Pure rename — `LL_DEV_OPEN_SOFTAP`→`LL_SOFTAP_PROVISIONING` and related symbols; dropped the "DEV-ONLY" framing from comments/logs. No functional or wire changes.

---

<a id="LL-035-6"></a>
#### [x] LL-035-6 — Sessions 6-8 collapsed: Bare RN app + 5 controls end-to-end

parent: LL-035 | sprint: 5
added: 2026-04-30 | first_engaged: 2026-04-30 | last_engaged: 2026-04-30 | resolved: 2026-04-30
artifacts: App/v1/ · App/v1/src/{protocol.ts,ws-client.ts} · App/v1/App.tsx

**Notes:** Sessions 6-8 collapsed into one ~2.5h push — bare RN app (Android-only, RN 0.85) with all 5 controls end-to-end against the live mirror. Three Windows build wedges fixed (gradlew.bat path, foojay-resolver 0.5.0→1.0.0, MAX_PATH — dropped two deep-path deps). `protocol.ts` ported verbatim from the webapp.

---

<a id="LL-035-7"></a>
#### [x] LL-035-7 — RN app provisioning + Find Mirror + brand pass + Pixel 9 install

parent: LL-035 | sprint: 5
added: 2026-04-30 | first_engaged: 2026-04-30 | last_engaged: 2026-04-30 | resolved: 2026-04-30
artifacts: App/v1/App.tsx · App/v1/src/find-mirror.ts · App/v1/android/app/src/main/java/com/v1/{WifiInfoModule,WifiInfoPackage}.kt · App/v1/scripts/gen_app_icon.py

**Notes:** RN provisioning end-to-end (SoftAP, no BLE) + Find Mirror + brand pass + first physical Pixel 9 install. Setup screen auto-routes on `provisioning_active`; discovery via a native Kotlin subnet-scan module (mDNS deferred to LL-040 over a Windows MAX_PATH wedge). Three legs verified: control on the existing network, factory-reset→SoftAP round-trip, discovery + control on a new network.

---

<a id="LL-035-8"></a>
#### [x] LL-035-8 — Cross-client sync demo verified

parent: LL-035 | sprint: 5
added: 2026-04-30 | first_engaged: 2026-04-30 | last_engaged: 2026-04-30 | resolved: 2026-04-30
artifacts: sprint_log.md
dependencies: LL-035-2-3, LL-035-7

**Notes:** Final demo-day DoD: webapp (dev PC) + RN app (Pixel 9) on the same mirror, state changes propagate both ways within broadcast latency. Mini-sprint DoD fully green 5 days ahead of the May 5 advisor meeting.

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

**Notes:** Retooled the four n8n flows in `admin_workflow` that consume this repo's tasks.md so the planning + reconciliation pipeline runs on stable LL-NNN IDs instead of slug-fuzzy-matching `sprint_plan.md` through an LLM. Four structural-parse Code nodes shipped (task_collector, accomplishments scraper, reconcile, validation); output conformed to the existing PascalCase collector envelope via a `normalizeTask` shim. Postgres scrubbed of legacy `github:sprint:*` rows; downstream namespace is now `github:task:LL-NNN`, immutable across renames.

---

<a id="LL-042"></a>
### [x] LL-042 — User Repair Guide / Repairability Manual

sprint: 4 | priority: high | deadline: 2026-05-23
added: 2026-04-22 | first_engaged: 2026-04-22 | last_engaged: 2026-05-20 | resolved: 2026-05-20
artifacts: [docs/user-repair-guide.md](docs/user-repair-guide.md)
dependencies: LL-030, LL-031, LL-036, LL-077

**Notes:** 2026-05-20 — Resolved. Customer-facing guide written at [docs/user-repair-guide.md](docs/user-repair-guide.md): repair-first intro, tools/safety, a symptom-finder decision tree, per-component repair procedures covering all 17 failure modes, the recessed-button recovery reference, a parts-sourcing table, and the RMA/contact path — in the first-person LL brand voice. The long-standing assembly-guide blocker was resolved by spinning out [LL-077](#LL-077) (Mirror Assembly & Teardown Guide); §2/§3 of the repair guide build on LL-077 §7's teardown procedure. [repair-index.md](docs/repair-index.md) §6 placeholder replaced with a live link. Repair-time estimates flagged provisional pending the golden-sample build. Surfaced one open item: the front/back panel labels in [acrylic-crack-scratch.md](Failure_Modes/acrylic-crack-scratch.md) read inverted vs. infinity-mirror physics — captured in the guide's Open Items for reconciliation. Active sprint: 8.
2026-04-30 — RtR portfolio docs (LL-036) covered the analytical layer; the user-facing guide itself was still pending, blocked on the assembly guide. Sprint 4 plan called for draft; sprint 8 plan calls for finalization.

---

<a id="LL-043"></a>
### [ ] LL-043 — User interview contact list and outreach

sprint: 4 | priority: medium | deadline: 2026-05-30
added: 2026-04-22 | first_engaged: 2026-04-28 | last_engaged: 2026-05-20
artifacts: [docs/outreach-list.md](docs/outreach-list.md)
dependencies: LL-032

**Notes:** Interview outline drafted via LL-032 (Apr 28). Deadline extended from May 9 to May 30 (Milestone 3) on May 14 to match the actual campaign window. Outreach started cold: 2 YouTubers (Philip DeFranco, Corridor Digital) + 2 Twitch streamers (Aztecross, Jake the Alright) contacted May 14. [docs/outreach-list.md](docs/outreach-list.md) tracks sent + next-round candidates by lane (smart-home, maker, gaming-setup, press, repair voices, cafes, end-buyer recruitment). Content-creator branch spun out as child [LL-043-1](#LL-043-1).

**May 15 update:** Sent table corrected — actual 2026-05-14 send count was 11 (DeFranco, Corridor, Aztecross, Jake, Reed, Robert, Lewis, Matt, Joe, **TechHut**, **Hardware Haven**), not 5 as the sprint-log undercount implied. First reply landed same day from **Colten / Hardware Haven** — swamped through end of month but receptive; followup sent with UW HCDE framing, smart-home-indicator positioning, studio-curation question (transposed from [user-interview-outline §1](docs/user-interview-outline.md)), and AI render of the mirror carrying his channel logo + real-prototype photo as visual hook. Two campaign rules locked: **no X/Twitter/Mastodon** ([feedback_outreach_channels memory](.claude/projects/C--Users-bowhi-Desktop-Independent-Study/memory/feedback_outreach_channels.md)) and **no Fri PM / weekend sends** ([feedback_outreach_timing memory](.claude/projects/C--Users-bowhi-Desktop-Independent-Study/memory/feedback_outreach_timing.md)).

---

<a id="LL-043-1"></a>
#### [ ] LL-043-1 — Content creator + streamer outreach campaign

parent: LL-043 | sprint: 7 | priority: medium | deadline: 2026-05-30
added: 2026-05-14 | first_engaged: 2026-05-14 | last_engaged: 2026-05-20
artifacts: [docs/outreach-list.md](docs/outreach-list.md)
dependencies: LL-032

**Notes:** Per-lane scripted cold outreach (streamer / smart-home / maker / press). Ask = 15-20 min on want/need/cool/problematic re: studio backgrounds and functionality. 14-day no-reply window. Streamer/gaming variant uses nano-banana logo-on-prototype render as visual hook — automatable via n8n; smart-home and press variants are manual. Pipeline outlined in [outreach-list.md § n8n workflow](docs/outreach-list.md#n8n-workflow-outline). Verified contacts captured for Smart Home Solver (Reed), DIY Perks, Hardware Haven, Aztecross (fallback); contact-form-only for Hook Up + Everything Smart Home; remaining names pending verification before send.

**May 15 update:** Hardware Haven (Colten) replied → followup sent same day, validates that the logo-render visual hook works beyond just streamers (Lane B / hardware-reuse creator engaged with it). Monday queue staged with 5 NEW targets, all email-verified via web search: Hackaday (`tips@hackaday.com`, Lane D), iFixit (`press@ifixit.com`, Lane E), Tested (`tips@tested.com`, Lane B/D), Strange Parts / Scotty Allen (`scotty@strangeparts.com`, Lane B indie-founder), Hardware Canucks / Dmitry (`dmitry@hardwarecanucks.com`, Lane C gaming/desk). Mix diversifies from yesterday's Lane-A-heavy batch. **bitluni** dropped to channel-blocked (X / Mastodon / Tindie only). **Drafting calibrations** from the Colten reply added to [feedback_cold_outreach_drafts memory](.claude/projects/C--Users-bowhi-Desktop-Independent-Study/memory/feedback_cold_outreach_drafts.md): positioning > specs (even for gear-heads), specific visual references, don't reflect recipient's stated state back, attach visual hook when the question is visual, "legitimate product" > "actually sellable", self-aware parentheticals are valid peer-tone. Also: no em dashes in outgoing prose ([feedback_no_em_dashes memory](.claude/projects/C--Users-bowhi-Desktop-Independent-Study/memory/feedback_no_em_dashes.md)).

**May 20 update:** All 5 Monday-queue targets drafted and staged in [outreach-list.md § Drafted](docs/outreach-list.md#drafted--ready-to-send-wed-may-20-batch) for Bill to review and send: Hackaday (`tips@hackaday.com`), iFixit (`press@ifixit.com`), Tested (`tips@tested.com`), Strange Parts / Scotty Allen (`scotty@strangeparts.com`), Hardware Canucks / Dmitry (`dmitry@hardwarecanucks.com`). Each draft is a per-recipient subject + body tuned to its lane (press tip-line / repair-editorial / indie-maker-story / indie-founder / gaming-desk). Tone per the outreach memories: one-sentence project description, no em dashes, no LLC signature, subjects under 8 words, hedge openers for the two creator contacts. Public-repo link dropped from the press template since the codebase is closed-binary. Sends pending Bill's review; not yet moved to the Sent table.

---

<a id="LL-044"></a>
### [ ] LL-044 — Packaging QR code

sprint: 4 | priority: low | deadline: —
added: 2026-04-27 | first_engaged: 2026-04-27 | last_engaged: 2026-05-14
artifacts: —
dependencies: LL-051

**Notes:** Laser-etch or perforate into outer cardboard in the same cutting pass; zero extra material, surfaces the repair philosophy at unboxing rather than hiding it on a marketing page. Captured in [docs/right-to-repair-philosophy.md](docs/right-to-repair-philosophy.md). Unblocked once LL-051 resolved (parametric dieline generators in [Assembly_docs/packaging/](Assembly_docs/packaging/) emit insert backer + mailer cuts with raster regions available for QR placement). Ready for QR raster wiring whenever it gets prioritized.

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

**Notes:** Implemented the [LL-041](#LL-041) multi-network spec across six steps, all shipped + OTA-verified on live mirror b2332c (May 7). New `core/ll_wifi/` module (host-tested list ops, per-entry NVS persistence, one-way dependency from provisioning) + a legacy-cred migration shim; provisioning refactored to drive STA from `ll_wifi` with a SCANNING/CONNECTING/ONLINE/BACKOFF state machine (5s/15s/60s backoff); wire ops `list/add/remove/connect_wifi_network`; RN Settings saved-networks UI. `set_wifi_creds` kept as a deprecated shim. Scope grew during the hardware test: a `connect_wifi_network` op was added after Bill hit a dual-network recovery gap, plus an OTA rollback safety net (`CONFIG_BOOTLOADER_APP_ROLLBACK_ENABLE` + `ll_ota_subscribe`) after a stack-overflow brick, plus a status-bar overlap fix. Step-by-step narrative in sprint_log_archive.md (Week 6).

---

<a id="LL-047"></a>
### [x] LL-047 — App design-rationale wireframes (SVG-native, portfolio chapters)

sprint: 5 | priority: medium | deadline: 2026-06-06
added: 2026-04-22 | first_engaged: 2026-05-14 | last_engaged: 2026-05-15 | resolved: 2026-05-15
artifacts: [docs/wireframes/](docs/wireframes/)
dependencies: —

**Notes:** Originally scoped as "Figma wireframes" but pivoted May 14 to SVG-native lo-fi wireframes paired with markdown rationale docs — works as portfolio chapters that index into the existing decision docs (multi-network-design, button-design-rationale, service-blueprint) rather than parallel artifacts in Figma. Per Week 5 plan: "wireframes inform implementation, working builds inform design rationale" — working builds shipped first via [LL-035](#LL-035) mini-sprint, so this is retrospective. Template established with chapter 1 ([settings-wifi](docs/wireframes/settings-wifi.md)). Deadline extended May 9 → Jun 6 to land alongside Week 10 portfolio assembly.

**2026-05-20 — Resolved (tracking catch-up).** All five chapters were authored May 14–15 and merged via PR #19; the registry entry had not been updated. Verified complete: each is an SVG lo-fi wireframe + a paired markdown chapter following the chapter-1 template (TL;DR, journey link, per-callout decisions, alternate states, considered & rejected, research-to-design honesty, implementation gaps, what shipped, references). Chapters: 1 [settings-wifi](docs/wireframes/settings-wifi.md), 2 [home-control](docs/wireframes/home-control.md), 3 [first-boot-setup](docs/wireframes/first-boot-setup.md), 4 [mirror-picker](docs/wireframes/mirror-picker.md), 5 [pattern-designer](docs/wireframes/pattern-designer.md). **Chapter 5 is `pattern-designer`, not the originally-planned "Custom design request (Stage 2b)"** — a sound swap: the custom-order consult flow is an email/web/conversation flow with no App/v1 screen to wireframe, whereas the V2 pattern editor is a genuine app surface (and the natural extension flagged by chapter 2's callout ⑤). All chapters carry `status: draft` (ch5 `speculative-v2`) by design — the design rationale stays provisional until [LL-048](#LL-048) end-buyer interviews validate it.

---

<a id="LL-048"></a>
### [ ] LL-048 — Informal end-buyer interviews (3-5 people)

sprint: 4 | priority: high | deadline: 2026-05-30
added: 2026-04-22
artifacts: —
dependencies: LL-032, LL-043

**Notes:** Test wireframe concepts informally; gather data on price brackets, customization appetite, control preferences, values alignment, discovery patterns. Per Week 4 plan + Week 5 continuation. Outline ready (LL-032); LL-043 outreach campaign engaged May 14 with 5/day cadence. Deadline extended from May 9 to May 30 to match the campaign's reply window. 6 testable hypotheses from button-design rationale (LL-033) tied to specific interview questions.

---

<a id="LL-049"></a>
### [ ] LL-049 — Coffee shop / showcase partner interviews

sprint: 4 | priority: medium | deadline: 2026-05-30
added: 2026-04-22 | first_engaged: 2026-05-20 | last_engaged: 2026-05-20
artifacts: [docs/outreach-list.md](docs/outreach-list.md)
dependencies: LL-032, LL-043

**Notes:** Conversations with local coffee shops to understand how they choose artists/makers to showcase. Outline ready (LL-032); contact list pending (LL-043). Distinct audience from end-buyer interviews — different incentives, different decision criteria.

**2026-05-20 — Contact-list scaffold + cafe outreach script built.** Deadline shifted May 16 → May 30 to match the [LL-048](#LL-048) interview window and the campaign's Milestone 3 boundary (the original May 16 date had already passed). [outreach-list.md Lane F](docs/outreach-list.md) reworked from a bullet-list approach into a fillable candidate table (cafe / neighborhood / shows-art-now / familiarity / reach-via / sent / status) with the 5-7-venue method spelled out; named venues are Bill's to fill from a local Maps walk-through — they can't be sourced here without risking a wrong name. New [Variant 4 cafe / showcase-partner script](docs/outreach-list.md#variant-4--cafe--showcase-partner): an in-person spoken opener as the default plus an email / IG-DM fallback, tuned to the owner/manager audience (the ask is "tell me how you decide," not "cover this"). The interviews themselves remain pending — Bill conducts them impromptu against the LL-032 showcase-partner outline once the venue list is filled.

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
### [x] LL-051 — Packaging concept

sprint: 6 | priority: medium | deadline: 2026-05-09
added: 2026-03-30 | first_engaged: 2026-05-12 | last_engaged: 2026-05-12 | resolved: 2026-05-12
artifacts: [docs/packaging-dielines.md](docs/packaging-dielines.md) · [Assembly_docs/packaging/](Assembly_docs/packaging/)
dependencies: —

**Notes:** Packaging concept — parametric dieline generators for the infinity-mirror box. Per Week 6 plan; slipped, started Week 7. Path A chosen: hand-derive parametric flat patterns from 10 CEFbox reference DXFs (in `Assembly_docs/Packaging_Templates/`, gitignored — paid content) rather than scrape. Two production generators shipped with a passing validation suite (10/10 reference DXFs): `mailer_reft.py` (Roll End Front Tuck outer box) and `insert_tray.py` (N-slot backer tray — the backer is the planned QR raster surface for [LL-044](#LL-044)), plus `validation.py` and PNG renderers in [Assembly_docs/packaging/](Assembly_docs/packaging/); parametric math + conventions in [docs/packaging-dielines.md](docs/packaging-dielines.md). Open follow-ons: `nest.py` sheet packing, first physical test cut, and wiring the QR raster region (unblocks [LL-044](#LL-044)). Iteration history in task_log.md.

---

<a id="LL-052"></a>
### [ ] LL-052 — PCB hardware integration and validation

sprint: 6 | priority: medium | deadline: —
added: 2026-03-30
artifacts: —
dependencies: LL-045
block_reason: Hard-blocked on LL-045 (PCB order slipped out-of-quarter Apr 18). Demo build path locked on XIAO ESP32-C3 dev kit per [project_pcb_status memory](.claude/projects/C--Users-bowhi-Desktop-Independent-Study/memory/project_pcb_status.md) — no custom PCB to integrate against this quarter. Deadline cleared; revisit if PCB order moves back into scope.

**Notes:** Per Week 6 plan: "PCBs arrive. Solder and test the new board. Validate that the dev board prototyping translates to the custom PCB." Out-of-quarter unless LL-045 unblocks.

---

<a id="LL-053"></a>
### [x] LL-053 — Service blueprint

sprint: 7 | priority: high | deadline: 2026-05-16
added: 2026-03-30 | first_engaged: 2026-05-13 | last_engaged: 2026-05-13 | resolved: 2026-05-13
artifacts: [docs/service-blueprint.md](docs/service-blueprint.md)
dependencies: LL-004, LL-007

**Notes:** Per Week 7 plan — the complete user journey as an HCDE "system of interactions" artifact. Resolved 2026-05-13: 10-stage blueprint (Discover → End-of-life), each stage covering Customer Journey / Frontstage / Backstage / Support with moments-of-truth and fail-point callouts, linking into the existing artifacts rather than restating them. Includes a dedicated Custom Order Deep Dive ([LL-054](#LL-054)) and a portfolio-skim summary table. Surfaced 4 new triageable open actions (repair-video pipeline, custom-order deposit structure, carrier insurance threshold, gift address mismatch).

---

<a id="LL-054"></a>
### [x] LL-054 — Custom order flow mapping

sprint: 7 | priority: high | deadline: 2026-05-16
added: 2026-03-30 | first_engaged: 2026-05-13 | last_engaged: 2026-05-13 | resolved: 2026-05-13
artifacts: [docs/service-blueprint.md § Custom Order Deep Dive](docs/service-blueprint.md#custom-order-deep-dive)
dependencies: LL-053

**Notes:** Per Week 7 plan — the bespoke design process, the primary value differentiator. Resolved 2026-05-13 as a dedicated section of [LL-053](#LL-053)'s blueprint: 10-step custom-order journey (Inquiry → Concept + price band → Design proof → ≤3 free revisions → Approval + 50% deposit → Manufacture → Pre-ship review → Balance + ship), frontstage/backstage table, 5 fail-point modes, and 5 open research questions threaded into the LL-048 interview agenda. Custom reuses 100% of the standard supply chain and line — only the laser-cut mirror plane differs — framing it as a high-margin on-ramp into the $250+ tiers.

---

<a id="LL-055"></a>
### [x] LL-055 — Firmware Wi-Fi/BLE resilience stress-test

sprint: 7 | priority: medium | deadline: 2026-05-16 (slipped by ~12h — see Notes)
added: 2026-03-30 | first_engaged: 2026-05-14 | last_engaged: 2026-05-15 | resolved: 2026-05-15
artifacts: [Firmware/v1/core/transport/transport.c](Firmware/v1/core/transport/transport.c) · [Firmware/v1/core/provisioning/provisioning.c](Firmware/v1/core/provisioning/provisioning.c) · [Firmware/v1/scripts/](Firmware/v1/scripts/)
dependencies: LL-029, LL-046

**Notes:** Wi-Fi drop handling + network jitter under load + the bug-doc fix cascade from [post-mini-sprint-bugs.md](docs/post-mini-sprint-bugs.md) #1-4. Closed 2026-05-15 after 4 sessions (May 14-15); deadline slipped ~12h, no de-scope. BLE reconnection split off to [LL-055-1](#LL-055-1) (parked on post-V1 BLE provisioning).

**Four sessions, all verified on the live mirror:** A — Wi-Fi power-save disabled (`WIFI_PS_NONE`), first-response latency roughly halved. B — broadcast fanout moved to a dedicated FreeRTOS task + depth-2 queue. C — per-IP rate limit (token bucket 10 msg/s, overrun → close 1008). D — characterization + a 5-script regression harness in [Firmware/v1/scripts/](Firmware/v1/scripts/) (reconnect hammer, latency/loss proxy, jitter, 5-min soak, AP-toggle). All 4 motivating bug-doc entries closed.

---

<a id="LL-055-1"></a>
#### [ ] LL-055-1 — BLE reconnection stress-test

parent: LL-055 | sprint: 7 | priority: low | deadline: —
added: 2026-05-15 | last_engaged: 2026-05-15
artifacts: —
dependencies: BLE provisioning landing in firmware ([post-mini-sprint-bugs.md #6](docs/post-mini-sprint-bugs.md), currently a post-V1 stretch goal)

**Notes:** Spun off from [LL-055](#LL-055) on 2026-05-15. Scope: BLE reconnection behavior after range loss / radio sleep, once `wifi_prov_mgr` BLE pairing is in. Untestable today — V1 firmware ships SoftAP-only provisioning per [post-mini-sprint-bugs.md #6](docs/post-mini-sprint-bugs.md); BLE provisioning is the architecturally-right fix for the cred-error-flow problem but explicitly post-V1. This task is parked-on-dependency, not abandoned: when BLE provisioning lands, reopen and run the same stress-test regime LL-055 is using (drop/restore, jitter under load) against the BLE transport.

---

<a id="LL-056"></a>
### [x] LL-056 — Terms of Service and Warranty policy

sprint: 8 | priority: high | deadline: 2026-05-23
added: 2026-03-30 | first_engaged: 2026-05-20 | last_engaged: 2026-05-20 | resolved: 2026-05-20
artifacts: [docs/terms-of-service-and-warranty.md](docs/terms-of-service-and-warranty.md)
dependencies: LL-001, LL-030

**Notes:** 2026-05-20 — Resolved. 13-section policy drafted at [docs/terms-of-service-and-warranty.md](docs/terms-of-service-and-warranty.md): terms of sale (standard + the custom-order flow with the 50% deposit), repair-first limited warranty + RMA process + 7-year parts commitment, returns/refunds (30-day standard window, custom-order deposit forfeiture + proprietary-design scrap rule), firmware/closed-binary terms, custom-design buyer-IP attestation, limitation of liability, privacy, WA governing law. **Ships as a draft, not legal advice** — carries a banner that it needs review against WA consumer-protection law / Magnuson-Moss / FTC guidance by counsel before going live. Three former [CONFIRM] items confirmed by Bill 2026-05-20: `support@layeredlogic.cc` as the contact email; a 1-year-from-ship warranty, with any statutory consumer-protection period overriding where the law mandates a different one; and 50/50 custom-order payment (50% deposit at approval, 50% balance at shipment). Remaining gate is counsel review before the doc goes live. RtR philosophy (LL-030) shaped the warranty ergonomics; the legal entity (LL-001) is the contracting party.

---

<a id="LL-057"></a>
### [x] LL-057 — App ↔ hardware integration polish

sprint: 8 | priority: medium | deadline: 2026-05-23
added: 2026-03-30 | first_engaged: 2026-05-15 | last_engaged: 2026-05-20 | resolved: 2026-05-20
artifacts: [App/v1/App.tsx](App/v1/App.tsx)
dependencies: LL-035, LL-040

**Notes:** Per Week 8 plan. Most integration was front-loaded into the LL-035 mini-sprint and LL-040 hardening; this is the polish + remaining-features pass. **Scope locked 2026-05-15 as "wide"** (Bill's call) — telemetry and paired-mode auth stay in-scope. Will slip past May 23; slipping preferred over truncating quality. Bug cascade from the original notes: #1 closed via [LL-055](#LL-055), #2 via [LL-075](#LL-075), #6 parked on BLE provisioning ([LL-055-1](#LL-055-1)).

**Four sessions, each its own build → verify cycle + sprint_log entry:** A — stuck-OTA-button fix ([LL-057-A](#LL-057-A), ✅). B — real OTA progress ([LL-057-B](#LL-057-B), 🟡 architecturally blocked, deferred). C — telemetry MVP ([LL-057-C](#LL-057-C), ✅). D — paired-mode auth ([LL-057-D](#LL-057-D), ✅ D1–D4 done).

**2026-05-20 — Resolved.** A, C, D landed; D (paired-mode auth) completed all four stages on May 20, in the Week 8 window — the predicted slip past May 23 didn't materialize. B (real-time OTA progress) is the one deferred piece: it is architecturally blocked behind moving `ll_ota_start` off the httpd task, stays open as [LL-057-B](#LL-057-B) with forward-compat scaffolding already shipped app-side. The polish pass is otherwise complete. [LL-055-1](#LL-055-1) (BLE reconnection) remains parked-on-dependency, unchanged.

---

<a id="LL-057-C"></a>
#### [x] LL-057-C — Session C: telemetry MVP

parent: LL-057 | sprint: 8
added: 2026-05-15 | first_engaged: 2026-05-15 | last_engaged: 2026-05-15 | resolved: 2026-05-15
artifacts: [cloudflare-workers/telemetry/](cloudflare-workers/telemetry/) · [Firmware/v1/core/telemetry/](Firmware/v1/core/telemetry/) · [App/v1/App.tsx](App/v1/App.tsx) · [Firmware/v1/webapp/src/pages/settings.tsx](Firmware/v1/webapp/src/pages/settings.tsx)
dependencies: LL-076 (resolved 2026-05-15)

**Notes:** End-to-end telemetry per [firmware-spec §4.10](docs/firmware-spec.md): mirror → POST → Cloudflare Worker → KV. Three pieces shipped — a Wrangler-deployable TS Worker at `telemetry.layeredlogic.cc/v1/beacon` (versioned schema, 90-day TTL + latest-snapshot pointer, `/healthz` + `/latest`); app + webapp "share diagnostics" toggles on the existing `telemetry_enabled` field; and a `core/telemetry/` firmware module (24h±2h jittered beacon, HTTPS via embedded CA bundle, boot-reason panic flag). **Build cost +74 KB → 1.23/1.25 MB partition (1.8% headroom — tight).** E2E-verified on the live mirror May 15; two bugs fixed during E2E (CF Bot Fight Mode needs an explicit User-Agent; `telemetry_enabled` was silently dropped by `op_set_state`). Deferred: full panic capture (file:line:task), Worker dashboard, panic alerting.

---

<a id="LL-057-B"></a>
#### [ ] LL-057-B — Session B: real OTA progress (partial close — architecturally blocked)

parent: LL-057 | sprint: 8
added: 2026-05-15 | first_engaged: 2026-05-15 | last_engaged: 2026-05-15
artifacts: [App/v1/App.tsx](App/v1/App.tsx) · [App/v1/src/protocol.ts](App/v1/src/protocol.ts) · [Firmware/v1/webapp/src/protocol.ts](Firmware/v1/webapp/src/protocol.ts)
dependencies: LL-076 (device USB recovery before any retry)

**Notes:** Partial close, deferred. Targeted real-time OTA progress via an `ota_progress` wire op. Bisect (May 15) found that any WS broadcast during `esp_https_ota` — or from the httpd task just before it — fails the install (ESP_FAIL or boot-validation rollback); three approaches all blocked on this. Forward-compat scaffolding shipped app-side (`OtaProgressBroadcast`, `onOtaProgress`, `otaProgressLabel`). Retry design: move `ll_ota_start` off the httpd task onto a dedicated OTA worker task (same pattern as LL-055's broadcast-fanout fix). The bisect chain corrupted the mirror's OTA partitions → recovered via [LL-076](#LL-076).

---

<a id="LL-057-A"></a>
#### [x] LL-057-A — Session A: stuck-OTA-button useEffect reset

parent: LL-057 | sprint: 8
added: 2026-05-15 | first_engaged: 2026-05-15 | last_engaged: 2026-05-15 | resolved: 2026-05-15
artifacts: [App/v1/App.tsx](App/v1/App.tsx)
dependencies: —

**Notes:** Closes the LL-040 follow-up note: app OTA button stuck on "Mirror downloading + rebooting…" forever after a successful OTA. Fix in [App.tsx](App/v1/App.tsx): `prevConnRef` tracks the previous `conn` state; a new `useEffect` fires when the transition is "non-open → open" *and* `otaState === 'rebooting'`, resetting to `'idle'`. The transition-not-steady-state distinction matters: when the user taps the button conn is already `'open'`, so a naive `useEffect` watching just `(conn, otaState)` would reset mid-OTA before the mirror reboots. 13 lines added. `tsc --noEmit` clean on App.tsx (4 pre-existing errors in protocol.ts/ws-client.ts about missing DOM globals — unrelated, predate this change). Runtime verification deferred to Bill's next Pixel 9 session — needs an actual OTA cycle through the app UI to confirm the button auto-resets.

---

<a id="LL-057-D"></a>
#### [x] LL-057-D — Session D: paired-mode auth

parent: LL-057 | sprint: 8
added: 2026-05-20 | first_engaged: 2026-05-20 | last_engaged: 2026-05-20 | resolved: 2026-05-20
artifacts: [Firmware/v1/core/auth/](Firmware/v1/core/auth/) · [Firmware/v1/core/transport/transport.c](Firmware/v1/core/transport/transport.c) · [Firmware/v1/scripts/ll_auth_test.py](Firmware/v1/scripts/ll_auth_test.py) · [App/v1/src/hmac.ts](App/v1/src/hmac.ts) · [App/v1/src/ws-client.ts](App/v1/src/ws-client.ts) · [App/v1/App.tsx](App/v1/App.tsx)
dependencies: LL-035, LL-040

**Notes:** HMAC-authenticated paired mode per [firmware-security.md §5](docs/firmware-security.md) + [control-protocol-spec.md](docs/control-protocol-spec.md). Scoping doc skipped — §5 already locks the design (pre-shared user secret, HMAC-SHA256 envelope, `hmac`-last canonicalization, no QR/DH/WPS). Staged D1–D4, each a build/flash/verify cycle.

**D1 done 2026-05-20** — `core/auth/` module created + host tests. New files: `auth_logic.h/.c` (pure-C, host-buildable — SHA-256, HMAC-SHA256, hex, constant-time compare, and the `,"hmac":`-last envelope split), `auth.h/.c` (ESP-IDF — `ll_auth` NVS namespace, RAM secret cache, init/subscribe/is_paired/has_secret/set_secret/clear_secret/verify), `CMakeLists.txt`, and `tests/core/auth/test_auth_logic.c`. Refinement from the approved plan: SHA-256/HMAC are vendored pure-C rather than `mbedtls/md.h`, so the crypto is host-testable and serves as the byte-exact reference for the app's JS HMAC. 21 host tests (SHA-256 + RFC 4231 HMAC known-answer vectors + envelope round-trips) — all pass; clean `-Wall -Wextra -Wpedantic -Werror` build.

**D2 done 2026-05-20** — `core/auth` wired into the standard variant. `ll_auth_init()` + `ll_auth_subscribe()` added to `variants/standard/main.c` (after `ll_nvs_subscribe`, before provisioning); `auth` added to the variant CMakeLists `REQUIRES`, listed last on purpose — listing it earlier shadows `core/nvs/nvs.h` with IDF's `nvs.h` and breaks `ll_nvs_init`. The factory-reset secret wipe (locked-out recovery) was already in `auth.c`'s subscribe handler from D1. The matter variant is left alone — still a bare scaffold; it picks up auth with every other core module at matter bring-up. Built clean for esp32c3 (2% partition headroom) and OTA-flashed to the live mirror: `fw_version` flipped `telemetry-final-1346` → `cec5bd3-dirty`, device booted clean (no rollback), `uptime_s` 188, WS `get_state` green with `auth_mode=open`.

**SNTP dropped 2026-05-20** (Bill's call): the device has no wall clock, but the recency-based replay guard is proportionate for this threat model and the device must work offline regardless — so the spec's absolute ±60s `ts` window is replaced by a monotonic-`ts` + `req_id`-dedup approach.

**D3 done 2026-05-20** — transport auth gate live. [`core/transport/transport.c`](Firmware/v1/core/transport/transport.c): a paired-mode gate in `handle_envelope()` (HMAC verify → `ts`/`req_id` presence → replay checks, all skipped in open mode so open-mode behavior is byte-identical to pre-LL-057-D); the `set_auth_mode` + `rotate_secret` wire ops; replay protection via a per-socket monotonic-`ts` guard (8 slots, reset on each handshake) plus a 32-entry device-wide `req_id` dedup ring. New `ll_auth_secret_matches()` in `core/auth/` backs the `rotate_secret` old-secret check; `auth` added to transport's CMake `REQUIRES`. Built for esp32c3 (**1% partition headroom** — the telemetry CA bundle dominates; flagged for trimming), OTA-flashed `d3auth-1349` to the live mirror, clean boot. Verified via new [`scripts/ll_auth_test.py`](Firmware/v1/scripts/ll_auth_test.py) — **16/16 checks**: open-mode regression, gate (unsigned → `auth_required`, signed OK, tampered → `bad_hmac`), replay (duplicate `req_id` and stale `ts` → `stale_ts`), `rotate_secret` (old secret invalidated, wrong `old_secret` → `bad_payload`), authenticated unpair. [`firmware-security.md`](docs/firmware-security.md) §5.4 and [`control-protocol-spec.md`](docs/control-protocol-spec.md) §3.1/§8 amended to the recency-based replay model. Mirror left in open mode.

**D4 done 2026-05-20** — app-side paired-mode support. New [`App/v1/src/hmac.ts`](App/v1/src/hmac.ts): vendored pure-TS SHA-256 (FIPS 180-4) + HMAC-SHA256 (RFC 2104) — Hermes has no dependable WebCrypto and signing must not pull a native dependency. [`ws-client.ts`](App/v1/src/ws-client.ts) gained `setSecret()`, the `hmac`-last frame signing in a new `frameFor()` (signed region = envelope JSON minus its closing brace, matching `auth_logic.c`), and `setAuthMode` / `rotateSecret` ops. [`App.tsx`](App/v1/App.tsx) gained a Settings → Pairing section (require-a-passphrase / change-passphrase / remove-passphrase forms) and a controls-screen unlock block for a paired mirror the app holds no secret for — a rejected `get_state` flips a `needsSecret` flag. The secret is held in memory only (not persisted — a paired mirror re-prompts after an app restart, same posture as the un-persisted mirror URL; flagged as a V1 limitation). `tsc --noEmit` clean except the 4 pre-existing DOM-lib errors (`crypto` / `MessageEvent`) that predate this work. `hmac.ts` verified byte-exact against Node's `crypto` over 8 cases (SHA-256 KATs, a realistic envelope region, the >64-byte hashed-key path, Unicode) — since the device matched Python's HMAC in D3 (16/16) and Python == Node == FIPS, the app's signatures will verify on the mirror. Runtime UI verification (the pairing screens on a real Pixel 9) deferred to Bill's next session, per the LL-057-A precedent. **LL-057-D complete (D1–D4 all done).**

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

sprint: 1 | priority: medium | deadline: 2026-10-31
added: 2026-03-30
artifacts: —
dependencies: LL-026

**Notes:** Verify whether business address requires a city or county endorsement on top of WA BLS state registration. Selling physical products online may require general business license. Seattle is the address of record, so the specific question is whether a Seattle city business license is required on top of the state BLS. **Batched 2026-08-31:** ~30-minute lookup. Given the Q3 excise deadline on purpose so it surfaces alongside [LL-069-3](#LL-069-3) and gets done in the same My DOR sitting — one batched compliance sitting, not a separate reminder. Source: `Business/layered-logic/strategy/venture-readiness.md` §5.

---

<a id="LL-068"></a>
### [ ] LL-068 — Reseller permit follow-up

sprint: 4 | priority: medium | deadline: 2026-10-31
added: 2026-04-22
artifacts: —
dependencies: LL-026

**Notes:** Separate application from BLS, free, 4-yr validity. Saves sales tax on component purchases from WA suppliers. File once UBI is confirmed (physical license arrives ~10 business days after BLS submission) — UBI 606 138 559 has been confirmed since the May 15 2026 DOR welcome letter, so this is unblocked. **Batched 2026-08-31:** ~30-minute lookup. Given the Q3 excise deadline on purpose so it surfaces alongside [LL-069-3](#LL-069-3) and gets done in the same My DOR sitting — one batched compliance sitting, not a separate reminder. Source: `Business/layered-logic/strategy/venture-readiness.md` §5.

---

<a id="LL-069"></a>
### [x] LL-069 — Tax filing setup (WA DOR + federal 1040-ES)

sprint: 1 | priority: medium | deadline: 2026-06-15
added: 2026-03-30 | first_engaged: 2026-05-14 | last_engaged: 2026-05-18 | resolved: 2026-05-18
artifacts: DOR welcome letter May 15 2026 (Letter ID L0034471313)
dependencies: LL-002, LL-026

**Notes:** Setup complete — WA DOR portal access live, filing frequency confirmed via mailed welcome letter. **WA DOR welcome letter (May 15, 2026):** quarterly filing assigned (NOT annual as initially expected when added to portal May 14). UBI / Account ID 606-138-559, Account Type Excise Tax. Must file every quarter even with zero activity (RCW 82.32 zero-return obligation). Standard due dates: Q1 → Apr 30, Q2 → Jul 31, Q3 → Oct 31, Q4 → Jan 31. **First return:** Q1 2026 due Jun 15, 2026 (one-time extension granted based on business open date). Electronic filing required via My DOR per RCW 82.32.080. Recurring quarterly returns tracked as children LL-069-1 through LL-069-4. **Federal 1040-ES:** not required while expected annual tax <$1K — currently zero income, deferred until revenue lands. Re-evaluate before Jun 15, Sep 15, Jan 15. If revenue projection later pushes above thresholds, both tracks need re-check.

---

<a id="LL-069-1"></a>
#### [x] LL-069-1 — Q1 2026 WA excise return (Jan 1 – Mar 31)

parent: LL-069 | sprint: 8 | priority: high | deadline: 2026-06-15
added: 2026-05-18 | resolved: 2026-07-09
artifacts: —

**Notes:** First return on the WA DOR excise account. One-time extended due date Jun 15, 2026 (vs. the standard Apr 30) per welcome-letter terms. Expected to file as "no business activity" — zero sales, zero gross receipts through Q1 2026. File electronically via My DOR. If any revenue lands before filing (e.g., friends-and-family pre-orders), Retailing (0.471%) + Manufacturing (0.484%) B&O classifications from LL-026 apply, and sales-tax collection rules engage. **FILED — closed 2026-07-09.** Bill had already filed it; the repo simply carried no record, and the venture-readiness audit briefly flagged it as past due before the correction. Registry row `github:task:LL-069-1` was closed `completed` on 2026-07-09; this checkbox is the markdown catching up.

---

<a id="LL-069-2"></a>
#### [x] LL-069-2 — Q2 2026 WA excise return (Apr 1 – Jun 30)

parent: LL-069 | sprint: 8 | priority: medium | deadline: 2026-07-31
added: 2026-05-18 | resolved: 2026-07-09
artifacts: —

**Notes:** Standard quarterly due date Jul 31, 2026. **FILED 2026-07-09 as "no business activity"** — zero sales, no tax due. Filed electronically via My DOR, ahead of the Jul 31 deadline.

---

<a id="LL-069-3"></a>
#### [ ] LL-069-3 — Q3 2026 WA excise return (Jul 1 – Sep 30)

parent: LL-069 | sprint: 8 | priority: medium | deadline: 2026-10-31
added: 2026-05-18
artifacts: —

**Notes:** Standard quarterly due date Oct 31, 2026. Zero-activity filing unless revenue lands first — the venture is paused for burnout recovery, so a "no business activity" return is the near-certain outcome, but RCW 82.32 requires it regardless to keep the LLC in good standing. File electronically via My DOR (UBI / Account ID 606-138-559). **This is the batched compliance sitting.** Two ~30-minute lookups from the same gate ride along and share this deadline so all three close in one visit: [LL-068](#LL-068) reseller permit and [LL-067](#LL-067) Seattle city/county license endorsement. Deliberately one sitting rather than three reminders — business admin is the specific locus of the burnout. Context: `Business/layered-logic/strategy/venture-readiness.md` §5.

---

<a id="LL-069-4"></a>
#### [ ] LL-069-4 — Q4 2026 WA excise return (Oct 1 – Dec 31)

parent: LL-069 | sprint: 8 | priority: medium | deadline: 2027-01-31
added: 2026-05-18
artifacts: —

**Notes:** Standard quarterly due date Jan 31, 2027. Zero-activity filing unless revenue lands first. File electronically via My DOR.

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

<a id="LL-073"></a>
### [x] LL-073 — RN app polish: haptics + color wheel

sprint: 6 | priority: medium | deadline: —
added: 2026-05-07 | first_engaged: 2026-05-07 | last_engaged: 2026-05-07 | resolved: 2026-05-07
artifacts: [App/v1/src/haptic.ts](App/v1/src/haptic.ts) · [App/v1/scripts/gen-color-wheel.py](App/v1/scripts/gen-color-wheel.py) · [App/v1/assets/color-wheel.png](App/v1/assets/color-wheel.png)
dependencies: LL-046

**Notes:** Two-feature RN app polish after LL-046. (1) Haptics via a new `src/haptic.ts` helper — light/medium/heavy tiers plus content-encoded `pattern(id)` and `brightness(level)` signatures (a tapped pattern feels like its LED vibe; brightness duration scales with the level, since Android amplitude isn't bridged from RN core). (2) Replaced the 12 brand-color dots with a 512×512 HSV color-wheel PNG — polar touch-to-`#RRGGBB`, ~10 Hz throttle, top-right preview swatch, touch-follow magnifier bubble, scroll-lock during drag.

---

<a id="LL-074"></a>
### [x] LL-074 — User-flow authoring methodology + Stage 1 (Discover) flow

sprint: 7 | priority: medium | deadline: —
added: 2026-05-13 | first_engaged: 2026-05-13 | last_engaged: 2026-05-13 | resolved: 2026-05-13
artifacts: [docs/user-flow-authoring.md](docs/user-flow-authoring.md) · [docs/service-blueprint-flows.md](docs/service-blueprint-flows.md) · [.preview/stage1.html](.preview/stage1.html)
dependencies: LL-053

**Notes:** Emerged from drilling down on [LL-053](#LL-053) — can service-blueprint stages be drawn as Figma-style user flows? Six iterations on Stage 1 (Discover) produced a locked HCDE methodology + a reusable rendering recipe, both in [docs/user-flow-authoring.md](docs/user-flow-authoring.md) / [docs/service-blueprint-flows.md](docs/service-blueprint-flows.md). Methodology: pain points as user-voice diamonds, two-layer pain→recovery→`Missing:` evaluation, orange=friction / red=exit, natural cognitive order, modality compression. Rendering: Graphviz via `@viz-js/viz` with `splines=ortho` + explicit ports (Mermaid ruled out). Key gotcha: in `rankdir=LR`, `rank=same` means same column, not row. Two memory entries added; Stages 2–10 are follow-on work against the locked grammar.

---

<a id="LL-075"></a>
### [x] LL-075 — On/brightness wire-state coupling

sprint: 7 | priority: medium | deadline: —
added: 2026-05-15 | first_engaged: 2026-05-15 | last_engaged: 2026-05-15 | resolved: 2026-05-15
artifacts: [Firmware/v1/core/state_bus/state_bus.c](Firmware/v1/core/state_bus/state_bus.c) · [Firmware/v1/scripts/ll_on_brightness_invariant_test.py](Firmware/v1/scripts/ll_on_brightness_invariant_test.py)
dependencies: —

**Notes:** Closes [post-mini-sprint-bugs.md #2](docs/post-mini-sprint-bugs.md) (state.on and state.brightness can disagree). Two coupled invariants enforced in `apply_event`: `LL_EV_BRIGHTNESS` now also sets `on = (value > 0)`; `LL_EV_POWER_TOGGLE{on:true}` auto-restores brightness to 75 if currently 0. Power-off preserves brightness (no UX regression on button-hold-then-single-press). Boot path also self-heals legacy NVS blobs that carry the contradictory (on=true, brightness=0). New host-runnable verification script tests all 6 cases (single-field brightness 0/non-zero, single-field on true/false, coupled webapp envelopes both ways) — all pass on the live mirror. Bug-doc #2 moved to Closed.

---

<a id="LL-076"></a>
### [x] LL-076 — Mirror OTA partition recovery (USB erase-flash)

sprint: 7 | priority: low | deadline: —
added: 2026-05-15 | first_engaged: 2026-05-15 | last_engaged: 2026-05-15 | resolved: 2026-05-15
artifacts: —
dependencies: —

**Notes:** Resolved 2026-05-15 ~12:40. The May 15 [LL-057-B](#LL-057-B) bisect chain triggered ~5 consecutive OTA install failures on the live mirror at 192.168.5.229. After the last failure the running firmware (`ota-diag-priv-req-1153`) refused all subsequent OTA installs with ESP_FAIL — even installing a fresh build with literally zero behavioral changes from a previously-working baseline (`ota-rollback-1214`). Most likely cause: otadata partition slot bookkeeping corrupted across consecutive partial-write failures. **Recovery executed:** Bill plugged in USB; `idf.py -p COM3 erase-flash` (14.4s chip erase) followed by `idf.py -p COM3 flash` brought the device back. Bill re-provisioned via the SoftAP captive flow, the mirror rejoined his Wi-Fi at the same DHCP lease (192.168.5.229). OTA-flashing not re-tested over the air (no need — USB recovery alone reset the partition state) but a full sequence of subsequent USB reflashes (telemetry-mvp → telemetry-ua → telemetry-fix → telemetry-final) all succeeded cleanly. Unblocks the LL-057-C end-to-end verification path.

---

<a id="LL-077"></a>
### [x] LL-077 — Mirror Assembly & Teardown Guide

sprint: 8 | priority: high | deadline: 2026-05-23
added: 2026-05-20 | first_engaged: 2026-05-20 | last_engaged: 2026-05-20 | resolved: 2026-05-20
artifacts: [docs/assembly-guide.md](docs/assembly-guide.md)
dependencies: —

**Notes:** Created 2026-05-20 as the operations-track prerequisite [LL-042](#LL-042) (User Repair Guide) had been blocked on since Week 4 — the repair guide needs a teardown procedure, and no procedural build/teardown document existed (only a station-details CSV, a build-cost spreadsheet, and housing STL/Python files). Written at [docs/assembly-guide.md](docs/assembly-guide.md): tools/workspace, per-unit parts inventory, the three sub-assemblies (frame / LED / controller), the 20-station manufacturing build sequence (S01–S20), final assembly, and §7 — the teardown/access procedure LL-042 consumes. Authored from `Assembly_docs/LED_Mirror_Station_Details.csv`, the `Assembly_docs/basic_housing/` scripts, [repair-design-decisions.md](docs/repair-design-decisions.md), and the [BOM](docs/bom-breakdown-basic-6x6.md). Open items flagged in the doc: assembly times pending the golden-sample build; the `basic_housing/` model is still dimensioned for the old STM8 test board and needs re-confirming against the ESP32-C3 controller PCB.

---

<a id="LL-078"></a>
### [x] LL-078 — Webapp paired-mode support

sprint: 8 | priority: medium | deadline: 2026-05-23
added: 2026-05-20 | first_engaged: 2026-05-20 | last_engaged: 2026-05-20 | resolved: 2026-05-20
artifacts: [Firmware/v1/webapp/src/hmac.ts](Firmware/v1/webapp/src/hmac.ts) · [Firmware/v1/webapp/src/ws-client.ts](Firmware/v1/webapp/src/ws-client.ts) · [Firmware/v1/webapp/src/pages/settings.tsx](Firmware/v1/webapp/src/pages/settings.tsx) · [Firmware/v1/webapp/src/app.tsx](Firmware/v1/webapp/src/app.tsx)
dependencies: LL-057-D

**Notes:** The webapp counterpart to [LL-057-D](#LL-057-D) D4. After D4 the mobile app could pair a mirror, but the device-hosted webapp had no pairing UI and couldn't sign frames — so enabling paired mode would lock the webapp out entirely. Bill flagged this 2026-05-20 (he could see `auth_mode: open` in the webapp's diagnostics but had no control to change it). Closes the webapp limitation [firmware-security.md §5.4](docs/firmware-security.md) had documented.

Mirrors the D4 work into [Firmware/v1/webapp/](Firmware/v1/webapp/): new `hmac.ts` (vendored pure-TS SHA-256 + HMAC-SHA256 — the webapp is served over plain HTTP, not a secure context, so `crypto.subtle` is unavailable; byte-identical to App/v1's copy); `ws-client.ts` gained `setSecret()`, `hmac`-last frame signing (`frameFor()`), and the `setAuthMode` / `rotateSecret` ops; `settings.tsx` gained a pairing panel (require / change / remove a passphrase); `app.tsx` gained a needs-secret unlock screen and owns the secret. **The webapp persists the secret in `localStorage`** (works over plain HTTP, unlike the secure-context crypto APIs) so a paired mirror doesn't re-prompt on every page load — a deliberate divergence from the mobile app's in-memory-only secret, each fitting its platform.

Webapp built clean (`tsc -b` + vite; bundle 13.9 KB gzipped), embedded in firmware `webauth-1444`, OTA-flashed to the live mirror. **Verified end-to-end in a browser** against the device-hosted webapp: the pairing panel renders; enabling pairing flips the device to `auth_mode: paired`; a signed `set_state` (color change) is accepted by the paired device's gate; and a full page reload re-authenticates transparently from the persisted secret with no re-prompt. The `remove passphrase` action uses `window.confirm` (same as factory reset) — fine for a real user, though it blocks browser automation, so the unpair leg was confirmed at the protocol level instead. Mirror left in open mode.

---

<a id="LL-079"></a>
### [x] LL-079 — Proto SoftAP fallback when no saved network is reachable

sprint: 8 | priority: high | deadline: 2026-05-23
added: 2026-05-20 | first_engaged: 2026-05-20 | last_engaged: 2026-05-20 | resolved: 2026-05-20
artifacts: [Firmware/v1/core/provisioning/provisioning.c](Firmware/v1/core/provisioning/provisioning.c)
dependencies: LL-046

**Notes:** Fixes a demo-blocker Bill hit in a test session. On an unexpected network where none of the mirror's saved Wi-Fi networks were in range, the device sat in the connection state machine's backoff loop indefinitely — no SoftAP, no reachable surface — and with no recessed button on the C3 proto there was no factory-reset escape hatch either. The app could not find the device and no demoable interaction was possible.

Change in `provisioning.c`: once the STA connection SM has failed to reach any saved network for `LL_WIFI_AP_FALLBACK_BACKOFFS` (2) backoff cycles (~30s), `ensure_ap_fallback_up()` layers the open SoftAP `LL-Mirror-<MAC>` onto the still-running STA (APSTA mode). The device becomes reachable at 192.168.4.1 — drivable directly over the AP or re-provisionable from /setup — while the STA side keeps scanning and reconnects if a saved network reappears. `post_wifi_disconnected()` suppresses the downstream "offline" signal while in APSTA so a STA drop doesn't tear down the AP-side HTTP server. AP-config building factored into `build_ap_config()`; the `AP_START` handler moved up into `ll_provisioning_init()` so the fallback path (reachable from the saved-creds STA boot) has it registered.

This deliberately overrides the locked "RF minimal — no broadcast without an explicit user gesture" stance (multi-network-design §10 Q3) — **authorized by Bill as a proto-only stopgap**; V2 firmware (ships with prod) will gate AP fallback behind a button gesture. Built as `demofix-1714` (clean esp32c3, 1% partition headroom; full host suite 140/140) and flashed to the live mirror. The WebSocket OTA path failed — `ESP_FAIL` inside `esp_https_ota` after a successful download, the same otadata symptom as [LL-076](#LL-076) — so it was recovered via a USB `idf.py flash` to COM3, which also rewrites `ota_data_initial.bin` and clears the otadata state. Clean boot verified: `fw_version: demofix-1714`, `uptime_s` 68, `get_state` green, STA reconnected to "IoT". **AP fallback verified live on hardware (2026-05-20):** with no reachable saved network the device broadcasts its SoftAP as designed.

---

<a id="LL-080"></a>
### [x] LL-080 — Single-button factory-reset combo

sprint: 8 | priority: high | deadline: 2026-05-23
added: 2026-05-20 | first_engaged: 2026-05-20 | last_engaged: 2026-05-20 | resolved: 2026-05-20
artifacts: [Firmware/v1/core/button/button_logic.c](Firmware/v1/core/button/button_logic.c) · [Firmware/v1/core/button/button.c](Firmware/v1/core/button/button.c)
dependencies: —

**Notes:** Adds a factory-reset path that works on the C3 proto, which has no recessed button (`LL_PIN_BUTTON_RESET = -1`) — so the recessed 10s-hold factory reset is physically unreachable, and a device locked out of the network (see [LL-079](#LL-079)) had no recovery gesture at all.

New `factory_combo_step()` recognizer in `button_logic.c` (pure-C, host-buildable): the deliberately unlikely sequence **5s hold → short press → 5s hold**, each step required within a 3s gap of the previous or the sequence resets. `button.c` feeds it the same primary-button edge stream as the existing gesture machine and posts `LL_EV_FACTORY_RESET` on completion — the same event the recessed hold posts, so the existing pattern_interp red/green confirmation cue and the provisioning factory-reset teardown fire unchanged. Normal primary gestures are suppressed once the combo is past its ambiguous first hold, so the middle tap and final hold don't trigger stray power toggles / colour changes. 8 new host tests in `tests/core/button/test_gesture.c`; full suite 140/140 green, clean `-Werror` build.

Open item — no in-progress LED cue: the user holds 5s twice with no visual feedback until the reset confirmation fires (the recessed hold has a blue-blink progress cue via `LL_EV_RECESSED_HOLD_BEGIN/END`). Acceptable for a dev/proto escape hatch; flagged for V2 if the combo survives into a shipping build. Shipped in the same `demofix-1714` USB flash as [LL-079](#LL-079); clean boot verified on silicon. **Combo verified live on hardware (2026-05-20):** the 5s-hold / short-press / 5s-hold sequence triggers the factory reset.

---

<a id="LL-081"></a>
### [x] LL-081 — LED count board-default stamp

sprint: 8 | priority: high | deadline: 2026-05-23
added: 2026-05-20 | first_engaged: 2026-05-20 | last_engaged: 2026-05-20 | resolved: 2026-05-20
artifacts: [Firmware/v1/core/state_bus/state_bus.c](Firmware/v1/core/state_bus/state_bus.c)
dependencies: LL-080

**Notes:** Bug surfaced the moment [LL-080](#LL-080)'s single-button factory reset was first exercised on the dev mirror. The mirror is the 12×12 / 66-LED prototype; after the factory reset it came back reporting `led_count: 32` — the 6×6 production SKU value. Root cause is pre-existing: `state_bus_defaults.c` hardcodes `.led_count = 32` (a pure-C file that can't see `board.h`), and a factory reset resets the live state to those compiled defaults. The C3 proto never had a factory-reset path before LL-080 (no recessed button), so the latent bug had never fired — building the factory-reset feature without checking the post-reset state on this board was the miss.

Fix in `state_bus.c`: a `stamp_board_led_count()` helper sets `led_count` from `LL_LED_COUNT_DEFAULT` (board.h — 66 for `c3_devkit`, 32 for the prod boards) on every path that seeds or resets `g_state` — both `ll_state_bus_init()` and the `LL_EV_FACTORY_RESET` handler. `led_count` is a fixed hardware property (the wire protocol already treats it read-only), so it is now always the board's value regardless of what NVS or the pure-C defaults carry — which also self-corrects a device whose NVS holds a stale count. The `state_bus_defaults.c` placeholder + TODO comment updated; no schema bump, no host-test change (140/140 still green). Shipped as `ledfix-1833`, OTA-flashed to the mirror (the OTA succeeded this time — the otadata reset by the prior USB flash cleared the `ESP_FAIL` condition); **verified on hardware — `get_state` reports `led_count: 66`.**

---

<a id="LL-082"></a>
### [x] LL-082 — IM_SVG_Maker: black-SVG → laser + plug fab files

sprint: 9 | priority: medium | deadline: 2026-05-30
added: 2026-05-26 | first_engaged: 2026-05-26 | last_engaged: 2026-06-04 | resolved: 2026-05-26
artifacts: [Assembly_docs/IM_SVG_Maker/](Assembly_docs/IM_SVG_Maker/), [Assembly_docs/IM_SVG_Maker/PLAN.md](Assembly_docs/IM_SVG_Maker/PLAN.md), [Assembly_docs/IM_SVG_Maker/README.md](Assembly_docs/IM_SVG_Maker/README.md)
dependencies: LL-012, LL-051

**Notes:** New Python CLI under [Assembly_docs/IM_SVG_Maker/](Assembly_docs/IM_SVG_Maker/) that takes a monochrome black SVG (or a raster line-art image via the included potracer preprocessor) and emits the four fabrication files for a Kintsugi-style infinity mirror tile: `laser.svg` + `laser.dxf` (150×150 mm acrylic cut, red-hairline stroke convention) and `plug.3mf` + `plug.stl` (clear-PETG two-layer plug — 3 mm inset that drops flush into the laser cutout + 1 mm base behind the mirror that bridges fragments). Pipeline is `svgelements → shapely → trimesh`; all tunables in [variables.json](Assembly_docs/IM_SVG_Maker/variables.json). Demo run on `demo_W_black.svg` (W extracted from the Washington Huskies halo) and raster run on `fox.jpg` both produce clean output — see [cut_lines_preview.png](Assembly_docs/IM_SVG_Maker/cut_lines_preview.png) and [fox_preview.png](Assembly_docs/IM_SVG_Maker/fox_preview.png). Bill confirmed the fox preview *and* the actual STL look correct; the tool runs without a virtual environment (plain `pip install -e .`).

**Where it slots into the ordering / assembly flow.** This is the geometry-engine half of the custom-manufacturing track that runs parallel to the [LL-051](#LL-051) parametric packaging dielines. The intended user-facing flow: a customer arrives at the **billwhite.me/infinity** site, designs/tests their logo against a live preview, and the site exports the agreed black SVG straight into IM_SVG_Maker. The tool emits the four fab files into `out/<order>/`. Downstream the same package can hand off to (a) the 3D printer queue (3MF straight into the slicer or a Bambu/Prusa farm API), (b) the laser controller (DXF or SVG into LightBurn / job queue), and eventually (c) the LL-051 packaging dieline generator parameterised on the order dimensions — so a single customer SVG fans out into laser job + print job + branded mailer dieline with no per-order CAD work. This is the same automation pattern as the packaging side: parametric generators feeding queueable machine jobs from a single canonical input. Service-blueprint Stage 2b (custom consult) and Stage 4 (Manufacture & QC) both land on this pipeline.

**Same-day extensions (also 2026-05-26):**
- **Size-floor thresholds shipped in the raster preprocessor** (not the main pipeline — that's where ratty sub-threshold geometry actually enters). New knobs `raster_min_island_area_mm2` and `raster_min_feature_width_mm` in [variables.json](Assembly_docs/IM_SVG_Maker/variables.json) and matching `--min-island-area-mm2` / `--min-feature-width-mm` CLI flags on [raster_to_black_svg.py](Assembly_docs/IM_SVG_Maker/scripts/raster_to_black_svg.py). Both default to 0 (off). Width-floor is implemented as an erosion-only predicate (`island.buffer(-w/2)` empty → drop the whole island) rather than a morphological opening — the first attempt at "open everything" *fractured* islands at thin necks and inflated the count from 5 → 34 on the fox at width=2.5mm. The drop-or-keep semantics is the right user mental model and gets verified end-to-end on `fox.jpg`: high-width threshold drops all islands, area threshold of 20mm² drops 2 of 5, the kept SVG re-parses cleanly through the main pipeline. PLAN.md Alternatives + Learnings updated.
- **Color-SVG → black-SVG converter shipped** as new [color_select.py](Assembly_docs/IM_SVG_Maker/im_svg_maker/color_select.py) + CLI [color_svg_to_black.py](Assembly_docs/IM_SVG_Maker/scripts/color_svg_to_black.py). svgelements does the CSS/inheritance/transform-resolved fill matching with `reify=True`; the tool has a list-fills mode (no `--color`) and an extract mode (`--color "#XXX"`, repeatable for union, `--tolerance N` for fuzzy matching, `--invert` to drop a background). Output is a flat black SVG that drops straight into `python -m im_svg_maker`. **Verified end-to-end on Washington_Huskies_logo.svg**: the list mode finds the two known fills (`#E8D3A2` gold halo + `#33006F` purple W) ranked by bbox area; extracting `#E8D3A2` and running the main pipeline produces the same 100×68.88mm 1-island geometry as the hand-extracted `demo_W_black.svg`, with clean `out/huskies_gold/laser.svg` + `laser.dxf` + `plug.3mf` + `plug.stl`. Multi-color union (both fills together) also produces a valid plug. README + PLAN.md updated; pyproject.toml gained the previously-missing `potracer` + `Pillow` deps surfaced during verification.
- **Still parked for a later iteration:** the printer/laser job-queue handoff (waits on queue tooling existing). The on-site design-preview piece moved out of "parked" and into [LL-083](#LL-083) — actively porting the preprocessing into the existing infinity_mirror_visualizer repo.

The directory itself is currently untracked in git ([Assembly_docs/IM_SVG_Maker/](Assembly_docs/IM_SVG_Maker/)) — Bill hasn't asked for the commit yet, so leaving it untracked until he says otherwise.

**2026-06-04:** core `laser_export.write_dxf` modified under [LL-086](#LL-086) — DXF cut entities now carry solid red ACI 1 + true-color `0xFF0000` on a `CUT` layer (were colorless / ByLayer white). Batch sheet-nesting + three new input preprocessors also landed there.

---

<a id="LL-083"></a>
### [x] LL-083 — IM_SVG_Maker preprocessing port → infinity_mirror_visualizer

sprint: 9 | priority: medium | deadline: 2026-06-06
added: 2026-05-26 | first_engaged: 2026-05-26 | last_engaged: 2026-05-27 | resolved: 2026-05-26
artifacts: [Assembly_docs/IM_SVG_Maker/INTEGRATION_PLAN.md](Assembly_docs/IM_SVG_Maker/INTEGRATION_PLAN.md), `C:/Users/bowhi/Desktop/infinity_mirror_visualizer/src/preprocess/` (separate repo)
dependencies: LL-082

**Notes:** Port [LL-082](#LL-082)'s JPG / colored-SVG preprocessing into the existing [infinity_mirror_visualizer](https://github.com/bowbikes/infinity_mirror_visualizer) React/Three.js app so users can upload a JPG (the format they actually have) and see a 3D preview that reflects the *manufacturable* art (post-binarize, post-trace, post-size-floor, post-nozzle-round). Python `IM_SVG_Maker` stays canonical for actual fab files at order time; the JS port is the **preview-time** layer.

Full scope + boundary + library choices + parity-test plan in [INTEGRATION_PLAN.md](Assembly_docs/IM_SVG_Maker/INTEGRATION_PLAN.md). Six chunks, each independently shippable:

1. **[done 2026-05-26]** JS module skeleton + parity harness. Vitest set up in the visualizer (new dev dep); stub modules under `src/preprocess/`; golden fixtures (W, fox, Huskies) + their `.canonical.svg` files generated by new [Assembly_docs/IM_SVG_Maker/tools/regenerate_canonicals.py](Assembly_docs/IM_SVG_Maker/tools/regenerate_canonicals.py); sync script in the visualizer at `scripts/sync-fixtures.mjs` (no submodule, plain copy). New visualizer deps queued in package.json: `vitest`, `jsdom`, `clipper-lib`, `potrace`. **Red baseline confirmed:** `npm test` reports 5 failed (each with concrete diff like `island count actual=0 vs canonical=5`, `area drift 100.0%`, `viewBox 100×100 vs 2000×2000`), 7 passed (existence checks + demo_W_black self-passthrough), 6 todo (sym-diff metric per fixture + the 3 future-chunk fixtures: tiny_specks, hairline, nested_holes). Harness is wired and reports cleanly.
2. **[done 2026-05-26]** Port `color_select.py` → `colorSelect.js`. Real implementation in `src/preprocess/colorSelect.js`: DOM walk via `DOMParser` (browser native; jsdom supplies it in tests), manual SVG-spec fill inheritance walk (inline `style="fill:..."` > attribute > parent inheritance > black default — `getComputedStyle` doesn't reliably resolve SVG-specific fill semantics under jsdom, so direct attribute walking is more bulletproof than relying on it), CSS Color Module Level 3 named-color table inlined (~150 entries, ~3KB) so no extra dep. Transform composition: leaf→root walk, reversed + joined as an SVG transform string and fed to `svgpath.transform()` (originally tried `DOMMatrix` but jsdom doesn't ship it — svgpath parses SVG-spec transform strings natively and is the cleaner path anyway). Shape support: `<path>`, `<rect>`, `<circle>`, `<ellipse>`, `<polygon>`, `<polyline>` all convert to d-strings before transform-baking. Output mirrors Python's writer exactly: flat list of `<path d="..." fill="#000000" fill-rule="evenodd"/>` with the original viewBox preserved. New dep: `svgpath`. Also improved the parity test's `approxPathBboxArea` helper from "match every number in the d-string" (broken — relative-command deltas lie about extent) to a proper endpoint-only walk via `SvgPath.abs().iterate()` (deliberately excludes curve control points which would convex-hull-bias the bbox). **Result:** all 4 Huskies tests now green. Test counts: 3 failed / 9 passed / 6 todo (was 5/7/6). The 3 remaining failures are all fox.jpg, all from the still-stubbed `raster.js` — chunk 4 territory.
3. **[done 2026-05-26]** Port `svg_parse.py` → `svgParse.js`. Hybrid: Three.js `SVGLoader` does the curve flattening (arcs, Béziers, all SVG path syntax), then clipper-lib's `pftEvenOdd` poly-fill does the actual XOR — replaces Python's `shapely.symmetric_difference`. Three.js's `path.toShapes(false)` uses winding-direction heuristics rather than pure evenodd; using clipper-lib for the XOR step gives byte-faithful parity with Python on nested holes. Verified on a new `nested_holes.svg` fixture (concentric arcs exercising evenodd): JS port and Python both yield 2 islands (big disc-with-hole + inner solid disc) with theoretical areas (π·80² - π·50² ≈ 12252 and π·20² ≈ 1257) matching within Bézier-sampling tolerance. New `nested_holes.svg` fixture + canonical added to IM_SVG_Maker/tests/fixtures. New `svgParse.test.js` covers: parses W to 1 polygon + 1 hole, parses nested_holes to 2 islands with correct holes/areas, parses Huskies canonical with holes, preserves source viewBox, handles empty SVG. Inverse helper `polygonsToSvg` round-trips polygons → SVG → polygons preserving structure. Test counts: 3 failed / 18 passed / 6 todo. The 3 failures remain fox.jpg-only (chunk 4 territory).
4. **[done 2026-05-26]** Port `raster.py` → `raster.js`. Pipeline: Jimp grayscales the input + Jimp's `threshold({max})` makes pixels ≤T pure black (matches `arr < threshold` in PIL); a hand-rolled min-filter dilation (`dilateForeground`) spreads black pixels by `thicknessMm * pixelsPerMm` to thicken the line work (foreground=0 / background=255 means morphological dilation of foreground = min-filter, mirroring PIL's `MaxFilter` on the inverted binary); the post-dilate bitmap is re-serialized to a PNG buffer and passed to the `potrace` npm package for tracing (passing a Jimp instance directly is not supported — the lib internally calls `Jimp.read()` again and fails on Jimp objects); potrace's single-`<path>`-with-many-subpaths output flows through `svgParse` so clipper-lib's `pftEvenOdd` does the shell/hole assembly the same way Python's `shapely` does; `polygonsToSvg` re-emits one `<path>` per island in the exact format `fox.canonical.svg` has. Switched test input from `new Blob([buffer])` to `new Uint8Array(buffer)` — jsdom v24's Blob is incomplete (no `arrayBuffer()`/`stream()` on the prototype); the production `toBuffer()` path covers Buffer / ArrayBuffer / Uint8Array / Blob / File so browser usage is unaffected. **All 3 fox parity tests now green.** Total test counts: 0 failed / 21 passed / 6 todo. New deps: `jimp` (explicit; was transitive via potrace). Size floors stay parked for chunk 5 — chunk 4 is just trace.
5. [ ] Port size-floor filters + nozzle rounding via `clipper-lib`'s `ClipperOffset` (round joins = `shapely.buffer(d, join_style='round')` equivalent). Adds the tiny_specks / hairline fixtures. Closes the sym-diff `.todo`s.
5. **[done 2026-05-26]** Port size-floor filters + nozzle rounding → `manufacturability.js` via clipper-lib `ClipperOffset` (round joins = `shapely.buffer(d, join_style='round')` equivalent). Order mirrors Python: nozzle round (open-then-close, `cut.buffer(-r).buffer(+r).buffer(+r).buffer(-r)`) → width floor (erosion-as-predicate, drop polygons whose `buffer(-w/2)` is empty — matches the May-26 IM_SVG_Maker decision) → area floor (drop polygons below `polygonArea < threshold`). All thresholds operate in SVG-unit space converted from mm via `longestSide / maxLogoDimMm` (matching `pixels_per_mm` in raster.py). Added `tiny_specks.svg` + `hairline.svg` fixtures; new `manufacturability.test.js` with 10 focused tests (area floor drops the 4 specks but keeps the square, width floor drops the hairline but keeps the square, nozzle rounding adds vertices to the rounded square, threshold composition + warnings + idempotence). Symmetric-difference metric implemented in parity test via clipper-lib `ctXor`; tolerance set to 35% (regression bound, not strict-match — Python `svgelements` applies an implicit viewport-to-viewBox transform on top of the explicit matrix while JS `svgpath` bakes only the explicit; Python `potracer` vs npm `potrace` produce different vector approximations of the same binarized pixels; algorithm correctness is verified by the focused unit tests). The 3 chunk-3/5 fixtures get passthrough parity checks instead of `.todo`s — feature-correctness is covered by their respective unit tests. **All 37 tests pass, zero todos, zero failures.**
6. **[done 2026-05-26]** UI wire-up. New `PreprocessPanel.jsx` is a self-contained section component the visualizer's `App.jsx` injects into `ControlsPanel` via a new `topSection` prop (less invasive than splitting the sidebar into two scrolling panels). State machine has three stops — `idle` (no file), `picking` (multi-color SVG awaiting color selection), `ready` (preprocessing complete, sliders re-run `applyManufacturability` against a cached intermediate SVG without re-tracing). File input accepts `.jpg/.jpeg/.png/.svg`; the dispatch branches: JPG → `preprocessRaster`; multi-fill SVG → `listColors` + color-swatch grid + `selectByColor` on pick; single-fill SVG → passthrough; all paths land in the intermediate. Threshold sliders cover nozzle diameter, min island area, min feature width, and max logo dim; warnings + drop counts render inline when islands get filtered. `npm run build` passes (1003 modules, 5.83s, ~713 KB gzipped). 37/37 unit tests still green after the UI work. **Browser-runtime caveat (open item, separate verification):** vite logs externalization warnings for `util`/`stream`/`fs`/`zlib`/`assert` imported by `potrace`'s transitive deps (`pngjs`, `gifwrap`, `strtok3`). The build doesn't fail but the JPG-trace path may hit a runtime stub in the browser. Needs `npm run dev` smoke test on a JPG to confirm; if it breaks, the fix is either a vite Node-polyfill plugin (e.g. `vite-plugin-node-polyfills`) or swapping `potrace` for the pure-JS `potracer` package.

Rough estimate 3–4 working days end-to-end. Sequential per chunk; each leaves the repo green.

Post-port follow-ups documented in INTEGRATION_PLAN.md (Netlify → Vercel hosting migration; new UX modes — the second is gated on Bill enumerating which modes). Both addressed in [LL-084](#LL-084) (visualizer hosting moved to Vercel; the bulk of UX cleanup landed).

---

<a id="LL-084"></a>
### [x] LL-084 — Visualizer cleanup pass + post-port bug fixes

sprint: 9 | priority: medium | deadline: 2026-05-30
added: 2026-05-26 | first_engaged: 2026-05-26 | last_engaged: 2026-05-27 | resolved: 2026-05-27
artifacts: `infinity_mirror_visualizer` repo (separate, GitHub `bowbikes/infinity_mirror_visualizer`)
dependencies: LL-083

**Notes:** After Bill deployed [LL-083](#LL-083)'s port to Vercel he asked for a broad code-review-driven cleanup of the visualizer. Spawned 19 small tracked work items covering correctness, perf, security honesty, and dead-code purge. Net diff for the cleanup commit alone: +516 / −1854 lines (1338 lines lighter), bundle dropped from 713 KB → 331 KB gzipped on initial load (preprocessing chunk lazy-loads on file upload via dynamic import).

**Headline correctness fixes (each its own small commit):**
- **Indexed-merge bug in SvgIcon** (the one that mattered most): `ShapeGeometry` produces indexed triangle buffers; the merge step in SvgIcon concatenated `.position` arrays without expanding indices, so every multi-island custom SVG (dog, snowflake, anything past 1 path) rendered as a garbage triangle salad. Fix expands each indexed geometry into a flat triangle list before concatenating.
- **`SvgIcon` swapped from Three.js `path.toShapes(false)` to `parseSvgToPolygons`** (clipper-lib `pftEvenOdd` XOR) — winding-direction-independent outer+holes structure, so ring shapes (the dog outline, the Huskies halo) render as actual rings instead of filled silhouettes.
- **`listColors` default fill** changed from `#000000` (SVG spec default) to `null` — paths whose `fill:none` came from a `<style>` CSS class (e.g. snowflake's `.cls-1`) were being misreported as 1-fill SVGs and routed through fill passthrough, where their stroke-only polylines became degenerate triangles. Null-fill SVGs now route through `strokesToBlackSvg` as intended.
- **`vite-plugin-node-polyfills` + `define: { __dirname, __filename }`** to make potrace's transitive deps (pngjs, gifwrap, strtok3) actually run in the browser. Without these the JPG-trace path silently died on "Buffer is not defined" or "__dirname is not defined" at runtime.
- **Removed the client-side "tamper-proof signature" theater** in `exportUtils.js`. The hardcoded `INFINITY_MIRROR_V1` secret was baked into the JS bundle — anyone with the page could re-sign any modified config, so the claim was misleading. Swapped for a SHA-256 integrity checksum and honest copy ("catches corruption, not tampering").
- **Manufacturer endpoint moved to `import.meta.env.VITE_MANUFACTURER_ENDPOINT`** — when unset, `ExportModal` hides the "Send to manufacturer" radio group so prod can't silently POST to the placeholder URL that was previously hardcoded.

**UX cleanup:**
- Single render mode (flat fill matching the laser cut); the stroke/outline branches were stripped from SvgIcon. The unified mode is "what's in the bundle is what the laser will cut," with the preview reflecting that directly.
- **Vector stroke-to-fill** in `svgParse.js::strokesToBlackSvg` — walks each stroked path via Three.js SVGLoader, offsets the polyline by `stroke-width/2` via clipper-lib's `ClipperOffset` with `etOpenRound`. Lossless, preserves the source stroke widths (`stroke-width:1.92px` on the snowflake comes through as a 1.92-unit-wide ribbon). Replaced the rasterize-via-canvas-then-trace fallback for stroke-only SVGs which was bitmap-resolution-bound and lost thin features.
- **Custom Art preprocessing panel gated on preset = "Custom Upload"** so there's never two upload boxes simultaneously. Panel renders directly under the Icon section in the sidebar.
- **File-input label** replaced with a styled `<label>` + hidden input — the native "No file chosen" never appears, the label always shows the actual current filename.
- Renamed `mirrorSpacing` → `frameDepthMm` across App / ControlsPanel / InfinityMirrorScene / InfinityMirrorBox / exportUtils so the state variable matches the user-visible slider label.

**Performance:**
- Module-level SVG parse cache in `SvgIcon` so the N reflection layers each pay one parse cost, not N.
- `THREE.Color` scratch reuse in `ReflectionLayers` (was allocating N colors per render frame).
- `frameBounds` array stabilized via `useMemo` in `InfinityMirrorBox`; per-layer `localFrameBounds` baked into the `ReflectionLayers` memo. SvgIcon wrapped in `React.memo` — now the camera-tick / orbit re-renders don't rebuild geometry.
- Preprocessing chunk lazy-loaded via `await import('../preprocess/index.js')` in PreprocessPanel — initial bundle ~half-sized for visitors who only browse the presets.

**Validation workflow established:** wired up the `mcp__Claude_Preview__*` browser-automation tools against the dev server. Switched from "push and hope" to "spin up the local preview, upload each torture-test SVG, screenshot, only push when renders look right." Caught two of the three rendering bugs above this way before they shipped.

**Validated on the torture-test corpus** (Bill committed these to `torture test/` in the visualizer repo): dog (thin cyan ring outline ✓), snowflake (6-arm radial with arrowhead tips and hollow center ✓), cage (birdcage with bars ✓), space-needle (tower silhouette ✓). `Rainier.svg` produces ~348K triangles from 61 subpaths and renders as dotted artifacts — deferred as a polygon-simplification follow-up (not in this task's scope).

**Hosting + branches:** Bill set up Vercel pointing at `bowbikes/infinity_mirror_visualizer` main. Merged `bug_fixes` → `main` so production deploys carry the cleanup. Tests stayed 37/37 throughout; build always clean.

LL-084 closed; remaining UX iteration on the controls themselves (sidebar information architecture, defaults, affordances) lives in [LL-085](#LL-085).

---

<a id="LL-085"></a>
### [x] LL-085 — Visualizer controls UX pass

sprint: 9 | priority: low | deadline: 2026-06-06
added: 2026-05-27 | first_engaged: 2026-05-27 | last_engaged: 2026-05-27 | resolved: 2026-05-27
artifacts: `infinity_mirror_visualizer/src/components/ControlsPanel.jsx`, `PreprocessPanel.jsx`, `CustomArtModal.jsx`, `PresetsSection.jsx`, `InfoPopover.jsx`, `ControlsLayout.css`
dependencies: LL-084

**Notes:** Started with a survey of the right sidebar and presented Bill a prioritized list of improvement opportunities across information architecture, defaults, affordances, live feedback, hierarchy, Custom Art flow, polish, and mobile. He picked Tier 1 (polish) and Tier 2 (most of it — skipped section reorder); after those shipped he greenlit the Tier 3 big-effort items too. Ended up shipping 16 commits across the visualizer repo in one session.

**What landed (in commit order on `infinity_mirror_visualizer` `main`):**

Manufacturability + Custom Art tuning (Bill's specific asks):
- f71a377 — Nozzle diameter became a discrete picker `[off, 0.25, 0.40, 0.60, 0.80, 1.00]` (no more arbitrary nozzles the printer can't swap to). Min island area default = `ceil(π·(d/2)² × 10) / 10` mm². Min feature width default = nozzle diameter. Both follow the nozzle chip until the user moves them. Yellow "the finished product will not be as fine as the rendered preview" warning under any threshold set to zero. `maxLogoDimMm` slider removed — it didn't affect the visible render, only the threshold unit conversion.
- d5ae16a — **Edge Thickness now actually does something for custom SVGs.** The slider was previously dead for uploaded art; now it dilates the parsed polygons by `edgeThickness / 2` in scene-space via the existing clipper-backed `offsetPolygons` (promoted from internal to a public preprocess export). Thin lines get thicker, solid shapes grow outward. Slider min lowered from 0.05 → 0 so users can render source art untouched. Thumbnail in the Custom Art panel got a Hide/Show toggle.
- d396772 — Edge Thickness step 0.05 → 0.01 (finer dial-in); Edge Thickness × Scale interaction deliberately kept proportional (matches manufacturing reality: scaling up the cut makes the cut lines physically thicker); Frame Depth label respects the unit toggle (`1.18in` when in inches, was always `30mm`); Bloom Effect toggle moved out of the floating canvas overlay into the Frame Controls section next to Auto-orbit.

Tier 1 polish:
- 4cf258d — `Reset all` button (top of panel, snaps every control back to DEFAULTS, preserves custom-art uploads); double-click any slider's label to reset just that slider; numeric input next to every slider (clamps to [min, max] on commit); Units chooser became a two-button `mm | in` toggle instead of a "Use inches" checkbox; hex color text inputs validate on blur with a red border on bad input.

Tier 2 PreprocessPanel:
- 4851556 — Manufacturability tuners (nozzle / min island / min feature) moved behind a default-closed `▸ Advanced (manufacturability)` toggle so the basic flow is upload → done. 150 ms debounce on the manufacturability re-run effect (was running the full Clipper pipeline on every slider tick). Inline thumbnail of the processed black SVG at the top of the ready stage so users see what the printer cuts without having to read the 3D canvas.

URL hash + presets:
- 1907b91 — All config knobs serialize to `window.location.hash` as `#cfg=<base64-json>` via `history.replaceState` (no Back-button pollution). Restored once on mount via the `useState` lazy initializer so the encode-on-change effect can't clobber the inbound hash with default state. `Copy share link` button in the Export section flashes "Link copied!" for 1.5s.
- 635e368 — `PresetsSection` at the top of the panel. Inline naming input → saves the current `SHARED_KEYS` snapshot into `localStorage["imv:presets"]` as `{name, cfg, savedAt}[]`. Saved presets list as name buttons with × delete affordances; click name to apply, × to remove. Same-name save overwrites. Custom-art SVGs are NOT included (too big to fit in a hash or preset, fast to re-upload).

Mobile drawer:
- ce05461 — `ControlsLayout.css` handles the responsive positioning. Above 900px the panel is a normal 320px flex child (existing desktop layout); below 900px it becomes a fixed-position drawer pinned to the right edge, translated offscreen until the user opens it via a hamburger button in the top-right. Dark scrim overlay; tap-to-close.

Tier 4 polish:
- b451db4 — Multi-color picker (toggle swatches with a ✓ marker, then `Apply (N colors)` button) — single-tap-and-go was lossy for art whose cut spans several near-black fills. Auto-orbit checkbox folded into Frame Controls (its own one-checkbox "Camera" section felt orphaned). Export note rewritten to "Bundles the configuration with a SHA-256 integrity hash for the manufacturer to verify against." (the old "tamper protection" copy oversold what the hash does). Edge Thickness moved under a new "Appearance" subsection inside Icon Transform — it's a visual property, not a transform.

Custom Art modal wizard (Tier 3):
- 840a8e0 — Pulled the entire upload + color-pick + manufacturability flow out of the sidebar into a `CustomArtModal` that wraps `PreprocessPanel`. Picking Custom Upload from the preset dropdown auto-opens the modal on first pick. Sidebar gets a compact summary card (64x64 thumbnail + filename + Edit button) when art is uploaded, or a single "Upload custom art…" CTA when not. Modal stays mounted after first open (hidden via `display:none` while closed) so `PreprocessPanel`'s internal state — file, picked colors, slider positions, Advanced collapse — survives close+reopen.

Light + emission perceptual uniformity (Bill's specific ask):
- 8a46e4e — Preload preprocess module on `PreprocessPanel` mount (was lazy on first file upload, ~200 KB after gzip — moving it earlier lets the import happen while the user is in the OS file picker). `Canvas frameloop="never"` while the Custom Art modal is open — Three.js continuous rendering was eating enough main-thread time that the OS file dialog lagged after clicking Choose file. Preview caption text fixed to "black = illuminated segments, white = mirror base."
- a382470 — `Light Intensity` slider (0–3×, default 1.0, step 0.05) threads through Scene → Box → ReflectionLayers → SvgIcon and multiplies the per-hue emissive intensity. Joins the URL hash / presets / Reset all.
- 8a8a08f — **Replaced the six-constant hand-tuned Gaussian curve with a principled inverse-luminance formula** `emission = BASELINE / intrinsic_luminance^POWER` (POWER=1.0 gives mathematically exact perceived uniformity across the entire hue circle; BASELINE=3.4 tuned so default cyan emission matches the previous curve's output). Every hue lands at perceived=3.40. Programmatic 64-hue luminance measurement via WebGL `readPixels` attempted but blocked by the headless preview's rAF being paused (canvas readback returns stale buffers), so derived the formula from first principles instead. Bill should visually scrub with bloom on to confirm.
- b739ebb — Default `lightIntensity` 1.0 → 0.5 (the new uniform formula lands brighter than the old hand-tuned curve under bloom; 0.5 gives the previous overall feel as the baseline).

Brand corner:
- f61a57b — `InfoPopover` pinned top-left. One-paragraph description + four-bullet how-to + "Built by Layered Logic — light that layers." footer linking to layeredlogic.cc. Italic Berkeley Mono per the brand decisions in [project_brand_locked_decisions.md](~/.claude/projects/.../memory/project_brand_locked_decisions.md). Dismissable; closed state collapses to a small `ⓘ` button that reopens; dismissal persists via `localStorage["imv:info-dismissed"]`.
- 9b7ecaa — Swapped the `ⓘ` Unicode glyph for an inline SVG icon (the codepoint was falling through to whatever emoji bitmap font happened to have U+24D8 — looked pixelated at 36 px). Hand-drawn vector now: outer circle + vertical stem + dot. Sharp at any DPR.

Defaults / hash / presets / Reset-all-cascade kept consistent across every state-bearing change. Tests stayed 37/37 throughout. Both `main` and `bug_fixes` kept in sync via `git push home_pc` + ff-only merge per the LL-084 workflow.

**Things Bill flagged for later (not in scope of LL-085):**
- Visually verify the emission uniformity by scrubbing through hues with Bloom on; bump `POWER` above 1.0 if bloom over-amplifies bright hues in practice.
- The inverse-luminance formula and the Light Intensity slider give the right tunables — if needed, expose POWER as a knob too.

Validation: dev-server reload pattern + DOM-level interaction tests via `mcp__Claude_Preview__*` carried over from LL-084. The headless WebGL pixel-readback path turned out to be unreliable (rAF appears paused, so the canvas returns whatever was last drawn manually), so visual confidence came from snapshot text + behavioral asserts (slider clamps, hash round-trips, modal open/close cycles, drawer toggle at 600 px viewport, etc.).

---

<a id="LL-086"></a>
### [x] LL-086 — IM_SVG_Maker: multi-order sheet nesting + stroke/outline/flourish preprocessors

sprint: 10 | priority: medium | deadline: 2026-06-06
added: 2026-06-04 | first_engaged: 2026-06-04 | last_engaged: 2026-06-04 | resolved: 2026-06-04
artifacts: [tile_sheet.py](Assembly_docs/IM_SVG_Maker/scripts/tile_sheet.py), [stroke_svg_to_black.py](Assembly_docs/IM_SVG_Maker/scripts/stroke_svg_to_black.py), [outline_black_svg.py](Assembly_docs/IM_SVG_Maker/scripts/outline_black_svg.py), [ll_flourish.py](Assembly_docs/IM_SVG_Maker/scripts/ll_flourish.py), [preview_black_svg.py](Assembly_docs/IM_SVG_Maker/scripts/preview_black_svg.py), [laser_export.py](Assembly_docs/IM_SVG_Maker/im_svg_maker/laser_export.py)
dependencies: LL-082

**Notes:** Extends the single-tile geometry engine ([LL-082](#LL-082)) into a **batch-order workflow that maximizes each physical cut sheet**. The motivating job: cut four different mirror logos (fox, the Huskies "W", the Layered Logic mark, and Bill's sister's empowder festival logo) from one 300×300 mm acrylic sheet in a single laser run instead of four separate 150×150 setups. Four new `scripts/` tools + a red-color fix in the core exporter.

**The nesting wrapper — `tile_sheet.py`** (the headline deliverable). Reads four IM_SVG_Maker `laser.dxf` tiles (each geometry centered on origin, 150 mm square spanning ±75) and translates them into a 2×2 grid on the stock sheet, each tile on its own DXF layer (`tile_1..4`), writing a combined `sheet.dxf` + `sheet.svg`. No outer sheet boundary cut. `--gap` (inter-tile spacing) and `--sheet` (stock edge, for the SVG frame + overhang check) are parameters. A `--squares-only` mode emits just the four 150 mm frame squares (no logo geometry) for cutting blank tiles — same positioning logic, no tile inputs needed. Key constraint surfaced: 4×150 = 300 exactly fills 300 mm stock, so any gap pushes the footprint past the material (0.5 mm gap → 300.5 mm → 0.25 mm overhang per side); the tool warns when footprint > sheet. Shipped both `sheet_gap0.dxf` (300.0, edge-to-edge, double-traced center seam) and `sheet.dxf` (300.5, clean 0.5 mm gap, needs slightly-oversize stock) and the matching squares-only frames. Filename gotcha logged: `--out foo_gap0.5` truncates to `foo_gap0` because `Path.with_suffix` treats `.5` as an extension — use dot-free stems.

**Three input-format preprocessors** (all narrow, all emit the black-fill SVG the main pipeline wants — same contract discipline as the LL-082 raster/color preprocessors):
- **`stroke_svg_to_black.py`** — stroke-based SVG → filled bands. The Layered Logic mono mark is `fill="none"` strokes (nested L's, width 7/6), which the existing color/raster preprocessors skip; this buffers each stroked segment by `stroke-width/2` with round caps to recover solid ribbons. Handles `<line>`/`<polyline>`/`<path>`.
- **`outline_black_svg.py`** — solid silhouette → constant-width ribbon (`ribbon = island − erode(island, band)`). Used to turn the empowder logo from a full-fill flower+mountain into a 3 mm outline so it reads as line-art and doesn't remove a huge mirror area. Band width given in true mm (input normalized to `max_logo_dim_mm` first). The empowder source was a raster PNG embedded in an SVG via a luminance mask — extracted the grayscale silhouette, inverted to black-on-white, traced through the existing raster preprocessor (2 islands, center hole preserved), then outlined.
- **`ll_flourish.py`** — LL-specific. Freezes the animated P5 mark's mouse-reactive histogram (documented in [logo-code-notes.md](assets/brand/logo/logo-code-notes.md)) into a static cut: Gaussian-enveloped, `barSteps`-quantized ticks growing perpendicular off each vertical arm, same stroke weight as the L. Bill chose symmetric/subtle (max-bar 40) with staggered per-L peaks (outer L upper 0.33, inner L mid 0.5). **Direction fix:** first render had bars facing inward (toward each other); flipped so each L's bars grow off its own outer side. A two-color diagnostic render disambiguated the direction before committing.

**Core exporter change — solid red cut color.** `laser_export.write_dxf` previously wrote geometry with no explicit color (defaulted to ACI 7 / ByLayer white, which laser software won't read as a cut). Now every cut entity is ACI 1 **and** true-color `0xFF0000`, on a red `CUT` layer; `tile_sheet.py` does the same on its `tile_1..4` layers. SVG side was already `#FF0000`. (This touches the LL-082 package file — its `last_engaged` bumped accordingly.)

**`preview_black_svg.py`** — shared raster-preview helper: parses any black-fill SVG exactly as the pipeline reads it (post `max_logo_dim` scaling) and renders islands/holes to PNG. Used throughout this session to eyeball every preprocessor output before committing it to a fab run.

**Open decision (not blocking):** which sheet to actually cut depends on real stock size — `sheet_gap0.dxf` for exactly-300 mm, `sheet.dxf` for slightly-over. **Still untracked in git** per the LL-082 convention; intermediate artifacts (extracted PNGs, variant previews) left in `inputs/Empowder/` + `inputs/LayeredLogic/` pending a cleanup call. Reopened the parked P5 logo work ([reference: logo P5 repo](~/.claude)) only far enough to freeze a cut-specific pose — no brand-side changes (palette, canonical rest pose stay parked).

---

<a id="LL-087"></a>
### [x] LL-087 — Home Assistant custom integration (HACS) for the mirror

sprint: 10 | priority: medium | deadline: —
added: 2026-06-05 | first_engaged: 2026-06-05 | last_engaged: 2026-06-05 | resolved: 2026-06-05
artifacts: https://github.com/bowbikes/layered-logic-mirror-ha (public, HACS custom-repo installable) · local working copy `../layered-logic-mirror-ha`
dependencies: LL-027, LL-035

**Notes:** Re-integrate the mirror into Home Assistant using the **existing V1 firmware unchanged** (Bill's ask). Chose a **HACS-ready custom integration** (pure-Python `custom_components`) over MQTT/ESPHome because those would require firmware changes; the firmware already exposes a complete LAN control surface ([control-protocol-spec](docs/control-protocol-spec.md)) that maps 1:1 onto a HA `light` entity.

**Shape.** One `light` entity per mirror: on/off, brightness (HA 0–255 ↔ device 0–100), RGB (`base_color`), and the 7 built-in patterns as the effect list. `iot_class: local_push` — entity state comes from the device's authoritative WS `state` broadcasts, not polling. `manifest.json` wires `zeroconf: ["_layeredlogic._tcp.local."]` so a same-LAN mirror auto-discovers; config flow also takes a **manual IP** (the primary path since Bill's HA is on a different subnet from the mirror and mDNS doesn't cross subnets). Optional `secret` field = paired-mode HMAC. Zero external Python deps (aiohttp from HA core; stdlib `hmac`/`hashlib`).

**Key port.** `protocol.py` reproduces `App/v1/src/ws-client.ts:frameFor` + `hmac.ts` byte-for-byte: compact `json.dumps(…, ensure_ascii=False)`, drop trailing `}`, HMAC the prefix, append `,"hmac":"…"}`. `coordinator.py` owns one reconnecting WS, correlates req/resp by `req_id`, and pushes broadcasts to entities.

**Verified on silicon (open mode).** `verify_mirror.py` ran ALL-PASS against the dev mirror (192.168.5.229, fw `ledfix-1833`, id `b2332c`): `GET /api/info` identity, `get_state`, and `set_state` for on/brightness/base_color/pattern_id — each confirmed via the authoritative `state` broadcast. Learned the firmware's `set_state` **response** is best-effort and races the writer task (returns stale fields), so the harness (and the integration) assert against broadcasts, never the response — matches the LL-035-2-2 design note. **Paired-mode HMAC** is a direct stdlib port (correct by construction) but not yet exercised on silicon — device is open mode; `verify_mirror.py --secret` will confirm it once Bill enables paired mode. **HA-side install + Assist-MCP cross-check still pending** — needs the component copied into a running HA instance (Bill's HA isn't reachable from this repo).

**Spun into a standalone HACS repo** at `../layered-logic-mirror-ha` (sibling to this study repo, the canonical home — manifest's documentation URL): repo-root `custom_components/layered_logic_mirror/`, `hacs.json`, `info.md`, MIT `LICENSE`, `.gitattributes`, `.github/workflows/validate.yml` (hassfest + HACS CI), and `scripts/verify_mirror.py` (re-ran ALL-PASS from the new layout). **Installed + verified live in Bill's HA (HA OS) on Jun 5.** Manual install (Samba add-on → copy into `config/custom_components/`); config flow added the mirror by IP (open mode). Caught + fixed a **coordinator concurrency bug**: the initial `get_state` was awaited *before* the read loop started, so the response was never drained → "no response" setup loop. Fix runs the snapshot `get_state` as a task concurrent with `_read_loop` (the same pattern `verify_mirror.py` always used). After the fix the entity goes green and controls the mirror. Then **published**: genericized the dev-tool IP examples (no device-specific IP/id in the repo), committed, and pushed public to **https://github.com/bowbikes/layered-logic-mirror-ha** (topics set, MIT). CI is **green 8/8** (hassfest + HACS validation): the `validate.yml` workflow was added via the GitHub web editor (the `gh` token lacks `workflow` scope), and the HACS **brands** check is satisfied by **local brand assets** rendered from the canonical rest-pose mark (`scripts/render_brand.py` → `custom_components/.../brand/{icon,logo}.png`, no logo redesign). The in-study `homeassistant/` copy was removed to keep a single source of truth; LL-087 tracking docs stay here, to be committed later.

**HACS default-store submission done (Jun 5):** created GitHub **release v0.1.0** and opened **[hacs/default#8245](https://github.com/hacs/default/pull/8245)** (adds `bowbikes/layered-logic-mirror-ha`). All 11 PR checks green (Sorted, Releases, HACS action, Hassfest, Owner, …), OPEN + MERGEABLE, awaiting maintainer merge — after which it's searchable in HACS with no URL. The expected `home-assistant/brands` PR turned out **unnecessary**: the local `brand/icon.png` + HA's brands-proxy (since 2026.3) satisfy the brands check. Deferred: Alexa/Google/Apple-via-HA expose guide (#2, held per Bill); paired-mode HMAC on-silicon check.

---

## Related

- [Sprint Plan](sprint_plan.md) — narrative roadmap; tasks here carry inline `[LL-NNN]` links into this registry
- [Sprint Log](sprint_log.md) — daily progress narrative
- [Task Event Log](task_log.md) — append-only event history
- [Task Format Spec](docs/task-format-v2.md) — schema, ID rules, migration plan
