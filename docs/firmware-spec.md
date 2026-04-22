---
title: Firmware Spec — ESP32 Infinity Mirror
type: engineering
phase: 2
week: 4
date: 2026-04-22
status: draft — initial component spec after Apr 22 architecture lock
tags: [engineering, firmware, esp32, spec]
---

# Firmware Spec — ESP32 Infinity Mirror

**Prepared by:** William White
**Date:** April 22, 2026
**Status:** First draft — authoritative component-level spec for the two firmware variants

---

## 1. Purpose

Defines what the ESP32 firmware *is* at the component level: what modules exist, what they own, how they communicate, what ESP-IDF components they depend on.

Pairs with:
- [Firmware Architecture Scoping](firmware-architecture-scoping.md) — the decisions this spec implements
- [Firmware Security Model](firmware-security.md) — signing, OTA, auth details
- [Control Protocol Spec](control-protocol-spec.md) — wire protocol this firmware serves
- [Button Interface Design](button-interface.md) — UX this firmware implements

Out of scope: line-level implementation, test strategy, CI. Those follow the repo scaffold.

---

## 2. Top-Level Architecture

```
            ┌───────────────────────────────────────────────┐
            │                  variants/                    │
            │   ┌───────────────┐      ┌───────────────┐    │
            │   │  standard/    │      │   matter/     │    │
            │   │   main.c      │      │   main.c      │    │
            │   │               │      │   matter_     │    │
            │   │               │      │   bridge.c    │    │
            │   └───────┬───────┘      └───────┬───────┘    │
            └───────────┼──────────────────────┼────────────┘
                        │                      │
                        ▼                      ▼
            ┌───────────────────────────────────────────────┐
            │                    core/                      │
            │                                               │
            │    ┌─────────────────────────────────────┐    │
            │    │            state_bus                │    │
            │    │  (single source of truth, pub/sub)  │    │
            │    └────┬────────────────────────────┬───┘    │
            │         │                            │        │
            │    ┌────▼─────┐  ┌────────┐   ┌──────▼─────┐  │
            │    │ button   │  │  ota   │   │ transport  │  │
            │    │ handler  │  │ client │   │ (WS/HTTP/  │  │
            │    │          │  │        │   │   BLE)     │  │
            │    └────┬─────┘  └────────┘   └──────┬─────┘  │
            │         │                            │        │
            │         ▼                            │        │
            │    ┌─────────────────────┐           │        │
            │    │ pattern_interp      │◄──────────┘        │
            │    │ (runs current       │                    │
            │    │  pattern at frame   │                    │
            │    │  rate)              │                    │
            │    └────┬────────────────┘                    │
            │         ▼                                     │
            │    ┌──────────────┐     ┌──────────────┐      │
            │    │ led_driver   │     │ provisioning │      │
            │    │ (RMT→WS2812) │     │ (wifi_prov)  │      │
            │    └──────────────┘     └──────────────┘      │
            │                                               │
            │    ┌──────────────┐     ┌──────────────┐      │
            │    │ nvs          │     │ mdns + auth  │      │
            │    └──────────────┘     └──────────────┘      │
            └───────────────────────────────────────────────┘
                        │                      │
                        ▼                      ▼
            ┌───────────────────────────────────────────────┐
            │                   boards/                     │
            │  c6_devkit.h  c3_devkit.h  prod_v1_pro.h ...  │
            └───────────────────────────────────────────────┘
```

Variant-specific code (Matter bridge, standard main) is thin. All behavior lives in `core/`.

---

## 3. State Bus

The one thing every module touches. Single in-memory struct representing current device state:

```c
typedef struct {
    bool on;
    char pattern_id[32];
    uint32_t base_color_rgb;       // 0xRRGGBB
    uint8_t brightness;            // 0-100, actual values: 25/50/75/100
    uint16_t led_count;
    ll_auth_mode_t auth_mode;      // OPEN / PAIRED
    bool telemetry_enabled;
} ll_state_t;
```

