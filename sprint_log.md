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

## Week 5 (Apr 28 – May 2): App UX/UI Design

### Done
- [x] **Business banking fully operational (Apr 28).** External personal accounts linked to the Layered Logic LLC business account and initial funding deposit cleared. Closes the Week 1 banking thread — account is now active and past the inactivity/fee threshold, no risk of closure. Pairs with the Apr 14 account approval.

- [x] **User interview outlines drafted (Apr 28).** Lightweight outlines for two audiences — end-buyer (~22 min) and showcase partner / coffee shops (~22 min) — at [docs/user-interview-outline.md](docs/user-interview-outline.md). Captures the data concepts to collect rather than a verbatim script (Bill conducts impromptu). End-buyer flow walks space/taste → shopping behavior → product reaction → customization appetite → control preferences → values alignment → discovery. Coffee-shop flow walks current practice → artist-selection criteria → economics → logistics → fit → referrals. Doc closes with a 7-row "data we want from every conversation" table tying answers back to specific open decisions (price brackets, parked native/webapp call, Pro vs Pro+ SKU split, repair-messaging value).

- [x] **Button Design Rationale drafted (Apr 28).** HCDE portfolio companion to [button-interface.md](docs/button-interface.md) at [docs/button-design-rationale.md](docs/button-design-rationale.md). Status: draft, pre-user-research — revises after Week 5 interviews. 10 sections covering: the question (light vs smart device), audience priority (Curator > Vibe-Seeker >> Spec-Head), the core "light first" tension, four rejected alternatives (app-only, single-button-only, capacitive touch, encoder), why two buttons + Eero precedent for the recessed button, why V0's gesture grammar was preserved, trade-offs explicitly accepted (sparse hardware UI, no combos, bounded discoverability, app-survival risk), six testable hypotheses tied to interview questions, and open/parked items. One-sentence summary: *"Two buttons because the mirror is a light first and a smart device second — and the difference between those two roles deserves a physical seam."* Closes the Week 5 sprint plan "design rationale" deliverable.

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
