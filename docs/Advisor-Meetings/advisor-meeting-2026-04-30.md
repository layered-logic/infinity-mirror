---
title: Advisor Meeting — April 30, 2026
type: meeting-log
phase: 2
week: 5
date: 2026-04-30
status: draft
tags: [meeting, advisor, milestone-2, week-5]
---

# Advisor Meeting — April 30, 2026
**Project:** Layered Logic / Infinity Mirror Venture
**Student:** William White (Bill)
**Quarter:** Spring 2026 — Week 5 of 11
**Purpose:** Update since Apr 21, May 5 mini-sprint demo preview, open questions

---

## Since Last Meeting (Apr 21 → Apr 30)

Nine days. The shape: engineering went through a focused multi-day push and ended ~5 days ahead of schedule on the May 5 advisor-demo gate; HCDE portfolio caught up at the end with the repair-rationale doc set and the brand-positioning §1 lock; one self-set late-day stretch goal — proving OTA infrastructure works end-to-end on silicon — landed cleanly.

### Engineering — Phase 2 firmware + apps

**Firmware architecture block locked (Apr 22).** All five decisions from the [scoping doc](../firmware-architecture-scoping.md) §5 closed: ESP-IDF + C, BLE+SoftAP provisioning via `wifi_prov_mgr` (radios dark at rest), two-variant build (Pro on C3 + Pro+ with Matter on C6), ECDSA-P256-signed self-hosted OTA at `ota.layeredlogic.cc`, JSON wire protocol over WebSocket+REST+BLE-GATT with mDNS discovery and user-selectable open/paired auth. Five downstream spec docs drafted in the same block: [firmware-spec](../firmware-spec.md), [firmware-security](../firmware-security.md), [control-protocol-spec](../control-protocol-spec.md), [app-spec](../app-spec.md), [webapp-spec](../webapp-spec.md).

**V1 firmware bring-up (Apr 22 → Apr 29).** Greenfield ESP-IDF rewrite. 8 of 11 planned modules implemented and on-silicon-verified on the XIAO ESP32-C3 prototype: `state_bus`, `button`, `led_driver`, `pattern_interp`, `provisioning`, `nvs`, `ll_mdns`, `transport`. Plus webapp asset embedding, captive-portal DNS hijack, and (today, Apr 30) an OTA module — the 9th. Three modules remain scaffold-only by design (`auth`, `telemetry`, full production OTA).

**App demo mini-sprint planned + executed (Apr 28 → Apr 30, 5 days early on the gate).** 8 sessions on the books targeting May 5; ran fully through the 5th session by end-of-day Apr 29 (transport + webapp + captive portal + factory-reset wire-op, all verified end-to-end on silicon). Sessions 6–8 (the React Native Android app) collapsed into a single morning push today and a follow-on push the same afternoon — RN scaffold, full 5-control surface, SoftAP-based provisioning UI, native subnet-scan mirror discovery, and a complete factory-reset → re-provision → control round-trip verified on Bill's Pixel 9 over a phone hotspot. Plan + risks + Definition-of-Done at [docs/mini-sprint-app-demo.md](../mini-sprint-app-demo.md).

**Cross-client sync proven** end-to-end Apr 30 — webapp open on the dev PC alongside the LLogic app on the Pixel 9, change a control on either surface, the other UI updates within broadcast latency. Closes the last Definition-of-Done line on the May 5 demo gate.

**OTA infrastructure validated end-to-end on silicon (Apr 30, late).** Throwaway dev-test path, but the actual mechanism: V1 (white welcome sequence + OTA support) USB-flashed; V2 (cyan welcome) pushed over Wi-Fi via `python -m http.server` and triggered from a button in the LLogic app. Mirror downloaded ~1.2MB, validated, swapped active partition, rebooted into V2 — visual confirmation via the cyan welcome, programmatic confirmation via WS ping. **Five days ago this was scaffold-only with the production architecture not implemented.** Today it's proven, even if the proof code is throwaway. Production OTA (signed binaries, anti-rollback eFuse, Cloudflare hosting, staged rollout, auto-revert) remains a parked workstream — but the foundational A/B partition layout, esp_https_ota client integration, and update UX are all validated.

### Brand — closed pieces

**§1 positioning sentence locked (Apr 30).** The last open template-Mad-Libs slot in the [brand-positioning worksheet](../brand-positioning.md) closed. Locked text:

> *"For design-conscious adults who want atmosphere over gadgetry, [product name] is a made-to-order LED light line that blends into a living space as ambient presence and that you can still repair with a screwdriver ten years later — designed, built, and documented as a system to keep, not a novelty to throw away."*

Single-sentence compression of the Apr 20 seed; category generalized from "infinity mirror" to "LED light line" so the same sentence works for the eventual Nixie clock and future product lines under the (still-deferred) Tier 2 mini-brand. The `[product name]` slot stays bracketed. Mirror-specific candidates (Depthwise, Inwards, Threshold) ruled out Apr 20 once parent expanded to multi-line.

