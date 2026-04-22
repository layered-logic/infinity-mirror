---
title: Firmware Architecture Scoping
type: engineering
phase: 2
week: 4
date: 2026-04-22
status: locked — all 5 architecture decisions resolved in Wed Apr 22 block
tags: [engineering, firmware, esp32, architecture, scoping]
---

# Firmware Architecture Scoping — ESP32 Greenfield Build

**Prepared by:** William White
**Date:** April 20, 2026 (drafted), April 22, 2026 (locked)
**Status:** All 5 §5 decisions locked. Serves as the authoritative architecture reference; downstream specs ([firmware-spec.md](firmware-spec.md), [app-spec.md](app-spec.md), [webapp-spec.md](webapp-spec.md), [control-protocol-spec.md](control-protocol-spec.md), [firmware-security.md](firmware-security.md)) implement decisions from this doc.

---

## 1. Purpose

This document scopes the Week 4 firmware architecture decisions. As of Apr 22 it captures the locked decisions from the Wednesday focus block, with the original options + tradeoffs preserved in-line for traceability.

It is **not** a spec. The downstream specs ([firmware-spec.md](firmware-spec.md), [app-spec.md](app-spec.md), [webapp-spec.md](webapp-spec.md), [control-protocol-spec.md](control-protocol-spec.md), [firmware-security.md](firmware-security.md)) implement the decisions from §5.

---

## 2. Context

The V0 infinity mirror prototype runs an ESPHome YAML config on an ESP32-C3 (see [`Firmware/ESP32/prototype_ESPHome_Version.yaml`](../../Firmware/ESP32/prototype_ESPHome_Version.yaml), outside this worktree). ESPHome was a fast path to a working prototype as a hobbyist, but it isn't a product-grade foundation:

- It ties the product to Home Assistant defaults and the ESPHome toolchain.
- Consumers can't reasonably be asked to flash ESPHome, use HACS, or operate inside the HA ecosystem.
- The right-to-repair philosophy's commitment to a closed-binary proprietary firmware with self-hosted OTA + reflash service (see [philosophy §3](right-to-repair-philosophy.md#3-what-i-commit-to), [§4](right-to-repair-philosophy.md#4-the-honest-limits)) only makes sense if I own the firmware stack outright.

So: **greenfield rewrite**, not a port. The prototype board will be fully reflashed once the new firmware boots. The ESPHome YAML is kept as a **feature-parity checklist** — what the new firmware must at least match — and nothing more.

---

## 3. Components In Scope

The Phase 2 engineering deliverable is three distinct components, not one:

| Component | Role |
|---|---|
| **Firmware** | Runs on ESP32. Drives LEDs, handles button input, runs pattern interpreter, handles provisioning + OTA + app protocol. |
| **Mobile app** | Primary user interface. Pattern editor, color picker, schedules, device pairing. |
| **Webapp** | Secondary interface / control panel. Same control surface as the mobile app, browser-reachable on local network. Also hosts account management and firmware update distribution. |

All three ship together as a system. The mobile app is the main user-facing surface; the webapp is the "any-device" fallback and the surface for features that don't fit on a phone.

---

## 4. Design Principles (Locked Apr 20)