### 3.1 Access rules

- **Single writer pattern via event-loop task.** Any module that wants to change state posts an event to the state-bus event loop (ESP-IDF `esp_event`).
- The state-bus task applies the change, then notifies subscribers.
- No module mutates `ll_state_t` directly.

### 3.2 Subscribers

Who listens for state changes:
- `pattern_interp` — re-selects pattern, picks up new base color / brightness
- `transport` — broadcasts the new state to all connected WebSocket clients
- `nvs` — persists changed fields (debounced — commits at most once per 500ms)
- `matter_bridge` (Pro+ only) — updates Matter attributes + triggers fabric subscription notifications

### 3.3 Event types

| Event | Emitted by | Payload |
|---|---|---|
| `LL_EV_POWER_TOGGLE` | button, transport, matter | `{ on: bool }` |
| `LL_EV_BASE_COLOR` | button, transport, matter | `{ rgb: uint32 }` |
| `LL_EV_PATTERN_CHANGE` | button, transport | `{ id: const char* }` |
| `LL_EV_BRIGHTNESS` | button, transport, matter | `{ value: uint8 }` |
| `LL_EV_AUTH_MODE` | transport | `{ mode: ll_auth_mode_t, secret?: const char* }` |
| `LL_EV_TELEMETRY` | transport | `{ enabled: bool }` |
| `LL_EV_PROVISION_START` | button (recessed 3s) | — |
| `LL_EV_FACTORY_RESET` | button (recessed 10s), transport | — |

---

## 4. Core Modules

### 4.1 `led_driver/`

- **Responsibility:** drive WS2812B LEDs via ESP32 RMT peripheral.
- **Inputs:** frame buffer (RGB8 array of length `led_count`), write call.
- **Outputs:** GPIO signal out `LL_PIN_LED_DATA`.
- **Dependencies:** `driver/rmt_tx` (ESP-IDF).
- **Frame rate:** target 60 fps. Actual cap = min(60, RMT bandwidth / led_count).
- **Brightness application:** uniform multiply applied here, not at pattern layer. Patterns produce 0–255 RGB; driver scales by `brightness/100` before RMT encode.

### 4.2 `pattern_interp/`

