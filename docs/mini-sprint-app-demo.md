---
title: "Mini-Sprint: App Demo (Apr 28 → May 5)"
type: planning
phase: 2
week: 5
date: 2026-04-28
status: active
tags: [app, webapp, react-native, mini-sprint, demo, transport]
---

# Mini-Sprint: App Demo — Apr 28 → May 5

**Window:** Tue Apr 28 → Tue May 5 (8 work sessions across ~7 days, full-time push)
**Goal:** Working end-to-end demos of **both** the device-hosted webapp **and** the React Native Android app, controlling a real ESP32 mirror, presentable to faculty advisor on May 5.
**Bar:** "Works enough to demo." Happy-path only is acceptable. No production hardening. Polish + BLE provisioning are stretch goals.
**Parent plan:** [Sprint Plan — Week 5](../sprint_plan.md#week-5-apr-28--may-5-app-uxui-design--app-demo-mini-sprint)

---

## 1. Locked Decisions

These are non-negotiable for this sprint. Re-litigating any of them blows the schedule.

| # | Decision | Detail |
|---|---|---|
| 1 | **Path** | A (transport) + B (webapp) + C (RN app) all in this sprint |
| 2 | **Demo board** | **ESP32-C3 dev kit** (the existing V0 prototype hardware, already wired). Built via `LL_BOARD_C3_DEVKIT`. C6 + Matter demo deferred until post-mini-sprint. |
| 3 | **Webapp stack** | Preact + Vite, served from ESP32 (≤80KB gzip per [webapp-spec.md](webapp-spec.md)) |
| 4 | **Native stack** | React Native (bare, *not* Expo). Android only this sprint — iOS deferred (needs Mac) |
| 5 | **Transport server** | `esp_http_server` — handles both REST and WebSocket from one handler. cJSON for JSON. |
| 6 | **Auth** | Open mode only. HMAC field stays optional in the envelope per [control-protocol-spec.md](control-protocol-spec.md), so this is *not* tech debt — sign-on-send / verify-on-receive can layer in later without protocol changes |
| 7 | **Provisioning** | Webapp captive portal handles all first-time pairing. **RN app skips BLE entirely for V1** — assumes mirror is already on Wi-Fi, discovers it via mDNS. |
| 8 | **Demo network** | Bill's phone hotspot. Avoids university Wi-Fi captive portals + IT security friction. |
| 9 | **State sync** | Server broadcasts `LL_EV_STATE_CHANGED` to all WS clients. Clients send `state.subscribe` on connect *and on app foreground* for fresh full-state snapshot. |
| 10 | **Pattern editor** | OUT. V1 = pattern *select* only (1 of 7 preloaded). Editor is V2. |
| 11 | **App scope** | 5 capabilities only: setup (webapp only), on/off, color, pattern select, factory reset |
| 12 | **Build flavor** | Stock unsigned dev builds — no secure boot, no flash encryption, no anti-rollback (those are eFuse burns and irreversible; production hardening is post-sprint). Reflash to ESPHome via device builder is always available as a recovery path. |

## 1a. Hardware Safety Rules (don't brick the device)

The C3 dev board can be reflashed back to ESPHome at any time *as long as we never burn an eFuse*. eFuse burns are silicon writes and irreversible. For this sprint:

**❌ Forbidden** (would lock or brick the device):
- Any `espefuse.py burn_*` command
- `CONFIG_SECURE_BOOT_V2_ENABLED` in sdkconfig
- `CONFIG_FLASH_ENCRYPTION_ENABLED` in sdkconfig
- `CONFIG_BOOTLOADER_APP_ANTI_ROLLBACK` in sdkconfig

**✅ Safe and reversible** (just rewrites SPI flash):
- `idf.py flash` — writes app + bootloader + partition table only
- `idf.py erase-flash` — clears SPI flash but leaves eFuses untouched
- Reflashing to ESPHome later via the ESPHome device builder

---

## 2. The Sessions

### Session 0 — Pre-flight smoke flash (Apr 28, 30–90 min)

Before writing any new code, validate that the existing 4 V1 modules build and flash cleanly to the C3 dev board. This de-risks Session 1 by isolating "build/flash pipeline works on real silicon" as a separate checkpoint.

- Build: `idf.py -DLL_VARIANT=standard -DLL_BOARD=c3_devkit set-target esp32c3 && idf.py build`
- Flash: `idf.py -p <COMv> flash monitor`
- Verify on the bench: primary button gestures (single → on/advance color, double → next pattern, triple → brightness step, hold → off); recessed-hold blue-blink cue; factory-reset red-flash → green-hold cue
- **If smoke flash fails:** stop, debug toolchain/board/build before piling transport on top
- **If smoke flash passes:** Session 1 proceeds with confidence

### The 8 sessions

| Day | Session | Goal | Milestone |
|---|---|---|---|
| **Tue Apr 28** | 1 | `core/transport/` scaffold — `esp_http_server` up, WebSocket endpoint, mDNS registration; `wscat` ping/pong works | Transport reachable |
| **Wed Apr 29** | 2 | Transport ↔ state_bus — full op set (`state.get`, `state.set`, `state.subscribe`); broadcasts on `LL_EV_STATE_CHANGED` to all clients | **Device controllable from any WS client** |
|  | 3 | Webapp scaffold — Vite + Preact, WS client class, first control screen wires color → device | First webapp click changes mirror color |
| **Thu Apr 30** | 4 | Webapp screens — home, control (color/brightness/pattern), settings, factory reset. All 5 capabilities wired. Bundle check. | All webapp capabilities functional |
|  | 5 | Captive portal — webapp served from ESP32 SoftAP, Wi-Fi entry form drives provisioning, fall-through to home Wi-Fi after success | **Webapp end-to-end demo works** |
| **Fri May 1** | 6 | RN bootstrap — bare RN project, Android tooling verified, "hello world" running on emulator + physical device, WS client connects to mirror over hotspot Wi-Fi | RN app talks to mirror |
| **Sat May 2** | 7 | RN control screens — same 5 capabilities, reuse webapp's WS client logic (port to TS), mDNS discovery via `react-native-zeroconf` | RN happy-path control works |
| **Sun May 3** | 8 | RN polish + buffer for whichever pain point bit hardest; demo dry-run | **Both demos run end-to-end** |
| **Mon May 4** | — | Buffer / rehearsal / screen recordings as backup | — |
| **Tue May 5** | — | Faculty advisor demo | — |

Note: Sessions 1–7 land in Week 5 (per sprint_plan calendar); Session 8 + buffer + demo bleed into the Sun/Mon gap and the start of Week 6.

---

## 3. Known Risks / Pain Points

Ranked by how badly they can derail the schedule.

### Tier 1 — schedule-killers

1. **Transport is greenfield (Sessions 1–2).** Biggest single risk. `core/transport/` doesn't exist yet. Use `esp_http_server` only — `esp_websocket_server` is deprecated. JSON via cJSON. If Session 2 slips, the whole plan slips by a day.
2. **RN environment setup tax.** First-time RN on Windows = JDK + Android Studio + Android SDK + AVD or physical device drivers. Could eat 1–4 hours depending on luck. **Mitigated:** Bill is installing tooling Apr 28 evening so it's off the critical path on Fri May 1.

### Tier 2 — manageable but want to flag

3. **mDNS over phone hotspot.** *Danger flag — Bill's call to push for the real solution first.* Some Android hotspot implementations don't forward mDNS / Bonjour multicast cleanly, and even when they do, IP discovery on hotspot networks is sometimes flaky. **Plan A:** make `react-native-zeroconf` work over the hotspot. **Plan B (escape hatch only):** hardcode the mirror's IP in the RN app for the demo. Hotspot-assigned IPs are usually stable for the same MAC, so this is a reliable last resort but ugly.
4. **Reflash iteration speed.** Every firmware change ≈ 30–60s build+flash. Webapp/RN are fast (HMR / live reload). If a transport bug requires 20 reflashes, that's 10–20 min of pure wait. Mitigation: use `idf.py monitor` for live `ESP_LOGI` output so most issues are diagnosable without reflashing.
5. **Webapp bundle size budget (≤80KB gzip).** Preact + hand-rolled CSS easily fits. Pulling in any UI component library (Material, shadcn-style imports, etc.) blows the budget. **Rule for this sprint: no UI libraries.** Verify with `vite build` end of Session 4.

---

## 4. Stretch Goals

Only attempt after the core 8-session plan is green.

- **BLE provisioning in RN app.** Use `react-native-ble-plx` + a community ESPProvision wrapper (or hand-rolled protobuf-over-GATT writes against the existing `wifi_prov_mgr` BLE service). Removes the "RN app needs the mirror to already be paired via webapp" caveat.
- **UI polish on both apps.** Brand palette ([brand-positioning.md](brand-positioning.md)), italic-default typography ([typography-decisions.md](typography-decisions.md)), real spacing/layout passes, motion on color/brightness sliders.
- **Settings/auth UX.** Surface the open vs. paired auth toggle in both apps (currently both apps assume open).
- **Multi-mirror UI.** Both apps currently assume one mirror. Add list/switcher.

Explicit non-goals — even as stretch:
- iOS RN build (needs a Mac)
- Pattern editor
- Hosted webapp at `layeredlogic.cc/controlmydevice` (mixed-content blocker, V2)
- Smart-home integrations (separate firmware variant, post-sprint)
- OTA flow surfaced in either app

---

## 5. Definition of Done — Demo Day

To call the demo done, all of these must work in front of the advisor:

- [ ] Plug in mirror → boots into SoftAP mode
- [ ] Laptop connects to mirror's SoftAP, opens webapp
- [ ] Webapp captive portal accepts hotspot Wi-Fi credentials → mirror joins hotspot
- [ ] Webapp reconnects to mirror over hotspot Wi-Fi (mDNS, or fallback IP)
- [ ] Webapp: change color → mirror updates within ~200ms
- [ ] Webapp: change brightness, change pattern, toggle on/off — all reflected on mirror
- [ ] Phone (RN app, Android) discovers same mirror via mDNS over hotspot
- [ ] RN app: same 4 controls work (color, brightness, pattern, on/off)
- [ ] **Cross-client sync:** webapp + RN app open simultaneously, change from one, the other UI updates within ~200ms (broadcast over WS)
- [ ] Trigger factory reset from either app

---

## 6. Pre-Sprint Setup (Apr 28 evening, off the critical path)

- [ ] Install Android Studio + Android SDK + AVD or USB drivers for physical device
- [ ] Install Node + JDK if not already on the dev machine
- [ ] Verify `npx react-native --version` runs without errors
- [ ] Configure phone hotspot, note the SSID + password the firmware will need
- [ ] Confirm ESP-IDF + MSYS2 + host test toolchain still working from prior sprint (per [reference_esp_idf_setup.md](../../../.claude/projects/C--Users-bowhi-Desktop-Independent-Study/memory/reference_esp_idf_setup.md))

---

## Related

- [Sprint Plan](../sprint_plan.md) — parent quarter plan
- [Sprint Log](../sprint_log.md) — daily progress tracker
- [Week 5 App Brainstorm](week5-app-brainstorm.md) — pre-decision exploration that fed this mini-sprint
- [App Spec](app-spec.md) — full RN app scope (this sprint hits the V1 subset)
- [Webapp Spec](webapp-spec.md) — device-hosted webapp scope + bundle constraints
- [Control Protocol Spec](control-protocol-spec.md) — wire protocol both apps speak
- [Firmware Spec](firmware-spec.md) — ESP32 component spec
- [Firmware Security](firmware-security.md) — auth modes (open/paired), HMAC, telemetry opt-in
