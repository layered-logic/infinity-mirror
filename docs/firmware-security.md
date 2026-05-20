---
title: Firmware Security Model
type: engineering
phase: 2
week: 4
date: 2026-04-22
status: draft — initial lock from firmware-architecture-scoping §5.4 decision
tags: [engineering, firmware, security, ota, auth, privacy]
---

# Firmware Security Model

**Prepared by:** William White
**Date:** April 22, 2026
**Status:** First draft — captures locked decisions from Apr 22 firmware architecture block

---

## 1. Purpose

This doc captures every security-shaped decision in the ESP32 firmware stack. Not a theoretical threat model — a concrete list of what's signed, what's authenticated, what's encrypted, what's stored, and what's sent off-device.

Source-of-truth for:
- OTA signing + rollback behavior
- Device-side authentication for app control
- Telemetry + privacy posture
- Secrets handling (keys, HMAC, NVS)

Out of scope:
- Threat model against nation-state adversaries (not the target)
- Secure Boot v2 (deferred — see §3.2)
- Hardware attack resistance (user-serviceable device, not a hardened token)

---

## 2. Security Principles

1. **Honest posture.** The mirror is a consumer light product on a home network. Protect against remote compromise of the OTA channel, protect user privacy, and be clear about what we don't protect against.
2. **User agency over auth.** Whether the local-control surface requires a password is a **user decision at provisioning time**, not a forced default. See §5.
3. **Minimum necessary data off-device.** Telemetry is opt-in, documented, and as small as possible. No phone-home on opt-out devices.
4. **No eFuse burn in V1.** Anti-rollback counter + signed-app verification use the existing eFuse infrastructure, but full Secure Boot v2 (which permanently locks the device to one signing key) is deferred. Keeps repair + signing-key rotation possible.
5. **Compromise is recoverable.** Every security state must have a path back to known-good: bad OTA → auto-rollback; lost HMAC secret → factory reset; compromised private key → re-issue + force-update via anti-rollback counter.

---

## 3. OTA Security

Implements [firmware-architecture-scoping §5.4](firmware-architecture-scoping.md#54-ota-update-channel).

### 3.1 Source

- **Endpoint:** `https://ota.layeredlogic.cc/v1/check`
- **Hosting:** Cloudflare Worker + R2 bucket. TLS terminated at Cloudflare edge (free TLS, auto-renewed).
- **Binary delivery:** Worker returns either `{"update": false}` or `{"update": true, "url": "<signed R2 URL>", "version": "x.y.z", "signature": "..."}`.
- **Signed URLs:** short-lived (5 min) pre-signed R2 URLs. Prevents hotlinking but the binary itself is the security boundary — anyone who intercepts a URL still can't modify the bytes without invalidating the signature.

### 3.2 Signing

- **Algorithm:** ECDSA P-256 (SHA-256). ESP-IDF supports this natively for signed-app verification.
- **Private key:** stored offline in a password manager (1Password vault "LL Firmware"). Never on any server, never in the repo, never in CI env. Manual signing step in the release flow.
- **Public key:** compiled into the bootloader via `CONFIG_SECURE_BOOT_V2_PUBLIC_KEY_PATH`. Same key ships in every device in a given generation.
- **What's enabled:** `CONFIG_SECURE_BOOT_V2=y` + signed-app verification (bootloader refuses to boot an app whose signature doesn't match the compiled-in public key).
- **What's NOT enabled:** Secure Boot v2 eFuse burn. The `CONFIG_SECURE_BOOT` eFuse bit stays unburned in V1. Signed-app verification still happens, but the bootloader itself can be replaced via JTAG/UART (intentional — keeps repair possible).

**Key rotation plan:** if the private key is ever compromised, ship an OTA that includes a new bootloader with the new public key + bump the anti-rollback counter. Devices that already updated are safe; devices still on a compromised key can only receive binaries signed with the old key (the compromise window).

### 3.3 Partition scheme

A/B OTA with `ota_0` + `ota_1`.

| Variant | Flash | Factory | ota_0 | ota_1 | NVS + PHY |
|---|---|---|---|---|---|
| **Pro (C3, 4MB)** | 4MB | 1.2MB | 1.2MB | 1.2MB | 0.4MB |
| **Pro+ (C6, 8MB)** | 8MB | — skipped — | 3.0MB | 3.0MB | 2.0MB |

Pro+ skips factory to give the Matter stack room. On Pro+, factory reset behavior is implemented by wiping NVS and reflashing defaults via the running ota app, not by booting to a factory partition. Recovery posture: if both A and B are corrupt, the device needs USB-C reflash (accepted tradeoff for the 8MB variant — expected rare).

