---
title: Firmware Architecture Scoping
type: engineering
phase: 2
week: 4
date: 2026-04-20
status: draft — pre-Wed architecture block
tags: [engineering, firmware, esp32, architecture, scoping]
---

# Firmware Architecture Scoping — ESP32 Greenfield Build

**Prepared by:** William White
**Date:** April 20, 2026
**Status:** Pre-session working doc — locked decisions + options for the Wed Apr 23 architecture block

---

## 1. Purpose

This document scopes the Week 4 firmware kickoff. It captures what is already decided, lays out the open architecture questions, and stages the Wednesday Apr 23 focus-day decisions so we walk into that block with the terrain mapped rather than from zero.

It is **not** a spec. The spec gets written after the decisions in §5 are made.

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
4. **Brand-palette default state.** The prototype's default-on color `(0.6, 0.0, 1.0)` = `#9900FF` is close to brand but not exact. New default: **Violet Signal `#A020D0`** at 80% brightness (confirm Wed). Every power-on returns the unit to this state.
5. **Closed-binary firmware, self-hosted OTA, reflash-service backstop.** Per right-to-repair philosophy §3 and §4. The firmware source is not public; the hardware (schematic, BOM, versioned PCBs) is. An owner with a dead board sends it to me for reflash; an owner with a bricked firmware pulls an OTA update. I'll revisit source escrow if the business EOLs.

---

## 5. Open Decisions For Wednesday Apr 23

Each decision below is staged with options and tradeoffs. No choice is made here — that's the Wednesday block.

### 5.1 Language + Framework

Options:

| Option | Pros | Cons |
|---|---|---|
| **ESP-IDF (C/C++)** | Native Espressif. Best OTA + BLE + Wi-Fi support. Matches the `type: esp-idf` already in the ESPHome config. Most documented path for ESP32-C3. | C/C++ ergonomics. Steeper learning if unfamiliar. |
| **Arduino / PlatformIO** | Fastest to first blink. Huge ecosystem. Good for prototyping. | Less runtime control. OTA and secure-boot stories are weaker. Abstraction layers can leak under load. |
| **esp-rs (Rust on Espressif)** | Memory safety, modern tooling, fun to work in. Async story is good. | Ecosystem younger than ESP-IDF. More hunting for working examples of LED + BLE + OTA together. Higher risk for a week-4 commitment. |
| **NuttX / Zephyr / other RTOS** | Real OS semantics. | Overkill for this scope, adds scheduling complexity that doesn't buy much on a single-product firmware. |

**My recommendation going in:** ESP-IDF C++. Proven, matches the prototype framework, strongest OTA + BLE + Wi-Fi coverage, best docs. Rust is interesting but the time cost of learning-while-building is real. Arduino is a trap at product scale — great for prototyping, bad for long-term ownership. Willing to be talked out of ESP-IDF if there's a strong reason to go Rust.

### 5.2 Provisioning UX

How does the user get the device onto their Wi-Fi the first time?

| Option | How it works | Pros | Cons |
|---|---|---|---|
| **BLE-to-Wi-Fi handoff via app** | Device advertises over BLE. User opens app, app discovers + pairs, user enters Wi-Fi creds in app, app pushes to device over BLE, device connects. | Clean UX. App-first flow aligns with brand. BLE is always available on ESP32-C3. | Requires the mobile app exists and works. Web fallback is awkward (browser BLE is spotty). |
| **SoftAP captive portal** | Device boots as its own Wi-Fi access point. User connects phone to "LayeredLogic-XXXX" network, browser redirects to a config page, user enters their home Wi-Fi creds. | Works with no app. Browser-only. | Breaks the "stay connected to your Wi-Fi" model. Some phones warn about "no internet on this network." |
| **Direct Wi-Fi + hardcoded creds** | User never configures Wi-Fi; device connects to something pre-set (dev-only). | Trivial for testing. | Obviously not shippable. |
| **Zigbee / Thread broadcast** | If ESP32-C6: device advertises as a Matter/Thread device; any Matter controller adopts it. | Zero-config for Matter users. | Requires ESP32-C6, requires the buyer has a Matter hub, doesn't help non-smart-home buyers. See §5.3. |

**Leading candidate:** BLE-to-Wi-Fi handoff via app, with SoftAP captive portal as a fallback for browser-only users and for when the app isn't installed. Both are already common patterns in commercial smart devices.

### 5.3 Smart-Home Integration Path

Not being solved Wednesday — but flagged now so the framework choice doesn't box us out.

| Path | Module | Integration cost | Reach |
|---|---|---|---|
| **ESP32-C6 + Matter/Thread** | Swap module | Low — Matter gets us HA, Google Home, Apple Home, Alexa "for free" via the standard protocols. | Broad. The Matter promise. |
| **ESP32-C3 + individual integrations** | Stay on current module | High — write a HA integration, a Google Home action, an Apple HomeKit accessory, an Alexa skill, each separately. | Same endpoints, far more work. |

