---
title: Telemetry Worker — deploy + test
type: engineering
phase: 2
tags: [server, cloudflare, telemetry, LL-057-C]
---

# `ll-mirror-telemetry` Cloudflare Worker

Accepts opt-in telemetry beacons from Layered Logic mirrors on
`telemetry.layeredlogic.cc/v1/beacon`. Stores each beacon in a KV
namespace with a 90-day TTL plus a no-TTL "latest snapshot per device"
pointer. Firmware emission lives in [firmware-spec §4.10](../../docs/firmware-spec.md),
implementation tracked by [LL-057-C](../../tasks.md#LL-057-C).

## First-time setup

```powershell
cd cloudflare-workers/telemetry
npm install                                  # pull wrangler + workers-types
npx wrangler login                           # browser auth into Bill's CF account
npx wrangler kv namespace create LL_TELEMETRY_KV
# → copy the id into wrangler.toml's [[kv_namespaces]] block
npx wrangler deploy                          # first deploy publishes the worker
```

Then in the Cloudflare dashboard:
1. Add the route `telemetry.layeredlogic.cc/v1/*` → Worker `ll-mirror-telemetry`.
2. Add a DNS CNAME `telemetry` → `<workers-subdomain>.workers.dev` (or use Custom Domain).
3. Verify with the healthz endpoint:
   ```powershell
   curl https://telemetry.layeredlogic.cc/v1/healthz
   # → {"ok":true,"service":"ll-mirror-telemetry"}
   ```

## Beacon schema

```json
{
  "v": 1,
  "device_id": "b2332c",
  "fw_version": "651afe8-dirty",
  "uptime_s": 12345,
  "heap_free_min": 89000,
  "boot_reason": "POWERON_RESET",
  "rssi": -52,
  "led_count": 66,
  "panic": {
    "filename": "main.c",
    "line": 42,
    "task_name": "state_bus"
  }
}
```

- `device_id` is the firmware's `ll_device_id_get()` — lowercase 6-hex MAC suffix.
- `fw_version` matches `esp_app_get_description()->version`.
- `panic` is omitted when no panic happened since the last successful beacon.

The Worker validates shape strictly (rejects garbage with 400) but
tolerates extra fields silently for forward-compatibility.

## Endpoints

| Method | Path | Purpose |
|---|---|---|
| `POST` | `/v1/beacon` | Submit a beacon. Returns `{ok, stored_at}` or `{ok:false, error}`. |
| `GET` | `/v1/healthz` | Liveness probe. Always 200. |
| `GET` | `/v1/latest/<device_id>` | Return the latest beacon for one device. 404 if none yet. |

## Test with a mock beacon

```powershell
curl -X POST https://telemetry.layeredlogic.cc/v1/beacon `
  -H 'content-type: application/json' `
  --data '{"v":1,"device_id":"b2332c","fw_version":"mock-0.0.1","uptime_s":42,"heap_free_min":80000,"boot_reason":"POWERON_RESET","rssi":-55,"led_count":66}'

# Then:
curl https://telemetry.layeredlogic.cc/v1/latest/b2332c
```

## Local dev (no real KV writes)

```powershell
npx wrangler dev
# → http://127.0.0.1:8787
curl -X POST http://127.0.0.1:8787/v1/beacon -H 'content-type: application/json' --data '{...}'
```

Local KV is miniflare-backed and ephemeral. To reset: stop + restart `wrangler dev`.

## Storage model

| Key pattern | TTL | Purpose |
|---|---|---|
| `beacon:<device_id>:<unix_ms>` | 90 days | One entry per submitted beacon. Enables future "last N beacons" / time-series queries via `LL_TELEMETRY_KV.list({prefix:"beacon:<id>:"})`. |
| `latest:<device_id>` | none | Most recent beacon for O(1) "is device alive + what was its state?" lookups. |

## Privacy / data retention

- Beacons are **opt-in** via the app's Settings → Telemetry toggle (`set_state {telemetry_enabled: true/false}`).
- `device_id` is a pseudonymous 6-hex MAC suffix; not linked to user identity.
- No location data, no IP retention (CF logs hold IPs briefly but we don't persist).
- 90-day TTL on per-beacon entries; latest pointer retained until overwritten by a fresh beacon.

## Auth

None on `POST /v1/beacon` for V1. The worst an attacker with a known `device_id` can do is submit false metrics. Real authentication should land alongside [LL-057-D](../../tasks.md#LL-057-D) (paired-mode HMAC), at which point the beacon envelope can carry an HMAC computed against the device's shared secret. Until then, ship as open and trust the Cloudflare-level rate limiting.

## Not yet built (deferred)

- Dashboard UI to view recent beacons.
- Alerting on panic payloads.
- Aggregation queries (heap LWM trend across devices, etc.).
- Data export (R2 dump for long-term cold storage).
