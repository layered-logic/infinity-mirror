---
title: Webapp Spec — Layered Logic Mirror Control Surface
type: engineering
phase: 2
week: 4
date: 2026-04-22
status: draft — initial spec alongside firmware/app for gap detection
tags: [engineering, webapp, cloudflare, device-hosted, control]
---

# Webapp Spec — Layered Logic Mirror Control Surface

**Prepared by:** William White
**Date:** April 22, 2026
**Status:** First draft — captures V1 device-hosted scope + V2 hosted deferral

---

## 1. Purpose

The webapp is the **"any device, no install" fallback** for controlling the mirror. Whatever the LL mobile app can do, a browser should be able to do too — for users who:

- Don't want to install an app
- Are using a desktop / laptop (where no native LL app exists)
- Are on a guest's phone and just want to turn the mirror off
- Prefer keyboards + mice for pattern editing (likely a real use case — pattern design on a phone screen is cramped)

V1 webapp is **device-hosted**: the ESP32 serves the webapp's static files itself, reachable at `http://layered-logic-mirror-<id>.local/`. V2 plan is a **hosted variant** at `layeredlogic.cc/controlmydevice` — deferred because of a specific browser security constraint (§5).

Pairs with:
- [Firmware Spec](firmware-spec.md) — the device side
- [Control Protocol Spec](control-protocol-spec.md) — wire protocol shared with the mobile app
- [App Spec](app-spec.md) — the primary client; webapp should match its capabilities

---

## 2. Principles

1. **Same protocol, same features.** The webapp uses the same WebSocket + HTTP + JSON envelope as the mobile app. Anything the app can do, the webapp can do, unless an underlying web platform limit blocks it (e.g., BLE in Safari).
2. **Zero install, zero account.** Open the URL, control the mirror. No login, no signup.
3. **Device-hosted in V1.** Webapp files served by the ESP32's HTTP server. Small HTML + CSS + a single JS bundle. No build-time server dependency — a browser on the same LAN reaches the device's IP directly.
4. **Keyboard-friendly.** Desktop is a first-class target for the webapp. Keyboard shortcuts for common actions (space = on/off, arrow keys = next pattern, number keys = brightness step).
5. **Future hosted variant stays open.** Code is written so that swapping `fetch(device_ip + '/api/...')` for `fetch('https://layeredlogic.cc/relay/...')` doesn't require rewriting the client.

---

## 3. V1 — Device-Hosted

### 3.1 Where it lives

Files bundled into the firmware image (compressed) and mounted into the ESP32 HTTP server's static file handler at the root path.

**URL:** `http://layered-logic-mirror-<id>.local/` (via mDNS) or `http://<mirror_lan_ip>/` (direct).

### 3.2 Bundle constraints

- Must fit in firmware flash budget: target **≤ 80KB gzipped**
- No dependencies that require a build pipeline hosted elsewhere
- Single-page app; routing via hash (`#/`, `#/patterns`, `#/settings`) to avoid server-side routing
- No web fonts fetched from a CDN (device is local-network, no guaranteed internet)
- No analytics, no telemetry inside the webapp (diagnostics come from firmware telemetry module)

### 3.3 Stack

**Recommendation:** vanilla TypeScript + Preact + Vite. Small bundle, familiar, easy to inline. Avoids React (too big) and Svelte (would love, but slightly more friction embedding into the firmware build).

Flagged as open: may re-evaluate once bundle size is actually measured.

### 3.4 What it does

