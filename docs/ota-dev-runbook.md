---
title: OTA Dev Runbook — Pushing Firmware Over Wi-Fi
type: engineering
phase: 2
status: living doc — update when the production OTA path lands
tags: [engineering, firmware, ota, runbook]
---

# OTA Dev Runbook — Pushing Firmware Over Wi-Fi

How to update a Layered Logic mirror's firmware over Wi-Fi alone (USB unplugged) while iterating in a coding session. Covers the **dev-test path** only — the production OTA workstream (signed binaries, `ota.layeredlogic.cc`, anti-rollback eFuse, staged rollout) is parked until the production hardware safety rules allow eFuse burns. See [firmware-architecture-scoping §5.4](firmware-architecture-scoping.md) for the production path.

## When to use this

Use the OTA path when:
- The mirror is powered up, on a real Wi-Fi network the dev PC can reach, and not in SoftAP/provisioning mode.
- You have firmware changes you want to flash and the device is physically inaccessible (in a frame, behind acrylic, on a ceiling, etc.) **or** USB is busy / already unplugged.
- You're not changing config that needs a re-flash through the bootloader (e.g., new partition table — those still need USB).

Use USB instead when:
- Schema-incompatible NVS partition layout changes (rare, but partition table edits can't be OTA'd).
- The current OTA-running firmware is broken in a way that prevents accepting `start_ota`.
- You need real-time serial logs to diagnose a panic (`idf.py monitor`).

## Prerequisites

- ESP-IDF environment exported in the current shell. PowerShell only (per [reference_esp_idf_setup memory](.claude/projects/.../reference_esp_idf_setup.md)):
  ```powershell
  $env:IDF_TOOLS_PATH = "C:\Espressif"
  & "C:\Espressif\frameworks\esp-idf-v5.5.4\export.ps1"
  ```
- The `websockets` Python package available in the IDF venv:
  ```powershell
  & "C:\Espressif\python_env\idf5.5_py3.14_env\Scripts\python.exe" -m pip install websockets
  ```
- Dev PC and mirror on the **same Wi-Fi subnet** with **routable HTTP** between them. The mirror downloads the binary from the dev PC's HTTP server — they need a clean L3 path on port 8000.

## Step 1 — Find the mirror's IP

The mirror doesn't advertise a fixed IP — DHCP assigns one on each network. Try in this order, stop at the first that works:

### 1a. ARP cache (fastest if the dev PC has talked to the mirror recently)

The webapp loads from `http://<mirror-ip>/`, so any browser session against the mirror puts an ARP entry on the dev PC. The mirror's MAC starts with the ESP-WROOM/ESP32-C3 OUI; in practice the *suffix* (last 3 bytes) matches the device id printed on the SoftAP SSID and in mDNS:

```powershell
arp -a | Select-String '192\.168\.|10\.'
```

Look for a row whose MAC suffix matches the expected hardware id (e.g., `b2-33-2c` → device id `b2332c`). The IP next to it is the mirror.

**Pitfall**: ARP entries age out after a few minutes of no traffic. If the cache is empty, ping the mirror's known mDNS hostname first (`ping layered-logic-mirror-<id>.local`) to repopulate it, or fall through to 1b.

### 1b. Subnet scan via `/api/info`

Available since Apr 30 — the mirror's `GET /api/info` returns `{product, id, name, fw_version}`, which is both the "is this a mirror" sentinel and the identity payload the LL app uses for its picker.

```powershell
# Replace 192.168.29 with whatever /24 your dev PC is on (Get-NetIPAddress).
$jobs = 1..254 | ForEach-Object {
    $ip = "192.168.29.$_"
    Start-Job -ScriptBlock {
        param($ip)
        try {
            $r = Invoke-WebRequest -Uri "http://$ip/api/info" -TimeoutSec 2 -UseBasicParsing -ErrorAction Stop
            if ($r.StatusCode -eq 200 -and $r.Content -like '*layered-logic-mirror*') { return "$ip => $($r.Content)" }
        } catch {}
    } -ArgumentList $ip
}
$jobs | Wait-Job -Timeout 12 | Out-Null
$jobs | ForEach-Object { Receive-Job -Job $_ } | Where-Object { $_ }
$jobs | Remove-Job -Force
```

