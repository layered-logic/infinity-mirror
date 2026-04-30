---
title: Control Protocol Spec — App ↔ Device
type: engineering
phase: 2
week: 4
date: 2026-04-22
status: draft — initial lock from firmware-architecture-scoping §5.5 decision
tags: [engineering, firmware, protocol, websocket, ble, http]
---

# Control Protocol Spec — App ↔ Device

**Prepared by:** William White
**Date:** April 22, 2026
**Status:** First draft — defines the shared wire protocol used by mobile app, webapp, and any third-party controller

---

## 1. Purpose

This spec defines the **wire protocol** between controllers (LL mobile app, webapp, HA custom integration, third-party clients) and the mirror firmware.

It is transport-agnostic: the same message envelope rides over WebSocket, HTTP, and BLE. Client implementations should target this spec — not the transports directly — so that fallback between transports (Wi-Fi → BLE) doesn't require protocol rework.

Implements [firmware-architecture-scoping §5.5](firmware-architecture-scoping.md#55-app--device-protocol). Auth model detailed in [firmware-security.md §5](firmware-security.md#5-local-authentication).

---

## 2. Transports

Three transports, same envelope across all.

| Transport | When | Endpoint | Notes |
|---|---|---|---|
| **WebSocket** | Primary runtime; controller on same LAN as device | `ws://<host>/ws` on port 80 | Bidirectional, state broadcasts to all connected clients |
| **HTTP REST** | One-shot ops; scripts, shortcuts, webapp boot | `http://<host>/api/*` on port 80 | No persistent connection; device responds synchronously |
| **BLE GATT** | Fallback when no Wi-Fi or during pairing | Custom service UUID (TBD) | Same message envelope wrapped in BLE notify/write |

### 2.1 Discovery order

Client should try in sequence:
1. **Cached last-known IP** from NVS or app storage (instant, stale-tolerant)
2. **mDNS query** for `_layeredlogic._tcp.local.` on local network
3. **BLE scan** for the device's advertised service UUID (only finds the device if it's in pairing mode OR running its BLE fallback mode due to Wi-Fi being down)

No manual IP entry in V1. If all three fail: UI surfaces a diagnostic ("device unreachable — check Wi-Fi or hold reset button to re-pair").

### 2.2 mDNS record

Published by device when Wi-Fi connected:
- Service: `_layeredlogic._tcp.local.`
- Hostname: `layered-logic-mirror-<device_id_short>.local`
- TXT record: `variant=standard|matter`, `version=<fw_version>`, `id=<device_id_short>`, `auth=open|paired`

Clients filter on service type + optional `id` to find a specific device among multiple.

---

## 3. Message Envelope

All messages — request and response, WebSocket and HTTP and BLE — use the same envelope.

### 3.1 Request envelope

```json
{
  "op": "set_state",
  "req_id": "7b3c...",
  "ts": 1745280000,
  "payload": { ... },
  "hmac": "<hex>"
}
```

| Field | Type | Required | Description |
|---|---|---|---|
| `op` | string | yes | Operation name — see §4 |
| `req_id` | string (UUIDv4) | yes | Client-generated. Device echoes in response for correlation. |
| `ts` | int (epoch seconds) | yes | Unix timestamp. Device rejects messages where `|device_ts - ts| > 60`. |
| `payload` | object | varies | Op-specific data — see §4 |
| `hmac` | string (hex) | only in paired mode | HMAC-SHA256 of the JSON object minus `hmac` itself, keyed on shared secret. Required when device is in paired mode, rejected (as unknown field) when in open mode. |

### 3.2 Response envelope

```json
{
  "op": "set_state",
  "req_id": "7b3c...",
  "ok": true,
  "result": { ... },
  "error": null
}
```

| Field | Type | Description |
|---|---|---|
| `op` | string | Echoes the request op |
| `req_id` | string | Echoes the request req_id |
| `ok` | bool | Success flag |
| `result` | object \| null | Op-specific result on success |
| `error` | object \| null | `{ code, message }` on failure |

### 3.3 State broadcast envelope

When device state changes (from any controller input including physical button), device broadcasts to **all connected WebSocket clients**:

```json
{
  "op": "state",
  "ts": 1745280000,
  "state": { ... current full state ... }
}
```

No `req_id` (unsolicited). No `hmac` on broadcasts (broadcasts are authoritative device state, not commands — HMAC is for authenticating commands, not facts).

---

## 4. Operations

### 4.1 Core operations (required on all transports)