1. **Standalone out-of-box.** The product must work fully without any smart-home connection. Smart-home integration is opt-in polish, never a gate on basic function. A buyer who never opens the app and never touches their Wi-Fi should still get a working mirror with sensible defaults.
2. **App is the pattern editor.** The firmware ships with a handful of default patterns and runs a generic pattern interpreter. Users create and modify their own patterns through the app; the app pushes pattern definitions to the device. This matches the "Living" adjective from [brand positioning §3](brand-positioning.md#3-brand-personality--five-adjectives) — the product is dynamic, adaptive, user-shaped.
3. **Configurable LED count.** The shipping product is the 6×6 (32 LEDs). The 12×12 unit (66 LEDs) is a firmware test prototype — used for development and scale testing, not for sale. Firmware must accept the count as a config value, not a hardcoded constant.
4. **Brand-palette default state.** The prototype's default-on color `(0.6, 0.0, 1.0)` = `#9900FF` is close to brand but not exact. New default: **Indigo Signal `#3214FF`** at 80% brightness (palette shifted to indigo Apr 21; confirmed). Every power-on returns the unit to this state.
5. **Closed-binary firmware, self-hosted OTA, reflash-service backstop.** Per right-to-repair philosophy §3 and §4. The firmware source is not public; the hardware (schematic, BOM, versioned PCBs) is. An owner with a dead board sends it to me for reflash; an owner with a bricked firmware pulls an OTA update. I'll revisit source escrow if the business EOLs.

---

## 5. Locked Decisions (Apr 22)

All five decisions resolved in the Wed Apr 22 focus block. Each section below captures the locked decision, rationale, and links to downstream spec docs where the decision is implemented. Original options + tradeoffs retained for traceability.

### 5.1 Language + Framework

Options:

| Option | Pros | Cons |
|---|---|---|
| **ESP-IDF (C/C++)** | Native Espressif. Best OTA + BLE + Wi-Fi support. Matches the `type: esp-idf` already in the ESPHome config. Most documented path for ESP32-C3. | C/C++ ergonomics. Steeper learning if unfamiliar. |
| **Arduino / PlatformIO** | Fastest to first blink. Huge ecosystem. Good for prototyping. | Less runtime control. OTA and secure-boot stories are weaker. Abstraction layers can leak under load. |
| **esp-rs (Rust on Espressif)** | Memory safety, modern tooling, fun to work in. Async story is good. | Ecosystem younger than ESP-IDF. More hunting for working examples of LED + BLE + OTA together. Higher risk for a week-4 commitment. |
| **NuttX / Zephyr / other RTOS** | Real OS semantics. | Overkill for this scope, adds scheduling complexity that doesn't buy much on a single-product firmware. |

**My recommendation going in:** ESP-IDF C++. Proven, matches the prototype framework, strongest OTA + BLE + Wi-Fi coverage, best docs. Rust is interesting but the time cost of learning-while-building is real. Arduino is a trap at product scale — great for prototyping, bad for long-term ownership. Willing to be talked out of ESP-IDF if there's a strong reason to go Rust.

### 🔒 Decision: **ESP-IDF with C as the default language**

C, not C++. ESP-IDF examples and docs are ~95% C; C++ support exists but creates friction with `extern "C"` boundaries and mixed compilation units. Small C++ islands acceptable where RAII genuinely helps (e.g., pattern object lifecycle) but not as the default idiom.

**What this means day-to-day:** paste-and-go from ESP-IDF examples, idiomatic `CMakeLists.txt`, explicit resource lifecycle (`_create` + `_delete` pairs). No RAII penalty at this scope — one person, product-scale firmware.

### 5.2 Provisioning UX

How does the user get the device onto their Wi-Fi the first time?

| Option | How it works | Pros | Cons |
|---|---|---|---|
| **BLE-to-Wi-Fi handoff via app** | Device advertises over BLE. User opens app, app discovers + pairs, user enters Wi-Fi creds in app, app pushes to device over BLE, device connects. | Clean UX. App-first flow aligns with brand. BLE is always available on ESP32-C3. | Requires the mobile app exists and works. Web fallback is awkward (browser BLE is spotty). |
| **SoftAP captive portal** | Device boots as its own Wi-Fi access point. User connects phone to "LayeredLogic-XXXX" network, browser redirects to a config page, user enters their home Wi-Fi creds. | Works with no app. Browser-only. | Breaks the "stay connected to your Wi-Fi" model. Some phones warn about "no internet on this network." |
| **Direct Wi-Fi + hardcoded creds** | User never configures Wi-Fi; device connects to something pre-set (dev-only). | Trivial for testing. | Obviously not shippable. |
| **Zigbee / Thread broadcast** | If ESP32-C6: device advertises as a Matter/Thread device; any Matter controller adopts it. | Zero-config for Matter users. | Requires ESP32-C6, requires the buyer has a Matter hub, doesn't help non-smart-home buyers. See §5.3. |

**Leading candidate:** BLE-to-Wi-Fi handoff via app, with SoftAP captive portal as a fallback for browser-only users and for when the app isn't installed. Both are already common patterns in commercial smart devices.

### 🔒 Decision: **BLE primary + SoftAP fallback, both via `wifi_prov_mgr`, user-initiated only**

- **Transport:** Espressif's Unified Provisioning library (`wifi_prov_mgr`) handles both BLE and SoftAP transports via the same firmware state machine. Official Android + iOS SDK components (ESPProvision) talk to it out of the box — saves ~2 weeks of custom BLE provisioning work.
- **Default posture:** **radios dark at rest.** No passive advertising. Device boots into normal running state (default color, button works) regardless of whether it's been provisioned.
- **User-initiated pairing:** triggered by a **3-second hold on the recessed button** (see [button-interface.md §5](button-interface.md#5-recessed-button--rare-operations)). Device enters pairing mode, advertises via BLE + SoftAP simultaneously, times out after 5 minutes.
- **No auto-pairing on first boot.** Consistent with principle §4.1 — the mirror is a light first; smart-device pairing is explicit opt-in.
- **Provisioned state:** device silently joins stored Wi-Fi, listens for app connections via WebSocket. BLE remains off unless user re-triggers pairing (for adding a second phone, etc.) OR Wi-Fi drops (minimal BLE advertising comes up as a discovery fallback per [control-protocol-spec.md](control-protocol-spec.md)).

### 5.3 Smart-Home Integration Path

Not being solved Wednesday — but flagged now so the framework choice doesn't box us out.

| Path | Module | Integration cost | Reach |
|---|---|---|---|
| **ESP32-C6 + Matter/Thread** | Swap module | Low — Matter gets us HA, Google Home, Apple Home, Alexa "for free" via the standard protocols. | Broad. The Matter promise. |
| **ESP32-C3 + individual integrations** | Stay on current module | High — write a HA integration, a Google Home action, an Apple HomeKit accessory, an Alexa skill, each separately. | Same endpoints, far more work. |

**Posture:** Decision deferred to customer research per index/open-questions. Wednesday's firmware architecture should not hardcode assumptions that would break a future C3→C6 swap. Specifically: keep the LED-driving + pattern-interpreter + provisioning layers **module-agnostic** so moving from C3 to C6 is a pin-mapping + radio-config change, not a rewrite.

### 🔒 Decision: **Two firmware variants, shared core library, Matter on Pro+ only**

The "C3 means writing per-platform integrations" framing this section originally used was pre-Matter. Matter-over-Wi-Fi works on the C3 via Espressif's `esp-matter` SDK, so the silicon decision and the Matter decision are independent. Key realization: **shipping Matter in every firmware binary forces flash and complexity costs on customers who don't want smart-home integration.** Split the firmware instead.

**Variant split:**

| Variant | Target audience | Control surfaces | Matter | Hardware target (shipping) |
|---|---|---|---|---|
| **Pro** | Everyone who just wants a working LED mirror + custom patterns | LL app + webapp + HA custom integration (HACS package) | Off | ESP32-C3-MINI-1 (4MB flash adequate) |
| **Pro+** | Smart-home enthusiasts who want Apple Home / Google Home / Alexa integration | Pro surfaces + Matter-over-Wi-Fi (+ Matter-over-Thread + Zigbee on C6) | **Uncertified, test VID 0xFFF1** for V1 | ESP32-C6 variant (8MB flash for Matter stack + headroom) |

Both variants:
- Share ~80% of firmware code via `Firmware/v1/core/` (LED driver, pattern interpreter, button handler, state bus, OTA, mDNS, WebSocket server)
- Diverge in `Firmware/v1/variants/standard/` vs `Firmware/v1/variants/matter/` (provisioning + additional control surfaces)
- Build from the same repo; CI produces both binaries in parallel
- Demo units (both variants) run on ESP32-C6-DevKitC-1 dev boards (8MB flash). Production silicon decision for the Pro variant remains C3 unless customer research signals otherwise.

**Product line taxonomy (three tiers):**

- **Basic** — STM8-based single-button model. No connectivity. Existing V0 PCB assets, no changes from this work.
- **Pro** — ESP32 + LL app + webapp + HA integration. No smart-home protocol support.
- **Pro+** — Pro + uncertified Matter (V1) → certified Matter (V2 when volume justifies CSA membership ~$10–15k/yr).

**Why uncertified Matter for V1:**
- Matter protocol spec is open; `connectedhomeip` / `esp-matter` stacks are open-source. Anyone can implement Matter without paying CSA.
- Test VID 0xFFF1 gets the device adopted by Home Assistant reliably, Apple Home / Google Home inconsistently (with a "not certified" warning users can accept).
- Investor demos showing Matter pairing work. Real-customer support burden mitigated by shipping Matter **disabled by default** on Pro+ units; user opts in via LL app with a clear "experimental" notice.
- Certification cost (~$10–15k/yr + per-SKU testing) is unjustified until product-market fit is proven.

**Matter architectural rules (Pro+ only):**
- Matter exposes only **On/Off + Level Control + Color Control** clusters. Patterns stay in the LL app; Matter-side view of the mirror is "a smart color light."
- Matter cluster callbacks write to the same [unified state bus](firmware-spec.md#state-bus) as WebSocket/BLE/button inputs. No controller has privileged state.
- Matter commissioning uses a **separate user gesture** on the recessed button (tentative: 6-second hold) to avoid conflicting with `wifi_prov_mgr` BLE advertising. See [button-interface.md](button-interface.md) for final gesture assignment.
- Matter fabric auth is independent of LL auth mode (see §5.5). Each ecosystem manages its own credentials.

**SKU structure decision parked:** whether Pro+ becomes a real shipping SKU or stays a prototype-only demo is a Week 5+ customer-research decision. Architecture supports either outcome without rework.

### 5.4 OTA Update Channel

Options:

| Option | How it works | Pros | Cons |
|---|---|---|---|
| **Self-hosted HTTPS OTA, signed binaries** | Device polls a known endpoint (layeredlogic.com or similar) for updates. Binaries signed with a key on my side, verified on-device. | Full control. Meets the philosophy's "I own the firmware stack" commitment. Standard ESP-IDF path. | I run the endpoint. Signing key management is on me. |
| **GitHub Releases as OTA source** | Device pulls signed binaries from a public GitHub release. | Zero infra to run. | Public release artifacts leak version cadence and binary contents (someone could reverse-engineer). |
| **No OTA, reflash-only** | Every update requires sending the unit in or a physical flash. | Simple. | Breaks the philosophy's explicit OTA commitment. Non-starter. |

**Leading candidate:** self-hosted HTTPS OTA with code-signed binaries. Plan for it in architecture even if the actual endpoint comes later — the firmware just needs to know how to verify a signed blob.

### 🔒 Decision: **Self-hosted HTTPS OTA on Cloudflare, ECDSA-signed binaries, A/B rollback**

Full security + operations detail in [firmware-security.md](firmware-security.md). Summary:

- **Source:** Cloudflare Worker + R2 at `ota.layeredlogic.cc` (infra already set up). Version-aware endpoint — returns the correct binary per variant (Pro / Pro+) and hardware target (C3 / C6 / future PCBs).
- **Signing:** ECDSA P-256 signed binaries. Private key stored offline in password manager, never on any server. Public key compiled into bootloader image. Signed app verification (`CONFIG_SECURE_BOOT_V2=y` via signed-app path, not full Secure Boot — no eFuse burn).
- **Partition scheme:** A/B OTA with `ota_0` + `ota_1`. Factory partition kept on flash-constrained Pro (C3, 4MB), skipped on Pro+ (C6, 8MB) to give Matter the room.
- **Rollback protection:** `CONFIG_BOOTLOADER_APP_ROLLBACK_ENABLE` (auto-revert if new app doesn't call `esp_ota_mark_app_valid_cancel_rollback()` within ~60s of boot) + `CONFIG_BOOTLOADER_APP_ANTI_ROLLBACK` (monotonic version counter in eFuse, refuses downgrades).
- **Staged rollout:** server-side. Each device gets a bucket (0–99) based on device-ID hash. New firmware ships with a rollout percentage. 1% → 10% → 50% → 100% over ~72 hours.
- **Telemetry:** **opt-in only.** Opt-in cohort forms the de-facto beta — they see new builds first. Opt-outs only get builds after opt-in data proves stability. Users who opt in know what, why, and how via in-app disclosure.
- **Check trigger:** poll on boot after Wi-Fi connect + every 24h at a device-specific time (hash-based offset + ±30min jitter per check) to avoid fleet-wide synchronized check traffic or consistent overlap with user network routines.
- **User control:** update visible in app with changelog. User can trigger "update now" or let scheduled auto-apply run.
- **Matter OTA on Pro+:** **not used in V1.** Single OTA path for both variants via self-hosted endpoint. Matter's native OTA Provider cluster revisited when Pro+ certifies (V2).

### 5.5 App ↔ Device Protocol

What does the app speak to the firmware?

| Layer | Options |
|---|---|
| **Local control (pairing, Wi-Fi setup, direct commands)** | BLE GATT (always available, no-Wi-Fi-required). |
| **Online control (app on mobile network, device on home Wi-Fi)** | HTTP REST (simple, debuggable) or WebSocket (real-time for live color-picker / live pattern preview). |
| **Message formats** | JSON (debuggable, slight overhead) or CBOR / Protobuf (compact, slight pain to work with). |
| **What we're avoiding** | MQTT + broker, because that drags Home-Assistant-shaped assumptions back into the architecture. |

**Leading candidate:** BLE GATT for setup + local fallback, plus WebSocket-over-Wi-Fi for primary runtime. JSON message format until message volume proves it's a problem.

### 🔒 Decision: **BLE + WebSocket + HTTP REST, JSON format, mDNS discovery, user-selectable auth**

Full protocol in [control-protocol-spec.md](control-protocol-spec.md). Summary:

- **Transports:**
  - **BLE GATT** — used for first-run provisioning (via `wifi_prov_mgr`) and as a fallback direct-control channel when Wi-Fi is unavailable or the device hasn't been provisioned yet. Only advertises when user explicitly opens pairing mode.
  - **WebSocket over Wi-Fi** — primary runtime channel. Low-latency bidirectional. State broadcasts are published to all connected clients so the LL app, webapp, and any additional phones stay in sync automatically.
  - **HTTP REST** — for one-shot operations that don't need a persistent connection: `GET /state`, `POST /pattern`, `GET /info`. Any HTTP client (curl, Shortcuts, a webapp fetch) can drive the mirror without a WebSocket dependency.
- **All three surfaces share the same state bus on-device.** LL app, webapp, physical button, Matter (on Pro+) all write to the same bus; no controller has privileged state. Whichever input arrived last wins, and the new state is broadcast to all connected clients.
- **Message format:** JSON. Human-readable, trivial to debug with a browser or curl, tiny code footprint. Per-endpoint CBOR escape hatch reserved for any future binary payloads (e.g., bulk pattern-upload) if JSON proves too fat — not enabled in V1.
- **Discovery:** mDNS on local network (`_layeredlogic._tcp.local.`, hostname `layered-logic-mirror-<id>.local`). Fallback chain: cached last-known IP → mDNS query → BLE scan (if device is in pairing mode or Wi-Fi-down). **No manual IP entry.** IPv4 only for V1; IPv6 deferred.
- **Authentication:** **user-selectable at provisioning time.**
  - **Open mode (default):** any client on the local network can control the device. Design rationale: a mirror in a shared space (living room, dorm common area) should be controllable by anyone in that space. A roommate disturbed by the light should not need credentials to turn it off.
  - **Paired mode (user opt-in):** user sets a shared secret at provisioning. HMAC-signed messages; unsigned messages rejected. Stored in NVS.
  - Mode is changeable after provisioning via any paired controller.
- **Multi-client model:** WebSocket server accepts N concurrent clients. All state changes broadcast to all clients. Idempotent message model (each message has a `req_id`) so clients can ack + dedupe.
- **Remote access (internet-side control):** **deferred to V2.** V1 is local-network only. If/when added, it goes through a Cloudflare Worker relay on `layeredlogic.cc` + long-lived WebSocket from device — not a direct internet-exposed endpoint on the device itself.
- **Webapp hosting:** device hosts the webapp in V1 (static files served by ESP32 HTTP server). Future hosted version at `layeredlogic.cc/controlmydevice` deferred — blocked by the mixed-content HTTPS→WS problem (browser refuses `ws://` from an `https://` origin without per-device TLS certs).

---

## 6. Variant Architecture Summary

Both firmware variants build from one repo. Layout (scaffold lives at `Firmware/v1/` alongside the legacy V0 `Firmware/ESP32/prototype_ESPHome_Version.yaml`):

```
Firmware/v1/
  core/                    # shared — 80%+ of the codebase
    led_driver/            # WS2812 via RMT, LED count from board header
    pattern_interp/        # pattern runtime + interpreter
    button/                # debounce, gesture state machine
    state_bus/             # unified state + broadcast
    provisioning/          # wifi_prov_mgr wrapper (BLE + SoftAP)
    transport/             # WebSocket server, HTTP server, BLE GATT
    mdns/                  # discovery advertising
    ota/                   # signed OTA client, A/B rollback, anti-rollback
    nvs/                   # persistence: creds, patterns, default color, auth
    auth/                  # HMAC-signed envelope (paired mode)
  variants/
    standard/              # Pro — no Matter
      main.c
      CMakeLists.txt
    matter/                # Pro+ — adds esp-matter stack
      main.c
      matter_bridge.c      # Matter clusters → state bus
      CMakeLists.txt
  boards/                  # compile-time board parameterization
    board.h                # abstract interface (LL_PIN_*, LL_HAS_*)
    c6_devkit.h            # ESP32-C6-DevKitC-1 (both demo units, Apr 2026)
    c3_devkit.h            # ESP32-C3-DevKitM-1 (legacy V0 prototype)
    prod_v1_pro.h          # Pro shipping hardware (C3-MINI-1 expected)
    prod_v1_pro_plus.h     # Pro+ shipping hardware (C6 expected)
  tests/
    core/                  # host-side unit tests for pure-C modules
```

CI produces four artifacts per commit: `standard-c6_devkit`, `standard-c3_devkit`, `matter-c6_devkit`, and later the two `prod_v1_*` targets once PCBs exist. OTA server delivers the correct binary based on `(variant, board_id)` tuple reported by device.

---

## 7. Board Parameterization

Hardware differences between dev boards and production PCBs are captured in a single board header, included by `core/` at compile time. No runtime detection — pick the board header at build time via `-D LL_BOARD=c6_devkit`.

### 7.1 Interface (what every board header must define)

**Pin assignments:**
- `LL_PIN_LED_DATA` — WS2812 data line (GPIO number)
- `LL_PIN_BUTTON_PRIMARY` — exposed user button (active-low, internal pullup)
- `LL_PIN_BUTTON_RESET` — recessed pinhole button (active-low, internal pullup)
- `LL_PIN_LED_POWER_EN` — optional LED rail enable (for brown-out / deep-sleep savings). Define as `-1` if unused.

**Radio capability flags (compile-time):**
- `LL_HAS_WIFI` — always 1 on C3/C6; reserved `0` for future non-Wi-Fi variants
- `LL_HAS_BLE` — 1 on C3/C6
- `LL_HAS_802154` — 0 on C3, 1 on C6 (Thread + Zigbee radio). Matter variant requires this for Thread fabric; Wi-Fi-only Matter works with `0`.

**Flash layout:**
- `LL_PARTITION_SCHEME` — `"ab_with_factory"` (4MB, Pro) or `"ab_no_factory"` (8MB, Pro+). Picks the right `partitions.csv`.

**LED driver config:**
- `LL_LED_COUNT_DEFAULT` — count to assume if NVS has no override (e.g., 32 for 6×6, 66 for 12×12 prototype)
- `LL_LED_COLOR_ORDER` — `GRB` for WS2812B

### 7.2 Board headers

| Header | Target | LED pin | Primary btn | Reset btn | 802.15.4 | Flash |
|---|---|---|---|---|---|---|
| `c6_devkit.h` | ESP32-C6-DevKitC-1 | GPIO8 | GPIO9 | GPIO10 (TBD — check silk) | 1 | 8MB, no factory |
| `c3_devkit.h` | ESP32-C3-DevKitM-1 (V0) | GPIO8 | GPIO9 | n/a (dev only, skip reset) | 0 | 4MB, with factory |
| `prod_v1_pro.h` | V1 Pro PCB (C3-MINI-1) | TBD at layout | TBD | TBD | 0 | 4MB, with factory |
| `prod_v1_pro_plus.h` | V1 Pro+ PCB (C6 module) | TBD at layout | TBD | TBD | 1 | 8MB, no factory |

Production board headers committed as stubs now (all pins `TBD`), filled in when PCB layout lands.

### 7.3 What is NOT in the board header

Anything that's a product or brand choice, not a hardware choice:
- Default color — `brand_defaults.h` in `core/`
- Color cycle spectrum — `pattern_interp/`
- Button gesture timings — `button/`
- OTA endpoint — `ota/` (compile-time const)
- Matter VID/PID — `variants/matter/`

Keeps the board header a pure "what is this chip / how is it wired" description.

---

## 8. Feature Parity Baseline (From ESPHome YAML)

The new firmware must at minimum match what the prototype already does. Baseline from [`prototype_ESPHome_Version.yaml`](../../Firmware/ESP32/prototype_ESPHome_Version.yaml):

- [ ] Drive WS2812 on GPIO8, GRB order, 250ms default transition
- [ ] Wi-Fi connectivity with stored credentials
- [ ] OTA firmware update path (self-hosted, not ESPHome)
- [ ] Button input on GPIO9, active-low with pullup
- [ ] Single-click: toggle on/off, default state = Indigo Signal `#3214FF` at 80% brightness
- [ ] Double-click: cycle through effects when on; turn on to default when off
- [ ] At least the 5 prototype effects: Rainbow, Color Wipe, Twinkle, Scanner, Random Twinkle (or better)

Extensions beyond parity (planned):

- [ ] BLE provisioning flow
- [ ] Configurable LED count (driver)
- [ ] Pattern interpreter (generic runtime, not hardcoded effects)
- [ ] App-pushed custom patterns persisted to NVS
- [ ] Signed OTA binaries
- [ ] Factory-reset path (long-press or app-triggered)

---

## 9. Success Criteria For Wed Apr 22 Block

End of Wednesday, we should have:

- [x] A picked language + framework (§5.1 — ESP-IDF with C)
- [x] A picked provisioning UX as primary, with a fallback identified (§5.2 — BLE primary + SoftAP fallback via `wifi_prov_mgr`, user-initiated only)
- [x] A documented posture on smart-home integration that doesn't box us in (§5.3 — two-variant split, Matter only on Pro+, uncertified test VID for V1)
- [x] A picked OTA architecture (§5.4 — self-hosted on Cloudflare, ECDSA-signed, A/B rollback, opt-in telemetry, staged rollout)
- [x] A picked app protocol stack (§5.5 — BLE + WebSocket + HTTP REST, JSON, mDNS discovery, user-selectable auth)
- [x] Confirmed default color hex (`#3214FF` — Indigo Signal; confirmed Apr 21 with palette shift)
- [x] A short working repo scaffold at `Firmware/v1/` — scaffold + blink-level `main.c` per variant + board headers complete

Also produced in the block beyond the original criteria:

- [x] Two-button hardware model spec ([button-interface.md](button-interface.md))
- [x] Product line taxonomy: Basic (STM8) / Pro / Pro+ (§5.3)
- [x] Board parameterization interface (§7)
- [x] Variant architecture layout (§6)

Downstream spec docs spawned from this block:
- [ ] [firmware-spec.md](firmware-spec.md)
- [ ] [app-spec.md](app-spec.md)
- [ ] [webapp-spec.md](webapp-spec.md)
- [ ] [control-protocol-spec.md](control-protocol-spec.md)
- [ ] [firmware-security.md](firmware-security.md)

We do **not** need feature parity with the prototype by end of day. That's the following two weeks' work.

---

## 10. Out Of Scope For This Doc

Captured here so it doesn't drift into downstream specs:

- SKU commitment for Pro+ (whether it becomes shipping or stays demo-only) — Week 5+ customer research
- Final Matter commissioning gesture timing on the recessed button (§5.3 tentatively 6s hold; revisit when Matter variant gets built)
- The companion app UI design — that's [Week 5's wireframing task](../sprint_plan.md#week-5-apr-28--may-2-app-uxui-design)
- Remote-access architecture (Cloudflare Worker relay on `layeredlogic.cc` for V2)
- Hosted webapp at `layeredlogic.cc/controlmydevice` (blocked on mixed-content HTTPS→WS; V2)
- Fleet telemetry system design (opt-in, minimal, transparent) — post-sprint task
- Firmware test harness / CI — deferred until the scaffold is in place
- Reseller permit follow-up — WA BLS UBI pending (~10 biz days from Apr 22)

---

## Related

- [Right-to-Repair Philosophy](right-to-repair-philosophy.md) — §3 and §4 are the source of the closed-binary / self-hosted OTA / reflash-service commitments
- [Brand Positioning](brand-positioning.md) — §3 "Living" adjective drives the pattern-interpreter + user-editable-patterns choice
- [ESP32 Module Shortlist](esp32-module-shortlist.md) — the ESP32-C3-MINI-1 selection this firmware targets
- [ESP32 Module Comparison](esp32-module-comparison.md) — the full comparison, including ESP32-C6 (relevant to §5.3)
- [Market Analysis Report §3.B](Market%20Analysis%20Report.md) — Matter/Thread maturity context that frames §5.3
- [Sprint Plan — Week 4](../sprint_plan.md#week-4-apr-21--apr-25-brand-identity--repairability) — engineering bullet and Wed focus block this doc feeds into
