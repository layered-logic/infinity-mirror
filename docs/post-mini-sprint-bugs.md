---
title: Post-Mini-Sprint Bug Backlog
type: engineering
phase: 2
opened: 2026-04-29
status: tracking
tags: [bugs, backlog, mini-sprint, firmware, webapp]
---

# Post-Mini-Sprint Bug Backlog

Bugs and rough edges deferred during the [Apr 28 → May 5 app demo mini-sprint](mini-sprint-app-demo.md) to keep the demo-readiness path unblocked. Triage these after the May 5 faculty advisor demo lands.

Each entry should carry: severity, surfaces, first observed, hypothesized cause, reproduction, possible fixes. Severity uses **High** (blocks demo / customer-visible / data-loss risk), **Medium** (visible but recoverable / ergonomic), **Low** (cosmetic / rare).

---

## Open

### #1 — Transient `socket closed` errors under user load

**Severity:** Low (cosmetic + auto-recoverable)
**Surfaces:** Webapp during rapid clicks across multiple controls; will likely also surface in the RN app once Sessions 6-8 ship
**First observed:** Session 4, 2026-04-29 ([sprint_log.md Week 5](../sprint_log.md#week-5-apr-28--may-5-app-uxui-design--app-demo-mini-sprint))

**Symptom**
"socket closed" or "not connected" briefly flashes in the connection-status panel of the webapp when the user clicks several controls in rapid succession. The webapp's reconnect-with-backoff (1s → 2s → 4s → 8s capped, [Firmware/v1/webapp/src/ws-client.ts](../Firmware/v1/webapp/src/ws-client.ts)) recovers within 1-8s and the demo stays functional, but the error is visible long enough to be noticed.

**Initial discovery**
The bug surfaced under the brightness slider's first implementation (live-during-drag with a 100ms throttle, ~10 msg/sec). Backing off to commit-on-release sliding eliminated the *bulk* of the issue, but it can still trigger when the user clicks color presets or toggles power+pattern in fast sequence — i.e. the issue isn't slider-specific, it's load-tolerance-of-the-firmware.

**Hypothesized root cause**
`esp_http_server`'s WS handler is single-task and serializes both inbound dispatch *and* outbound broadcasts. Any state change (even one inbound `set_state`) triggers a fanout to all connected clients via `broadcast_state` ([transport.c](../Firmware/v1/core/transport/transport.c)), so a single inbound message produces N+1 frames worth of work on that one task. Add network jitter on the SoftAP and the framework's send queue can fill, dropping the connection. Spec §7.1's 10 msg/sec rate limit is documented but not actually enforced in transport.c — there's no policy doing the closing, just backpressure.

**Reproduction**
1. Connect dev PC to `LL-Mirror-XXXXXX` SoftAP, open the webapp dev server (`npm run dev` in `Firmware/v1/webapp/`).
2. Hard-reload the page so the reconnect-equipped client is active.
3. Click 6-8 color preset swatches in fast succession (one per ~100ms).
4. Watch the "last error" line in the connection panel — "socket closed" appears intermittently. Badge transitions briefly to amber/red and returns to green within seconds.

**Possible fixes (ordered by ROI)**
1. **Coalesce broadcasts when `set_state` carries multiple fields.** Already noted as TODO in the Session 2c sprint_log entry — multi-field set_state currently broadcasts N times for N fields, all carrying the same final state. One broadcast per inbound dispatch is sufficient. Easiest win, ~1 hour.
2. **Move broadcast fanout to a separate task / queue.** So inbound dispatch on the httpd task doesn't block on outbound socket writes. Lets bursts of inbound work proceed even if the outbound side is slow. Medium effort, ~1 day. Probably the *right* fix.
3. **Implement spec §7.1 rate limit (10 msg/sec per IP)** so the device proactively rejects flood traffic with close code 1008 before backpressure builds. Doesn't help the steady-state case; helps malicious / buggy clients. Easy, ~half day.
4. **Investigate SoftAP power management.** ESP32 power-save modes can introduce latency that compounds with the queue-fill issue. Audit `esp_wifi_set_ps(WIFI_PS_NONE)` for the SoftAP path. Diagnostic, ~half day.

**Webapp side already does the right things** — commit-on-release sliders, reconnect with backoff, no flood traffic. The bug is firmware-side capacity.

**Demo-day workaround if it bites during the May 5 advisor demo:** reload the page once at the start to ensure a fresh socket; avoid rapid-fire clicks across controls during the 3-4 minute demo window.

### #2 — `state.on` and `state.brightness` can disagree (misleading wire state)

**Severity:** Medium (visible, ergonomic — affects first-impression of any new user)
**Surfaces:** Webapp control page on initial load; will also surface in the RN app once Sessions 6-8 ship
**First observed:** Session 5 sub-4 closing UAT, 2026-04-29

**Symptom**
On a freshly-erased / first-boot device, the wire `state.on` field reports `true` while `state.brightness` is 0 — meaning the LEDs are physically dark but the webapp's "power" toggle (which derives `isOn` from `state.brightness > 0`) shows OFF, and the diagnostics panel's `on` field shows `true`. The two facts contradict each other from the user's point of view.

**Hypothesized root cause**
`ll_state_t` carries `on` and `brightness` as independent fields, mutated by independent state-bus events (`LL_EV_POWER_TOGGLE` vs `LL_EV_BRIGHTNESS`). The button-firmware's gesture grammar treats them as separate (single-press cycles base color *and* sets `on=true`; long-hold sets `on=false`; brightness gestures don't touch `on`). Defaults from `ll_state_defaults` set `on=true, brightness=50` (or whatever the compiled default is), but the boot path can land in a state where `brightness` got set to 0 from elsewhere (e.g., webapp power-off) without `on` being cleared.

**Reproduction**
1. `idf.py erase-flash && idf.py flash` to clear NVS.
2. Boot the device, observe the LED strip — dark.
3. From the webapp, run `get_state` (e.g. via the diagnostics panel on `#/settings`). Observe `on=true` while `brightness=0` (or some small value).
4. The webapp's `#/` power toggle reads OFF (because it uses `brightness > 0`), contradicting the diagnostics view.

**Possible fixes (ordered by ROI)**
1. **Make `on` derived from `brightness`** at the firmware-state level: drop the independent `on` field; serialize `state.on` as `brightness > 0` in `state_to_json()`. Cleaner mental model, removes the contradiction by construction. ~30 minutes. Requires updating the button handler's `LL_EV_POWER_TOGGLE` to set brightness=0 instead of mutating a separate `on` flag.
2. **Couple them in `apply_event`**: when `brightness` is set to non-zero, also set `on=true`; when `on=false`, also set `brightness=0`. Preserves the field but enforces the invariant. ~15 minutes. Slightly less clean but doesn't ripple to other modules.
3. **Webapp-side coupling**: ignore the firmware's `on` field, derive everything client-side from `brightness`. Already partially what the webapp does. Doesn't fix the diagnostics view, but the user-visible "power" toggle is consistent.

### #6 — Error flow during set_wifi_creds is bare-bones

**Severity:** Medium (functional but ergonomic — affects every "wrong password" recovery)
**Surfaces:** Webapp `/setup` page, after a `set_wifi_creds` submission with credentials the device can't actually use
**First observed:** Session 5 sub-4 closing UAT, 2026-04-29

**Symptom**
When the user submits credentials that don't work (wrong password, network out of range, typo'd SSID, hidden network not in scan range, etc.), the device does what it's told: tears down the SoftAP, attempts STA join, fails over the 15s window, falls back to SoftAP via the `apply_creds_fallback_cb` timer. From the webapp's perspective, the only signal is "the countdown ran out and I never got a fresh state broadcast." The user has to manually re-join `LL-Mirror-...`, reload the page, and reattempt. There's no failure-specific messaging ("password was wrong" vs "network not found" vs "out of range") because the webapp's WS connection died before any of that information could be relayed.