Both variants: single application size ceiling = flash / 2 − overhead. Currently comfortable (Pro core + standard build ~600KB; Pro+ Matter build ~1.8MB).

### 3.4 Rollback protection

- **`CONFIG_BOOTLOADER_APP_ROLLBACK_ENABLE=y`** — after a new app boots, it must call `esp_ota_mark_app_valid_cancel_rollback()` within 60 seconds (configurable) or the bootloader reverts to the previous app on next boot. Guards against a shipped build that bricks the device.
- **Validation function runs after:**
  - Wi-Fi reconnects to saved network (proves provisioning still works)
  - Minimum one LED frame renders (proves the driver loads)
  - WebSocket server binds to port 80 (proves HTTP/WS transport loads)
- If any of those fail within 60s, the device stays unreleased → reboot → bootloader rolls back.
- **`CONFIG_BOOTLOADER_APP_ANTI_ROLLBACK=y`** — monotonic version counter in eFuse (eFuse_BLK3). Each release increments the counter. Bootloader refuses to boot any app with a counter < the current eFuse value. Prevents an attacker from downgrading a patched device to an old vulnerable version.

Anti-rollback budget: eFuse has 256 bits for the counter. At one increment per serious security release, that's 256 releases before we run out — effectively forever.

### 3.5 Staged rollout

Server-side, not device-side. The device just asks "is there an update for me?" and the server decides.

- Each device has a **bucket 0–99** derived from SHA-256 hash of the device ID (manufacturing-assigned, stored in NVS).
- New build published with rollout policy: `{"version": "1.2.3", "rollout": 1}` (1%)
- Server compares device bucket to rollout percentage. If bucket < rollout, device sees the update.
- Typical ramp: **1% → 10% → 50% → 100%** over 72 hours, with manual gates between each step.
- Each step waits for telemetry (from opt-in devices) to show no new crash signals before advancing.

### 3.6 Check trigger

Per-device schedule, no fleet-wide sync:

- On boot after Wi-Fi connects (one-shot, ~30s delay after connect to avoid boot-time network congestion)
- Daily at a device-specific hour derived from `hash(device_id) % 24`, with ±30 min jitter per check

Rationale: without jitter, every device made in the same batch with sequential IDs hits the endpoint at nearly the same minute. With jitter, check traffic spreads across the day. The 24-hour hash offset also ensures a given device doesn't consistently overlap with the user's own network-heavy tasks (backups, scheduled downloads).

---

## 4. Telemetry & Privacy

### 4.1 Posture

**Opt-in only, default off.** User enables telemetry in the LL app during provisioning or later via settings. Clear disclosure: what's collected, why, and where it goes.

### 4.2 What telemetry collects (when opted in)

- `device_id` (manufacturing-assigned, not tied to user identity)
- `variant` (standard / matter)
- `fw_version`
- `hw_board` (c3_devkit / c6_devkit / prod_v1_pro / prod_v1_pro_plus)
- `uptime_s` — seconds since last boot
- `boot_reason` — the ESP-IDF `esp_reset_reason_t` enum (one of: power-on, watchdog, brownout, panic, etc.)
- `free_heap_min` — low watermark for free heap since boot
- `panic_log` — if the last boot was a panic: the filename + line + task name (no stack bytes, no addresses)
- `rssi` — Wi-Fi signal strength (for diagnosing pattern-stutter reports)

That's it. **Nothing** about pattern content, color choices, on/off schedule, what the app is showing, or who is connected.

### 4.3 What telemetry never collects

- User-defined patterns (stay on device + in app)
- Color / brightness settings
- App-connection events (who connected when)
- Wi-Fi SSID or BSSID
- IP addresses (dropped at ingest)
- Any audio/video/ambient-sensor data (the mirror has none, but worth stating)

### 4.4 Delivery

- POST to `https://telemetry.layeredlogic.cc/v1/beacon` once per 24h (same jittered schedule as OTA check)
- Request body ~300 bytes; on failure, drop silently (no retry queue, no persistence)
- TLS 1.2+, no client-cert — telemetry is signed low-value data, not authenticated sessions

### 4.5 User-visible controls

- **Enable / disable** in app settings — takes effect immediately
- **View last sent payload** in app — user can see exactly what went off-device in the most recent beacon
- **Wipe local telemetry buffer** — clears the pending beacon

### 4.6 Opt-in cohort = early rollout cohort