**Posture:** Decision deferred to customer research per index/open-questions. Wednesday's firmware architecture should not hardcode assumptions that would break a future C3→C6 swap. Specifically: keep the LED-driving + pattern-interpreter + provisioning layers **module-agnostic** so moving from C3 to C6 is a pin-mapping + radio-config change, not a rewrite.

### 5.4 OTA Update Channel

Options:

| Option | How it works | Pros | Cons |
|---|---|---|---|
| **Self-hosted HTTPS OTA, signed binaries** | Device polls a known endpoint (layeredlogic.com or similar) for updates. Binaries signed with a key on my side, verified on-device. | Full control. Meets the philosophy's "I own the firmware stack" commitment. Standard ESP-IDF path. | I run the endpoint. Signing key management is on me. |
| **GitHub Releases as OTA source** | Device pulls signed binaries from a public GitHub release. | Zero infra to run. | Public release artifacts leak version cadence and binary contents (someone could reverse-engineer). |
| **No OTA, reflash-only** | Every update requires sending the unit in or a physical flash. | Simple. | Breaks the philosophy's explicit OTA commitment. Non-starter. |

**Leading candidate:** self-hosted HTTPS OTA with code-signed binaries. Plan for it in architecture even if the actual endpoint comes later — the firmware just needs to know how to verify a signed blob.

### 5.5 App ↔ Device Protocol

What does the app speak to the firmware?

| Layer | Options |
|---|---|
| **Local control (pairing, Wi-Fi setup, direct commands)** | BLE GATT (always available, no-Wi-Fi-required). |
| **Online control (app on mobile network, device on home Wi-Fi)** | HTTP REST (simple, debuggable) or WebSocket (real-time for live color-picker / live pattern preview). |
| **Message formats** | JSON (debuggable, slight overhead) or CBOR / Protobuf (compact, slight pain to work with). |
| **What we're avoiding** | MQTT + broker, because that drags Home-Assistant-shaped assumptions back into the architecture. |

**Leading candidate:** BLE GATT for setup + local fallback, plus WebSocket-over-Wi-Fi for primary runtime. JSON message format until message volume proves it's a problem.

---

## 6. Feature Parity Checklist (From ESPHome YAML)

The new firmware must at minimum match what the prototype already does. Baseline from [`prototype_ESPHome_Version.yaml`](../../Firmware/ESP32/prototype_ESPHome_Version.yaml):

- [ ] Drive WS2812 on GPIO8, GRB order, 250ms default transition
- [ ] Wi-Fi connectivity with stored credentials
- [ ] OTA firmware update path (self-hosted, not ESPHome)
- [ ] Button input on GPIO9, active-low with pullup
- [ ] Single-click: toggle on/off, default state = brand violet at 80% brightness
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

## 7. Success Criteria For Wed Apr 23 Block

End of Wednesday, we should have:

- [ ] A picked language + framework (§5.1)
- [ ] A picked provisioning UX as primary, with a fallback identified (§5.2)
- [ ] A documented posture on smart-home integration that doesn't box us in (§5.3)
- [ ] A picked OTA architecture (§5.4)
- [ ] A picked app protocol stack (§5.5)
- [ ] Confirmed default color hex (`#A020D0` or other)
- [ ] A short working repo scaffold at `Firmware/ESP32/` for the new firmware (`main.c`/`main.cpp`/`src/main.rs`, whatever §5.1 lands on) — first commit is "scaffold + blink," not working features

We do **not** need feature parity with the prototype by end of day. That's the following two weeks' work.

---

## 8. Out Of Scope For This Doc

Captured here so it doesn't drift into the architecture block:

- The Matter/Thread SKU decision (C3 vs. C6) — user research in Weeks 4–5 informs this
- The companion app UI design — that's [Week 5's wireframing task](../sprint_plan.md#week-5-apr-28--may-2-app-uxui-design)
- Webapp build stack — separate scoping, follow-on task
- Firmware test harness / CI — deferred until the scaffold is in place

---

## Related

- [Right-to-Repair Philosophy](right-to-repair-philosophy.md) — §3 and §4 are the source of the closed-binary / self-hosted OTA / reflash-service commitments
- [Brand Positioning](brand-positioning.md) — §3 "Living" adjective drives the pattern-interpreter + user-editable-patterns choice
- [ESP32 Module Shortlist](esp32-module-shortlist.md) — the ESP32-C3-MINI-1 selection this firmware targets
- [ESP32 Module Comparison](esp32-module-comparison.md) — the full comparison, including ESP32-C6 (relevant to §5.3)
- [Market Analysis Report §3.B](Market%20Analysis%20Report.md) — Matter/Thread maturity context that frames §5.3
- [Sprint Plan — Week 4](../sprint_plan.md#week-4-apr-21--apr-25-brand-identity--repairability) — engineering bullet and Wed focus block this doc feeds into