**Pitfalls**:
- `Wait-Job -Timeout` will kill in-flight jobs whose IP hasn't responded yet. **Set the wait at least 5× the per-job timeout** (12s for 2s probes is the floor; bump higher on flaky Wi-Fi). The Apr 30 OTA session lost the mirror to this exact race — first scan wait was 5s, mirror was at `.61` and never got polled.
- 254 parallel `Start-Job` invocations is heavy on Windows. If the dev PC is loaded, drop to a `ForEach-Object -Parallel` loop with `-ThrottleLimit 32`.
- **Pre-Apr-30 firmware doesn't have `/api/info`.** If the mirror is running older firmware and you can't tell from history, fall back to probing `GET /` and matching `<title>Layered Logic Mirror</title>` instead.

### 1c. mDNS (most reliable when it works, but works inconsistently)

```powershell
# Resolve via Windows DNS (mDNS responder must be installed — Bonjour or
# the built-in Windows 10+ stack):
Resolve-DnsName layered-logic-mirror-<id>.local -Type A
```

**Pitfall**: mDNS resolution on Windows is finicky and can hang for 5+ seconds before failing. Use it as a backup, not the primary lookup.

### 1d. Ask the user

If the mirror is currently controlled by the LL app or the webapp, the URL bar / app's "Mirror URL" field has the IP. Always cheaper than guessing.

## Step 2 — Build the new firmware

```powershell
cd Firmware/v1
idf.py -DLL_BOARD=c3_devkit build
```

**Verify the embedded version string before flashing.** ESP-IDF derives `fw_version` from `git describe`, with `-dirty` appended for uncommitted changes. After OTA you'll use that string to confirm the new image is actually running, so it has to be distinct from the currently-running firmware:

```powershell
$bin = "Firmware/v1/build/layered_logic_mirror_standard.bin"
$bytes = [System.IO.File]::ReadAllBytes($bin)
$ascii = [System.Text.Encoding]::ASCII.GetString($bytes)
[regex]::Matches($ascii, '[a-f0-9]{7,}-?(dirty)?').Value | Select-Object -Unique -First 5
```

**Pitfalls**:
- If the new and currently-running firmware both report e.g. `f1db077-dirty`, you can't distinguish them post-OTA. Either commit the change first (gives a new short SHA) or rely on probing for a new endpoint / state field unique to the new code.
- Binary slot is `0x140000` (1.25MB) — current builds run ~91% utilization. If `idf.py build` reports < 5% free, address before adding more features (drop unused IDF components from `REQUIRES`, or expand the slot from the 128KB top-of-flash reserve).

## Step 3 — Host the binary on the dev PC

The mirror downloads from a plain HTTP URL; the dev PC needs to serve `build/layered_logic_mirror_standard.bin` on a port reachable from the mirror's subnet.

### 3a. Kill stale HTTP servers first

A single port-8000 process from an earlier session can serve **the wrong binary** silently — you trigger OTA, the server log shows a 200, and the mirror reboots into the same firmware it had before. Always check for leftovers before starting fresh:

```powershell
Get-NetTCPConnection -LocalPort 8000 -ErrorAction SilentlyContinue |
    Select-Object LocalAddress, OwningProcess
# If anything's listed, kill it:
Stop-Process -Id <PID> -Force
```

### 3b. Start the server

```bash
cd Firmware/v1/build && python -u -m http.server 8000 --bind 0.0.0.0
```

Use `-u` so stdout flushes per request — without it, access logs buffer and you can't tell in real-time whether the mirror actually hit the server.

**Pitfalls**:
- Python's `http.server` writes access logs to **stderr**, not stdout. If you're running it in a backgrounded shell that only captures stdout, redirect with `2>&1` so you can inspect the log later.
- The server binds to whatever directory it was launched from. If you `cd` into `build/` and start it, it serves only that dir's files. Don't start it from the project root and expect to fetch `/build/...` — relative paths matter.

### 3c. Confirm reachability from the LAN side, not just localhost

```powershell
# From the dev PC itself (loopback — proves the process is up but not that
# the LAN can reach it):
Invoke-WebRequest -Uri "http://<dev-pc-lan-ip>:8000/layered_logic_mirror_standard.bin" -Method Head -UseBasicParsing
```

