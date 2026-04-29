---
title: Spring 2026 Sprint Log
type: tracking
quarter: Spring 2026
status: active
tags: [sprint, log, progress]
---

# Sprint Log — Spring 2026

Progress tracker for the Infinity Mirror sprint plan. Organized by sprint week.
Each entry is a human-written description with a date. The sprint scheduler LLM
fuzzy-matches these against [sprint_plan.md](sprint_plan.md) to determine remaining work.

---

## Week 1 (Mar 30 – Apr 4): Competitive Landscape & Stakeholders

### Done
- [x] LLC Certificate of Formation filed (Mar 23, pre-quarter)
- [x] EIN received from IRS (Apr 2) — 41-5282028
- [x] Operating agreement — finalized, printed, and signed (Apr 6)
- [x] Pricing calculator MVP built — Streamlit dashboard with dynamic BOM, size scaling, tier/platform selection, and margin analysis (`tools/pricing/`) (Apr 2)
- [x] Stakeholder mapping exercise completed (Apr 2) — 45-min coached sprint with Gemini acting as investor/board member. Produced comprehensive 5-phase stakeholder map covering 24 stakeholders across Creation, Distribution, Acquisition, Lived Experience, and Support/Legacy. Formalized as [docs/stakeholder-map.md](docs/stakeholder-map.md).

- [x] ESP32 module selected — ESP32-C3-MINI-1 (Apr 6). Chosen for smallest footprint, lowest cost (~$2 LCSC), wide availability, and built-in USB Serial/JTAG (no external UART chip needed on PCB). Researched all current Espressif modules; full comparison in [docs/esp32-module-comparison.md](docs/esp32-module-comparison.md), shortlist analysis in [docs/esp32-module-shortlist.md](docs/esp32-module-shortlist.md). Note: ESP32-C6-MINI-1 (adds Zigbee/Thread/Matter) flagged as a potential future SKU variant — decision deferred to customer research (survey early buyers on smart home usage).

### In Progress

### Done (continued)
- [x] Business bank account application submitted (Apr 9) and approved (Apr 14). NAICS 335129 (Other Lighting Equipment Manufacturing), calendar fiscal year (12/31), established 03/2026.

- [x] Market analysis — competitive landscape research completed (Apr 6). Cross-platform analysis of 30+ listings across Etsy, Amazon, Pinterest, and commercial signage vendors. Produced [docs/Market Analysis Report.md](docs/Market%20Analysis%20Report.md). Key findings: market is bifurcated into 4 segments (Bespoke Luxury $800–$5k+, B2B Branding $250–$1.2k, Sensory/Therapy $230–$4.3k, Lifestyle/Gaming $40–$250). The $50–$120 bracket has 1,900+ Etsy listings — saturation risk if industrial design doesn't signal $200+ value. Matter/Thread protocol fragmentation flagged as a product risk for wi-fi-only devices. Repairability legislation (EU + WA/CA) is both a compliance requirement and a differentiator. FCC Part 15 compliance noted for LED driver EMI.

### Blocked

---

## Week 2 (Apr 7 – Apr 11): Pricing & The Sustainability Audit

### Done
- [x] Sustainability audit completed (Apr 9). Full 7-stage cradle-to-grave lifecycle analysis produced as [docs/sustainability-audit.md](docs/sustainability-audit.md). All power data measured via smart plug (6,368 samples): idle 0.1W, rainbow cycling 1.25W avg, full white 2.8W (firmware-capped at ~29% of WS2812B rated max). Annual energy at worst-case usage: 4.91 kWh (typical) – 10.34 kWh (peak). LED lifespan projected at 14+ years. Embodied carbon ~2.5–2.7 kg CO2 cradle-to-door. Key hotspots: coated acrylic panels (non-recyclable at EOL), PSU oversizing (25W brick at 11% load — flagged for v2 right-sizing). Cardboard packaging and PETG separability are sustainability strengths.

- [x] BOM breakdown completed (Apr 9). Full material + labor + fixed cost analysis at 1/50/100/1,000 units for basic 6x6 model. Produced [docs/bom-breakdown-basic-6x6.md](docs/bom-breakdown-basic-6x6.md). Key findings: variable COGS ranges from $81.62 (1 unit) to $49.80 (1,000 units); labor is the dominant cost at all volumes (exceeds materials). $50 retail only reaches $60/hr effective rate at 1,000 units — $65 recommended as base price for early volumes. Fixed capital (laser + printer) totals $7,153.11.

- [x] JLCPCB PCB cost analysis (Apr 9). Analyzed basic STM8 PCB BOM (7 component lines, 9 parts, 40 joints). JLCPCB estimate: $3.61/board at qty 5, $1.04/board at qty 50, $0.90/board at qty 100. Pricing_config.yaml $5.00 controller placeholder is significantly overstated — actual assembled PCB is ~$1/board at production volume. Extended parts: STM8 (C52717) + USB-C connector (C456012) each add $3 setup. TTP223 capacitive touch sensor evaluated as potential add-on: ~$0.10/board all-in, basic part (no extended fee), senses through PETG at 1–2mm. ESP32 PCB BOM pending refresh.

### In Progress

### Blocked

---

## Week 3 (Apr 14 – Apr 18): Operations & Critical Path

### Done
- [x] Factory operations planning completed (Apr 14). Full made-to-order workflow mapped in an external Claude session. Produced three artifacts in `Assembly_docs/`:
  - `LED_Mirror_Project_Brief.md` — full context summary of all decisions made
  - `LED_Mirror_Station_Details.csv` — 20-station production line detail (tools, fixtures, consumables, inputs/outputs, QC criteria per station)
  - `LED_Mirror_Assembly_Calculator.xlsx` — per-unit time calculator with 3 sheets: Time Standards, Unit Calculator, Batch Planner

  Key decisions: 3 parallel production lanes (Frame, Mirrors, Electronics) feeding final assembly; batch-by-operation strategy (not one-unit-start-to-finish); milled mirror channels (not spacer layup); pre-test LED strips at receiving, not mid-build; print a full week of 3D parts in one overnight run; physical traveler card per unit through all 20 stations. Garage workshop layout defined: wood dust zone on one end, electronics bench on the other, pack & ship fully isolated. Phase 1 tooling list finalized (Hakko FX-888D, Dewalt 12" sliding miter saw, 7" wet tile saw, Dewalt 1-3/4hp router, bench PSU, Fluke 115). Solder operations flagged as first bottleneck at scale (two per unit: LED connector and controller PCB — first outsource candidate).

### In Progress

### Blocked

---

