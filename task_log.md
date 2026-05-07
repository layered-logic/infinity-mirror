---
title: Task Event Log
type: task-log
---

# Task Event Log

Append-only. Every meaningful task status transition gets one row. Format spec: [docs/task-format-v2.md](docs/task-format-v2.md) §4.2.

**Rules:**
- One row per event; multiple events in one session = multiple rows.
- Never edit an existing row. Corrections are appended as new rows with explanatory notes.
- `note` is ~1 sentence; full narrative lives in `sprint_log.md`.
- `artifact` is one link or `—`.

---

| date | id | event | note | artifact |
|------|----|-------|------|----------|
| 2026-03-23 | LL-001 | done | LLC Certificate of Formation filed pre-quarter for Layered Logic LLC (WA-formed domestic single-member) | — |
| 2026-04-02 | LL-002 | done | EIN 41-5282028 issued by IRS online | — |
| 2026-04-02 | LL-003 | done | Streamlit pricing dashboard with dynamic BOM, size scaling, tier/platform selection, and margin analysis | tools/pricing/ |
| 2026-04-02 | LL-004 | done | 5-phase stakeholder map covering 24 stakeholders across Creation, Distribution, Acquisition, Lived Experience, Support/Legacy | docs/stakeholder-map.md |
| 2026-04-06 | LL-005 | done | Single-member operating agreement finalized, printed, and signed with Schedule A capital contributions | — |
| 2026-04-06 | LL-006 | done | ESP32-C3-MINI-1 selected (smallest footprint, USB Serial/JTAG built in); C6 flagged as future Pro+ variant | docs/esp32-module-comparison.md |
| 2026-04-06 | LL-007 | done | 30+ listings analyzed across Etsy/Amazon/Pinterest; bifurcated 4-segment market | docs/Market%20Analysis%20Report.md |
| 2026-04-09 | LL-009 | done | 7-stage cradle-to-grave lifecycle; 6,368 energy samples; LED lifespan 14+ yr; key hotspot coated acrylic panels | docs/sustainability-audit.md |
| 2026-04-09 | LL-010 | done | Variable COGS $81.62 (qty 1) → $49.80 (qty 1,000); $65 retail recommended for early volumes | docs/bom-breakdown-basic-6x6.md |
| 2026-04-09 | LL-011 | done | JLCPCB STM8 PCB at $0.90/board at qty 100; pricing config $5 controller placeholder overstated | — |
| 2026-04-14 | LL-012 | done | Made-to-order workflow: 3 parallel lanes, 20 stations, batch-by-operation; Phase 1 tooling list finalized | Assembly_docs/ |
| 2026-04-20 | LL-013 | done | 9-section brand worksheet locked: 3-tier architecture, italic-default typography, hybrid palette, "[X] that layers." motif | docs/brand-positioning.md |
| 2026-04-20 | LL-014 | done | Three palette swatches as SVGs (Option B, Option C, Hybrid) with dark/light previews; Hybrid locked | assets/brand/color/ |
| 2026-04-20 | LL-015 | done | YAML frontmatter + Related cross-linking; MOC at index.md; standard markdown links over wikilinks for GitHub | index.md |
| 2026-04-20 | LL-016 | done | Asset folders scaffolded with subfolders for brand, moodboards, product-photos, icons, figma, source-files | assets/README.md |
| 2026-04-20 | LL-017 | done | First-pass review of Bill's separate logo P5 repo; 5 parked items captured in brand-positioning §7 | docs/brand-positioning.md |
| 2026-04-20 | LL-018 | done | Firmware architecture scoping doc with 5 design principles locked + 5 open decisions staged for Wed Apr 22 | docs/firmware-architecture-scoping.md |
| 2026-04-21 | LL-019 | done | Advisor meeting M1 close-out doc with 5 open questions for advisor | docs/Advisor-Meetings/advisor-meeting-2026-04-21.md |
| 2026-04-21 | LL-020 | done | Logo color resolved (outer #4A25FF Indigo Reference, inner #3214FF Indigo Signal); 3 deterministic SVG exports | assets/brand/logo/ |
| 2026-04-21 | LL-021 | done | Whole-vault palette re-anchored magenta-violet → indigo after rendered artifacts read too pink | docs/brand-positioning.md |
| 2026-04-21 | LL-022 | done | 3 Gemini JSON exports converted to paired .md with frontmatter + summaries; reusable converter script committed | external_chats/ |
| 2026-04-21 | LL-023 | done | WCAG audit caught indigo on near-black failing AA; section headers swapped to cream (~14:1 AAA); advisor doc trimmed 140→85 lines | assets/brand/color/ |
| 2026-04-21 | LL-024 | done | Typography rationale + logo spec spun out into dedicated docs; cross-links tightened across vault | docs/typography-decisions.md |
| 2026-04-22 | LL-025 | done | Two-button hardware model locked: primary (4 gestures) + recessed pinhole (3s pair / 10s factory reset) | docs/button-interface.md |
| 2026-04-22 | LL-026 | done | WA Business Licensing Service application submitted; UBI issued; $90 fee paid; Retailing+Manufacturing activities | — |
| 2026-04-22 | LL-027 | done | All 5 firmware architecture decisions locked: ESP-IDF+C, BLE+SoftAP provisioning, 2 variants (Pro/Pro+), self-hosted OTA, JSON envelope protocol | docs/firmware-architecture-scoping.md |
| 2026-04-22 | LL-028 | done | Confirmed N/A — FinCEN interim final rule exempts all US-domestic entities; re-check before Jun 21 | https://fincen.gov/boi |
| 2026-04-22 | LL-029 | done | V1 firmware: 4 core modules (state_bus, button, led_driver, pattern_interp) + ~50-test host harness; ESP-IDF 5.5.4 toolchain end-to-end | Firmware/v1/ |
| 2026-04-27 | LL-030 | done | Right-to-Repair philosophy doc finalized: 8 commitments, honest limits, EU/WA/CA compliance reinforcement | docs/right-to-repair-philosophy.md |
| 2026-04-27 | LL-031 | done | 17 failure-mode files across 8 categories: 12 fully user-repairable, 5 partial, 0 non-repairable | Failure_Modes/ |
| 2026-04-28 | LL-008 | done | External personal accounts linked + initial deposit cleared; account active and past inactivity threshold | — |
| 2026-04-28 | LL-032 | done | Two interview outlines (end-buyer + showcase partner) with 7-row "data we want" table tying questions to open decisions | docs/user-interview-outline.md |
| 2026-04-28 | LL-033 | done | HCDE button design rationale: 10 sections, 4 rejected alternatives, Eero precedent, 6 testable hypotheses | docs/button-design-rationale.md |
| 2026-04-28 | LL-034 | done | Pre-flash review caught critical bug in button.c (LL_PIN_BUTTON_RESET=-1 unguarded) + LED_COUNT_DEFAULT 32→66 | Firmware/v1/core/button/button.c |
| 2026-04-28 | LL-035-0 | done | V1 firmware booted on XIAO ESP32-C3 after fixing TASK_CORE 1→0 and led_count 32→66 in-session; button gestures + LED response confirmed | sprint_log.md |
| 2026-04-29 | LL-035-1-1 | done | 4MB ab_with_factory partition table verified on silicon; 25% headroom; existing NVS state survived partition swap | Firmware/v1/partitions.csv |
| 2026-04-29 | LL-035-1-2 | done | core/ll_mdns module advertises _layeredlogic._tcp.local with TXT records; folder rename mdns→ll_mdns to avoid build-graph collision | Firmware/v1/core/ll_mdns/ |
| 2026-04-29 | LL-035-1-3 | done | Transport + dev SoftAP + wscat ping/pong verified end-to-end; 2 in-session bugs fixed (sdkconfig stale, AP fast path race) | Firmware/v1/core/transport/ |
| 2026-04-29 | LL-035-1 | done | Session 1 closed; all 3 grandchildren resolved | Firmware/v1/core/transport/ |
| 2026-04-29 | LL-035-2-1 | done | get_state op live; state_to_json serializes 7 fields with #RRGGBB hex conversion | Firmware/v1/core/transport/transport.c |
| 2026-04-29 | LL-035-2-2 | done | set_state op live; 4 mutable fields, sensitive fields rejected; LEDs went bright green on first wire test | Firmware/v1/core/transport/transport.c |
| 2026-04-29 | LL-035-2-3 | done | broadcast_state + LL_EV_STATE_CHANGED subscription; two-watcher triangle verified at ~65ms latency | Firmware/v1/core/transport/transport.c |
| 2026-04-29 | LL-035-2 | done | Session 2 closed; all 3 grandchildren resolved; cross-client sync foundation laid | Firmware/v1/core/transport/transport.c |
| 2026-04-29 | LL-035-3 | done | Webapp scaffold (Vite + Preact + TS); first click changes mirror color; bundle 8.6KB gzipped (11% of 80KB budget) | Firmware/v1/webapp/ |
| 2026-04-29 | LL-035-4 | done | All 5 webapp capabilities wired (brightness, pattern, on/off, color, factory-reset stub); bundle 9.87KB gzipped (12% of budget) | Firmware/v1/webapp/src/app.tsx |
| 2026-04-29 | LL-035-5-1 | done | Webapp bundle embedded in firmware; pre-gzip in npm build; mDNS-over-SoftAP works on Windows as bonus | Firmware/v1/core/webapp_assets/ |
| 2026-04-29 | LL-035-5-2 | done | mDNS-over-hotspot Plan A confirmed for PC + browser via Win32 getaddrinfo; spike reverted clean | sprint_log.md |
| 2026-04-29 | LL-035-5-3 | done | Captive-portal DNS hijack + HTTP 404 redirect; Unicode arrow in format strings broke vsnprintf — defensive ASCII rule documented | Firmware/v1/core/captive_dns/ |
| 2026-04-29 | LL-035-5-4 | done | Setup screen + set_wifi_creds wire op; 15s fallback timer (typo'd password no longer strands device); webapp router + 3 pages | Firmware/v1/core/transport/transport.c |
| 2026-04-29 | LL-035-5-5 | done | LL_DEV_OPEN_SOFTAP → LL_SOFTAP_PROVISIONING rename + comment refresh; pure cleanup, no functional changes | Firmware/v1/core/provisioning/ |
| 2026-04-29 | LL-035-5 | done | Session 5 closed; all 5 sub-milestones resolved; webapp half of demo fully functional | Firmware/v1/webapp/ |
| 2026-04-30 | LL-035-6 | done | Sessions 6-8 collapsed: bare RN app + 5 controls end-to-end against live mirror; 3 Windows wedges hit and fixed | App/v1/ |
| 2026-04-30 | LL-035-7 | done | RN provisioning + Find Mirror (subnet scan + WifiInfo) + brand pass + first install on physical Pixel 9; 3 verification legs green | App/v1/ |
| 2026-04-30 | LL-035-8 | done | Cross-client sync verified: webapp + LLogic app simultaneously, state changes from either propagate to the other within broadcast latency | sprint_log.md |
| 2026-04-30 | LL-035 | done | Mini-sprint parent closed during cleanup; all 8 sessions resolved; demo gate hit 5 days ahead of May 5 advisor meeting | docs/mini-sprint-app-demo.md |
| 2026-04-30 | LL-036 | done | 3 RtR portfolio docs land (decisions matrix + scorecard + sub-MOC index); §5 comparative table TBD pending iFixit data | docs/repair-index.md |
| 2026-04-30 | LL-037 | done | Brand-positioning §1 sentence locked; product-name slot remains bracketed (Tier 2 naming deferred) | docs/brand-positioning.md |
| 2026-04-30 | LL-038 | done | OTA infrastructure V1→V2 verified end-to-end on silicon, USB unplugged; cyan welcome on reboot proves OTA replaced running code | Firmware/v1/core/ota/ |
| 2026-04-30 | LL-039 | done | User-definable mirror name + multi-mirror picker; NVS schema bumped 1→2; OTA cycle V2→V3 verified end-to-end | Firmware/v1/core/state_bus/ |
| 2026-05-01 | LL-040 | done | Three coupled fixes shipped: cleartext-traffic AndroidManifest hardcode, native NsdManager mDNS module, ip field in DeviceState | App/v1/android/app/src/main/java/com/v1/MdnsModule.kt |
| 2026-05-01 | LL-041 | done | Multi-network design doc locked with 5 §10 decisions (last-used wins, N_MAX=4, no auto-SoftAP, proactive disconnect, deprecation horizon) | docs/multi-network-design.md |
| 2026-04-18 | LL-045 | slipped | Week 3 PCB order critical-path item silently deferred; 5 STM8 test boards on hand cover near-term firmware work; production order out of quarter scope | — |
| 2026-04-22 | LL-042 | started | Customer-facing User Repair Guide drafting begun in parallel with right-to-repair philosophy doc | — |
| 2026-04-28 | LL-043 | started | User interview outlines drafted (LL-032); contact list compilation and outreach commencing | docs/user-interview-outline.md |
| 2026-04-28 | LL-044 | blocked | Awaiting LL-051 (packaging concept) — needs cardboard stock, dimensions, and print/laser process decided before QR placement is meaningful | — |
| 2026-05-07 | LL-046 | started | Step 1/6 (NVS layer) shipped — new core/ll_wifi/ module with list ops + migration shim hook + 27 host tests; provisioning wiring deferred to Step 2 | Firmware/v1/core/ll_wifi/ |
| 2026-05-07 | LL-046 | progress | Step 2/6 (provisioning refactor) build-clean on esp32c3 — STA driven from ll_wifi, legacy esp_wifi cred migration, set_wifi_creds writes to ll_wifi first, fallback removes failed entry, factory reset wipes both stores; flash + boot capture pending hardware availability | Firmware/v1/core/provisioning/provisioning.c |
| 2026-05-07 | LL-046 | progress | Step 2 OTA-flashed onto live mirror (b2332c at 192.168.5.229) — fw_version flipped ip-state-v3 → 4c1f888-dirty, STA auto-reconnected to "IoT" at same IP, all 7 ll_settings fields preserved across reboot. Migration shim verified: legacy esp_wifi cred read from `nvs.net80211` and copied into ll_wifi[0] on first boot of multi-network firmware | Firmware/v1/scripts/trigger_ota_step2.py |