If `Test-NetConnection` returns true but the actual OTA fails with no entry in the access log, check the **Windows Firewall network category** of the active Wi-Fi interface:

```powershell
Get-NetConnectionProfile | Select-Object Name, NetworkCategory, InterfaceAlias
```

If `NetworkCategory` is `Public`, inbound TCP 8000 is **blocked by default**. Three remediation options, in order of preference:

1. **Add a scoped firewall rule** (admin required, reversible, doesn't change the network's trust level):
   ```powershell
   New-NetFirewallRule -DisplayName "ESP32 OTA dev (port 8000)" `
       -Direction Inbound -Action Allow -Protocol TCP -LocalPort 8000 `
       -Profile Public -RemoteAddress 192.168.29.0/24
   ```
2. **Reclassify the network as Private** (admin required, persists for that SSID): `Set-NetConnectionProfile -InterfaceAlias 'Wi-Fi' -NetworkCategory Private`. Only do this on networks you trust as your own LAN.
3. **Tunnel via Tailscale** (no firewall surgery, but the mirror has to be on Tailscale too — not currently set up).

## Step 4 — Trigger OTA over WebSocket

Send `start_ota {url}` to the mirror's `/ws`. The device opens an HTTP client to the dev PC, downloads, validates, swaps the active partition, and `esp_restart()`s. Per [transport.c](../Firmware/v1/core/transport/transport.c), on success the WS socket dies before the response is sent — that's the success signal, not an error.

The repo's [scripts/ws_probe.py](../Firmware/v1/scripts/ws_probe.py) helper is convenient but PowerShell mangles JSON arguments. **Inline Python is more reliable**:

```powershell
& "C:\Espressif\python_env\idf5.5_py3.14_env\Scripts\python.exe" -c "
import asyncio, json, websockets
async def main():
    # ping_interval=None — disable WS-protocol keepalive so the client doesn't
    # drop the socket while the device is busy in ll_ota_start (10-30s blocking
    # call). Without this, the websockets library will close the connection
    # mid-OTA and you'll see 'keepalive ping timeout' that masks the real signal.
    async with websockets.connect('ws://192.168.29.61/ws', ping_interval=None, close_timeout=5) as ws:
        env = {'op':'start_ota','req_id':'ota1','ts':0,
               'payload':{'url':'http://192.168.29.176:8000/layered_logic_mirror_standard.bin'}}
        await ws.send(json.dumps(env))
        try:
            print('RESPONSE:', await asyncio.wait_for(ws.recv(), 90))
        except asyncio.TimeoutError:
            print('TIMEOUT — device may have rebooted (success path) or hung')
        except websockets.ConnectionClosed as e:
            print('CONN_CLOSED (expected on success):', e.code, repr(e.reason))
asyncio.run(main())
"
```

**Pitfalls**:
- `ping_interval=None` is non-negotiable. The default 20s WS-protocol ping will fire mid-download, the device's HTTP server task is busy with OTA so it can't respond, and the `websockets` library closes the socket and reports "keepalive ping timeout". The OTA usually succeeds anyway, but you've lost the response channel.
- PowerShell strips inner quotes from JSON arguments. Either use inline `python -c` (above) or pass the payload via a temp file.
- The dev-test OTA accepts plain `http://` URLs because [sdkconfig.defaults](../Firmware/v1/sdkconfig.defaults) sets `CONFIG_ESP_HTTPS_OTA_ALLOW_HTTP=y`. The production path will require HTTPS — flip that flag when wiring up `ota.layeredlogic.cc`.

## Step 5 — Verify the new firmware is running

This is the step the Apr 30 session got wrong twice. **Don't trust the first WS read after a reboot** — sockets can serve stale cached responses from before the reboot. Verify with multiple independent transports:

### 5a. Watch the HTTP server access log

The most direct evidence the device pulled the binary at all:

```
192.168.29.61 - - [30/Apr/2026 13:23:45] "GET /layered_logic_mirror_standard.bin HTTP/1.1" 200 -
```

No log entry → device never reached the server (firewall, wrong IP, captive portal redirect, etc.). Stop here and fix the network path.