| Op | Description | Request payload | Response result |
|---|---|---|---|
| `ping` | Liveness check | `{}` | `{ "fw_version": "...", "uptime_s": 12345 }` |
| `get_state` | Fetch current full state | `{}` | State object — see §5 |
| `set_state` | Partial state update | Partial state object | Updated state object |
| `get_info` | Device metadata | `{}` | `{ device_id, variant, hw_board, fw_version, led_count, auth_mode }` |

### 4.2 Pattern operations

| Op | Description | Request payload | Response result |
|---|---|---|---|
| `list_patterns` | List stored patterns | `{}` | `{ "patterns": [{ id, name, builtin }] }` |
| `get_pattern` | Fetch one pattern definition | `{ "id": "..." }` | Pattern definition — see §6 |
| `upload_pattern` | Add or replace a pattern | Pattern definition | `{ "id": "..." }` |
| `delete_pattern` | Remove a user pattern | `{ "id": "..." }` | `{}` |

### 4.3 Auth operations

| Op | Description | Request payload | Response result |
|---|---|---|---|
| `set_auth_mode` | Change between open + paired | `{ "mode": "open"\|"paired", "secret": "..." }` | `{ "mode": "..." }` |
| `rotate_secret` | Change shared secret (paired mode only) | `{ "old_secret": "...", "new_secret": "..." }` | `{}` |

### 4.4 OTA operations

| Op | Description | Request payload | Response result |
|---|---|---|---|
| `get_update_status` | Is there an update? | `{}` | `{ available: bool, version: "...", changelog: "..." }` |
| `apply_update` | User-triggered update now | `{}` | `{}` — device reboots into new version |

### 4.5 Telemetry operations

| Op | Description | Request payload | Response result |
|---|---|---|---|
| `set_telemetry` | Enable / disable | `{ "enabled": bool }` | `{ "enabled": bool }` |
| `peek_telemetry` | View pending beacon | `{}` | The exact bytes that would be sent next |

---

## 5. State Object

The canonical state object returned by `get_state` and broadcast on change:

```json
{
  "on": true,
  "pattern_id": "color_wipe",
  "base_color": "#3214FF",
  "brightness": 80,
  "led_count": 32,
  "auth_mode": "open",
  "telemetry_enabled": false,
  "id": "b2332d",
  "name": "Living Room"
}
```

