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

---

## Closed

*(none yet)*
