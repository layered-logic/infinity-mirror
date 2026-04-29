---
title: "Week 5 App Brainstorm Guide"
type: planning
phase: 2
week: 5
date: 2026-04-27
status: active
tags: [app, webapp, wireframe, brainstorm, planning]
---

# Week 5 App Brainstorm Guide

**Date:** April 27, 2026
**Context:** Week 5 (Apr 28 – May 2) is "App UX/UI Design." The academic deliverable is wireframes in Figma + design rationale for the portfolio. But there are also real engineering decisions that determine what gets *built* — and the 4-path decision from Apr 22 is still open.

This doc collects everything that's decided, everything that's open, and frames the choices to make this week.

---

## 1. What Already Exists

### Locked decisions (from Apr 22 architecture block)

These are done — don't re-litigate:

- **Two control surfaces:** mobile app + device-hosted webapp. Same wire protocol, same capabilities.
- **Wire protocol:** WebSocket (primary, bidirectional) + HTTP REST (one-shot) + BLE GATT (provisioning/fallback). JSON envelope with `op`, `req_id`, `ts`, `payload`, optional `hmac`. Fully specced in [control-protocol-spec.md](control-protocol-spec.md).
- **Auth model:** User-selected at provisioning — open (anyone on Wi-Fi) or paired (HMAC-signed with shared secret). Default is open.
- **Discovery:** mDNS `_layeredlogic._tcp.local.` → cached IP → BLE fallback. No manual IP entry.
- **Local-first, no cloud, no accounts.** App stores device list + patterns locally. No login.
- **Webapp is device-hosted in V1.** ESP32 serves static files at `http://<mirror>.local/`. Hosted variant at `layeredlogic.cc/controlmydevice` deferred (mixed-content blocker).
- **Pattern model:** V1 = 7 built-in patterns only (firmware-side). V2 = user-uploaded declarative patterns via the app.
- **OTA:** Device pulls its own update from `ota.layeredlogic.cc`. App just triggers "update now."

### Specs already drafted

All first-draft, all flagging Week 5 as the decision point:

| Doc | What it covers |
|---|---|
| [app-spec.md](app-spec.md) | Mobile app — screens, flows, data model, multi-client behavior |
| [webapp-spec.md](webapp-spec.md) | Device-hosted webapp — bundle constraints (≤80KB gzip), stack, V2 hosted plan |
| [control-protocol-spec.md](control-protocol-spec.md) | Wire protocol — ops, envelope, transports, error codes |
| [firmware-security.md](firmware-security.md) | Auth modes, HMAC, telemetry opt-in |

### Firmware modules that exist

| Module | Status | What it does |
|---|---|---|
| `state_bus/` | Built, not compiled | Central state (on, brightness, color, pattern, led_count) + event broadcast |
| `button/` | Built, not compiled | Debounced GPIO + gesture state machines |
| `led_driver/` | Built, not compiled | RMT-backed WS2812B driver |
| `pattern_interp/` | Built, not compiled | 7 pattern renderers at 60Hz |
| `provisioning/` | Built, not compiled | `wifi_prov_mgr` BLE + SoftAP |
| `nvs/` | Built, not compiled | State persistence across reboots |
| **`transport/`** | **Does not exist** | **WebSocket + HTTP + HMAC auth — needed before any app can talk to the device** |

---

## 2. The Parked Decision (Apr 22)

Four paths were presented. The decision gates all engineering work:

| Path | What it is | Time | What it unblocks |
|---|---|---|---|
| **A** | Build `core/transport/` (WebSocket + HTTP server + mDNS) | ~2 sessions | Makes the device controllable from any client — app, webapp, curl, browser |
| **B** | Build the `webapp/` (Preact + Vite, device-hosted) | 2–3 sessions | Captive-portal setup + control UI. Doubles as the SoftAP landing page |
| **C** | Build native mobile app (pick Kotlin/Flutter/RN) | 3+ sessions | Full app with provisioning + control. No control until transport lands |
| **D** | Step off engineering entirely | 0 sessions | Focus on Week 5 graded HCDE work (wireframes, interviews, rationale) |

**Dependencies:**
- B requires A (webapp talks over WebSocket — needs transport first)
- C requires A for control features (provisioning works via BLE without transport, but "control mirror from app" needs WebSocket)
- D has no dependencies

**Possible combos:**
- **A then B** — transport + webapp in the same week. Webapp becomes the captive portal *and* the control surface. Mobile app is "nice to have" later.
- **A then C** — transport + start mobile app. More ambitious; mobile app is harder than webapp.
- **D with wireframes** — pure design week. Wireframes in Figma, design rationale, interviews. No code. Strongest HCDE portfolio value.
- **A + D hybrid** — transport is a focused ~2-session build. Do that first, then spend the rest of the week on wireframes. Both tracks move forward.

---

## 3. The Stack Decision (Mobile App)

Only matters if you pick a path that includes the mobile app this week. Otherwise, decide later.

| Option | Pros | Cons | BLE/ESP Provisioning |
|---|---|---|---|
| **React Native + Expo** | One codebase, fast iteration, JS/TS | ESPProvision SDK needs custom native module; BLE library (`react-native-ble-plx`) is mature but requires ejecting from Expo Go | Possible but friction |
| **Flutter** | One codebase, beautiful UI, `flutter_blue_plus` for BLE | Dart learning curve, no official ESPProvision SDK wrapper (community packages exist) | Possible, community-maintained |
| **Native (Swift + Kotlin)** | First-class ESPProvision SDK from Espressif, best BLE behavior | Two codebases. Solo founder maintaining both. | Best — Espressif ships official SDKs |
| **Web app only (no native)** | Zero install friction, works on any phone, doubles as captive portal | No BLE (Web Bluetooth fragmented), lower UX ceiling, no push notifications ever | No BLE — provisioning must be SoftAP-only or paired via separate flow |

