---
title: "LED Strip — Total Failure"
type: failure-mode
component: led-strip
severity: high
probability: very-low
user-repairable: yes
tags: [failure-mode, led, ws2812b]
---

# LED Strip — Total Failure

The entire LED strip is dark — no LEDs light up at all, even though the mirror has power and the controller appears to be running.

---

## Symptoms

- Mirror is plugged in and the controller board has power (check for any indicator LED on the controller if applicable), but the strip is completely dark.
- Pressing the primary button produces no visible response.
- Patterns, colors, and brightness changes have no effect.

## Likely Causes

- **Disconnected data or power wire** — the 3-wire connection (5V, GND, Data) between the controller and strip has come loose. Most likely cause.
- **First LED in chain is dead** — because WS2812B is a serial daisy chain, a dead first LED kills the entire strip. See [LED — Dead Pixel(s)](led-dead-pixel.md).
- **Broken solder joint** at the strip-to-controller connection — vibration from shipping or moving the mirror can stress the joint.
- **5V power rail issue** — the strip's power wire is disconnected even if the data line is fine. LEDs need both data and power.
- **Controller data pin failure** — the GPIO driving the strip has failed. See [Controller — Hardware Failure](controller-hardware-failure.md).

## Probability

**Very low.** The strip itself is robust. This failure mode is almost always a connection issue, not a strip defect. Pre-ship QC at Stations S11 and S19 catches loose connections before the unit ships.

## Repair Path

Start with the simplest check and work inward.

1. **Verify power.** Unplug and replug the PSU barrel jack. Confirm the PSU LED (if present) is on.
2. **Open the back panel.** Remove Phillips-head screws, lift the panel.
3. **Check the strip connection.** Look at where the strip connects to the controller:
   - If connectorized (JST): unplug and reseat firmly.
   - If soldered: inspect the three joints (5V, GND, Data) under good light. Look for cracked or cold joints.
4. **If the connection looks good**, try a known-good replacement strip to isolate whether the strip or controller is the problem.
5. If the strip lights on a different controller (or a different strip lights on this controller), you've identified the failed component. Replace accordingly.

### Tools Required

- Phillips-head screwdriver
- (Optional) Multimeter to check continuity on the 5V and GND lines
- (Optional) Soldering iron if re-flowing a joint

### Parts Required

- **WS2812B LED strip, 60 LEDs/m** — if the strip itself is confirmed dead. Same commodity part as [LED — Dead Pixel(s)](led-dead-pixel.md). ~$1.50–$5.00.

### Estimated Difficulty

Easy (reseat connector) to Moderate (re-solder or replace strip).

### Estimated Time

5 minutes (connector reseat) to 25 minutes (full strip replacement).

## When to Contact Support

- If both the strip and the connection check out fine — the issue may be the controller's data output pin. See [Controller — Hardware Failure](controller-hardware-failure.md).
- If you don't have a multimeter or soldering iron and the connection looks suspect.

## Design Mitigations

- **Connectorized strip connections** are preferred over direct solder in production, making reseat trivial.
- **Post-solder power-on test** at Station S11 catches every dead strip and bad joint before final assembly.
- **Final QC** at Station S19 is zero-tolerance on dead LEDs — no unit ships with a strip issue.

---

## Related

- [LED Strip — Dead Pixel(s)](led-dead-pixel.md) — partial strip failure
- [Wiring — Solder Joint Failure](wiring-solder-joint-failure.md) — connection-level diagnosis
- [PSU — No Output](psu-no-output.md) — if the whole mirror is dark, check power first
- [Controller — Hardware Failure](controller-hardware-failure.md) — if the data pin is the problem
