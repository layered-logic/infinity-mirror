---
title: "Wiring — Solder Joint Failure"
type: failure-mode
component: wiring
severity: medium
probability: low
user-repairable: partial
tags: [failure-mode, wiring, solder, connection]
---

# Wiring — Solder Joint Failure

A solder connection inside the mirror has failed — either a cold joint from manufacturing, a cracked joint from vibration/thermal cycling, or a broken wire at a solder point.

---

## Symptoms

- **Intermittent behavior** — the mirror works sometimes but cuts out when touched, moved, or bumped. The hallmark of a bad solder joint.
- LEDs flicker when the mirror is gently tapped or repositioned.
- The mirror works in one orientation but not another (gravity shifts a wire with a marginal connection).
- Complete LED failure that can be temporarily resolved by pressing on the back panel (applying pressure to the internal connections).

## Likely Causes

- **Cold solder joint from manufacturing** — insufficient heat during soldering leaves a crystalline, high-resistance joint that appears connected but fails under stress. Assembly QC at Stations S11 and S15 (visual inspection under loupe + power-on test) catches most of these.
- **Vibration fatigue** — repeated small vibrations (shipping, wall-mounted mirror near a door that slams) can crack a marginal joint over time.
- **Thermal cycling** — the LED strip heats up during operation and cools when off. Over thousands of cycles, solder joints at the strip connection can fatigue.
- **Mechanical stress** — pulling on a wire during a previous repair, or a wire routed under tension.

## Probability

**Low.** There are exactly two solder operations per unit: the LED strip connection and the controller PCB connection. Both are inspected under loupe and power-tested before final assembly. Solder joint failure is flagged in the production plan as the first bottleneck at scale and the first outsource candidate — the soldering operations get disproportionate QC attention.

## Repair Path

### Diagnosing which joint

1. Unplug the mirror.
2. Open the back panel (Phillips-head screws).
3. Visually inspect all solder joints under good light:
   - **LED strip connection** (3 joints: 5V, GND, Data) — where the strip connects to the controller or extension wire.
   - **Controller board** — the header pins or pads where wires attach.
   - **Barrel jack input** — where the power input wires meet the board.
4. Look for: dull/grainy solder (cold joint), cracked solder, a wire that's loose in its pad, or discoloration around a joint.

### Repairing the joint

1. Heat the suspect joint with a soldering iron (~350°C).
2. Add a small amount of fresh solder (leaded 60/40 or lead-free — match what's there).
3. Let it cool naturally (don't blow on it).
4. The repaired joint should be shiny and smooth (leaded) or slightly matte but uniform (lead-free).
5. Power on and test before reassembling.

### Tools Required

- Phillips-head screwdriver
- Soldering iron (any temperature-controlled iron — a Hakko FX-888D is the production reference, but any iron works)
- Solder (60/40 or lead-free)
- (Optional) Loupe or magnifying glass for inspection
- (Optional) Multimeter for continuity testing

### Parts Required

- None, unless the wire itself is broken (in which case: a few inches of silicone-insulated hookup wire, ~22 AWG).

### Estimated Difficulty

Moderate — requires basic soldering skills. If you've never soldered before, this is learnable (plenty of tutorials for through-hole rework), but contact support if you'd rather not.

### Estimated Time

10–20 minutes (diagnosis + reflow).

## When to Contact Support

- If you're not comfortable soldering.
- If you can't identify which joint is bad — support can walk you through diagnosis over email/photos.
- If reflowing the joint doesn't fix the issue — the problem may be a PCB trace crack or controller failure rather than the solder joint.

## Design Mitigations

- **Minimal solder operations** — only 2 solder points per unit (LED strip + controller board). Fewer joints = fewer potential failure points.
- **Loupe inspection at production** — every joint is visually inspected under magnification at Stations S11 and S15.
- **Power-on test after every solder operation** — no joint ships untested.
- **Connectorized connections preferred** — where possible, JST connectors are used instead of direct solder, making joints unnecessary.
- **Solder flagged as first outsource** — at volume, this is the first operation to be handled by a specialist, not a generalist. The production plan recognizes this as the highest-skill operation.

---

## Related

- [LED Strip — Total Failure](led-strip-total-failure.md) — if the strip appears dead, check the connection first
- [PSU — Voltage Instability](psu-voltage-instability.md) — intermittent power can look like a bad joint
- [Barrel Jack — Intermittent Connection](barrel-jack-intermittent.md) — the mechanical power connector, not the solder joint
- [Controller — Hardware Failure](controller-hardware-failure.md) — if the board itself is the issue