| Field | Type | Description |
|---|---|---|
| `on` | bool | Power state |
| `pattern_id` | string | Currently-running pattern ID |
| `base_color` | string | Hex `#RRGGBB` — the single-press advance color (see [button-interface §4.3](button-interface.md#43-base-color-cycle-single-press-semantics)) |
| `brightness` | int (0–100) | One of the 4 discrete steps: 25 / 50 / 75 / 100 |
| `led_count` | int | Read-only (set at provisioning or from board header) |
| `auth_mode` | string | `"open"` or `"paired"` |
| `telemetry_enabled` | bool | User opt-in status |
| `id` | string | Read-only. Lowercase 6-hex MAC suffix — same suffix the mDNS hostname and SoftAP SSID expose. Stable across factory resets. |
| `name` | string | User-set device name; empty when unnamed. Settable via `set_state`. Clients display `name` if non-empty, else fall back to `id`. |

Partial updates via `set_state` — send only the fields changing. Device broadcasts full state after every change.

`id` is read-only and cannot be set via `set_state`; setting `name` is the user-rename surface.

---

## 6. Pattern Definition Format

### 6.1 V1 — built-in patterns only

V1 ships with 7 hardcoded patterns, fully defined in [pattern-dictionary.md](pattern-dictionary.md). Their IDs (wire format):

`solid`, `rainbow`, `scanner`, `spinner`, `random`, `breathing`, `twinkle`

`list_patterns` returns these with `builtin: true`. `upload_pattern` and `delete_pattern` return `error: "unsupported_in_v1"` until V2.

### 6.2 V2 — declarative user-uploaded patterns (deferred)

Shape (illustrative, not final):

```json
{
  "id": "my_pattern",
  "name": "My Pattern",
  "uses_base_color": true,
  "frames": [
    { "duration_ms": 100, "leds": [["base", 0, 1, 2], ["off", 3, 4, 5]] },
    { "duration_ms": 100, "leds": [["off", 0, 1, 2], ["base", 3, 4, 5]] }
  ],
  "loop": true
}
```

Full grammar gets its own spec doc when the app's pattern-editor UI is designed. Deferred because no V1 client produces these — app doesn't exist yet.

**Upload size limit (when V2 ships):** 16KB per pattern (NVS budget). Larger patterns rejected with `error: "pattern_too_large"`.

---

## 7. Transport-Specific Details

### 7.1 WebSocket

- Endpoint: `ws://<host>/ws`
- Max concurrent clients: **8**. Ninth connection refused with close code 1013 ("try again later").
- Per-IP rate limit: 10 messages/sec. Exceeding results in close code 1008.
- Ping/pong: device sends ping every 30s, closes connection if no pong within 60s.
- On state change (from any source): device broadcasts `{"op":"state", ...}` to all connected clients.

### 7.2 HTTP REST

Convenience wrapper over ops. Every op reachable via HTTP for curl-friendliness:

| Method + path | Op |
|---|---|
| `GET /api/state` | `get_state` |
| `POST /api/state` | `set_state` (body = payload) |
| `GET /api/info` | `get_info` (V1: tiny discovery payload — see below) |
| `POST /api/ping` | `ping` |
| `GET /api/patterns` | `list_patterns` |
| `POST /api/patterns` | `upload_pattern` |
| `DELETE /api/patterns/:id` | `delete_pattern` |

**V1 `GET /api/info` (implemented):** lightweight HTTP-only sentinel + identity payload, used by the LL app's subnet-scan discovery to (a) confirm an IP is a Layered Logic mirror and (b) read its `id` + `name` for the multi-mirror picker without opening a WebSocket. Response shape:

```json
{ "product": "layered-logic-mirror", "id": "b2332d", "name": "Living Room", "fw_version": "1b86d5e" }
```

Returned with `Content-Type: application/json` and `Access-Control-Allow-Origin: *` (no secrets — same identity already advertised over mDNS). The full V2 `get_info` op (board variant, hardware revision, etc.) extends this shape additively.

Envelope fields (`req_id`, `ts`, `hmac` when paired) sent as HTTP headers:
- `X-LL-Req-Id: <uuid>`
- `X-LL-Ts: <epoch>`
- `X-LL-Hmac: <hex>` (paired mode only)

HTTP responses return the bare `result` object with HTTP status (200 ok, 400 bad op, 401 bad hmac, 403 rate limit, etc.).

### 7.3 BLE GATT

Minimal surface. Only used during provisioning or as fallback when Wi-Fi is unavailable.

- Service UUID: TBD (assign from 128-bit private range)
- Characteristic: `cmd` (write + notify) — client writes request envelope as JSON bytes, device notifies response on the same characteristic.
- MTU: request 185 bytes; if negotiated lower, client must chunk and send with continuation flag in `req_id`.
- No state broadcasts over BLE (would thrash battery) — client must poll `get_state` if using BLE as runtime transport.

---

## 8. Error Codes

| Code | Meaning |
|---|---|
| `unknown_op` | Op not recognized by this firmware version |
| `bad_payload` | Missing or malformed field in payload |
| `auth_required` | Paired mode enabled, no HMAC provided |
| `bad_hmac` | HMAC did not verify |
| `stale_ts` | Timestamp outside ±60s window |
| `rate_limit` | Per-IP message rate exceeded |
| `pattern_too_large` | Pattern exceeds 16KB |
| `nvs_full` | Pattern storage full |
| `unsupported_variant` | Op requires Matter variant but device is standard |
| `internal` | Unhandled firmware error (should be rare; log + telemetry if opt-in) |

---

## 9. Versioning

- Protocol version surfaced via `get_info` as `proto_version`.
- V1 is `"1.0"`.
- Breaking changes bump major; additive changes (new ops, new fields) bump minor.
- Clients must tolerate unknown fields in responses (forward compatibility).

---

## 10. Out Of Scope

- Pattern grammar (separate spec, written alongside firmware pattern interpreter)
- Matter protocol messages (handled by `esp-matter` stack, not this protocol)
- OTA binary format (see [firmware-security.md §3](firmware-security.md#3-ota-security))
- Remote-access tunneling (V2)

---

## Related

- [Firmware Architecture Scoping §5.5](firmware-architecture-scoping.md#55-app--device-protocol) — source decision
- [Firmware Security Model §5](firmware-security.md#5-local-authentication) — auth envelope details
- [Firmware Spec](firmware-spec.md) — device-side implementation
- [Pattern Dictionary](pattern-dictionary.md) — V1 pattern definitions (shared across firmwares)
- [App Spec](app-spec.md) — mobile client implementation
- [Webapp Spec](webapp-spec.md) — browser client implementation
