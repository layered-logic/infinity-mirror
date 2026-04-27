---
title: "LED Strip — Color Degradation"
type: failure-mode
component: led-strip
severity: low
probability: very-low
user-repairable: yes
tags: [failure-mode, led, ws2812b, aging]
---

# LED Strip — Color Degradation

LEDs gradually dim, shift color, or lose brightness uniformity over extended use.

---

## Symptoms

- The strip is noticeably dimmer than when new, even at 100% brightness.
- Colors appear washed out or shifted (e.g., white looks yellowish or pinkish).
- Some LEDs are dimmer than their neighbors, creating uneven brightness across the strip.
- The mirror "doesn't look the same as it used to" — subtle but persistent.

## Likely Causes

- **Phosphor aging in the blue/green die** — WS2812B LEDs use InGaN dies. Over tens of thousands of hours the phosphor layer degrades, shifting color temperature and reducing output. This is normal semiconductor aging.
- **Prolonged high-brightness operation** — running at 100% white continuously accelerates aging. The firmware caps at ~29% of rated current, but "full white at 100% brightness" is still the highest-stress mode.
- **Environmental factors** — heat from nearby sources (radiators, direct sunlight on the mirror, enclosed shelving with poor airflow) accelerates LED aging.

## Probability

**Very low over the product's expected life.** WS2812B LEDs are rated at 50,000 hours (L70 — 70% of original brightness). At the firmware's conservative ~29% current limiting, real degradation is pushed well past 14 years at 8 hours/day. Noticeable color shift before year 10 would be unusual under normal indoor conditions.

## Repair Path

This is a gradual wear failure — by the time it's noticeable, the strip has served for many years. The fix is a full strip replacement.

1. Unplug from power.
2. Open the back panel (Phillips-head screws).
3. Disconnect and remove the old strip.
4. Install a fresh WS2812B strip (32 LEDs for 6×6, match data direction).
5. Reconnect and reassemble.

### Tools Required

- Phillips-head screwdriver
- (Optional) Soldering iron if the connection is soldered

### Parts Required

- **WS2812B LED strip, 60 LEDs/m** — ~$1.50–$5.00. Same commodity part as other LED failure modes.

### Estimated Difficulty

Easy to Moderate (same as any strip replacement).

### Estimated Time

15–25 minutes.

## When to Contact Support

- If degradation appears within the first year — that's not normal aging, it may indicate a defective batch or an electrical issue (overvoltage from PSU). See [PSU — Voltage Instability](psu-voltage-instability.md).
- If only a few LEDs are dim while others are bright — that's more likely [Dead Pixel(s)](led-dead-pixel.md) than general degradation.

## Design Mitigations

- **Conservative current limiting** (~29% of rated max) is the single biggest lifespan lever. Less current = less heat = slower phosphor aging.
- **White dimming factor** — the firmware applies a 217/255 (~85%) scaling factor on solid white specifically to reduce thermal stress in the highest-power mode.
- **No sealed enclosure** — the open-backed frame allows passive airflow, preventing heat buildup around the strip.

---

## Related

- [LED Strip — Dead Pixel(s)](led-dead-pixel.md) — sudden single-LED failure vs. gradual degradation
- [LED Strip — Total Failure](led-strip-total-failure.md) — complete strip failure
- [Right-to-Repair Philosophy](../docs/right-to-repair-philosophy.md) — §2 commodity parts commitment
