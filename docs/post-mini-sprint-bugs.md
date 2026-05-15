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

### #2 — `state.on` and `state.brightness` can disagree (misleading wire state)

**Closed:** 2026-05-15 by [LL-075](../tasks.md#LL-075) — fix #2 from the original ROI list (couple them in `apply_event`), with an additional auto-restore on power-on so the button single-press path doesn't strand at brightness=0.

**Severity (at close):** Was Medium (visible, ergonomic). Now eliminated at the data-model layer.

**Symptom (historical)**
The wire `state.on` field could report `true` while `state.brightness` was 0 — LEDs dark, but the diagnostics panel showed `on=true`. The webapp's power toggle (which uses `state.on` directly per closed bug #3) had already been disambiguated from the brightness-derived inference, so the user-facing impact was mostly diagnostics-view confusion. The data model still permitted the contradictory state, and the symmetric case (`on=false, brightness>0`) also leaked through if a client posted `{on:false}` alone.

**Root cause (historical)**
`ll_state_t` carried `on` and `brightness` as independent fields mutated by independent state-bus events. No invariant was enforced anywhere — `apply_event` set whichever field the event named and left the other one alone.

**Fix shipped — LL-075** ([Firmware/v1/core/state_bus/state_bus.c](../Firmware/v1/core/state_bus/state_bus.c))

Two coupled invariants in `apply_event`:

- `LL_EV_BRIGHTNESS{value}` now also sets `on = (value > 0)`. brightness=0 implies !on; brightness>0 implies on.
- `LL_EV_POWER_TOGGLE{on:true}` auto-restores `brightness` to 75 (the `LL_BRIGHTNESS_RESUME_DEFAULT` constant) when current brightness is 0. So a webapp `{brightness:0}` set followed by a button single-press from off leaves the device at (on=true, brightness=75) — not stranded at brightness=0 with LEDs still dark.
- `LL_EV_POWER_TOGGLE{on:false}` deliberately **preserves brightness** — button long-hold → single-press cycle keeps the user's brightness setting. The renderer gates on `s->on` ([pattern_interp.c:151](../Firmware/v1/core/pattern_interp/pattern_interp.c)) so (on=false, brightness=75) renders dark; brightness is just preserved memory.

Boot self-heal in `ll_state_bus_init` catches legacy NVS blobs that carry the contradictory (on=true, brightness=0) and corrects on first load.

Verified on the live mirror via [ll_on_brightness_invariant_test.py](../Firmware/v1/scripts/ll_on_brightness_invariant_test.py) — all 6 cases pass (single-field brightness 0/non-zero, single-field on true/false, coupled webapp envelopes both ways).

### #1 — Transient `socket closed` errors under user load

**Closed:** 2026-05-15 across [LL-055](../tasks.md#LL-055) Sessions A-C (all four proposed fixes shipped).

**Severity (at close):** Was Low (cosmetic + auto-recoverable). Now fixed end-to-end at the firmware capacity layer.

**Symptom (historical)**
"socket closed" or "not connected" briefly flashes in the connection-status panel of the webapp when the user clicks several controls in rapid succession. Webapp reconnect-with-backoff (1-8s) recovers, but the error was user-visible.

**Root cause (historical)**
`esp_http_server`'s WS handler is single-task and serializes inbound dispatch + outbound broadcasts. Any state change triggers a fanout to all connected clients via `broadcast_state`, so one inbound message → N+1 frames of work on that one task. Network jitter on the SoftAP filled the framework's send queue and dropped connections. Spec §7.1's 10 msg/sec rate limit was documented but not enforced.

**Fixes shipped (chronological)**
1. **Coalesce broadcasts within a debounce window** — May 14 ([sprint_log entry](../sprint_log.md), commit on `claude/happy-lehmann-907629`). 30ms one-shot timer in [transport.c](../Firmware/v1/core/transport/transport.c); first call arms the timer, subsequent calls within the window coalesce. 3-field set_state went from 3 broadcasts → 1; 6 rapid clicks at ~30ms went from 6 → 2.
2. **Move broadcast fanout to a dedicated FreeRTOS task** — May 15 (LL-055 Session B). New `ll_broadcast` task (4 KB stack, prio 4) consumes a depth-2 queue of broadcast tokens; debounce timer cb posts to the queue instead of running do_broadcast_state synchronously. Verified via [ll_slow_client_test.py](../Firmware/v1/scripts/ll_slow_client_test.py): a slow client pinning a socket no longer affects inbound RTT (median 72→71 ms, 20/20 responses in both phases).
3. **Implement spec §7.1 rate limit** — May 15 (LL-055 Session C). Per-IP token bucket (10 msg/s, burst 10) in WS receive path; overrun → close 1008 + `httpd_sess_trigger_close`. Verified via [ll_ratelimit_test.py](../Firmware/v1/scripts/ll_ratelimit_test.py): steady 10 msg/s = 30/30 clean; flood 50 msg/s = close 1008 at 0.43s.
4. **Disable Wi-Fi power-save on the STA path** — May 15 (LL-055 Session A). Audit found zero `esp_wifi_set_ps` calls; new `ll_wifi_disable_ps()` helper + `WIFI_PS_NONE` at all four `esp_wifi_start` sites in [provisioning.c](../Firmware/v1/core/provisioning/provisioning.c). First-response latency dropped 155 → 84 ms (Test A, −46%) and 136 → 64 ms (Test B, −53%).

The full resilience characterization suite from LL-055 Session D (reconnect hammer, latency/loss proxy, 5-minute soak) confirms the firmware now handles realistic user loads without the originating symptom. Webapp-side mitigations (commit-on-release sliders, reconnect-with-backoff) remain in place as defense-in-depth.

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