**The "web app only" path** is worth seriously considering. For a set-up-once ambient device:
- Users interact with the app rarely (set a color, maybe edit a pattern once)
- The webapp is already needed for the SoftAP captive portal
- No app store review, no app store fees, no two-platform maintenance
- Pattern editor is probably better on a bigger screen anyway

The tradeoff: no BLE provisioning from the webapp. SoftAP captive portal is the only provisioning path. That's fine — it's already specced as the fallback in the firmware architecture.

---

## 4. The Wireframe Scope

The graded Week 5 deliverable is **wireframes in Figma** with **design rationale** for the portfolio. This is required regardless of what gets built.

### Screens to wireframe (from app-spec §6)

| Screen | Priority | Notes |
|---|---|---|
| **Onboarding / first launch** | High | Sets the tone. "Add a mirror" CTA → pairing flow |
| **Pairing flow** | High | Recessed button instruction → BLE/SoftAP scan → Wi-Fi entry → auth choice → done |
| **Home (mirror list)** | High | Known mirrors + connection status |
| **Mirror detail / live control** | High | Color picker, brightness slider, pattern selector — the core daily UI |
| **Pattern library** | Medium | Grid of built-in + custom patterns |
| **Pattern editor** | Medium | The "killer feature" — creating custom patterns. Could be phone or desktop or both |
| **Device settings** | Low | Rename, auth mode, telemetry, factory reset |
| **Firmware update** | Low | Changelog + update button |

### Design questions the wireframes should answer

These are the questions the wireframes should answer for the portfolio's "design rationale" section:

1. **Why an app at all?** The mirror works perfectly from two physical buttons. What does the app add that justifies its existence? (Answer: pattern creation/editing, the "Living" brand adjective, multi-color access beyond the 17-step button cycle.)

2. **Phone vs. desktop for pattern editing?** The pattern editor is the highest-value screen. Is it better on a phone (always available, can sit next to the mirror) or desktop (more screen real estate, precise mouse input)? Or both with responsive layout?

3. **How does the color picker work?** The button gives you 17 colors. The app should give you the full spectrum. But: full HSV wheel? Curated palette? Both? How do you handle the mismatch between "app picks any color" and "button cycles 17 steps"?

4. **What does "pattern editor" even look like?** V1 patterns are built-in. V2 patterns are user-uploaded. What's the authoring UX? Timeline-based? Keyframe? Node graph? Drag-and-drop LED positions? This is the biggest open design question.

5. **Accessibility of the color picker.** How do users with color-vision deficiency pick colors? Named colors? Numeric input? Preview on the actual mirror?

6. **What happens when the mirror is unreachable?** Offline state design — cached last-known state? Greyed out controls? Retry button?

7. **Multi-mirror household.** How do you switch between mirrors? Tabs? List? Swipe?

---

## 5. The User Interview Angle

Week 5 also calls for continuing user interviews. The wireframes should be informed by what users actually want. Questions to weave in:

- "If you could change one thing about the light from your phone, what would it be?" (validates whether pattern editing is the right investment)
- "Would you install an app for a lamp?" (validates whether webapp-only is viable)
- "How often would you change the color/pattern?" (informs whether the app is a daily tool or a setup-once tool)
- "Do you share your home Wi-Fi with roommates/guests? Would you want them to control the lamp too?" (validates the open-vs-paired auth decision)
- Show the wireframes mid-interview and get reactions.

---

## 6. Suggested Plan for the Week

This is one way to sequence it — not prescriptive:

### Monday (Apr 28)
- **Decide the path.** Pick from the A/B/C/D options (or a combo). This unblocks everything else.
- **Start wireframes.** Even if you're building code, the wireframes are the graded artifact. Start in Figma with the highest-priority screens (onboarding, pairing, live control).

### Tuesday–Wednesday (Apr 29–30)
- **Engineering** (if doing A or A+B): Build `transport/` — WebSocket server, HTTP REST endpoints, mDNS registration. If A+B: start the webapp scaffold.
- **Wireframes:** Fill out the pattern library and pattern editor screens. These are the hardest design problems.
- **Interviews:** Start conversations (even informal ones — coffee shop, friends, classmates).

### Thursday–Friday (May 1–2)
- **Refine wireframes** based on interview feedback.
- **Write the design rationale** — the portfolio artifact explaining *why* each screen looks the way it does.
- **Engineering:** If building the webapp, get it serving from the ESP32 HTTP server and talking over WebSocket.

---

## 7. What This Week Does NOT Need to Solve

- Pattern definition grammar / JSON format (deferred to V2 per protocol spec §6.2)
- App store submission (way down the road)
- Hosted webapp at `layeredlogic.cc` (V2, blocked by mixed-content)
- Smart home integration in the app (runs server-side, not in the app)
- Remote access / control from outside the home (V2)
- Final stack decision if you're wireframing only — pick the stack when you start coding

---

## Related

- [App Spec](app-spec.md) — mobile app scope + open questions
- [Webapp Spec](webapp-spec.md) — device-hosted webapp scope + V2 plan
- [Control Protocol Spec](control-protocol-spec.md) — wire protocol both apps use
- [Firmware Security Model](firmware-security.md) — auth + telemetry UX the apps surface
- [Button Interface Design](button-interface.md) — the physical UI the app extends
- [Brand Positioning](brand-positioning.md) — voice + palette the wireframes inherit
- [Sprint Plan — Week 5](../sprint_plan.md#week-5-apr-28--may-2-app-uxui-design)