Staged rollout uses opt-in telemetry as the signal for advancing through rollout percentages. Explicit consequence: **opt-in devices get new builds first**, opt-out devices wait until the build is proven.

This is deliberate:
- Opt-in users are effectively beta testers. They've agreed to share diagnostics; they get the newest features first as a value exchange.
- Opt-out users get conservative, validated builds. Their privacy choice doesn't cost them anything except freshness.

---

## 5. Local Authentication

Implements [firmware-architecture-scoping §5.5](firmware-architecture-scoping.md#55-app--device-protocol). Full protocol in [control-protocol-spec.md](control-protocol-spec.md).

### 5.1 Two modes, user-selected

**Open mode (default):**
- No authentication on the local control surface (WebSocket, HTTP REST, BLE)
- Anyone on the local network can control the mirror
- Intended default because: shared-space devices should be controllable by anyone in the space (roommate, guest, family member)

**Paired mode (user opt-in):**
- User sets a shared secret (password / passphrase) at provisioning or later
- All control messages must include an HMAC-SHA256 of the payload, keyed on the shared secret
- Unsigned or incorrectly-signed messages rejected
- Secret stored in NVS (plaintext — see §5.3 tradeoff note)

### 5.2 Mode changes

- Any currently-authorized client can change the mode
- In open mode: any client can enable paired mode (and must supply the new secret in the transition message)
- In paired mode: only authenticated clients can disable paired mode

Trade-off accepted: an attacker already on your Wi-Fi in open mode can lock you out by enabling paired mode with a secret you don't know. Mitigated by: factory reset via recessed button always restores the device to unprovisioned state (wipes NVS including auth).

### 5.3 Secret storage

- Stored in NVS under namespace `ll_auth`, key `shared_secret`
- **Plaintext storage** — no flash encryption in V1. Rationale: enabling flash encryption costs an eFuse burn (one-way) and complicates repair/debug. The threat model for a local-network shared secret doesn't justify it. An attacker with physical flash access can already reflash the device.
- Revisit in V2 if a specific threat emerges.

### 5.4 HMAC envelope

A request frame in paired mode:

```json
{
  "op": "set_state",
  "req_id": "<uuid>",
  "ts": 1745280000,
  "payload": { "color": "#3214FF", "brightness": 80 },
  "hmac": "<hex-encoded HMAC-SHA256>"
}
```

*Implemented in [LL-057-D](../tasks.md#LL-057-D). The replay model below
diverges from the original ±60 s design — see the SNTP note.*

**What the HMAC covers — `hmac`-last canonicalization.** JSON has no
canonical byte form, so device and client agree on a structural rule
rather than a re-serialization step: `hmac` is always the final key, and
the signed bytes are exactly the frame text *before* the `,"hmac":`
token. The client builds the `{op, req_id, ts, payload}` object,
serializes it, drops the trailing `}`, HMAC-SHA256s that brace-less
string, then transmits `<signed-string>,"hmac":"<hex>"}`. The device
HMACs the received prefix, so the two byte strings match with no parsing
on the signing path. The signed region is deliberately not itself valid
JSON; HMAC signs bytes, not JSON.

**The gate.** When `auth_mode` is `paired`, the transport dispatcher
verifies every inbound frame before dispatching its op. Failures map to
[control-protocol-spec §8](control-protocol-spec.md#8-error-codes) error
codes: `auth_required` (no `hmac`), `bad_hmac` (signature did not
verify, or paired with no secret stored), `bad_payload` (missing
`ts`/`req_id`). Open mode skips the gate entirely.

**Replay protection — recency, not a wall clock.** V1 has no SNTP and no
RTC, so the original "reject `ts` more than 60 s from device time"
window is not implementable and was dropped. It is replaced by two
recency checks that need no wall clock:

- **Per-socket monotonic `ts`.** Each WebSocket connection tracks the
  highest `ts` it has accepted; a frame with a lower `ts` is rejected
  (`stale_ts`). Stops in-band replay and reordering on a live
  connection. Reset to 0 on each new handshake.
- **Device-wide `req_id` dedup.** A ring of the 32 most recently
  accepted `req_id`s. A frame whose `req_id` is still in the ring is
  rejected (`stale_ts`). Stops a captured frame being replayed on a
  fresh connection, where the per-socket `ts` guard has reset.

`ts` is therefore a client-supplied monotonic counter, not an
authenticated wall-clock value — the client may use epoch seconds or any
non-decreasing sequence.

**Residual window — stated honestly.** A captured signed frame can still
be replayed *if* its `req_id` has aged out of the 32-entry ring *and*
the replay is sent on a new connection; a device reboot clears both
guards. Given the threat model (local network, non-sensitive mirror
state — §5.5) this is proportionate: a successful replay re-applies a
stale, already-authorized command, not privilege escalation or data
disclosure.

**Client coverage.** Paired-mode signing is implemented in both the LL
mobile app ([App/v1/](../App/v1/), LL-057-D) and the device-hosted
webapp ([Firmware/v1/webapp/](../Firmware/v1/webapp/), LL-078). Both
vendor the same pure-JS SHA-256/HMAC rather than using WebCrypto: the
app's Hermes engine has no dependable `crypto.subtle`, and the webapp is
served over plain `http://` (not a secure context), where `crypto.subtle`
is unavailable too. The webapp persists the secret in `localStorage` so a
paired mirror doesn't re-prompt every page load; the mobile app holds it
in memory only.

### 5.5 What auth does NOT protect

- **Eavesdropping** — messages are plaintext JSON over plain ws:// or http:// on local network. Anyone with Wi-Fi password already sees the traffic. Mirror state is not sensitive.
- **Denial-of-service** — a neighbor who guessed/learned your Wi-Fi password can spam WebSocket connections. Acceptable threat; the damage is "mirror gets weird." Mitigated by: WebSocket server enforces max concurrent client count + per-IP rate limit.
- **Physical access** — anyone with USB-C access to the device can factory-reset it and get full control. Intentional: right-to-repair posture.

---

## 6. Secrets & Keys Inventory

| Secret | Location | Scope | Rotation |
|---|---|---|---|
| OTA signing private key | Offline (1Password "LL Firmware") | One key per firmware generation (V1) | Rotate only on compromise; ships new bootloader via OTA |
| OTA signing public key | Compiled into bootloader image | Per-device (same in all V1 devices) | See above |
| Device ID | NVS, manufacturing-assigned | Per device | Never |
| LL shared secret (paired mode) | NVS `ll_auth/shared_secret`, plaintext | Per device, user-controlled | User-triggered |
| Wi-Fi credentials | NVS (stored by `wifi_prov_mgr`) | Per device | Re-provisioning |
| Matter fabric credentials (Pro+) | NVS, managed by `esp-matter` | Per fabric | Matter commissioning / decommissioning |

---

## 7. Recovery Paths

What the user can do when things are wrong:

| Situation | Recovery |
|---|---|
| Bad OTA bricks boot | Auto-rollback via `CONFIG_BOOTLOADER_APP_ROLLBACK_ENABLE` |
| Old firmware has CVE, user on LTS channel | Force an update via staged rollout + bump of anti-rollback counter |
| Lost paired-mode password | Factory reset via recessed button (10s hold) — wipes NVS including auth |
| Device compromised / weird behavior | Factory reset; mirror returns to unprovisioned state |
| Signing key compromised (ours) | Rotate key, ship new bootloader via OTA, bump anti-rollback counter |
| Lost signing key (ours) | Device fleet can no longer receive updates. Mitigations: 1Password backup, paper cold-storage backup of the key |
| USB-C physical brick | Reflash via UART + esptool (documented public repair path per [right-to-repair §3](right-to-repair-philosophy.md#3-what-i-commit-to)) |

---

## 8. What We Deliberately Don't Do

- **No Secure Boot v2 eFuse burn.** Keeps signing-key rotation possible and keeps repair/debug open. Signed-app verification still happens.
- **No flash encryption.** No secrets worth the eFuse cost live in NVS.
- **No cloud session tokens, no OAuth, no account system.** V1 is local-network only.
- **No remote management channel.** Device does not hold a persistent connection to any server. It polls OTA + telemetry endpoints on a jittered schedule and otherwise sits idle on the local network.
- **No per-device TLS.** Generating + rotating per-device certs for `ws://` upgrade is the kind of infra that fights us until we have users. Deferred to V2 if/when a hosted webapp needs to reach devices over `wss://`.

---

## Related

- [Firmware Architecture Scoping](firmware-architecture-scoping.md) — §5.4 and §5.5 are this doc's source of truth
- [Control Protocol Spec](control-protocol-spec.md) — defines the HMAC envelope used in §5.4
- [Right-to-Repair Philosophy](right-to-repair-philosophy.md) — §3 and §4 constrain security-vs-repairability tradeoffs
- [Button Interface Design](button-interface.md) — §5 factory-reset gesture is the universal recovery lever