**Brand pass on the LLogic app** — the Pixel 9 launcher icon is the nested-L logo at all 5 mipmap densities, generated from the canonical SVG geometry by a 100-line PIL script (no inkscape/cairosvg dep). App labeled "LLogic" on the launcher; app shell uses the Hybrid palette (near-black ground, cream type, indigo `#3214FF` for active selections). No external UI library — sticking to the no-deps rule.

### HCDE portfolio — repair ecosystem completed

**Three new repair docs (Apr 30)** turn the existing [right-to-repair-philosophy](../right-to-repair-philosophy.md) (Bill's first-person values doc, Apr 20) and [Failure_Modes inventory](../../Failure_Modes/README.md) (17 failure modes, Apr 27) into a portfolio-ready artifact set:

- [**docs/repair-design-decisions.md**](../repair-design-decisions.md) — 20-row matrix mapping specific design choices to five repair values (Reopenable / Commodity / Self-recovery / Long-life / OTA-fixable) and the failure modes each pre-empts. Format forces every row to terminate at a specific failure-mode link and a specific source citation, not at narrative.
- [**docs/repairability-scorecard.md**](../repairability-scorecard.md) — quantitative companion. Top-line: 17 modes, 12 fully user-repairable (71%), 0 non-repairable, 1-tool requirement (Phillips), 7-year minimum parts commitment. Per-mode tables; some repair-time cells flagged TBD pending golden-sample stopwatch measurement. §5 comparative table (vs. Etsy / sensory-therapy / smart-home lighting) is the most-TBD section — needs iFixit / EU-scoring data.
- [**docs/repair-index.md**](../repair-index.md) — sub-MOC pulling philosophy + inventory + decisions + scorecard + supporting context (sustainability, brand, BOM, firmware) into one navigable artifact. §How-to-read-this-set routes different audiences (buyer / advisor / regulator / engineer / repair shop) to the right entry path.

Linked from the home note's HCDE Artifacts section. All cross-references use standard markdown so they render in both Obsidian and GitHub.

### Compliance + Operations

**WA Business Licensing Service registration filed (Apr 22).** Retailing primary (0.471% B&O) + Manufacturing secondary (0.484% B&O); MATC covers the overlap. NAICS 335129. UBI issued at submission; physical license expected by mail. Reseller permit application is a separate follow-up once UBI confirmed — saves sales tax on WA-supplier component purchases.

**BOI Report (FinCEN) — confirmed exempt (Apr 22).** Per current FinCEN interim final rule, all US-domestic entities including Layered Logic LLC are exempt. Calendar reminder set to re-check guidance before Jun 21 in case the rule reverses.

**Business banking fully operational (Apr 28).** External accounts linked to the LLC business account; initial funding deposit cleared. Closes the Week 1 banking thread — no inactivity-closure risk.

### Ops + Tooling

- [Failure-mode inventory](../../Failure_Modes/README.md) (17 modes across 8 component categories) drafted Apr 27 — the technical evidence file behind the repair philosophy.
- [Button design rationale](../button-design-rationale.md) drafted Apr 28 — HCDE portfolio companion to the [button-interface](../button-interface.md) spec; pairs *why* with *what*.
- [User interview outlines](../user-interview-outline.md) drafted Apr 28 — two audiences (end-buyer + showcase partner), 20–25 min each.

---

## Status vs. Milestone 2 (May 9)

> **M2 — Brand + app UI + hardware validated + supply chain mapped.**

| Component | Status |
|---|---|
| Brand identity finalized | ✅ palette, type, tagline, §1 sentence, logo SVG set, app icon all locked |
| App UI designed + informed by user research | ⚠️ UI **built** (LLogic for Android + webapp for laptop); design rationale drafted (button rationale, app-spec). **User research interviews not yet conducted** — outlines drafted but no sessions held. |
| Hardware validated | ✅ V1 firmware on silicon since Apr 22; full transport + control + provisioning + OTA proven end-to-end |
| Supply chain mapped | ⏳ pending — Week 6 deliverable per [sprint plan](../../sprint_plan.md) |

**Net read:** M2 is mostly delivered ahead of schedule. The single material gap is user research — interviews are unstarted. Outlines are ready. Need to actually talk to people.

### May 5 mini-sprint demo (5 days out)

Demo gate scope from [docs/mini-sprint-app-demo.md §5](../mini-sprint-app-demo.md):

- ✅ Plug in mirror → boots, joins network
- ✅ Webapp captive portal accepts Wi-Fi creds → mirror joins hotspot
- ✅ Webapp control surface: color / brightness / pattern / on-off
- ✅ Phone (LLogic, Android) discovers mirror via subnet-scan ("Find mirror" button)
- ✅ RN app: same 5 controls including factory reset
- ✅ Cross-client sync — change from one surface, other updates
- ✅ Trigger factory reset from either app, re-provision via either app
- ✅ **Stretch**: OTA firmware update, triggered from the LLogic app

All seven gates green five days early. Stretch goal also met.

---

## Open Questions

**1. Pacing — engineering ran ahead, HCDE caught up at the end, user research is unstarted.** The independent study is HCDE-framed: the system's design + values + ethics + venture readiness should be co-equal with engineering, arguably more weighted. The actual delivery this week was very engineering-heavy in time, with the HCDE portfolio docs catching up in a single afternoon block. The repair-doc set landed strong, but **user research interviews are still zero-conducted** — outlines are ready, contact list partially identified ([Fremont Coffee Co., Broadcast Coffee mentioned at the Apr 21 meeting]). What's the right way to approach the next two weeks: (a) immediately schedule + run 3–5 interviews even at the cost of further engineering polish, (b) keep building and let interviews happen organically as people pass through, (c) something else entirely?

**2. OTA — production hardening as a workstream of its own.** Today's OTA test is throwaway code: plain HTTP, no signing, no anti-rollback, no health-check rollback. Per the [right-to-repair §3 commitment](../right-to-repair-philosophy.md#3-what-i-commit-to), production OTA needs to be a real thing: signed binaries, self-hosted on `ota.layeredlogic.cc` (Cloudflare Worker + R2), `CONFIG_BOOTLOADER_APP_ANTI_ROLLBACK` (an eFuse burn — irreversible silicon write), auto-revert on health-check failure, staged rollout via device-ID hash buckets. That's a focused multi-day workstream. **Question:** does it sit before or after May 30 M3 (golden sample)? Production OTA is arguably part of "ready to sell" — a unit shipping to a buyer should have working production-grade firmware update from day one.

**3. Demo positioning at May 5.** What does the advisor most want to see in the meeting: (a) the live demo (laptop webapp + Pixel 9 LLogic app + mirror, all controlling each other in real time, including the OTA cycle), (b) the portfolio artifacts (repair docs, brand positioning, scoping/spec docs), (c) a structured walk through the mini-sprint plan vs. delivery? They're not mutually exclusive, but allocating 30 min meaningfully across them is its own decision.

**4. Carryover from Apr 21 still parked.** Two of three Apr-21 open questions weren't formally answered:
- *Brand scope overreach (three-tier hierarchy + tagline motif + consulting arm — over-architected for a single product?)*
- *Right-to-repair as differentiator vs. liability (published repair docs + on-request STLs + reflash service — IP/design protection tradeoff?)*

Both still relevant. The answers shape decisions for the rest of the quarter (e.g., should the Tier 2 mini-brand naming track reopen now that §1 is locked? Does the public schematic publication commitment in repair-philosophy §3 stay verbatim?).

---

## Milestones Ahead

| Date | Milestone | Status |
|---|---|---|
| ✅ Apr 18 | M1 — Business foundation locked | Done |
| **May 9** | **M2 — Brand + app UI + hardware validated + supply chain mapped** | On track; user research is the open gap; supply chain mapping is Week 6 |
| May 30 | M3 — Go-to-market ready + golden sample | Production PCB order timing decision still open; OTA hardening pending |
| Jun 12 | M4 — Portfolio complete + venture readiness | Repair docs done; service blueprint (Week 7) + portfolio case study (Week 10) ahead |

---

## Demo Material Available

- **Live demo path** (May 5 readiness verified Apr 30):
  - XIAO ESP32-C3 prototype on phone hotspot
  - Webapp at `http://<mirror-ip>/` from any browser on the same network
  - LLogic Android app on Pixel 9 (find-mirror discovery, full control, OTA, settings page)
  - Mirror visibly responds; broadcast loop keeps both surfaces in sync
- **Portfolio artifacts** (linked from [index.md](../../index.md)):
  - HCDE: stakeholder map, sustainability audit, brand positioning, brand research notes, typography decisions, right-to-repair philosophy + 3 new companion docs, failure-mode inventory, button-interface + button-design-rationale, user-interview outlines
  - Engineering: firmware architecture scoping, firmware spec, firmware security, control protocol spec, app spec, webapp spec, ESP32 module comparison + shortlist, mini-sprint plan + (live) sprint log
  - Business: market analysis report, BOM breakdown, pricing calculator (Streamlit), assembly/operations docs

---

## Related

- [Index / MOC](../../index.md)
- [Advisor Meeting — Apr 21](advisor-meeting-2026-04-21.md) (prior — Milestone 1 close-out)
- [Advisor Meeting — Apr 14](advisor-meeting-2026-04-14.md)
- [Sprint Plan](../../sprint_plan.md) · [Sprint Log](../../sprint_log.md)
- [Mini-Sprint: App Demo (Apr 28 → May 5)](../mini-sprint-app-demo.md)
- [Repair Index](../repair-index.md) · [Brand Positioning](../brand-positioning.md) · [Firmware Architecture Scoping](../firmware-architecture-scoping.md)
