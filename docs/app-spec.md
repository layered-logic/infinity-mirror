---
title: App Spec — Layered Logic Mobile App
type: engineering
phase: 2
week: 4
date: 2026-04-22
status: draft — initial spec drafted alongside firmware/webapp for gap detection
tags: [engineering, app, mobile, ios, android, ux]
---

# App Spec — Layered Logic Mobile App

**Prepared by:** William White
**Date:** April 22, 2026
**Status:** First draft — scopes what the LL mobile app is responsible for. UI wireframing is Week 5 (distinct task).

---

## 1. Purpose

The LL app is the **primary user interface** for the Layered Logic infinity mirror. It is where users:

- Pair a new mirror with their Wi-Fi
- Create and edit custom patterns
- Adjust color / brightness / on-off in real-time
- Manage multiple mirrors in one household
- Approve and monitor firmware updates
- Choose their security + privacy posture (open vs paired auth, telemetry opt-in)

It is **not**:
- Required for basic operation (see [firmware-architecture-scoping §4.1](firmware-architecture-scoping.md#4-design-principles-locked-apr-20) "standalone out-of-box")
- An account or social layer — no login, no cloud profile
- A remote-access tool in V1 — controls the mirror only when app + mirror are on the same network

Pairs with:
- [Firmware Spec](firmware-spec.md) — the device side
- [Control Protocol Spec](control-protocol-spec.md) — the wire protocol
- [Firmware Security Model](firmware-security.md) — auth modes the app surfaces to users
- [Webapp Spec](webapp-spec.md) — the alternate control surface

---

## 2. Principles

1. **Local-first.** No account. No cloud. App stores everything it needs on-device. Multiple phones with the LL app can each independently connect to the same mirror; state stays in sync via WebSocket broadcasts from the device.
2. **Fast to first light.** Unboxing → mirror lit with a user-selected color should take under 2 minutes. Provisioning screens prioritize speed over exhaustive options.
3. **The mirror is the authority.** App reads state from the device on connect. Never assumes local cache is correct after a round-trip gap. State broadcasts from the device overwrite app-local view.
4. **User-chosen security.** Expose the open-vs-paired decision clearly at provisioning; do not default to paired (see [firmware-security §5.1](firmware-security.md#51-two-modes-user-selected)).
5. **Transparency over clever.** Telemetry opt-in screen shows the exact fields collected, not a generic "help improve our product" toggle. Firmware updates show the changelog and the OTA check schedule. The user should always be able to know what their device is doing.

---

## 3. Platform + Stack

**Targets:** iOS 16+ (SE 3rd gen and newer), Android 12+.

**Stack decision: open question (to be resolved Week 5 wireframing block).**

| Option | Pros | Cons |
|---|---|---|
| **React Native + Expo** | Fast iteration, one codebase, good BLE libraries (`react-native-ble-plx`), Expo dev build handles native builds | BLE + ESPProvision SDK integration requires ejecting or custom modules |
| **Flutter** | Beautiful UI out of the box, strong BLE support (`flutter_blue_plus`), fast hot reload | Ecosystem smaller for ESP provisioning specifically |
| **Native (Swift + Kotlin)** | First-class ESPProvision SDK support (Espressif ships both), tightest BLE behavior | Two codebases to maintain solo |

**Leaning:** React Native + Expo for dev velocity, native if the ESPProvision SDK integration proves painful. Decision logged here; the architecture in this spec is stack-agnostic.

---

## 4. Architecture

```
┌───────────────────────────────────────────────┐
│                  UI Layer                     │
│  screens (pair, home, pattern editor, device  │
│  manager, settings, update, telemetry)        │
└──────────────────┬────────────────────────────┘
                   │
                   ▼
┌───────────────────────────────────────────────┐
│               State Layer                     │
│  - device list (local storage)                │
│  - active device connection state             │
│  - pattern library (local)                    │
└──────────┬────────────────────┬───────────────┘
           │                    │
           ▼                    ▼
┌────────────────────┐ ┌──────────────────────┐
│  Transport Layer   │ │  Local Storage       │
│  - BLE (provision) │ │  - known devices     │
│  - WebSocket       │ │  - user patterns     │
│  - HTTP REST       │ │  - prefs (telemetry, │
│  - mDNS discovery  │ │     theme, etc.)     │
└────────────────────┘ └──────────────────────┘
```

---

## 5. Key User Flows

### 5.1 First-run: pair a new mirror

1. User opens app. App shows "Add a mirror" CTA.
2. User taps it. App shows instruction: "Use a paperclip to press the recessed button on the back of your mirror for 3 seconds. The mirror will pulse indigo when it's ready."
3. User follows instruction; mirror enters pairing mode.
4. App starts BLE scan + SoftAP probe in parallel.
5. App finds mirror → shows "Found 'Layered Logic Mirror (abc123)'".
6. User taps to select. App shows Wi-Fi network list (scanned via ESPProvision).
7. User picks their Wi-Fi, enters password.
8. App sends creds via BLE to mirror. Mirror connects; app sees mirror on Wi-Fi via mDNS.
9. App prompts: **"How should your mirror be controlled?"**
   - **Open (default, recommended):** Anyone on your Wi-Fi can control this mirror
   - **Password-protected:** Only people with the password can control it
10. User picks mode. If paired: app prompts for password.
11. App shows telemetry opt-in screen — see §5.6.
12. Pairing complete. User drops to home screen with the mirror live.

### 5.2 Daily use: open app, control mirror

1. App opens. Shows list of known mirrors.
2. App attempts reconnect in priority order:
   - Cached IP → try WebSocket upgrade
   - If fail: mDNS query on local network
   - If fail: show "can't find mirror" with troubleshooting link
3. On connect: device broadcasts full state → app renders home screen.
4. User taps color → app sends `set_state { base_color: ... }` over WebSocket → mirror changes → device broadcasts back → all connected clients refresh.

### 5.3 Create a custom pattern

Full pattern editor UX is Week 5 wireframing; protocol surface is:

1. User builds pattern in editor (client-side only).
2. User taps "Save to mirror".
3. App sends `upload_pattern { definition }` over WebSocket.
4. Mirror stores to NVS, adds to pattern list, confirms.
5. User can now trigger the pattern via `set_state { pattern_id }`.

### 5.4 Firmware update

1. App polls device state periodically; `get_update_status` returns whether an update is available.
2. If available: banner appears in app with version + changelog.
3. User taps "Update now" → app sends `apply_update` → mirror reboots → app shows "Updating..." spinner with timeout.
4. Mirror comes back on new version → app reconnects → banner disappears.

App does not download firmware itself. App only triggers the device to pull its own update from `ota.layeredlogic.cc`.

### 5.5 Switch auth mode post-provisioning

1. User goes to device settings.
2. Taps "Who can control this mirror?"
3. Picks new mode. If switching to paired: enter password.
4. App sends `set_auth_mode { mode, secret }`.
5. Device confirms. All currently-connected clients must re-auth with the new secret (or disconnect if open→paired).

### 5.6 Telemetry opt-in

Separate screen (not buried in settings). Shows:

- **Bold question:** "Help us make Layered Logic more reliable?"
- **What we collect:** list of the exact fields (uptime, free heap, crash info with filename/line, Wi-Fi signal strength, firmware version, device ID, board type) — lifted straight from [firmware-security §4.2](firmware-security.md#42-what-telemetry-collects-when-opted-in).
- **What we don't collect:** list including pattern content, colors, schedule, Wi-Fi SSID, IP addresses.
- **How it's used:** "Opt-in devices get new firmware first. Opt-out devices wait for the build to be proven stable."
- **Toggle:** Enable / disable (default **disabled**).
- **Footer:** "You can view any telemetry payload before it's sent, in Settings → Advanced."

---

## 6. Screens (V1)

Roughly — final count + names settle in Week 5 wireframing.

| Screen | Purpose |
|---|---|
| Onboarding | First-launch welcome; handoff to pair flow |
| Pair device | BLE scan + Wi-Fi handoff flow |
| Home | List of known mirrors + connection status |
| Mirror detail (live) | Current pattern, color, brightness; picker for all three |
| Pattern library | List of built-in + custom patterns |
| Pattern editor | Create / edit custom patterns (Week 5 design) |
| Device settings | Rename, auth mode, telemetry, factory-reset warning |
| Update | Changelog + update-now button |
| Advanced | Peek pending telemetry, view connection diagnostics |
| About | Version, license notices, support link |

---

## 7. Local Data Model

Nothing in the cloud. Everything on-device.

| Entity | Fields | Storage |
|---|---|---|
| KnownDevice | `device_id`, `name`, `last_ip`, `auth_mode`, `secret_ref`, `variant` | Encrypted app storage (iOS Keychain, Android Keystore for secret) |
| Pattern | `id`, `name`, `definition`, `source` (built-in/user/from-device) | App storage (plain) |
| Prefs | theme, default brightness step, last-used pattern, telemetry reminder state | App storage (plain) |

---

## 8. Multi-Client Behavior

- Multiple phones running LL app can all be connected to the same mirror simultaneously.
- Each phone maintains its own WebSocket.
- State changes from any phone are broadcast back to all phones (and to the webapp if open).
- **No ownership model.** No "primary" phone. Anyone with network access (and optional shared secret) is equal.
- Follows from [firmware-security §5.1](firmware-security.md#51-two-modes-user-selected) user-agency framing.

---

## 9. Error States

| State | UX |
|---|---|
| Can't find mirror (all transports) | "Check that mirror is powered on and on your Wi-Fi. If issue persists, try holding the reset button for 3 seconds to re-pair." |
| WebSocket disconnected mid-session | Silent auto-reconnect with 1s→30s backoff. Badge shows "reconnecting..." |
| Paired mode, wrong secret | "Incorrect password." — no clue about what's correct. |
| Update failed / timed out | "Update didn't complete. Your mirror rolled back to the previous version. Try again later or check for network issues." |
| Mirror unreachable after auth mode change | Surface "Your phone is no longer authorized to control this mirror" + CTA to re-enter password. |

---

## 10. Accessibility

- Color picker must work for users with color-vision deficiency (not yet designed; flag for Week 5).
- Screen reader support at all screens.
- Minimum tap target 44×44 pt.
- Respect system font scaling.

---

## 11. Out Of Scope (V1)

- User accounts / cloud profile
- Remote access (control from outside the home)
- Home-automation integration in-app (HA integration runs on the HA server-side; Matter runs on Pro+ firmware independently)
- Widget / Siri / Google Assistant shortcuts (V2)
- Apple Watch / Wear OS app
- Group control (one action across multiple mirrors) — V2

---

## 12. Open Questions

| Question | Answer by |
|---|---|
| React Native vs Flutter vs native | Week 5 wireframing block |
| App-internal theming (dark mode, brand surface) | Week 5 — driven by brand cascade |
| How much of pattern editor fits on phone screen vs only in webapp | Week 5 wireframing |
| App onboarding for users who opened the app before buying a mirror | Week 5 — low priority |

---

## Related

- [Firmware Architecture Scoping](firmware-architecture-scoping.md) — design constraints this app obeys
- [Firmware Spec](firmware-spec.md) — device behavior this app consumes
- [Control Protocol Spec](control-protocol-spec.md) — wire protocol
- [Firmware Security Model](firmware-security.md) — auth + telemetry posture surfaced to users
- [Webapp Spec](webapp-spec.md) — alternate control surface (same protocol)
- [Button Interface Design](button-interface.md) — physical UX the app mirrors + co-exists with