## Week 4 (Apr 21 – Apr 25): Brand Identity & Repairability

### Done
- [x] Brand positioning worksheet drafted and substantially locked (Apr 20). Full 9-section worksheet produced as [docs/brand-positioning.md](docs/brand-positioning.md). Decisions locked:
  - **Architecture:** three-tier (Layered Logic parent → product mini-brand → product lines; consulting practice also lives under the Layered Logic umbrella)
  - **Audience priority:** Interior Curator > Vibe-Seeker >> Spec-Head with "tech disappears" founding principle
  - **Five adjectives:** Ambient, Considered, Quiet, Crafted, Living
  - **Voice:** first-person "I" while solo-founder; transition to "we" as team grows
  - **Typography:** Neue Haas Grotesk ExtraLight Italic as *default posture* with upright reserved for emphasis and gear-shift moments. Berkeley Mono as technical accent (ABC Diatype rejected for its "R" tail; JetBrains deprioritized)
  - **Palette:** Hybrid three-tier — structural/editorial layer (near-black `#0B0A0F`, warm cream `#F4EFE6`, mid grey `#8A8A8E`, Indigo Deep `#150A4A`) + brand anchor (Indigo Reference `#4A25FF`, Indigo Signal `#3214FF`, Indigo Stone `#4F3FB0`) + glow spectrum restricted to gradients/motion (Indigo→Cyan, Indigo→Pink). *Originally locked as magenta-violet Apr 20; shifted to indigo Apr 21 after rendered artifacts read too pink.*
  - **Tagline:** *"Light that layers."* for the infinity mirror line; `"[X] that layers."` as parent-brand motif extending to other product lines and the consulting arm (*"data that layers"*)
  - Companion research doc [docs/brand-research-notes.md](docs/brand-research-notes.md) analyzes five moodboard references (Paris Electronic Week, SPRING/LAVA, Intrinsic/Pentagram + Felix Faire, IBM Quantum/Field.io) and distills five signature visual devices
  - Still open: §1 positioning sentence final edit, mini-brand name (Tier 2, deferred)

- [x] Palette swatches produced (Apr 20) as SVGs in [assets/brand/color/](assets/brand/color/): Option B (LED spectrum, reference), Option C (editorial warm, reference), and Hybrid (locked). Hybrid swatch includes in-situ dark/light mode previews with italic tagline and mono callout.

- [x] Repository restructured as an Obsidian vault (Apr 20). YAML frontmatter + `## Related` cross-linking added across README, sprint_plan, sprint_log, all `docs/` content. MOC established at [index.md](index.md) as the home note. Standard markdown links (not wikilinks) chosen for GitHub compatibility.

- [x] Design assets folder scaffolded (Apr 20) at [assets/](assets/) with subfolders for brand (color/logo/type/tokens), moodboards, product-photos, icons, figma, source-files. Conventions documented in [assets/README.md](assets/README.md).

