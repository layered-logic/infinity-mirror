---
title: Advisor Meeting — May 12, 2026
type: meeting-log
phase: 3
week: 7
date: 2026-05-12
status: draft
tags: [meeting, advisor, milestone-2, week-7]
---

# Advisor Meeting — May 12, 2026
**Project:** Layered Logic / Infinity Mirror Venture
**Student:** William White (Bill)
**Quarter:** Spring 2026 — Week 7 of 11
**Purpose:** Update since Apr 30, Milestone 2 close-out, transition into Phase 3 (Service Blueprinting & Value Strategy)

---

## Since Last Meeting (Apr 30 → May 12)

Twelve days. Two distinct workstreams: the **multi-network firmware track** closed out as the Week 6 engineering deliverable replacing the no-longer-real "PCBs arrive" item, and the **physical housing track** opened on May 11 as the first hands-on industrial-design block since the brand lock. Process side also got tighter — task registry migrated to a stable-ID schema, and the supply chain map landed as the last M2 deliverable. **Milestone 2 (May 9) closed on time on every component except user research, which remains the standing gap.**

### Engineering — multi-network firmware + connectivity hardening

**App release-build connectivity rescue ([LL-040](../../tasks.md#LL-040), May 1).** Three independent fixes shipped end-to-end on the XIAO ESP32-C3 prototype + Pixel 9, all OTA-delivered:
- **Cleartext-traffic block on release APKs fixed.** Hardcoded `usesCleartextTraffic="true"` directly into [AndroidManifest.xml](../../App/v1/android/app/src/main/AndroidManifest.xml) to defeat RN gradle's `finalizeDsl` clobber. Release builds now talk to the mirror (debug builds always did).
- **Native mDNS discovery via Android `NsdManager`** ([MdnsModule.kt](../../App/v1/android/app/src/main/java/com/v1/MdnsModule.kt)) replaces the prior 254-host subnet scan. The scan was hitting OkHttp's 64-concurrent-request cap, never reaching the mirror's slot before the 1500ms abort fired. New path resolves in 2–3s on any LAN topology.
- **`ip` field in DeviceState broadcast** — Settings page now surfaces the mirror's IP, which the webapp hid inside its mDNS hostname URL.

**Multi-network design doc locked ([LL-041](../../tasks.md#LL-041), May 1).** [docs/multi-network-design.md](../multi-network-design.md) status flipped from draft to implementation-ready. All five §10 open questions answered: last-used wins; `N_MAX=4` (Bill's realistic personal-use ceiling: home, work, parents, travel hotspot); stay in BACKOFF indefinitely with **no auto-SoftAP** (overrode the recommendation per the "less RF unless asked" principle); proactive disconnect on remove-while-connected, gated by app-side confirm dialog; `set_wifi_creds` deprecated through V2 lifetime. Two cross-cutting product principles surfaced from this and saved as feedback memories for the rest of the quarter: **RF-minimal-unless-asked** and **respect-explicit-actions** (destructive ops execute immediately, not iOS-style passive).

**Multi-network firmware implementation ([LL-046](../../tasks.md#LL-046), May 7).** All 5 design-doc steps shipped, hardware-verified on the prototype, plus a meaningful bonus op and a safety net:
- **Step 1 (NVS layer).** New `Firmware/v1/core/ll_wifi/` module, logic split from ESP-IDF plumbing the same way `core/nvs/` is. 27 host tests cover insert/update/full/invalid, remove + active_idx fixup, last-used-wins priority with recently-failed mask, and non-null-terminated-string sanitization. Per-entry NVS blob keys so add/remove only rewrites what changed.
- **Step 2 (provisioning refactor).** STA driven from `ll_wifi` rather than `esp_wifi`'s saved cred. Migration shim copies any legacy cred into `ll_wifi[0]` at first boot — **real-hardware verified**: device booted with empty `ll_wifi`, read the legacy cred, and reconnected without user intervention.
- **Step 3 (state machine).** SCANNING → CONNECTING → ONLINE / BACKOFF cycle (backoff 5s/15s/60s, 8s connect-watchdog). The first OTA attempt **bricked the live mirror** — a 1.6 KB scan-records array on the system event task's 2.3 KB stack. Recovery via USB reflash; fix was heap-allocation.
- **OTA rollback safety net.** The brick revealed a missing dev safeguard. Enabled `CONFIG_BOOTLOADER_APP_ROLLBACK_ENABLE` + an `ll_ota_subscribe()` that marks the app valid on the first `LL_EV_WIFI_CONNECTED` after boot. If a freshly-OTA'd image panics first or never reaches the network, the bootloader rolls back to the previous slot on the next reset. Orthogonal to the production-OTA anti-rollback eFuse (downgrade-attack defense vs. boot-loop recovery). Future OTAs are now self-healing — this directly de-risks the production OTA workstream.
- **Step 4 (wire protocol).** Three new ops: `list_wifi_networks`, `add_wifi_network`, `remove_wifi_network`. `wifi_saved_count` added to DeviceState; full list lives behind the read op to keep state broadcasts small.
- **Step 5 (RN app Settings UI).** Saved-networks list with active checkmark + Forget buttons, "+ Add a network" inline form, confirm dialog gated on removing the active SSID.
- **Bonus `connect_wifi_network` op.** Hardware testing surfaced a UX gap the spec's no-auto-SoftAP rule doesn't recover from cleanly: provisioning a bad second-network cred and forgetting the first leaves the mirror stuck in SCAN→fail→BACKOFF with no easy way back. Added a "switch to this network" op + Connect button per non-active row. Caught and fixed a sequencing bug along the way (op handler was tearing STA down before the WS response could flush); refactored to event-bus dispatch with the same 250ms response-flush yield used by apply-creds. Optimistic UI clear + brand-indigo banner + haptic confirmation.
- **Status-bar overlap fix.** Settings nav icon was sometimes drawn under the Android system status bar (race on `StatusBar.currentHeight` returning 0 in cold-start). First attempt — `react-native-safe-area-context` — hit Windows MAX_PATH on the deep worktree; backed out and shipped a 32px-floor read-at-render-time instead. Same outcome, no native deps.

**RN app polish ([LL-073](../../tasks.md#LL-073), May 7).** Per-LED-pattern content-encoded haptic feedback (each pattern has its own buzz signature, intensity tracks brightness); full-width 512×512 color wheel with touch-follow magnifier bubble in the top-right preview; scroll-lock during drag so the page doesn't jitter while picking. The app now feels deliberate in a way it didn't before.

**User-definable mirror name + multi-mirror discovery** ([LL-039](../../tasks.md#LL-039), Apr 30 late, after the prior advisor doc was drafted). `name` (user-set) and `id` (read-only MAC suffix) added to `ll_state_t`; new `GET /api/info` HTTP discovery endpoint lets the RN app probe each candidate with one roundtrip that's both "is this a mirror" sentinel AND the picker's display label. RN app gets a picker for the multi-mirror case; Settings page gets a rename field. End-to-end verified via OTA-only — NVS schema bump v1→v2 forced state-to-defaults on first boot of the new firmware, as designed (no migration path until shipped users exist).

### Hardware — first physical housing iteration

**Basic Infinity Mirror PCB housing — seven iterations in one day (May 11).** First industrial-design block since the brand lock. matplotlib-driven layout sketch + Fusion CAD model for the 3D-printed housing that holds the existing 6×6 PCB. Lives at [Assembly_docs/basic_housing/](../../Assembly_docs/basic_housing/). The iteration cadence reads as a one-day design cycle that needed seven loops to converge:

1. **v1 — initial layout** ([PR #10](https://github.com/bowbikes/Independent_Study/pull/10)). matplotlib sketch establishes peg positions, button cavity, USB cutout, PCB pocket. Fusion script generates the parametric model.
2. **v2 — button raised + snaps moved inside** ([PR #11](https://github.com/bowbikes/Independent_Study/pull/11)). First prototype showed the button protruding past the USB cap height; raised the button to match, moved the snap-fit pegs inward to clear the PCB outline.
3. **Fusion sketch-plane Z-axis fix** ([PR #12](https://github.com/bowbikes/Independent_Study/pull/12)). `_cyl_y` cylinders were extruding on the wrong axis because Fusion's xZ sketch plane swaps Y/Z conventions vs. matplotlib's preview.
4. **Mirror snap-peg X positions** ([PR #13](https://github.com/bowbikes/Independent_Study/pull/13)). Pegs ended up on the wrong side of the centerline relative to the PCB orientation.
5. **X-mirror the entire layout** ([PR #14](https://github.com/bowbikes/Independent_Study/pull/14)). Realized the previous fix was a half-measure — the whole housing needed to flip, not just the pegs, to match how the PCB sits in-hand.
6. **6×6 button cavity + Y-split snap pegs** ([PR #15](https://github.com/bowbikes/Independent_Study/pull/15)). Round button cavity didn't capture the actual switch geometry; squared off to 6×6mm. Snap pegs split along the Y axis so they can flex inward during PCB insertion.
7. **Tolerance loosen** ([PR #16](https://github.com/bowbikes/Independent_Study/pull/16)). Final dimensional pass — peg clearance and button cavity opened up to land in the printable+assemblable window.

**Net read.** Seven iterations is a lot for one design day, but every loop terminated in a print or a fitment test, not in a paper rework. This is the cadence that closes the gap between the digital prototype and a unit you can actually hand to a buyer. The housing is the first physical artifact in the project that isn't off-the-shelf or laser-cut acrylic.

### Operations + Tooling

**Stable task registry ([PR #7](https://github.com/bowbikes/Independent_Study/pull/7), [#8](https://github.com/bowbikes/Independent_Study/pull/8), May 6).** Repo migrated from fuzzy-match task tracking to a stable `LL-NNN` ID schema. [tasks.md](../../tasks.md) is the structured registry; [task_log.md](../../task_log.md) is the append-only event history; [docs/task-format-v2.md](../task-format-v2.md) is the format spec. [sprint_plan.md](../../sprint_plan.md), [sprint_log.md](../../sprint_log.md), [README.md](../../README.md), [index.md](../../index.md), and [CLAUDE.md](../../CLAUDE.md) all reference task IDs inline as `[LL-NNN](tasks.md#LL-NNN)`. IDs are immutable — a child whose scope grows spawns grandchildren (`LL-NNN-M-K`), never gets promoted. The collector / n8n pipeline still runs on the old fuzzy-match code and is pending a separate update.

**Supply chain map ([LL-050](../../tasks.md#LL-050), May 7).** 13-line component table with sourcing cascades (Amazon → AliExpress → Temu → factory; JLCPCB → PCBWay → OSH Park) and executive risk framing. Closes the last Milestone 2 deliverable. Risk priority order from the doc: **tariff exposure > lead time > single-source**. Tariffs are the dominant short-horizon variable (US/China policy whiplash); lead time matters but is hedgeable; single-source risk is genuinely low because every line has a viable substitute.

**Client color-swatch unification ([PR #4](https://github.com/bowbikes/Independent_Study/pull/4), Apr 30).** Webapp + RN app drifted into inconsistent preset color codes during the mini-sprint. Unified both into a shared `BRAND_SWATCHES` source. Polish-only; closes a follow-up flagged in the Apr 30 cross-client sync demo.

---

## Status vs. Milestone 2 (May 9)

> **M2 — Brand + app UI + hardware validated + supply chain mapped.**

| Component | Status |
|---|---|
| Brand identity finalized | ✅ palette, type, tagline, §1 sentence, logo SVG set, app icon all locked Apr 30 |
| App UI designed + informed by user research | ⚠️ UI **shipped and polished** (LLogic + webapp; multi-network surface, haptics, color wheel, settings hierarchy). **User research interviews remain at zero conducted.** Outlines drafted Apr 28; no sessions held. |
| Hardware validated | ✅ V1 firmware on silicon since Apr 22; multi-network state machine + OTA rollback safety net add genuine robustness, not just feature surface |
| Supply chain mapped | ✅ [LL-050](../../tasks.md#LL-050) closed May 7 |

**Net read:** M2 closed three of four components on time. **User research is the single open gap from M2 and the standing gap from the Apr 30 meeting.** It's now four weeks old as a flagged-but-unactioned item.

---

## Open Questions

**1. User research — escalation, deferral, or scope change?** Same question as Apr 30, four weeks staler. Outlines are ready, contact list partially identified (Fremont Coffee Co., Broadcast Coffee from the Apr 21 meeting). Three framings worth picking between:
- **(a) Schedule 3–5 interviews this week**, accepting the cost to Week 7 service-blueprint work — interviews actually feed the service blueprint, so the cost may be lower than it looks.
- **(b) Convert the interview deliverable into something faster** — survey-form distribution to the LinkedIn / friends-of-friends graph, async, lower-rigor.
- **(c) Acknowledge it's not happening at quarter-level rigor and revise the M2 scorecard** — say so explicitly in the portfolio rather than carrying it as a stale TBD.

**2. Production OTA — does it sit before or after Milestone 3 (May 30)?** The May 7 in-flight rollback safety net materially de-risks production OTA — boot-loop recovery now works in the dev path, so adding signed binaries + anti-rollback eFuse + Cloudflare Worker/R2 hosting is incremental rather than greenfield. Architecture decisions from May 7 already locked in [LL-071](../../tasks.md#LL-071) notes: pull-based manifest-then-binary, 48h poll interval, prompt-and-confirm UX via the LL app. **Question:** if M3 is "ready to sell," does a unit shipping to a real buyer need production OTA from day one, or is the right gate "OTA infrastructure exists and is testable, production hardening is post-M3"?

**3. Physical housing — print + assemble cycle as the Week 7 industrial-design block?** The May 11 housing track is the first hands-on industrial-design work in weeks. Natural extension: assemble the first complete prototype unit (PCB + housing + frame + LEDs + mirror panels) as the artifact the Week 7 service blueprint maps customer-journey friction against. **Question:** is the first complete unit a Week 7 deliverable (golden-sample-adjacent, used as the service-blueprint anchor), or does it stay deferred to Week 9 ([LL-060](../../tasks.md#LL-060) golden sample) per the original plan?

**4. PCB production order timing.** Carryover from Apr 21. Five STM8 test boards already on hand from late April; production PCB order has been deferred since then. The multi-network firmware work proceeded on the prototype board without blocking, and the housing track is targeted at the existing PCB outline — so the deferral cost has been zero so far. **Question:** does the production PCB order need to land in Week 8 to be in hand for the M3 golden sample, or does the prototype board carry through to demo at M3 with a real-board commitment in Week 9–10?

**5. Brand scope overreach + right-to-repair-as-liability — still parked.** Both Apr 21 / Apr 30 questions unanswered. Both shape decisions for the rest of the quarter (Tier 2 mini-brand naming, public schematic publication). Worth picking up explicitly this meeting since the rest of the quarter compresses.

---

## Milestones Ahead

| Date | Milestone | Status |
|---|---|---|
| ✅ Apr 18 | M1 — Business foundation locked | Done |
| ✅ May 9 | M2 — Brand + app UI + hardware validated + supply chain mapped | Closed 3 of 4; user research is the standing gap |
| **May 30** | **M3 — Go-to-market ready + golden sample** | On track on engineering; production OTA hardening + production PCB order timing + housing-to-golden-sample bridge are the open levers |
| Jun 12 | M4 — Portfolio complete + venture readiness | Repair docs done; service blueprint (Week 7) + portfolio case study (Week 10) ahead |

---

## Demo Material Available

- **Live demo path** (unchanged from May 5; all gates still green):
  - XIAO ESP32-C3 prototype on phone hotspot
  - Webapp at `http://<mirror-ip>/` from any browser on the same network
  - LLogic Android app on Pixel 9 — Find mirror via native mDNS, multi-network Settings surface (list / add / forget / connect), OTA with self-healing rollback, haptic + color-wheel polish
- **New since Apr 30:**
  - **Multi-network surface end-to-end**: add a second network from the app → Connect → mirror switches → app reconnects on the new network. Recovery from the no-creds-match-no-known-network case via the Connect button.
  - **OTA rollback safety net**: deliberately push a panicking image → mirror auto-recovers to the previous slot on next boot. Demonstrable on request via a contrived test build.
  - **Basic Infinity Mirror PCB housing** — printed iterations available for in-person fitment review.
- **Portfolio artifacts** (linked from [index.md](../../index.md)):
  - HCDE: stakeholder map, sustainability audit, brand positioning, brand research notes, typography decisions, right-to-repair philosophy + 3 companion docs, failure-mode inventory, button-interface + button-design-rationale, user-interview outlines
  - Engineering: firmware architecture scoping, firmware spec, firmware security, control protocol spec, app spec, webapp spec, multi-network design, ESP32 module comparison + shortlist, OTA dev runbook, mini-sprint plan + live sprint log
  - Business: market analysis report, BOM breakdown, supply chain map, pricing calculator (Streamlit), assembly/operations docs + basic_housing CAD

---

## Related

- [Index / MOC](../../index.md)
- [Advisor Meeting — Apr 30](advisor-meeting-2026-04-30.md) (prior — Week 5 / mini-sprint demo preview)
- [Advisor Meeting — Apr 21](advisor-meeting-2026-04-21.md) (Milestone 1 close-out)
- [Sprint Plan](../../sprint_plan.md) · [Sprint Log](../../sprint_log.md) · [Task Registry](../../tasks.md) · [Task Event Log](../../task_log.md)
- [Multi-Network Design](../multi-network-design.md) · [Repair Index](../repair-index.md) · [Brand Positioning](../brand-positioning.md)
