---
title: Multi-Network Wi-Fi Design
type: engineering
phase: 2
week: 6
date: 2026-05-01
status: spec — implementation-ready, decisions locked May 1
tags: [engineering, firmware, protocol, wifi, provisioning, app]
---

# Multi-Network Wi-Fi Design

**Prepared by:** William White
**Date:** May 1, 2026
**Status:** Locked. All §10 questions answered May 1; ready for step-1 NVS layer implementation.

---

## 1. Purpose

Today the mirror remembers exactly **one** Wi-Fi network. Re-provisioning to a new SSID erases the old one. Customers who move the mirror between locations (home ↔ office, take it to a friend's, bring it on a trip) have to re-run setup every time.

This doc specifies a multi-network design: the mirror remembers up to N saved Wi-Fi networks and joins whichever one is in range on boot or after disconnect.

Implements a feature gap discovered during [project_app_stack.md] usability work in week 5.

---

## 2. Non-Goals

- **Network roaming** (handing off between two BSSIDs of the *same* SSID): out of scope. ESP-IDF has separate APIs for that; we don't need it for the customer story.
- **Enterprise / 802.1X**: out of scope. WPA2-PSK and open networks only — same as today.
- **Hidden SSIDs**: out of scope for V1. Adds a probe-by-name code path that's annoying to test and unnecessary for the target customer.
- **Per-network preferences** (e.g. dim by default at the office): out of scope. Networks are just connection profiles, not user-config keys.

---

## 3. Current State (what we're replacing)

| Layer | Today |
|---|---|
| **Firmware storage** | One `wifi_config_t` written to `esp_wifi`'s NVS namespace via `esp_wifi_set_config(WIFI_IF_STA)` ([provisioning.c:329-355](Firmware/v1/core/provisioning/provisioning.c)). Overwrites on every `set_wifi_creds`. |
| **Boot behavior** | If `esp_wifi` reports a saved cred → start STA → connect. Otherwise stay dark, wait for provisioning. |
| **Reconnect** | Retry the *same* SSID on disconnect. After `LL_APPLY_FALLBACK_US` (15s) of failure during cred-apply, wipe and fall back to SoftAP. |
| **Wire protocol** | `set_wifi_creds {ssid, password}` — single network at a time. |
| **App UX** | One SSID/password form in setup; "Reconfigure Wi-Fi" replaces the saved network. |

---

## 4. Target UX

### 4.1 Provisioning (first run, unchanged)

User joins SoftAP, opens the app, enters one network's creds. Mirror joins that network. **No change** — the multi-network feature is about *adding* networks after the first one, not changing first-run setup.

### 4.2 Adding a second network (new flow)

From Settings → Wi-Fi:

```
Networks
┌──────────────────────────────────┐
│ ✓ HomeWiFi          (connected)  │
│   OfficeWiFi                     │
│ + Add a network                  │
└──────────────────────────────────┘
```

Tapping "Add a network" surfaces an SSID/password form. On submit, the mirror:
1. Saves the new entry to its own NVS list (does **not** overwrite the active connection).
2. Stays on the currently-connected network — adding doesn't trigger a switch.

### 4.3 Removing a network

Swipe-to-delete or row-detail "Forget" button.

If the network being removed is **not** currently connected: forget silently, no prompt.

If the network being removed **is** currently connected: surface a confirm dialog ("Forget HomeWiFi? The mirror will disconnect from this network.") before any action. On confirm, the mirror proactively disconnects and falls into the SCANNING flow. This is a deliberate departure from iOS/macOS's passive behavior — if the user says forget, we respect that immediately rather than letting them wonder why nothing happened.

### 4.4 Boot / reconnect behavior (no UI)

On boot or after STA disconnect, the mirror:
1. Runs a passive scan (2-4s).
2. Picks the highest-priority saved network whose SSID appears in scan results.
3. Connects.
4. If connect fails, marks that entry as "recently failed" (in-RAM only, cleared on reboot) and tries the next match.
5. If no saved networks are visible, retries scan with backoff (5s, 15s, 60s, 60s…) until one appears. **The mirror does not auto-fall-back to SoftAP** — the user must put it into pairing mode by holding the recessed button (existing flow). Rationale: avoid polluting the space with unnecessary RF advertisements when the device is just out of range or the user doesn't intend to use it on a new network.

---

## 5. Storage Layout

### 5.1 NVS

New namespace `ll_wifi` (separate from `ll_settings` to keep concerns isolated; separate from `esp_wifi`'s namespace because that one is owned by the IDF's prov_mgr and we shouldn't share it).

```
ll_wifi/
  count       u8        — number of valid entries (0..N_MAX)
  entry_0     blob      — packed wifi_entry_t
  entry_1     blob      — packed wifi_entry_t
  ...
  active_idx  u8        — index of last-successfully-connected entry (0xFF = none)
```

```c
typedef struct __attribute__((packed)) {
    char     ssid[33];      // 32 + null
    char     password[65];  // 64 + null; empty = open network
    int64_t  last_used_us;  // esp_timer_get_time() at last successful connect
} wifi_entry_t;
```

Storing entries as separate blobs (vs. one big blob) means add/remove only rewrites one key, not the whole list — friendlier to flash wear and to partial-write recovery.

### 5.2 Capacity

`N_MAX = 4`. Rationale: realistic personal-use ceiling (home, work, parents, one travel hotspot). Each entry is ~106 bytes packed; 4 entries = ~424 B + small NVS overhead.

If a 5th add comes in, response is `wifi_list_full` error — let the user decide what to evict rather than picking for them. Easy to bump later if real usage proves the cap is too tight; hard to lower without forcing a migration.

### 5.3 Migration from current single-cred state

On first boot of multi-network firmware, if `ll_wifi` is empty AND `esp_wifi` has a saved STA cred, copy it into `ll_wifi[0]`. Don't delete the `esp_wifi` copy — the runtime still uses `esp_wifi_set_config` to connect; `ll_wifi` is the "remembered list", and `esp_wifi`'s slot is just the *currently-loaded* cred. (See §6.2 for how these stay in sync.)

---

## 6. Firmware: Connection State Machine

### 6.1 States

```
        ┌──────────────────┐
   ┌───▶│   SCANNING       │
   │    └────────┬─────────┘
   │             │ scan complete
   │             ▼
   │    ┌──────────────────┐
   │    │   PICKING        │  pick highest-priority
   │    └────────┬─────────┘  visible saved entry
   │             │
   │       ┌─────┴──────┐
   │       │            │
   │       ▼            ▼
   │  no match     match found
   │       │            │
   │       │            ▼
   │       │    ┌──────────────────┐
   │       │    │   CONNECTING     │
   │       │    └────────┬─────────┘
   │       │             │
   │       │       ┌─────┴──────┐
   │       │       │            │
   │       │       ▼            ▼
   │       │   success      timeout/fail
   │       │       │            │
   │       │       ▼            │
   │       │  ┌──────────┐      │
   │       │  │ ONLINE   │◀─────┴── (mark entry recently_failed, try next match)
   │       │  └────┬─────┘
   │       │       │ STA disconnect
   │       │       │
   │       └───────┴────────────┐
   │                            │
   │                            ▼
   │              ┌──────────────────────┐
   │              │   BACKOFF            │  5s → 15s → 60s
   └──────────────┤   (then re-scan)     │
                  └──────────────────────┘
```

Provisioning (SoftAP) is a separate top-level mode triggered **only** by recessed-button hold. BACKOFF stays in BACKOFF indefinitely — no auto-SoftAP fallback (see §10 Q3).

### 6.2 Connection-attempt mechanics

For each candidate entry:
1. `esp_wifi_set_config(WIFI_IF_STA, …)` with that entry's ssid/password.
2. `esp_wifi_connect()`.
3. Wait up to 8s (configurable) for `IP_EVENT_STA_GOT_IP`.
4. On success: write `last_used_us = now`, `active_idx = i`, post `LL_EV_WIFI_CONNECTED`.
5. On timeout/fail: mark `recently_failed[i] = true` in RAM, return to PICKING.

The "recently failed" flag is cleared on reboot and on successful connect to *any* network — so a one-off auth blip doesn't permanently shadow a network.

### 6.3 Priority

**Last-used wins.** Pick the visible saved entry with the highest `last_used_us`. Simple, matches what every consumer device does, no UI for ordering. (If §10 Q1 lands the other way, swap to explicit user-set order — the storage layout already supports it via insertion-position semantics.)

---

## 7. Wire Protocol Changes

Net-new ops, plus a soft-deprecation of `set_wifi_creds`.

### 7.1 New: `list_wifi_networks`

Request: no payload.

Response:
```json
{
  "networks": [
    { "ssid": "HomeWiFi",   "is_active": true,  "last_used": 1745280000 },
    { "ssid": "OfficeWiFi", "is_active": false, "last_used": 1745190000 }
  ]
}
```

Passwords never leave the device. Order is by `last_used` desc.

### 7.2 New: `add_wifi_network`

Request payload: `{ ssid, password }`.

Behavior:
- If SSID already exists in the list → update the password in-place (same key replaces). Returns `ok`.
- If SSID is new and `count < N_MAX` → append. Returns `ok`.
- If `count == N_MAX` → returns `ok=false, error="wifi_list_full"`.

**Does not switch the active connection.** Adding ≠ joining. (For the provisioning-time first-network case, the existing code path that switches to STA stays in place — we just write to `ll_wifi[0]` in addition to `esp_wifi`.)

### 7.3 New: `remove_wifi_network`

Request payload: `{ ssid }`.

Behavior:
- Removes the entry. If the removed entry was the active connection, the mirror **proactively disconnects** (`esp_wifi_disconnect()`) and falls into the SCANNING flow. The app surfaces a confirmation dialog before sending this op when it knows the target is the active network — see §4.3.
- Returns `ok=false, error="not_found"` if the SSID isn't in the list.

### 7.4 Existing: `set_wifi_creds` (deprecated, kept working)

Behavior change: now equivalent to `add_wifi_network` followed by an immediate switch (the old SoftAP→STA handoff in [provisioning.c:300-375](Firmware/v1/core/provisioning/provisioning.c)). Stays on the protocol so older app builds keep working through the transition.

Mark it `deprecated: true` in the protocol spec; remove in V2.

### 7.5 `DeviceState` additions

```diff
 interface DeviceState {
   ...
-  wifi_ssid?: string | null;     // currently connected SSID
+  wifi_ssid?: string | null;     // currently connected SSID (unchanged)
+  wifi_saved_count?: number;     // entries in ll_wifi
 }
```

Full list lives behind `list_wifi_networks` to keep state broadcasts small — most clients don't need it.

---

## 8. App Changes (App/v1)

### 8.1 Settings page

Replace the single "Network: HomeWiFi  /  Reconfigure Wi-Fi" block with a list rendered from `list_wifi_networks` plus an "Add a network" button. Active row gets a checkmark.

Add/remove flows reuse the existing SSID/password form component — just wire it to the new ops.

### 8.2 First-run setup (unchanged)

Setup screen still calls `set_wifi_creds` for backward compatibility. Behind the scenes the firmware writes to `ll_wifi[0]` *and* loads it into `esp_wifi`, so the first network is automatically a saved network.

### 8.3 No mDNS dependency

Multi-network is independent of the deferred mDNS work. The Find-mirror subnet scan still works on whatever network the phone is currently on.

---

## 9. Implementation Order

Sized in rough day-units. Assumes the firmware feedback loop from `feedback_verify_each_change.md` (build → flash → boot capture → sprint_log) per step.

| Step | Scope | Days |
|---|---|---|
| 1 | NVS layer: `ll_wifi.{c,h}` with add/remove/list/get-by-ssid + the migration shim from §5.3 | 1 |
| 2 | Refactor `provisioning.c` to drive STA from `ll_wifi` instead of `esp_wifi`'s saved cred | 1.5 |
| 3 | Implement SCANNING / PICKING / BACKOFF state machine | 1.5 |
| 4 | Protocol ops: `list/add/remove_wifi_network` in `transport.c`; rewrite `set_wifi_creds` as a thin wrapper | 0.5 |
| 5 | App: Settings page list UI + add/remove flow | 1 |
| 6 | End-to-end testing across 2-3 real networks (home + phone hotspot) | 0.5 |

**Total: ~6 days.** Front-loaded firmware (steps 1-3 are the risky parts).

---

## 10. Locked Decisions (May 1)

All five questions answered. Recorded here for posterity so future readers can see what was considered, not just what was chosen.

| # | Question | Decision | Why |
|---|---|---|---|
| Q1 | Priority model | **Last-used wins** | Simplest. Matches consumer-device norms. Storage layout still supports explicit ordering if we want to revisit later. |
| Q2 | Capacity (N_MAX) | **4** | Realistic personal-use ceiling per Bill (home, work, parents, travel hotspot). ~424 B in NVS. Easy to bump if real usage proves it tight. |
| Q3 | No-network-visible behavior at boot | **Stay in BACKOFF indefinitely** — no auto-SoftAP. User must trigger pairing mode via recessed-button hold. | Avoid polluting the space with unnecessary RF when the device is just out of range or the user doesn't intend to re-provision. Aligns with the product's broader "less RF noise unless asked" stance. |
| Q4 | Removing the currently-active network | **Proactive disconnect**, gated by a confirm dialog in the app. | If the user says forget, respect it immediately rather than the iOS-style "stay connected until natural disconnect" pattern, which can read as a bug. The dialog covers the "I didn't mean to disconnect" case. |
| Q5 | `set_wifi_creds` deprecation timeline | **Remove with the next firmware major** (V2). App keeps it as the first-network setup path until then. | Lets older app builds keep working through the rollout; clean break at the next NVS schema bump. |

---

## 11. References

- [control-protocol-spec.md](control-protocol-spec.md) — wire envelope this doc extends
- [firmware-spec.md](firmware-spec.md) — overall firmware module ownership
- [Firmware/v1/core/provisioning/provisioning.c](Firmware/v1/core/provisioning/provisioning.c) — current single-cred implementation
- [Firmware/v1/core/nvs/nvs.h](Firmware/v1/core/nvs/nvs.h) — namespace ownership boundary (this doc adds `ll_wifi`, leaves `ll_settings` alone)
- [App/v1/App.tsx:566-577](App/v1/App.tsx) — current single-network Settings UI being replaced