Same operations exposed via [control-protocol-spec §4](control-protocol-spec.md#4-operations):
- View + change on/off, pattern, base color, brightness
- Browse pattern library, trigger patterns
- Create / edit / delete custom patterns (same editor or a simplified version vs mobile app)
- View device info (firmware version, variant, uptime)
- Toggle telemetry opt-in, view pending beacon
- Toggle auth mode, set shared secret
- Trigger "check for update" + view changelog + apply update

### 3.5 What it doesn't do

- **No BLE.** Webapp has no provisioning flow — it can't run until the device is already on Wi-Fi. Pairing is mobile-app-only in V1 (or factory-reset + re-provision via mobile app).
- **No persistent client-side storage for secrets.** The auth-mode shared secret is entered per session (localStorage caching it is too risky on shared computers). User pastes it each time in paired mode, or enables the browser's password manager.
- **No remote access.** Browser on mobile data cannot reach `http://layered-logic-mirror-<id>.local/`.

---

## 4. UX

Final wireframes align with mobile app (Week 5 task). Baseline:

- Home: big color picker + brightness slider + pattern dropdown
- Pattern library: grid of patterns, click to run, edit icon per pattern
- Pattern editor: larger-canvas version of whatever the mobile app's editor shape lands on
- Settings: device info, auth mode, telemetry
- Update: changelog + "update now"

Keyboard shortcuts:
- `Space` — on/off
- `→` / `←` — next / previous pattern
- `↑` / `↓` — brightness up / down (snaps to 25/50/75/100)
- `1–9` — quick pattern slots

---

## 5. V2 — Hosted Variant at `layeredlogic.cc/controlmydevice`

**Deferred.** Documenting the plan so V1 doesn't foreclose it.

### 5.1 Goal

User types `layeredlogic.cc/controlmydevice` into any browser → reaches their mirror without needing the mDNS hostname or knowing its IP.

### 5.2 Why deferred: the mixed-content problem

- `layeredlogic.cc` is served over HTTPS (Cloudflare TLS).
- The mirror serves its WebSocket over plain `ws://`.
- Browsers **refuse** to open `ws://` from an `https://` origin ("mixed content" error).
- Fixes: all bad in V1.
  - **Per-device TLS certs:** requires a cert per device per domain. Infeasible with current device count, would need auto-renewing ACME over local network or a per-device subdomain of `layeredlogic.cc` with DNS rebinding tricks.
  - **Serve the hosted page over `http://`:** gives up HTTPS for the hosted surface. Unacceptable — Cloudflare auto-upgrades, browsers warn, passwords leak.
  - **Relay the WebSocket through Cloudflare:** possible but requires devices to hold a persistent outbound connection to a Cloudflare Worker, which is the "remote access" architecture we explicitly deferred in [firmware-architecture-scoping §5.5](firmware-architecture-scoping.md#55-app--device-protocol).

V2 will likely pick the relay approach once remote-access design is tackled.

### 5.3 Forward-compatible design now

V1 client code uses a `base_url` abstraction. Swapping from `http://<mirror_ip>` to `https://layeredlogic.cc/relay/<mirror_id>` is a one-line change.

---

## 6. Development + Distribution

### 6.1 Repo location

`Firmware/v1/webapp/` directory in the main firmware repo (same repo, co-versioned with firmware).

### 6.2 Build pipeline

- Vite produces a single HTML + single JS + single CSS bundle
- Build script gzips the assets
- Firmware build embeds the gzipped bundle via `idf.py component` asset registration
- HTTP server responds with `Content-Encoding: gzip` to save flight + flash

### 6.3 Versioning

Webapp version tracks firmware version — they ship together. The `/api/info` endpoint returns `fw_version` which is also the webapp's effective version (since webapp files live inside the firmware).

Migration is simple: OTA the device → webapp updates next time the user refreshes the browser tab.

---

## 7. Multi-Client Behavior

- Multiple browsers can connect to the same mirror simultaneously (up to the WebSocket max client limit, 8)
- Browsers + LL app + Matter controllers all coexist — same [state bus](firmware-spec.md#3-state-bus) authority on the device
- Paired mode: each browser session needs the shared secret entered at start of session

---

## 8. Security Posture

- HTTP, not HTTPS, for V1 device-hosted surface. Acceptable because: local network only, mirror state is not sensitive, no passwords cross the wire except the paired-mode secret (which the user chose for local-network access specifically).
- Same HMAC envelope as mobile app in paired mode (see [firmware-security §5.4](firmware-security.md#54-hmac-envelope)).
- No CORS restrictions needed — same-origin (device serves both the page and the API).
- No cookies, no sessions.

---

## 9. Accessibility

- Keyboard navigation on every control (already called out in §4 as UX principle)
- ARIA labels on color picker, brightness slider, pattern grid
- Screen reader flow matches visual flow
- Respects system `prefers-color-scheme` and `prefers-reduced-motion`

---

## 10. Out Of Scope (V1)

- Hosted variant at `layeredlogic.cc/controlmydevice` (V2)
- Remote access (V2)
- BLE provisioning in browser (probably never — Web Bluetooth support is fragmented; mobile app is the pairing path)
- User accounts / saved pattern library across devices (V2, requires cloud)
- Multi-device management dashboard (V2; V1 webapp talks to one mirror at a time, the mirror it's hosted by)

---

## 11. Open Questions

| Question | Answer by |
|---|---|
| Vanilla TS + Preact vs alternative (SolidJS, lit-html) | When bundle size is measured against the 80KB cap |
| Whether pattern editor lives in webapp and not mobile app (inverse of current plan) | Week 5 wireframing — may reveal desktop is actually better for pattern authoring |
| mDNS reliability across platforms | Test with real Android / Windows / Linux browsers |
| How to communicate "you're on the wrong Wi-Fi" vs "device is off" | Needs UX design for unreachable case |

---

## Related

- [Firmware Architecture Scoping §5.5](firmware-architecture-scoping.md#55-app--device-protocol) — hosted deferral rationale
- [Firmware Spec](firmware-spec.md) — device serves these static files
- [Control Protocol Spec](control-protocol-spec.md) — same wire protocol as mobile app
- [App Spec](app-spec.md) — feature parity target
- [Firmware Security Model](firmware-security.md) — auth + HTTPS mixed-content context