### 5b. Probe an HTTP endpoint unique to the new firmware

If the new firmware adds or changes a non-WS endpoint, hit it directly. HTTP requests don't reuse stale sockets the way persistent WS connections do:

```powershell
(Invoke-WebRequest -Uri "http://<mirror-ip>/api/info" -UseBasicParsing).Content
```

Check that:
- `fw_version` matches the string you grep'd from the new binary in Step 2.
- Any new fields you added (e.g., on Apr 30 we added `id` + `name` to `/api/info`) are present.

### 5c. Open a *fresh* WS connection and ping

Only after 5b passes — and crucially, **don't reuse the OTA-trigger WS client** for verification:

```powershell
& "C:\Espressif\python_env\idf5.5_py3.14_env\Scripts\python.exe" -c "
import asyncio, json, websockets
async def main():
    async with websockets.connect('ws://<mirror-ip>/ws') as ws:
        await ws.send(json.dumps({'op':'ping','req_id':'p','ts':0}))
        print(await asyncio.wait_for(ws.recv(), 5))
        await ws.send(json.dumps({'op':'get_state','req_id':'gs','ts':0}))
        print(await asyncio.wait_for(ws.recv(), 5))
asyncio.run(main())
"
```

Cross-check: `fw_version` from ping, all expected fields in `get_state`, low `uptime_s` (< 2 minutes proves the reboot took).

## Step 6 — Cleanup

```powershell
# Stop the dev HTTP server.
Stop-Process -Id (Get-NetTCPConnection -LocalPort 8000 -ErrorAction SilentlyContinue | ForEach-Object OwningProcess) -Force
```

If you added a firewall rule in 3c:
```powershell
Remove-NetFirewallRule -DisplayName "ESP32 OTA dev (port 8000)"
```

## Pitfalls Cheat Sheet

| Symptom | Root cause | Fix |
|---|---|---|
| Subnet scan returns "no mirrors" but mirror is reachable from a browser | `Wait-Job` timeout too short — scan killed before slow IPs polled | Bump `-Timeout` to ≥ 5× per-job timeout, or use ARP cache lookup |
| `set_state` works from webapp but `start_ota` fails with TIMEOUT | WS-protocol ping closing the socket mid-download | Pass `ping_interval=None` to `websockets.connect()` |
| OTA "succeeds" but post-reboot ping shows old `fw_version` | (a) WS socket reuse → stale cached response, OR (b) stale `python -m http.server` was serving a different binary | Verify via fresh HTTP probe to a new endpoint first; check `Get-NetTCPConnection -LocalPort 8000` for leftover servers |
| HTTP server access log empty, OTA times out | Windows Firewall blocking inbound on Public network profile | `New-NetFirewallRule ... -Profile Public -LocalPort 8000`, or reclassify network as Private |
| Device reboots fine, but user-set state (color, brightness, name, pattern) reset to defaults | NVS schema bump in the new firmware — old blob discarded by `ll_nvs_deserialize` schema-version check | Expected, by design — see [nvs_logic.h](../Firmware/v1/core/nvs/nvs_logic.h). Wi-Fi creds survive (separate namespace). For shipped users, this is the trigger to add a real migration path. |
| `idf.py build` succeeds but binary doesn't include latest source changes | ccache or stale build artifacts from a previous worktree | `idf.py fullclean && idf.py build` |
| `ws_probe.py` errors with `JSONDecodeError` | PowerShell stripped the inner quotes from the JSON payload arg | Use inline `python -c` or write payload to a file and `Get-Content` it |

## Related

- [docs/control-protocol-spec.md §4](control-protocol-spec.md) — `start_ota` op definition + envelope shape
- [docs/control-protocol-spec.md §7.2](control-protocol-spec.md) — `GET /api/info` discovery payload
- [Firmware/v1/core/ota/ota.c](../Firmware/v1/core/ota/ota.c) — `ll_ota_start()` implementation
- [Firmware/v1/core/nvs/nvs_logic.h](../Firmware/v1/core/nvs/nvs_logic.h) — schema versioning rules
- [docs/firmware-architecture-scoping.md §5.4](firmware-architecture-scoping.md) — the production OTA path this dev runbook is the throwaway predecessor to