- **Responsibility:** execute the currently-selected pattern, produce a frame buffer per tick.
- **Pattern format:** declarative JSON-ish (see [control-protocol-spec §6](control-protocol-spec.md#6-pattern-definition-format)).
- **Pattern table:** built-in + user-uploaded, keyed by `pattern_id`.
- **Base color integration:** patterns reference `"base"` as a color token; interp substitutes current `base_color_rgb`.
- **Self-palette patterns (Rainbow, Random Twinkle):** ignore `base_color_rgb` at render, but state still updates (per [button-interface §4.3](button-interface.md#43-base-color-cycle-single-press-semantics)).
- **Built-in patterns for V1:** solid, color_wipe, twinkle, scanner, rainbow, random_twinkle, indigo_pulse (the pairing-mode cue), red_flash (factory-reset cue).

### 4.3 `button/`

- **Responsibility:** debounce + gesture recognition for both buttons.
- **Inputs:** GPIO interrupts on `LL_PIN_BUTTON_PRIMARY` and `LL_PIN_BUTTON_RESET`.
- **Outputs:** state-bus events.
- **Gestures (primary):** single, double, triple, hold (500ms). Window for multi-press: 300ms after first press.
- **Gestures (recessed):** short-hold (3s) → `LL_EV_PROVISION_START`; long-hold (10s) → `LL_EV_FACTORY_RESET`. Release between thresholds fires the shorter action.
- **LED feedback during recessed hold:** pattern_interp shifts to a "threshold cue" pattern (color shift at 3s mark, red flash at 10s) — see [button-interface §7](button-interface.md#7-led-feedback-conventions).

### 4.4 `provisioning/`

- **Responsibility:** handle Wi-Fi provisioning via `wifi_prov_mgr`.
- **State machine:**
  - On boot: check NVS for creds. Present → connect Wi-Fi. Absent → do nothing, radios dark.
  - On `LL_EV_PROVISION_START`: start `wifi_prov_mgr` with BLE + SoftAP transports. Advertise for 5 minutes. On success: save creds, reboot into provisioned state. On timeout: stop advertising, return to previous state.
- **Dependencies:** `wifi_provisioning` (ESP-IDF component).

### 4.5 `transport/`

Subdivided into three transport submodules that share one dispatcher.

#### 4.5.1 `transport/ws/`
- WebSocket server on port 80, path `/ws`.
- Max 8 concurrent clients.
- Subscribes to state-bus events → broadcasts JSON state to all clients on change.
- Dependencies: `esp_http_server` (supports WS via extension).

#### 4.5.2 `transport/http/`
- REST handlers for `/api/*`. Same `esp_http_server` instance as WS.
- Maps HTTP requests to state-bus events or direct state reads.

#### 4.5.3 `transport/ble/`
- GATT service with a single `cmd` characteristic.
- Used during `wifi_prov_mgr` pairing and as fallback when Wi-Fi down.
- Not active during normal operation.

#### 4.5.4 Shared dispatcher
- Parses message envelope (JSON, see [control-protocol §3](control-protocol-spec.md#3-message-envelope))
- Verifies HMAC if device is in paired mode
- Validates `op`, dispatches to handler
- Handlers either read state, or post a state-bus event

### 4.6 `mdns/`

- Advertises `_layeredlogic._tcp.local.` on Wi-Fi connect.
- Hostname: `layered-logic-mirror-<device_id_short>.local`.
- TXT records include variant, version, auth mode (kept fresh by subscribing to state-bus).

### 4.7 `ota/`

Full behavior in [firmware-security §3](firmware-security.md#3-ota-security). Module responsibilities:
- Polls `ota.layeredlogic.cc/v1/check` on jittered schedule.
- Downloads signed binary to standby partition.
- Verifies signature + version counter before marking boot candidate.
- On boot of new version: validate critical services (Wi-Fi, LEDs, WebSocket) within 60s window, then `esp_ota_mark_app_valid_cancel_rollback()`.

### 4.8 `nvs/`

- Wrapper over `nvs_flash` for structured state persistence.
- Namespaces: `ll_state` (power, pattern, color, brightness, auth_mode, telemetry_enabled), `ll_auth` (shared_secret), `ll_patterns` (uploaded patterns, indexed by ID), `wifi_creds` (owned by `wifi_prov_mgr`).
- Debounced writes: state-bus triggers NVS commit at most once per 500ms.

### 4.9 `auth/`

- HMAC-SHA256 over message envelope (less the `hmac` field).
- Invoked by transport dispatcher when device is in paired mode.
- Dependency: `mbedtls/md.h`.

### 4.10 `telemetry/`

- Builds beacon payload from runtime metrics (heap low-watermark, boot reason, uptime, RSSI).
- POST to `telemetry.layeredlogic.cc/v1/beacon` on jittered 24h schedule — only if user opted in.
- On panic: captures `filename:line:task_name` to NVS, flushed on next successful beacon.

---

## 5. Variant-Specific Modules

### 5.1 `variants/standard/main.c`

- Minimal. Initializes core modules, sets up board via `board.h`, enters idle loop.
- Does not reference Matter at all.

### 5.2 `variants/matter/main.c` + `matter_bridge.c`

- Initializes core modules (same as standard).
- Additionally initializes `esp-matter` stack (Matter-over-Wi-Fi; Thread on C6 optional).
- Registers Matter On/Off + Level Control + Color Control clusters.
- `matter_bridge.c` translates Matter cluster callbacks → `ll_state_t` events on the state bus.
- Matter commissioning uses test VID `0xFFF1` + a Layered Logic test PID (see [firmware-security §6](firmware-security.md#6-secrets--keys-inventory)).
- Matter commissioning gesture: recessed button 6s hold (between `LL_EV_PROVISION_START` and `LL_EV_FACTORY_RESET` thresholds). Final timing confirmed when variant is built.

---

## 6. Board Headers

See [firmware-architecture-scoping §7](firmware-architecture-scoping.md#7-board-parameterization) for the full interface.

Each board header defines the hardware constants the core uses:
- Pin assignments
- Radio capability flags
- Flash layout selector
- Default LED count

Picked at compile time via `idf.py set-target esp32c6 -D LL_BOARD=c6_devkit` (approximate CMake passthrough).

---

## 7. ESP-IDF Components Required

### 7.1 Always required (both variants)

- `nvs_flash`
- `driver` (for RMT)
- `esp_wifi`
- `esp_event`
- `esp_timer`
- `esp_http_server`
- `wifi_provisioning`
- `mdns`
- `app_update` (OTA)
- `esp_https_ota`
- `bootloader_support`
- `mbedtls`
- `bt` (for BLE GATT + Unified Provisioning BLE transport)
- `nvs_sec_provider` (for encrypted NVS — optional, evaluated V2)

### 7.2 Additional on Pro+ (matter variant)

- `esp-matter` (external component)
- `connectedhomeip` (brought in by esp-matter)
- `openthread` (C6 only, for Thread)
- `ieee802154` (C6 only)

Matter component brings a large transitive closure; flash budget reserved for this is why Pro+ uses 8MB C6.

---

## 8. Boot Sequence

1. Bootloader verifies app signature (from §3.2 of firmware-security).
2. App starts. Initializes NVS, loads `ll_state_t` from `ll_state` namespace.
3. Initialize `state_bus` (event loop task pinned to core 0).
4. Initialize `led_driver`, `pattern_interp`. Render first frame (default color if hard-off, restored state if soft-off per [button-interface §4.1](button-interface.md#41-persistence)).
5. Initialize `button` module. Buttons live.
6. Check Wi-Fi credentials in NVS.
   - Present → connect Wi-Fi. On connect: start `mdns`, `transport/ws`, `transport/http`, `ota` (jittered check scheduled).
   - Absent → stay dark on radios. Device is fully usable offline.
7. (Matter variant only) Initialize `esp-matter` stack. Check for saved Matter fabric; if present, rejoin.
8. Enter idle loop. All work runs in tasks (button task, state-bus task, pattern tick timer, HTTP server task, OTA task).
9. ~30s after Wi-Fi connect + every 24h thereafter: OTA check.
10. ~60s after new-version boot: validation window runs `esp_ota_mark_app_valid_cancel_rollback()` if everything is healthy.

---

## 9. Flash + Memory Budget

| Resource | Pro (C3, 4MB) | Pro+ (C6, 8MB) |
|---|---|---|
| Application size (each slot) | ≤ 1.2MB | ≤ 3.0MB |
| NVS partition | 64KB | 128KB |
| Pattern storage (subset of NVS) | ~48KB (3 slots × 16KB) | ~96KB |
| Free heap target (min) | ≥ 80KB | ≥ 120KB |
| Matter fabric data | n/a | ~40KB |

Build budget is tracked in CI per commit. Going over triggers build fail.

---

## 10. Open Questions

| Question | Answer by |
|---|---|
| Final GPIO for recessed button on dev boards | First hardware bring-up |
| Final Matter commissioning gesture timing | When matter variant gets implemented |
| Whether encrypted NVS is worth the eFuse cost | V2 if a concrete threat emerges |
| Pattern grammar full spec | When pattern_interp implementation starts |

---

## Related

- [Firmware Architecture Scoping](firmware-architecture-scoping.md) — parent
- [Firmware Security Model](firmware-security.md) — security-facing internals
- [Control Protocol Spec](control-protocol-spec.md) — wire protocol this firmware serves
- [Button Interface Design](button-interface.md) — UX contract
- [App Spec](app-spec.md) — primary client
- [Webapp Spec](webapp-spec.md) — secondary client (device-hosted in V1)
