---
title: Button Interface Design
type: engineering
phase: 2
week: 4
date: 2026-04-22
status: locked — Apr 22 UX decisions complete, one minor clarification parked (§6.1)
tags: [engineering, firmware, hardware, ux, button, interface]
---

# Button Interface Design — Two-Button Model

**Prepared by:** William White
**Date:** April 22, 2026
**Status:** First draft — captures the two-button decision from the Wed Apr 22 firmware architecture block

---

## 1. Purpose

This doc specifies the physical control interface for the Layered Logic Infinity Mirror v1. It defines:

- The two-button hardware model (primary exposed + recessed pinhole)
- The interactions each button handles
- The state machine the firmware runs to interpret input
- LED feedback conventions
- Persistence and boot-state behavior

It is **not** the firmware implementation — that's downstream. This doc is the spec the firmware implements.

---

## 2. Design Principles

Inherit from [Firmware Architecture Scoping §4](firmware-architecture-scoping.md#4-design-principles-locked-apr-20):

1. **Mirror is a light first, smart device second.** Daily controls stay on the exposed button and require zero documentation to use.
2. **Destructive or configuration-changing actions require deliberate effort.** Factory reset and pairing mode live behind a recessed button — accessible only with a paperclip. No accidental triggers from normal handling.
3. **No passive radio advertising at rest.** BLE and SoftAP are dark until the user explicitly enters pairing mode. See [firmware scoping §5.2](firmware-architecture-scoping.md#52-provisioning-ux).
4. **Out-of-box behavior must be complete.** A user who never opens the app and never presses the recessed button still gets a working mirror with sensible defaults.

---

## 3. Hardware

| Button | Placement | Type | Purpose |
|---|---|---|---|
| **Primary** | Exposed, accessible on front/side of enclosure | Tactile momentary SPST, internal pullup | Daily use — on/off, color, pattern, brightness |
| **Recessed** | Pinhole on back or bottom of enclosure, paperclip-operable | Tactile momentary SPST, internal pullup | Rare operations — BLE pairing, factory reset |

**GPIO assignments (tentative, PCB-layout dependent):**
- Primary: GPIO9 (inherits from V0 prototype, keeps the existing ESPHome pinout reference valid)
- Recessed: second GPIO TBD at PCB layout time — GPIO3 or GPIO5 are likely candidates on the ESP32-C3-MINI-1

Both inputs active-low with internal pullup. Debouncing handled in firmware (not hardware — keeps the BOM trivial).

---

## 4. Primary Button — Daily UI

This is the interaction model inherited from the V0 prototype, preserved intentionally to avoid re-teaching users behavior that already works.

| Gesture | When OFF | When ON |
|---|---|---|
| **Single press** | Turn on, resume last pattern + last color | Advance base color (see §4.3) |
| **Double press** | Turn on, default state | Change pattern (cycle through stored patterns) |
| **Triple press** | — | Advance brightness (see §4.4) |
| **Hold** | — | Turn off |

### 4.1 Persistence

- **Soft-off (user hold-to-turn-off):** Current pattern, color, and brightness stored in NVS. Next single press resumes exactly that state.
- **Hard-off (physical power loss):** On boot, always resume to **default color** (factory: Indigo Signal `#3214FF` at 80% brightness). Default color is user-configurable via the app.

This distinction is important. A user who unplugs their mirror to move it shouldn't come back to an unexpected state; a user who toggled it off before bed should resume where they left off.

### 4.2 No mode-entry gestures on primary button

The primary button handles *daily* interactions only. It does not enter pairing mode, trigger factory reset, or initiate smart-home commissioning. Those live on the recessed button by design — keeps the daily UI uncluttered and prevents accidental entry into operational states.

### 4.3 Base color cycle (single-press semantics)

Single press (when on) advances a **base color** variable held in firmware. The cycle is 17 positions total:

- **16 spectrum steps** — HSV hue advancing in 22.5° increments (0°, 22.5°, 45°, … 337.5°), saturation and value both at full (scaled by current brightness setting)
- **1 white step** — equal RGB at current brightness

Wraps: step 17 → step 1. The cycle is independent of the brand palette. Brand palette informs the *default* state on boot; the spectrum cycle is the user's runtime control.

**Interaction with patterns:** patterns fall into two categories.
- *Base-color-using patterns* — Color Wipe, Twinkle, Scanner, solid. Single-press visibly changes the color.
- *Self-palette patterns* — Rainbow, Random Twinkle, future multi-hue effects. Single-press still advances the base color in firmware state, but there's no visual change until the user switches to a base-color-using pattern.

This keeps intent sticky: "my base color is cyan" persists across pattern changes, even if the currently running pattern ignores it.

### 4.4 Brightness steps (triple-press semantics)

Triple press advances brightness through 4 discrete steps:

**25% → 50% → 75% → 100% → 25%** (cycle)

Brightness scales the output across all LEDs uniformly. Patterns and colors remain unchanged; only the output magnitude shifts. Saved to NVS as part of the soft-off persistence state.

---

## 5. Recessed Button — Rare Operations

Triggered with a paperclip, SIM tool, or pin. Not discoverable from normal handling.

| Gesture | Action |
|---|---|
| **Short hold (3s)** | Enter BLE + SoftAP pairing mode (5-min timeout) |
| **Long hold (10s)** | Factory reset — clear Wi-Fi creds, stored patterns, and user config. Return to out-of-box state. |
| **Reserved for future** | Matter/Zigbee commissioning (if ESP32-C6 variant ships — see [firmware scoping §5.3](firmware-architecture-scoping.md#53-smart-home-integration-path)) |

All actions require a hold — no tap-only gestures on the recessed button. This is the Eero convention: mistake-proof, consistent semantics, and the LED threshold feedback (§7) makes it clear which action is imminent before the user commits.

### 5.1 Pairing mode behavior

- Triggered → firmware starts `wifi_prov_mgr` with BLE + SoftAP transports
- Mirror visually enters pairing mode: LED pattern shifts to slow indigo pulse (distinct from any normal pattern)
- Times out after 5 minutes if no pairing completes; returns to prior state
- Successful pairing: credentials saved to NVS, Wi-Fi connects, mirror returns to normal running state (with Wi-Fi now enabled)

### 5.2 Factory reset behavior

- Triggered → LED flashes a distinct warning cue (red, brief)
- NVS cleared: Wi-Fi credentials, paired app instances, stored patterns, custom default color
- Mirror reboots into out-of-box state: default Indigo Signal color, no radios advertising, unprovisioned
- This is the repair-path fallback per [right-to-repair philosophy §3](right-to-repair-philosophy.md#3-what-i-commit-to) — when something's wrong, a user can always return the device to a known state.

---

## 6. Resolved Decisions (Apr 22)

The three open questions from the initial draft were resolved in the Wed Apr 22 firmware architecture block. Summary:

- **Color cycle →** 16 spectrum steps (HSV hue, 22.5° increments) + 1 white = 17 positions. Independent of brand palette. See §4.3 for full semantics including interaction with self-palette patterns.
- **Brightness cycling →** 4 discrete steps: 25% / 50% / 75% / 100%, cycling. See §4.4.
- **Recessed button →** Hold-only semantics. Short hold (3s) = pairing, long hold (10s) = factory reset. See §5.

### Parked clarification

One minor item deferred from the base-color semantics (§4.3): the behavior of single-press when on a self-palette pattern is documented as "advances base color in firmware state, no visual change until user returns to a base-color-using pattern." This may feel unresponsive — user presses the button and sees nothing happen. Possible refinement later: have self-palette patterns flash the new base color briefly (~200ms) as acknowledgement before returning to the pattern. Defer until user testing surfaces whether this is actually a problem.

---

## 7. LED Feedback Conventions

The display itself doubles as the status indicator. No separate LED needed.

| State | Display behavior |
|---|---|
| Normal (on) | Running selected pattern + color + brightness |
| Off | Dark |
| Entering pairing mode | Brief flash (1 frame) to acknowledge, then slow indigo pulse |
| Pairing mode active | Slow indigo breath pattern (~2s cycle), visibly distinct from any user pattern |
| Pairing timeout | Brief fade, then return to prior state |
| Pairing success | Brief bright green flash, then return to default running state |
| Recessed button hold at 3s mark | Color shifts to indigo pulse (pairing threshold reached) |
| Recessed button hold at 10s mark | Brief red flash, then reboot to out-of-box defaults (factory reset) |
| Recessed button release between 3s and 10s | Enters pairing mode |

These cues need to be designed alongside the pattern library — they must be distinguishable from every shipped pattern so the user can tell state from display alone.

---

## 8. State Machine Summary

```
                     ┌──────────────────────┐
                     │   OUT-OF-BOX STATE   │
                     │  (no Wi-Fi creds)    │
                     │  Default color only  │
                     │  Radios: DARK        │
                     └──────────┬───────────┘
                                │
              ┌─────────────────┴─────────────────┐
              │ Recessed button triggers pairing  │
              ▼                                   │
     ┌─────────────────┐                          │
     │  PAIRING MODE   │◄── 5-min timeout ────────┘
     │  BLE + SoftAP   │
     │  advertising    │
     └────────┬────────┘
              │ User pairs via app or browser
              ▼
     ┌──────────────────────┐
     │  PROVISIONED STATE   │
     │  (Wi-Fi creds saved) │
     │  Wi-Fi: connected    │
     │  BLE: dark           │◄── can re-enter pairing
     │  WebSocket listening │    any time via recessed
     └──────────┬───────────┘    button (add new phone,
                │                 re-pair, etc.)
                │
                │ Primary button = daily UI
                │ Recessed hold 10s = factory reset
                │ App commands via WebSocket
                ▼
             [normal operation]
```

---

## 9. Out of Scope for This Doc

- Pattern interpreter / pattern definition format (separate spec, downstream)
- App UI for pattern creation (Week 5 wireframing)
- Matter/Zigbee commissioning gesture details (revisited when C3→C6 decision is made)
- Multi-button combos (none planned; documented here to confirm absence)

---

## Related

- [Firmware Architecture Scoping](firmware-architecture-scoping.md) — parent doc; this design implements §5.2 at the UX layer
- [Right-to-Repair Philosophy](right-to-repair-philosophy.md) — §3 drives the factory-reset requirement
- [Brand Positioning](brand-positioning.md) — §4 default color (Indigo Signal `#3214FF`)
- [Sprint Plan — Week 4](../sprint_plan.md#week-4-apr-21--apr-25-brand-identity--repairability) — context