- [x] Layered Logic logo P5.js repo reviewed (Apr 20). Bill is developing the nested-L mark independently at [layered-logic/layered-logic-logo](https://github.com/layered-logic/layered-logic-logo) — mouse-reactive histogram bars on two nested L paths with Gaussian quantization and corner-blending. First-pass review captured in [brand-positioning §7](docs/brand-positioning.md) with 5 parked items (color unification with brand palette, canonical rest pose, favicon variant, horizontal-arm asymmetry, optional indigo bloom layer). Logo work is out of scope this sprint — parked until Bill reopens the track.

- [x] Advisor meeting doc drafted (Apr 21). [docs/Advisor-Meetings/advisor-meeting-2026-04-21.md](docs/Advisor-Meetings/advisor-meeting-2026-04-21.md) — M1 close-out, Week 4 brand/engineering deliverables summary, 5 open questions staged for advisor (brand scope overreach, greenfield firmware scope, right-to-repair liability stress-test, user research rigor, PCB production order timing).

- [x] Logo color decision resolved + full SVG set exported (Apr 21). Parked item #1 from logo-spec closed: pulled the mark into the brand palette. Initial resolution used magenta-violet `#BF00FF`/`#A020D0`, but rendered artifacts read too pink, so the whole palette was shifted to indigo the same afternoon. **Final values:** outer L `#4A25FF` (Indigo Reference), inner L `#3214FF` (Indigo Signal). Three deterministic SVG exports: [logo-primary-dark.svg](assets/brand/logo/logo-primary-dark.svg), [logo-primary-light.svg](assets/brand/logo/logo-primary-light.svg), [logo-primary-mono.svg](assets/brand/logo/logo-primary-mono.svg). P5 source still runs `#420AFF` on the outer — minor sync (`→ #4A25FF`) parked with the logo track; inner already matches.

- [x] Palette shifted magenta-violet → indigo (Apr 21). After the logo color resolution rendered artifacts in `#BF00FF`/`#A020D0`, the magenta cast read too pink. Palette re-anchored on indigo: Reference `#4A25FF`, Signal `#3214FF`, Stone `#4F3FB0`, Deep `#150A4A`. Cascade across the whole vault: [brand-positioning.md](docs/brand-positioning.md) §Color Anchor ladder + Five Signature Visual Devices + Hybrid palette block, [brand-research-notes.md](docs/brand-research-notes.md) §Five Visual Devices + three moodboard "avoid" notes, [palette-hybrid-recommended.svg](assets/brand/color/palette-hybrid-recommended.svg) re-derived and now includes the full four-rung ladder, [palette-option-c-editorial-warm.svg](assets/brand/color/palette-option-c-editorial-warm.svg) recolored (retained as rejected-alternative record), [palette-option-b-led-spectrum.svg] deleted (redundant with the hybrid's glow-spectrum block), logo SVGs re-rendered with the indigo anchors, [firmware-architecture-scoping.md](docs/firmware-architecture-scoping.md) default-color updated and confirmed, [advisor-meeting-2026-04-21.md](docs/Advisor-Meetings/advisor-meeting-2026-04-21.md) brand summary + §5.4 Q updated, memory file updated. P5 source still renders `#420AFF` on the outer — minor sync (`→ #4A25FF`) parked with the logo track.

- [x] External chat archive converted to markdown (Apr 21). Three Gemini JSON exports (market research + repo sharing, bank account recommendation) converted to paired `.md` files with YAML frontmatter, summaries, and tags for Obsidian vault integration. The earlier Apr 2 "Sharing GitHub Repos" export flagged `status: superseded` since it's a partial of the Apr 6 "Market_Research" export of the same thread. [LED_Mirror_Project_Brief.md](external_chats/LED_Mirror_Project_Brief.md) retrofit with frontmatter + summary. [external_chats/README.md](external_chats/README.md) added as folder index. [convert_chats.py](external_chats/convert_chats.py) is the reusable converter — hand-written summaries are configured inside the script (they require reading each thread).

- [x] Palette swatch contrast fixes + advisor doc condensed (Apr 21). Ran WCAG contrast audit on indigo type in both surviving swatches: `#3214FF` on `#0B0A0F` fails AA at ~2.5:1. Fixes applied to [palette-hybrid-recommended.svg](assets/brand/color/palette-hybrid-recommended.svg) and [palette-option-c-editorial-warm.svg](assets/brand/color/palette-option-c-editorial-warm.svg): section headers swapped to cream `#F4EFE6` (~14:1, AAA); mono tick-mark labels on dark ground swapped to lighter indigo tint `#8E7FFF` (~6.2:1, AA large); mono labels on cream ground left as `#3214FF` (passes AAA on cream). Hybrid swatch also: background rect extended to full viewBox (was 60px short); light-mode bloom opacity boosted `0.5 → 0.6` (~20% stronger glow). Advisor meeting doc [advisor-meeting-2026-04-21.md](docs/Advisor-Meetings/advisor-meeting-2026-04-21.md) rewritten for concision — trimmed from ~140 to ~85 lines while preserving all 5 open questions and deliverable summary.

- [x] **Button interface design doc drafted + locked (Apr 22).** Two-button hardware model locked in [docs/button-interface.md](docs/button-interface.md): primary exposed button for daily UI (single = on/advance base color, double = change pattern, triple = advance brightness, hold = off), recessed pinhole button for rare ops (short-hold 3s = BLE pairing, long-hold 10s = factory reset). Inherits V0 prototype's single-button interaction grammar for continuity; adds recessed button to keep destructive/configuration actions behind a "you meant this" physical barrier (Eero convention). Three UX decisions resolved in same session: color cycle = 16 HSV spectrum steps + 1 white (17 total, independent of brand palette), brightness = 4 discrete steps (25/50/75/100%), recessed button = hold-only (no tap gestures). Persistence model: soft-off (hold) resumes prior state, hard-off (power loss) resumes to default color. Doc also locks state-machine, LED feedback cues, and GPIO assignment plan (primary stays on GPIO9 from V0; recessed TBD at PCB layout). One minor refinement parked for user testing: whether self-palette patterns should flash briefly on single-press to acknowledge the base-color change.

- [x] **WA BLS registration filed (Apr 22).** Submitted Business Licensing Service application with WA Dept. of Revenue. Activities registered: Retailing (primary, 0.471% B&O) + Manufacturing (secondary, 0.484% B&O) — MATC covers the overlap. NAICS 335129. $90 business license fee paid. UBI issued at submission; physical license arrives by mail in ~10 business days. Business description: "Design, manufacture, and online retail sales of custom LED infinity mirror art and related lighting products." Follow-ups: (1) reseller permit application (separate, free, 4-yr validity) once UBI is confirmed — saves sales tax on component purchases from WA suppliers; (2) verify no city endorsement gap depending on business address; (3) first B&O return due quarterly or annually based on revenue threshold assigned by DOR.

- [x] **Firmware architecture block locked (Apr 22).** Full Wed block of the sprint plan — all 5 architecture decisions from [firmware-architecture-scoping.md](docs/firmware-architecture-scoping.md) §5 resolved. Scoping doc updated in-place with 🔒 Decision blocks capturing each lock + rationale; original options/tradeoffs preserved for traceability. Decisions:
  - **§5.1 Language/framework:** ESP-IDF with C (not C++). Small C++ islands acceptable where RAII helps (pattern object lifecycle), but C is the default. Picked for paste-and-go from ESP-IDF's ~95%-C example base and clean FFI boundaries.
  - **§5.2 Provisioning:** BLE primary + SoftAP fallback, both via Espressif's `wifi_prov_mgr`. **Radios dark at rest** — no passive advertising. User-initiated pairing only, triggered by 3s hold on recessed button. Consistent with "mirror is a light first" principle — smart-device pairing is explicit opt-in, not a boot-time dance.
  - **§5.3 Smart-home integration:** **Two firmware variants, shared core library.** Pro (no Matter, C3-MINI-1 target, 4MB flash) + Pro+ (uncertified Matter with test VID 0xFFF1, C6 target, 8MB flash). Matter stays off by default on Pro+ (user opts in). Full three-tier product line taxonomy locked: Basic (STM8) / Pro (ESP32 + LL app + webapp + HA integration) / Pro+ (Pro + Matter). Matter architectural rules: On/Off + Level Control + Color Control clusters only; all controllers write to a unified state bus. Demo units ship on ESP32-C6-DevKitC-1 for both variants.
  - **§5.4 OTA:** Self-hosted HTTPS on Cloudflare Worker + R2 at `ota.layeredlogic.cc`. ECDSA P-256 signed binaries (private key offline in password manager, public key in bootloader). A/B partitions with `CONFIG_BOOTLOADER_APP_ROLLBACK_ENABLE` (auto-revert if new build fails health check within 60s) + `CONFIG_BOOTLOADER_APP_ANTI_ROLLBACK` (monotonic version counter in eFuse — refuses downgrades). Server-side staged rollout: 1% → 10% → 50% → 100% via device-ID hash buckets. **Opt-in telemetry** where opt-in cohort gets builds first (de-facto beta); opt-outs only get builds after stability proven. OTA checks on boot + every 24h with device-specific hash-based time offset + ±30min jitter (no fleet-wide sync, no consistent overlap with user network routines).
  - **§5.5 App protocol:** BLE GATT (provisioning + fallback) + WebSocket over Wi-Fi (primary, broadcasts to all clients) + HTTP REST (one-shot ops). JSON envelope with `op`, `req_id`, `ts`, `payload`, `hmac`. mDNS discovery on `_layeredlogic._tcp.local.` with BLE fallback when device is in pairing mode or Wi-Fi down — **no manual IP entry**. **User-selectable auth at provisioning:** Open (default, shared-space friendly) or Paired (HMAC-signed messages). Rationale: in a shared space a roommate should be able to turn off the mirror without credentials; the user knows their situation best. Device hosts the webapp in V1; hosted webapp at `layeredlogic.cc/controlmydevice` deferred (mixed-content HTTPS→WS blocker).

  Downstream spec docs drafted in same block: [firmware-security.md](docs/firmware-security.md) (signing, auth, OTA, telemetry), [control-protocol-spec.md](docs/control-protocol-spec.md) (shared wire protocol), [firmware-spec.md](docs/firmware-spec.md) (ESP32 component spec across both variants), [app-spec.md](docs/app-spec.md) (LL mobile app), [webapp-spec.md](docs/webapp-spec.md) (device-hosted webapp + V2 hosted plan). Scoping doc added new §6 Variant Architecture and §7 Board Parameterization — `firmware/core/` shared library, `firmware/variants/{standard,matter}/` thin layers, `firmware/boards/{c6_devkit,c3_devkit,prod_v1_pro,prod_v1_pro_plus}.h` abstract hardware differences via compile-time `LL_PIN_*` + `LL_HAS_*` flags. Post-sprint parked: fleet telemetry system design (opt-in, minimal, transparent).

- [x] **BOI Report — confirmed exempt (Apr 22).** Checked [fincen.gov/boi](https://fincen.gov/boi); site banner reads: *"All entities created in the United States — including those previously known as 'domestic reporting companies' — and their beneficial owners are now exempt from the requirement to report beneficial ownership information (BOI) to FinCEN."* Layered Logic LLC (WA-formed, Mar 23, 2026) is a US-domestic entity and qualifies. No filing required under the current FinCEN interim final rule. Sprint plan updated. Calendar note: re-check FinCEN guidance before Jun 21 original deadline in case the rule is reversed or narrowed.

- [x] Brand vault interlink pass (Apr 21). Typography rationale pulled out of brand-positioning §5 into its own doc at [docs/typography-decisions.md](docs/typography-decisions.md) — covers italic-default inversion, why NHG over Inter/GT America/Söhne, why Berkeley Mono over JetBrains/Plex/Diatype Mono, licensing. Logo §7 similarly delegated to a dedicated spec in [assets/brand/logo/](assets/brand/logo/) with three artifacts: [logo-spec.md](assets/brand/logo/logo-spec.md) (branding + constraints + 5 parked items), [logo-code-notes.md](assets/brand/logo/logo-code-notes.md) (P5 parameters + architecture + dev recipes), and [logo-primary-dark.svg](assets/brand/logo/logo-primary-dark.svg) (deterministic rest pose derived from P5 params — resolves parked item #2). Moodboards and type folders scaffolded with READMEs ([assets/moodboards/README.md](assets/moodboards/README.md) with 5 reference links + summaries, [assets/brand/type/README.md](assets/brand/type/README.md) with what-belongs-here guidance). Cross-links tightened across all docs.

- [x] **Firmware v1 bring-up — first four core modules + host test harness (Apr 22).** Greenfield ESP-IDF firmware moves from blink-level scaffold to a runnable end-to-end loop. Modules landed under [Firmware/v1/core/](Firmware/v1/core/):
  - **`state_bus/`** — single source of truth for runtime state (on, brightness, base_color_rgb, pattern_id, led_count). Owns a dedicated `esp_event` loop on its own task to enforce single-writer concurrency. `LL_EV_STATE_CHANGED` (0x1000) is the broadcast event; transient events at 0x1001+ pass through without state mutation (covers recessed-hold begin/end and future provisioning/factory cues).
  - **`button/`** — debounced GPIO ISR + FreeRTOS task driving two gesture state machines: primary button (single/double/triple/hold, 600ms hold, 200ms multi-press window, 20ms debounce — values inherited from canonical STM8) and recessed button (3s = provision, 10s = factory reset). Pure logic split into `button_logic.[ch]` for host testing; ISR/task glue stays in `button.c`. Recessed press posts `LL_EV_RECESSED_HOLD_BEGIN/END` for live feedback.
  - **`led_driver/`** — ESP-IDF RMT-backed WS2812B driver. Custom compound encoder composes `bytes_encoder` (T0H=4, T0L=8, T1H=8, T1L=4 ticks @ 10MHz / 100ns per tick) + `copy_encoder` (500-tick reset). Pure C `led_driver_encode_frame()` does GRB reorder + brightness scaling (0–100% clamped, integer math) — fully host-testable, separated from RMT.
  - **`pattern_interp/`** — render task pinned to core 1 at priority +3, 60Hz frame loop. Owns 7 stateless renderers (`solid`, `rainbow`, `scanner`, `spinner`, `random`, `breathing`, `twinkle`) implementing [pattern-dictionary.md](docs/pattern-dictionary.md) §3. All animation derived from `tick_ms - pattern_start_ms` so pattern changes reset cleanly with zero allocation. Cue overlay state machine in `cue_logic.c` handles recessed-hold blue blink (0.5s on/off) and factory-reset red flash → 1s green hold; cues bypass user brightness so feedback is always visible. Subscribes to state_bus and drives led_driver. Standard variant's [main.c](Firmware/v1/variants/standard/main.c) now wires state_bus → pattern_interp → button in order; on first power-on the device responds to all primary gestures and recessed feedback.

  Reference docs locked in same block to make the firmware behavior portable across SKUs:
  - **[docs/pattern-dictionary.md](docs/pattern-dictionary.md)** (new) — cross-firmware visual contract: 7 patterns × 13 colors × tick cadence × white-only dimming rule (217/255 factor on solid white only). Defines the wire-format pattern IDs and the porting source-of-truth (STM8 `stm8_150mm.ino`).
  - **[Firmware/STM8/spec.md](Firmware/STM8/spec.md)** (new, 273 lines) — canonical reference for the shipping STM8 Basic firmware. Documents the gesture timings, state machine, pattern behavior, and color palette as **product contract** for all future Layered Logic firmwares to honor.
  - **[docs/firmware-spec.md](docs/firmware-spec.md), [docs/control-protocol-spec.md](docs/control-protocol-spec.md), [docs/button-interface.md](docs/button-interface.md)** — updated to point at the new dictionary, lock V1 patterns as built-in (V2 = uploaded declarative), and reflect the canonical timings.

  **Host test harness** at [Firmware/v1/tests/](Firmware/v1/tests/) — plain CMake + gcc, no ESP-IDF. Covers `button_logic`, `led_driver_logic`, `patterns`, and `cue_logic` (~50 tests across 8 suites). On-target glue (esp_event handlers, RMT, GPIO ISR, render task) deferred to vendored Unity + on-target test app when coverage gap matters.

  **Dev toolchain installed end-to-end on the dev machine (Apr 22):** ESP-IDF 5.5.4 at `C:\Espressif\frameworks\esp-idf-v5.5.4\` (per-session activation via `export.ps1`/CMD/sh — saved as memory `reference_esp_idf_setup.md`); MSYS2 MinGW64 gcc/cmake at `C:\msys64\mingw64\bin\` on permanent PATH for host tests. Nothing has been compiled or flashed yet — first action next session is verification (`cmake -S tests -B tests/build -G "MinGW Makefiles" && ctest` + `idf.py -DLL_VARIANT=standard -DLL_BOARD=c6_devkit build`). Phase 2 firmware now has a real bring-up surface to iterate on; next modules to add are NVS persistence and transport (WebSocket/HTTP).

- [x] **Right-to-Repair philosophy doc finalized (Apr 27).** Status updated from draft to complete. The doc at [docs/right-to-repair-philosophy.md](docs/right-to-repair-philosophy.md) covers: core principle, three-part rationale (pro-consumer → environmental → legal), concrete repair commitments (8 items), honest limits (acrylic EOL, ESP32 obsolescence, closed firmware, solo support), compliance reinforcement (EU RtR, WA HB-1933, CA SB-244), and buyer ask. Cross-linked to the new failure mode inventory.

- [x] **Failure-mode inventory completed (Apr 27).** 17 individual failure mode files created in [Failure_Modes/](Failure_Modes/), covering all v1.0 components across 8 categories: LED strip (3), PSU (2), controller/PCB (4), acrylic panels (2), wood frame (1), PETG parts (1), wiring/connections (2), buttons (2). Each file documents symptoms, likely causes, probability, step-by-step repair path, tools and parts required, difficulty/time estimates, when to contact support, and design mitigations. Summary: 12 fully user-repairable, 5 partially (require soldering or firmware reflash), 0 non-repairable. Index at [Failure_Modes/README.md](Failure_Modes/README.md). Validates the philosophy doc's claim that every failure mode has a user-serviceable path.

### In Progress
- User Repair Guide — customer-facing
- User interview script + contact list — outline drafted Apr 28 (see Week 5); contact list still pending

### Blocked
- Packaging QR code — laser-etch or perforate into outer cardboard in the same cutting pass; zero extra material, surfaces the repair philosophy at unboxing rather than hiding it on a marketing page. **Blocked Apr 28** — needs final packaging design (cardboard stock, dimensions, print/laser process) decided first. Concept stays parked until packaging is far enough along to know what surface the QR lives on.

### Scheduled — Wednesday Apr 22 focus work day
- ~~**BOI Report** filing with FinCEN~~ — **resolved as N/A / exempt Apr 22.** See Done entry below.
- ~~**Sales tax permit** — register with WA Dept. of Revenue~~ — **filed Apr 22.** See Done entry below. Reseller permit follow-up still pending once UBI confirmed.
- ~~**ESP32 firmware architecture block**~~ — **completed Apr 22.** See the "Firmware architecture block locked" entry below.

### Apr 20 end-of-day
- Firmware architecture scoping doc produced at [docs/firmware-architecture-scoping.md](docs/firmware-architecture-scoping.md). Locks 5 design principles (standalone out-of-box, app is pattern editor, configurable LED count, brand-palette default, closed-binary + self-hosted OTA) and stages 5 open decisions for Wed with options/tradeoffs for each. Also captures: feature-parity checklist extracted from the ESPHome YAML, success criteria for Wed, and explicit out-of-scope items (Matter/Thread SKU decision, app UI design, webapp stack, test harness).
- Clarified product-line scope: 6×6 (32 LEDs) is the shipping product; 12×12 (66 LEDs) is a firmware test prototype only, not a second SKU. Sustainability audit + BOM remain 6×6-only.

---

## Week 5 (Apr 28 – May 5): App UX/UI Design + App Demo Mini-Sprint

### Done
- [x] **Business banking fully operational (Apr 28).** External personal accounts linked to the Layered Logic LLC business account and initial funding deposit cleared. Closes the Week 1 banking thread — account is now active and past the inactivity/fee threshold, no risk of closure. Pairs with the Apr 14 account approval.

- [x] **User interview outlines drafted (Apr 28).** Lightweight outlines for two audiences — end-buyer (~22 min) and showcase partner / coffee shops (~22 min) — at [docs/user-interview-outline.md](docs/user-interview-outline.md). Captures the data concepts to collect rather than a verbatim script (Bill conducts impromptu). End-buyer flow walks space/taste → shopping behavior → product reaction → customization appetite → control preferences → values alignment → discovery. Coffee-shop flow walks current practice → artist-selection criteria → economics → logistics → fit → referrals. Doc closes with a 7-row "data we want from every conversation" table tying answers back to specific open decisions (price brackets, parked native/webapp call, Pro vs Pro+ SKU split, repair-messaging value).

- [x] **Button Design Rationale drafted (Apr 28).** HCDE portfolio companion to [button-interface.md](docs/button-interface.md) at [docs/button-design-rationale.md](docs/button-design-rationale.md). Status: draft, pre-user-research — revises after Week 5 interviews. 10 sections covering: the question (light vs smart device), audience priority (Curator > Vibe-Seeker >> Spec-Head), the core "light first" tension, four rejected alternatives (app-only, single-button-only, capacitive touch, encoder), why two buttons + Eero precedent for the recessed button, why V0's gesture grammar was preserved, trade-offs explicitly accepted (sparse hardware UI, no combos, bounded discoverability, app-survival risk), six testable hypotheses tied to interview questions, and open/parked items. One-sentence summary: *"Two buttons because the mirror is a light first and a smart device second — and the difference between those two roles deserves a physical seam."* Closes the Week 5 sprint plan "design rationale" deliverable.

- [x] **V1 firmware pre-flash review + critical fixes (Apr 28).** Read through the existing 4 V1 modules (`state_bus`, `button`, `led_driver`, `pattern_interp`) plus supporting pieces (`nvs`, `provisioning`) before the Session 0 smoke flash. **One critical bug found** that would have panicked on first boot of the C3 dev board: `button.c::ll_button_init()` was unconditionally calling `gpio_isr_handler_add(LL_PIN_BUTTON_RESET, ...)` and `gpio_get_level(LL_PIN_BUTTON_RESET)`, but `c3_devkit.h` sets `LL_PIN_BUTTON_RESET = -1` because the V0 prototype board has no recessed button (header comment said "button module skips registration" but the code didn't actually skip — host tests didn't catch it because they exercise pure `button_logic.c`, not the IDF glue). Patched: all three `LL_PIN_BUTTON_RESET` operations in `ll_button_init` now guarded by `if (LL_PIN_BUTTON_RESET >= 0)`; init log message handles both cases; comment in code explains the contract. Compiler dead-code-eliminates the guarded blocks on c3_devkit since `(-1) >= 0` constant-folds to false — zero runtime cost. Also corrected `c3_devkit.h::LL_LED_COUNT_DEFAULT` 32 → 66 to match the actual V0 prototype hardware (the 12×12 test rig per `project_quarter_spring2026` memory; production 6×6 / 32-LED ships on `prod_v1_pro.h`, unchanged). Pin assignments verified against the ESPHome V0 YAML: GPIO 8 = LED data, GPIO 9 = primary button (active-low, internal pullup, inverted) — matches `c3_devkit.h`. Host tests don't reference either macro, so changes are isolated. Non-blocking observations also surfaced: no `sdkconfig.defaults` or `partitions.csv` files exist (IDF will auto-generate from chip defaults — fine for smoke flash where OTA is out of scope); two event loops by design (state_bus's custom loop + IDF default for esp_wifi); GPIO 9 is a strap pin (boot-mode select), so holding the button during reset puts the chip in download mode — handy fallback if `idf.py flash` ever fails to auto-reset.

- [x] **Session 0 — V1 firmware first flash on real silicon (Apr 28).** Greenfield V1 firmware booted end-to-end on the XIAO ESP32-C3 prototype for the first time after two more pre-flash-review-missed bugs surfaced during boot and were patched on the spot. **Bug 1 (panic):** `pattern_interp.c::TASK_CORE` was `1` but the C3 is Unicore — `xTaskCreatePinnedToCore` asserted on `xCoreID < 1` mid-init (after `led_driver: init: pin=8 leds=32` and before `button_init`). Fixed by setting `TASK_CORE = 0` with a comment noting C3 + C6 are both single-core from FreeRTOS's perspective (C6's low-power core isn't scheduled). **Bug 2 (data):** `state_bus_defaults.c::ll_state_defaults` had `led_count = 32` hardcoded, which propagated through nvs.c's first-boot path and overrode the board header's `LL_LED_COUNT_DEFAULT=66`. Fixed to `66` with a TODO documenting the proper architecture (nvs.c should pull `LL_LED_COUNT_DEFAULT` from board.h after calling defaults; state_bus_defaults.c shouldn't carry a board-dependent value at all). After patching: incremental rebuild in 8s, reflash via `idf.py flash` (auto-reset over USB Serial/JTAG worked, no manual BOOT button needed). Boot log clean: `Unicore app`, app version `50035df` (matching commit), `nvs/state_bus/led_driver/pattern_interp` all init with `leds=66` end-to-end, `provisioning: no creds — radios dark`, `button: init: primary=GPIO9 (no recessed button on this board)` confirming the earlier `LL_PIN_BUTTON_RESET=-1` guard. Bench test over 30s captured 7 state-bus events: 5 single-press color cycles (#3214FF → #8000FF → #DF00FF → #FF00BF → #FF0060), then 2 double-press pattern advances (solid → rainbow → scanner). Bill confirmed visible LED response on the strip. Session 0 closes green; Session 1 (transport scaffold) is next. Build flagged: app binary is 983KB / 1MB partition (94% used) — adding transport + webapp + mDNS will require a custom `partitions.csv` with the `ab_with_factory` 4MB scheme that `c3_devkit.h` already names.

- [x] **App demo mini-sprint locked (Apr 28).** Apr 22 parked app decisions resolved this session. Path A + B + C (transport + webapp + RN app) committed for an Apr 28 → May 5 push targeting an end-to-end faculty-advisor demo of both apps controlling a real ESP32 mirror. Stack locked: React Native (bare, Android only this sprint — iOS deferred, needs a Mac), Preact + Vite for webapp, `esp_http_server` for transport (built-in, handles both REST and WS from one handler), cJSON for envelopes. **Demo board = ESP32-C3 dev kit** (the existing V0 prototype hardware, already wired) — V0 ESPHome firmware will be overwritten by V1, recoverable via ESPHome device builder if needed. C6 + Matter demo deferred until post-mini-sprint. Demo network = Bill's phone hotspot (avoids university Wi-Fi captive portals + IT security). Auth = open mode only for the demo — HMAC stays specced and the wire envelope's `hmac` field stays optional, so this incurs zero protocol debt. **RN app skips BLE provisioning entirely for V1**: webapp captive portal handles all first-time pairing, RN app discovers an already-paired mirror via mDNS (`react-native-zeroconf`). Pattern editor confirmed OUT (V2). 5 demo capabilities: setup (webapp only), on/off, color, pattern select, factory reset. State sync over WS = server broadcasts `LL_EV_STATE_CHANGED` to all clients + clients re-`state.subscribe` on connect or app-foreground for snapshots. **Build flavor = stock unsigned dev builds** — no secure boot, no flash encryption, no anti-rollback (all eFuse burns, irreversible; production hardening is post-sprint). Hardware safety rules locked: no `espefuse.py burn_*`, no `CONFIG_SECURE_BOOT_V2_ENABLED`, no `CONFIG_FLASH_ENCRYPTION_ENABLED`, no `CONFIG_BOOTLOADER_APP_ANTI_ROLLBACK` for the duration of the sprint. **Session 0 added** ahead of Session 1: smoke flash existing 4 V1 modules (state_bus, button, led_driver, pattern_interp) to the C3 dev board to validate toolchain + build/flash pipeline + button-LED end-to-end on real silicon before piling transport on top. 8-session plan, known risks (greenfield transport, RN setup tax, mDNS over hotspot flagged as primary danger, webapp ≤80KB bundle budget, reflash iteration speed), stretch goals (BLE provisioning in RN, UI polish), and demo-day Definition of Done all tracked in [docs/mini-sprint-app-demo.md](docs/mini-sprint-app-demo.md). RN dev environment confirmed working post-reboot (ANDROID_HOME / JAVA_HOME / PATH all resolve cleanly; AVD `Pixel_9_Pro_-_emulator` created and paired with physical Pixel 9 over wireless ADB). Sprint plan Week 5 updated to extend through May 5 and link the mini-sprint doc.

- [x] **Session 1 foundation: 4MB ab_with_factory partition table verified on silicon (Apr 29).** Prerequisite for the rest of Session 1 — yesterday's Session 0 closed with the V1 binary at 983KB / 1MB (94% used), and Session 1 piles `esp_http_server` + cJSON + mDNS on top, so the default single-app partition would overflow before transport even compiled. Foundation laid before any transport code lands. Two new files at the firmware project root:
  - **[Firmware/v1/partitions.csv](Firmware/v1/partitions.csv)** — `ab_with_factory` scheme on 4MB flash (the layout `c3_devkit.h::LL_PARTITION_SCHEME` already names): `nvs` 24KB at 0x9000 (kept at the IDF default offset on purpose so existing NVS contents survive the partition swap), `otadata` 8KB at 0xf000, `factory` 1.25MB at 0x20000, `ota_0` 1.25MB at 0x160000, `ota_1` 1.25MB at 0x2a0000. Total used 3.875MB; top 128KB intentionally unallocated as room for a future coredump partition without reshuffling app slots.
  - **[Firmware/v1/sdkconfig.defaults](Firmware/v1/sdkconfig.defaults)** — minimal: 4MB flash size, custom partition table pointing at `partitions.csv`. Production-hardening flags (secure boot, flash encryption, anti-rollback) explicitly *commented* with a pointer to the mini-sprint hardware safety rules — they're eFuse burns, forbidden this sprint, and the comment makes that load-bearing.

  Verified end-to-end on the XIAO ESP32-C3 prototype:
  1. **Build green** (`idf.py -DLL_VARIANT=standard -DLL_BOARD=c3_devkit set-target esp32c3 build`) — gen_esp32part.py produced a partition table dump matching the CSV verbatim (5 rows, correct offsets/sizes); link reported `binary size 0xf0010 bytes. Smallest app partition is 0x140000 bytes. 0x4fff0 bytes (25%) free` — i.e. 983KB binary / 1.25MB slot, 320KB headroom for the rest of Session 1.
  2. **Flash green** (`idf.py -p COM3 flash`) — esptool wrote bootloader at 0x0, partition table at 0x8000, otadata at 0xf000, app at 0x20000 (the new factory offset), all four with hash verification.
  3. **Boot green** (12s pyserial capture after RTS-pulse reset) — bootloader read the new partition table from flash and dumped all 5 entries; "Defaulting to factory image" → "Loaded app from partition at offset 0x20000"; app version `37be211` (matches the Session 0 commit, no source change); all four core modules init cleanly in order, no panics. **NVS state from yesterday survived the partition swap** (`nvs: loaded: on=1 pattern=random color=#FF00BF brightness=25 leds=66`) — confirms the 0x9000 nvs offset choice was correct.

  No source code touched in this step — pure flash-layout + build-config foundation. The 25% free in the factory slot is the budget for transport + mDNS + webapp embed in the rest of Session 1; if it goes above ~85% utilization mid-sprint, raise per-slot size by stealing from the unused top region. `Firmware/v1/sdkconfig` is gitignored so each dev's machine regenerates it from `sdkconfig.defaults` on first build; existing Session 0 sdkconfigs need a `Remove-Item Firmware/v1/sdkconfig` followed by a clean build to pick up the partition flags. Worktree-root `*.log` files (build/flash/boot capture) added to `.gitignore` — narrative of each verification run lives here in sprint_log.md, raw logs aren't worth versioning.

- [x] **Session 1 mini-milestone: `core/ll_mdns/` scaffolded + alive on silicon (Apr 29).** First of the two new components for Session 1. Module advertises the device on the local network so app/webapp clients can find it without manual IP entry — implements [control-protocol-spec §2.2](docs/control-protocol-spec.md). Wraps the espressif/mdns managed component (declared as a dependency in [Firmware/v1/core/ll_mdns/idf_component.yml](Firmware/v1/core/ll_mdns/idf_component.yml), fetched by the IDF Component Manager into `managed_components/`).
  - **Public surface** (in [ll_mdns.h](Firmware/v1/core/ll_mdns/ll_mdns.h)): two functions matching the existing init/subscribe pattern other modules use — `ll_mdns_init()` brings up mDNS + sets the hostname; `ll_mdns_subscribe()` wires `LL_EV_WIFI_CONNECTED` → publish, `LL_EV_WIFI_DISCONNECTED` → tear down, `LL_EV_STATE_CHANGED + which==AUTH_MODE` → republish only the auth TXT.
  - **Service published** (when wifi connects): `_layeredlogic._tcp.local` on port 80 with TXT records `variant=<standard|matter>`, `version=<fw>`, `id=<6-hex MAC suffix>`, `auth=<open|paired>`. Three of those four are fixed at boot (cached at init); only `auth` mutates at runtime so we use `mdns_service_txt_item_set` for that one rather than a full re-add.
  - **Hostname**: `layered-logic-mirror-XXXXXX.local` where XXXXXX is the lower 3 bytes of the WiFi STA MAC, lowercase hex. On the V0 prototype that resolves to `layered-logic-mirror-b2332c.local` (b2:33:2c is the actual silicon ID).
  - **Compile-time variant string**: top-level [Firmware/v1/CMakeLists.txt](Firmware/v1/CMakeLists.txt) now also injects `LL_VARIANT_NAME` as a quoted string macro alongside the existing `LL_BOARD_*` flag, so any source file can read the variant at runtime. mdns is the first consumer; future telemetry/OTA paths will share it.
  - **Component name lesson learned**: first build attempt failed with `Target "__idf_mdns" links to itself` because both my local component and `espressif/mdns` registered as `mdns` in the build graph. Fixed by renaming the folder `core/mdns/` → `core/ll_mdns/` (component name is derived from folder name; can't be overridden in `idf_component_register`). Header was already named `ll_mdns.h` for the same reason on the C side, but the folder rename completes the symmetry. Documented in [the CMakeLists comment](Firmware/v1/core/ll_mdns/CMakeLists.txt) so future contributors don't repeat it.
  - **Verified on silicon** (XIAO ESP32-C3, COM3): build green (binary 0xfad30 / 1.25MB slot, 22% free — mdns added ~42KB on top of the foundation 983KB); flash green (4 hash-verified writes); boot green (12s pyserial capture after RTS-pulse reset). Boot log confirmed (a) the espressif/mdns task announcing `mDNS task will be created from internal RAM`, (b) our `mdns: init: hostname=layered-logic-mirror-b2332c.local (service deferred until wifi up)` line, (c) correct boot ordering with mdns slotted between provisioning and led_driver, (d) zero regression in the four existing modules — NVS state from yesterday survived (`on=1 pattern=random color=#FF00BF brightness=25 leds=66`). The "service deferred" branch firing is the right behavior for the wifi-down state — won't advertise a record pointing at an IP we don't have. Service publication itself will get its first end-to-end test once wifi comes up via the captive portal in Session 4.

- [x] **Session 1 milestone HIT: `core/transport/` + dev SoftAP + wscat ping/pong verified end-to-end on silicon (Apr 29).** The Session 1 Definition of Done from the mini-sprint plan was "transport reachable; `wscat` ping/pong works." All green. Three subsystems landed in this milestone:
  - **[core/transport/](Firmware/v1/core/transport/)** — wraps `esp_http_server`. Public surface is the now-familiar `ll_transport_init` / `ll_transport_subscribe` pair. Server lifecycle is wifi-gated: starts httpd on `LL_EV_WIFI_CONNECTED`, stops on disconnect (no point listening on a netif we don't have an IP on). WS handler at `/ws` parses the [control-protocol-spec.md](docs/control-protocol-spec.md) JSON envelope via cJSON, dispatches by `op` field. Session 1 implements only `ping` (returns `{fw_version, uptime_s}`); every other op returns `{ok:false, error:{code:"unknown_op"}}` to keep clients honest about which protocol version they're talking to. Open auth mode only — `hmac` field stays optional in the envelope. WS-related types are gated behind `CONFIG_HTTPD_WS_SUPPORT=y` which `sdkconfig.defaults` now sets (re-added after deferring it from the foundation commit).
  - **Dev SoftAP path in [core/provisioning/](Firmware/v1/core/provisioning/)** — gated by `#define LL_DEV_OPEN_SOFTAP 1` (default ON for Sessions 1-4, override to 0 to test the production "radios dark" boot path). When no Wi-Fi creds are saved, the device opens a plain SoftAP `LL-Mirror-XXXXXX` (open, no password, max 4 clients) at boot. Bypasses the `wifi_prov_mgr` protocomm dance entirely — Sessions 1-4 don't need it because we're testing functionality, not the demo network path. Session 5 captive portal flow replaces this with the real `wifi_prov_mgr` SoftAP scheme. New public function `ll_provisioning_kick_dev_softap()` is a no-op in production builds, safe to call unconditionally from main.c.
  - **Compile-time variant string + `CONFIG_HTTPD_WS_SUPPORT=y`** — top-level [Firmware/v1/CMakeLists.txt](Firmware/v1/CMakeLists.txt) injects `LL_VARIANT_NAME` (added in the previous mdns commit; documented here for completeness). `sdkconfig.defaults` adds the WS support flag — required for `httpd_ws_frame_t`, `HTTPD_WS_TYPE_TEXT`, and `httpd_ws_send_frame` to be defined. Without it, transport.c fails to compile.

  **Two bugs caught and fixed in-session because of the verify-each-change cadence:**

  1. **`sdkconfig` doesn't pick up `sdkconfig.defaults` once it exists.** First transport build failed with `unknown type name 'httpd_ws_frame_t'`. Diagnosis: the foundation build had already generated `sdkconfig` *without* `CONFIG_HTTPD_WS_SUPPORT=y` (because that line wasn't in defaults at foundation time), and IDF doesn't re-merge `sdkconfig.defaults` into an existing `sdkconfig`. Fix: `Remove-Item Firmware/v1/sdkconfig` then rebuild; defaults regenerate it from scratch. This is exactly the workaround we documented in the foundation commit — confirmed it works in practice. Bill's main worktree will need the same one-time delete when he flashes there.

  2. **AP fast path raced subscribe-order.** First `start_dev_softap` impl called `esp_wifi_start` synchronously inside `ll_provisioning_init`. AP comes up in <50ms (vs the real STA path's 5-30s connect), so `WIFI_EVENT_AP_START` fired, the synthesized `LL_EV_WIFI_CONNECTED` posted, and the state-bus loop dispatched it — all *before* mdns and transport had subscribed. Both modules silently missed the event, kept logging "deferred until wifi up" forever even though wifi was up. Fix: split `start_dev_softap` into `init_dev_softap` (config + handler register, no start) and a new public `ll_provisioning_kick_dev_softap` (just `esp_wifi_start`); main.c calls the kick at the end after every `*_subscribe` has run. Real STA path doesn't have this race (natural connect delay) so it stays in `ll_provisioning_init` unchanged. Lesson surfaced: the firmware's init phase uses `init` for setup-only and `subscribe`/`kick` for anything that posts events — synchronizing event-emission with handler-registration is now an explicit pattern, not implicit.

  **Verified on silicon** (XIAO ESP32-C3, COM3, post-fix):
  - **Build green**: binary 0xfe400 / 1.25MB slot, 21% free (264KB headroom). Transport+dev-SoftAP added ~13KB on top of mdns; `esp_http_server` and `cJSON` were already linked transitively, only the WS handler symbols + transport.c are new.
  - **Boot ordering correct**: provisioning configures the AP and logs `DEV SoftAP configured (...) — deferred until subscribers wired`; mdns/transport/led_driver/pattern_interp/button all init in order; THEN at I (604) `wifi:mode : softAP`, `DHCP server started ... 192.168.4.1`, `DEV SoftAP up: ssid="LL-Mirror-B2332D"`. Mdns receives the event and logs `published _layeredlogic._tcp.local:80 (variant=standard version=9874b74-dirty id=b2332c auth=open)` — service is on the air with all 4 TXT records. Transport receives it and logs `listening on :80 (ws /ws)` — esp_http_server up, /ws URI handler registered.
  - **End-to-end Session 1 Definition of Done validated from a real client**: Bill's PC sees `LL-Mirror-B2332D` in its wifi list, connects (open network), gets a 192.168.4.x DHCP lease; `wscat -c ws://192.168.4.1/ws` connects; sending `{"op":"ping","req_id":"t1","ts":...}` returns `{"op":"ping","req_id":"t1","ok":true,"result":{"fw_version":"9874b74-dirty","uptime_s":...},"error":null}`. Unknown ops correctly return `{ok:false, error:{code:"unknown_op"}}`. Transport reachable, app-layer ping/pong round-trips, JSON envelope contract works. Session 1 closed.

### In Progress

### Blocked

---

## Week 6 (May 5 – May 9): Hardware Integration & Supply Chain

### Done

### In Progress

### Blocked

---

## Week 7 (May 12 – May 16): Service Blueprinting & The Custom Experience

### Done

### In Progress

### Blocked

---

## Week 8 (May 19 – May 23): Sustainability Deep Dive & Legal

### Done

### In Progress

### Blocked

---

## Week 9 (May 26 – May 30): Marketing Storytelling & Assets

### Done

### In Progress

### Blocked

---

## Week 10 (Jun 2 – Jun 6): Portfolio Construction

### Done

### In Progress

### Blocked

---

## Week 11 (Jun 9 – Jun 12): Launch Readiness Audit

### Done

### In Progress

### Blocked

---

## Related

- [Index / MOC](index.md)
- [Sprint Plan](sprint_plan.md) — the plan this log tracks against
- [Advisor Meeting — Apr 21](docs/Advisor-Meetings/advisor-meeting-2026-04-21.md) — most recent check-in
