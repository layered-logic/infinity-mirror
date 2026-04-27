---
title: "LED Strip — Dead Pixel(s)"
type: failure-mode
component: led-strip
severity: low
probability: very-low
user-repairable: yes
tags: [failure-mode, led, ws2812b]
---

# LED Strip — Dead Pixel(s)

One or more individual LEDs on the strip stop lighting, show the wrong color, or flicker while the rest of the strip operates normally.

---

## Symptoms

- A single LED stays dark while others around it work.
- An LED is stuck on one color regardless of pattern.
- An LED flickers or stutters out of sync with the pattern.
- All LEDs *after* a certain point in the chain are dark (WS2812B is a daisy chain — a dead LED breaks the data signal downstream).

## Likely Causes

- **Manufacturing defect** in the WS2812B IC — rare but possible in commodity strips.
- **Solder joint fatigue** at the LED pad, especially if the strip was bent sharply during assembly.
- **Thermal damage** — unlikely under normal operation (firmware caps drive current at ~29% of rated max), but possible from external heat sources near the mirror.
- **ESD damage** during assembly or handling — the WS2812B IC is sensitive to static discharge.

## Probability

**Very low.** WS2812B LEDs are rated for 50,000 hours. At the firmware's conservative current limiting (~29% of rated max), thermal stress is minimal and real-world lifespan exceeds the 50,000-hour spec. At 8 hours/day, that's 17+ years before age-related failure is expected. Pre-assembly testing at Station S10 catches defective strips before they ship.

## Repair Path

The LED strip is a single continuous piece — individual LEDs can't be swapped. The repair is a full strip replacement.

1. Unplug the mirror from power.
2. Remove the back panel screws (Phillips-head).
3. Lift the back panel to access the interior.
4. Disconnect the LED strip from the controller (3-pin JST or solder pads — depends on build revision).
5. Remove the strip from the LED spacer channel.
6. Seat the replacement strip in the channel (match the data direction arrow).
7. Reconnect to the controller.
8. Reassemble and test.

### Tools Required

- Phillips-head screwdriver
- (Optional) Soldering iron, if the strip is soldered rather than connectorized

### Parts Required

- **WS2812B LED strip, 60 LEDs/m density** — commodity part available from Amazon, AliExpress, Adafruit, or any electronics supplier. Cut to 32 LEDs for the 6×6 model. Cost: ~$1.50–$5.00 depending on source and quantity.

### Estimated Difficulty

Easy — no soldering if connectorized. Moderate if the strip connection is soldered.

### Estimated Time

15–25 minutes.

## When to Contact Support

- If you're uncomfortable with soldering (if applicable).
- If the replacement strip also shows dead pixels — may indicate a controller issue rather than a strip defect. See [Controller — Hardware Failure](controller-hardware-failure.md).
- If you'd like a pre-cut, pre-tested replacement strip shipped to you.

## Design Mitigations

- **Pre-ship testing:** Every LED strip is tested at receiving (Station S10) and again after solder (Station S11) — dead pixels are caught before the unit ships.
- **Current limiting:** Firmware drives LEDs at ~29% of rated max, reducing thermal stress and extending lifespan well beyond the 50,000-hour specification.
- **Commodity parts:** WS2812B strips are the most widely available addressable LED format in the world. No proprietary sourcing needed.

---

## Related

- [LED Strip — Total Failure](led-strip-total-failure.md) — when the entire strip goes dark
- [LED Strip — Color Degradation](led-color-degradation.md) — gradual dimming or color shift
- [Wiring — Solder Joint Failure](wiring-solder-joint-failure.md) — if the issue is at the connection, not the strip
- [Right-to-Repair Philosophy](../docs/right-to-repair-philosophy.md) — §2, §3