**Hypothesized root cause**
Architectural: the cred-application path tears down the very network the webapp is using to talk to the device, so the device can't tell the webapp anything about success or failure of the join attempt. The webapp can only infer "didn't work" from "didn't get a state update within the timeout window."

This is largely intrinsic to SoftAP-based provisioning: the laptop/phone client and the device share the same SoftAP for the cred-submission conversation, but the device has to leave that SoftAP to test the new creds. Any failure information generated post-leave is unreachable.

**Why BLE provisioning sidesteps this**
With BLE, the link to the provisioning client doesn't depend on the wifi state. The device can:
- Apply creds → try connecting → succeed → notify the BLE client "joined SSID X with IP Y"
- Apply creds → try connecting → fail with `WIFI_REASON_AUTH_EXPIRE` → notify the BLE client "wrong password, try again"
- Stay in BLE-pairing mode the whole time, with no SoftAP teardown

Espressif's `wifi_prov_mgr` BLE scheme already does exactly this — `WIFI_PROV_CRED_FAIL` events carry the underlying `wifi_prov_sta_fail_reason_t` which the webapp / RN app can surface as actionable text.

**Possible fixes (ordered by feasibility)**
1. **Adopt BLE provisioning post-V1.** Cleanest fix; aligns with the stretch goal in [mini-sprint-app-demo.md §4](mini-sprint-app-demo.md). Webapp keeps its SoftAP-only role for users who don't want a phone app; RN app gets BLE for richer error messaging. This is probably where this lands.
2. **Persist a "last-cred-attempt" diagnostic in NVS** the firmware writes before falling back. When the fallback restores SoftAP and the user reconnects, the webapp's `get_state` returns the last failure reason and the setup page shows it ("last attempt: wrong password"). Survives the SoftAP teardown via NVS persistence. Medium effort, ~half-day. Doesn't help mid-flight UX but improves recovery.
3. **APSTA mode during the cred-test window.** Run the SoftAP and the STA simultaneously while testing creds, only tear down the SoftAP after a successful join. Lets the webapp keep its socket alive and receive a real success/failure event. Tradeoff: APSTA mode uses more RAM + has its own quirks (channel must match the STA's, complicating things). Half-day.

**Demo-day workaround**
Document the recovery path: "if the mirror doesn't appear on your network within 15-20 seconds, the credentials were wrong — re-join `LL-Mirror-...` and try again." Acceptable for a 3-minute advisor demo; not acceptable for end-user shipping.

---

## Closed

### #3 — Color / pattern controls don't auto-power-on the device when LEDs are off

**Closed:** Session 5 sub-4, 2026-04-29 (commit pending)
**Fix:** Webapp `pages/control.tsx` now treats any active selection (color preset, color picker, pattern dropdown, brightness slider above 0) as an implicit power-on. The handlers send `{on: true, ...}` patches when `state.on` is false, restoring the last observed active brightness if `state.brightness` was 0. Brightness slider is also coupled the other way: dragging to 0 sends `{brightness: 0, on: false}`. The power toggle now reads `state.on` directly (firmware truth) instead of inferring from `brightness > 0`, so the hardware power button's effect is correctly reflected in the webapp's UI.

The underlying wire-state coupling (#2) is still open — the firmware can theoretically have `on=true, brightness=0` from button-modality interactions — but #3's user-facing symptom (web controls don't turn on the LEDs) is gone.

### #5 — Windows captive sheet's "open browser and connect" link goes to MSN, not the webapp

**Severity:** Medium (documented workaround — manually navigate; phone-side path works correctly)
**Surfaces:** Windows 10/11 dev PC connecting to the mirror's SoftAP via Chrome (and likely Edge / other browsers)
**First observed:** Session 5 sub-3 closing UAT, 2026-04-29

**Symptom**
On a Windows machine joining `LL-Mirror-XXXXXX`, the OS shows a notification: *"Open browser and sign in to Wi-Fi network."* Clicking it opens Chrome to `https://www.msn.com/?ocid=wispr&pc=u477` instead of the webapp at `http://192.168.4.1/`. Waiting on that page does nothing — it's the real MSN. The user can manually navigate to the device URL and the webapp loads fine.

**Hypothesized root cause**
Two-layer issue:

1. **WISPr v2 fallback in Windows.** Windows expects either (a) RFC 8910 DHCP option 114 (Captive-Portal URL) advertising the login URL at DHCP-lease time, or (b) a WISPr v2 XML payload embedded in the captive-portal probe response. We provide neither — our 404 redirect returns plain HTTP 302 to `/`. With no proper "where's the login page?" signal, Windows falls back to its hardcoded default sign-in page (`msn.com/?ocid=wispr`).
2. **HSTS + DoH on `msn.com`.** `msn.com` is in Chrome's HSTS preload list, so the browser refuses to speak HTTP. Even if our DNS hijack catches the resolution, Chrome would only attempt HTTPS:443, which the device doesn't serve. And many Chrome installs use DNS-over-HTTPS to bypass local DNS entirely. Combined: the browser opens the *real* msn.com over the dev PC's other internet path (ethernet, in this case), bypassing the captive-portal flow entirely.

**Reproduction**
1. Dev PC must have a working secondary internet path (ethernet, Tailscale, etc.) — without that, Chrome would just fail.
2. Forget the `LL-Mirror-XXXXXX` profile in Windows (so it re-runs the full captive probe on rejoin).
3. Join the SoftAP. Wait for the "Open browser and sign in" notification.
4. Click the notification. Chrome opens to `https://www.msn.com/?ocid=wispr&pc=u477`.

**Why phones work**
Android and iOS captive-portal probes run on a single network path (the Wi-Fi the phone just joined), so when we hijack DNS + return a 302, the OS opens the redirect target directly without the WISPr fallback. They also tend to use HTTP probes that don't go through HSTS-locked hosts.

**Possible fixes (ordered by effort)**
1. **Add DHCP option 114** with the captive-portal URL (`http://192.168.4.1/`) in the SoftAP's DHCP server response. RFC 8910-compliant. Modern Windows / iOS / Android all parse this and use it as the captive landing URL. Requires extending `esp_netif`'s DHCP server to emit a non-standard option, which means dropping into lwip's `dhcp.c` or post-DHCPACK injection. Medium effort, 1-2 days.
2. **Embed WISPr v2 XML** in the captive-portal probe response body (alongside the existing 302). Older spec but still respected by Windows when it sees one. The XML response is small (~300 bytes). Easier than DHCP option 114. Half a day.
3. **Ship the demo with the workaround.** Document that the dev-PC user should bypass the captive notification and navigate to the URL manually. Phone path is the primary demo target anyway.

**Demo-day workaround:** if presenting from the dev PC, ignore the Windows captive notification, open a new tab in Chrome, and navigate to `http://192.168.4.1/` directly. Works around the issue without code changes. Phone demo (the more interesting one for a faculty advisor anyway) uses the captive sheet correctly.

### #4 — Brightness slider scale labels are decorative, not interactive

**Severity:** Low (polish — affordance-discoverability rather than blocking)
**Surfaces:** Webapp `#/` control page, brightness slider
**First observed:** Session 5 sub-4 closing UAT, 2026-04-29

**Symptom**
The 0/25/50/75/100 marks below the brightness slider are static labels — they look like they should be tap targets that snap the slider to that value (especially on mobile where precise dragging is hard), but clicking them does nothing. Misses an accessibility win for users with motor-control challenges or anyone using a phone screen.

**Hypothesized root cause**
The labels were added in Session 4 as visual reference for the four button-modality stops. They were never meant to be interactive. UX intuition has caught up with the implementation.

**Reproduction**
1. From the webapp on `#/`, look at the brightness slider's scale row.
2. Click any of the 0/25/50/75/100 labels.
3. Nothing happens.

**Possible fix**
Wrap each label in a `<button>` that calls `sendBrightness(value)` and styles to look like the existing label. ~15 minutes. Update `.brightness-marks` CSS to give the buttons hit areas while keeping the visual layout.
