---
title: "Barrel Jack — Intermittent Connection"
type: failure-mode
component: power-connector
severity: medium
probability: low
user-repairable: yes
tags: [failure-mode, barrel-jack, power, connector]
---

# Barrel Jack — Intermittent Connection

The DC barrel jack connector on the mirror has a loose or intermittent connection — the mirror flickers, resets, or loses power when the cable is bumped.

---

## Symptoms

- Mirror cuts out momentarily when the DC cable is touched or moved.
- The mirror reboots (flashes default color briefly) when the cable is jiggled — this is a power-cycle reboot.
- Power works only when the plug is held at a specific angle.
- Flickering that correlates with cable movement (as opposed to [PSU — Voltage Instability](psu-voltage-instability.md), which is independent of cable position).

## Likely Causes

- **Worn barrel jack socket** — the spring contacts inside the socket have relaxed from repeated plug/unplug cycles. The plug no longer seats firmly.
- **Bent center pin** — the PSU plug's center pin is slightly bent, making inconsistent contact with the socket's center conductor.
- **Solder joint failure at the socket** — the barrel jack socket's leads have cracked where they're soldered to the PCB or wiring. See [Wiring — Solder Joint Failure](wiring-solder-joint-failure.md).
- **Damaged cable** — the DC cable from the PSU has an internal break near the plug, making contact intermittent.

## Probability

**Low.** The barrel jack is a robust connector, and the mirror is typically plugged in once and left — not repeatedly connected and disconnected. This failure mode is more likely if the mirror is frequently moved or if the cable is stressed (routed at a sharp angle, pulled taut, or stepped on).

## Repair Path

### Step 1: Check the PSU plug

1. Inspect the PSU's barrel plug — is the center pin straight? Is the plug visibly damaged?
2. Try a different 5V PSU with a matching barrel plug. If the issue goes away, the problem is the PSU cable, not the mirror's socket. Replace the PSU per [PSU — No Output](psu-no-output.md).

### Step 2: Check the socket (if the PSU plug is fine)

1. Unplug from power.
2. Open the back panel (Phillips-head screws).
3. Inspect the barrel jack socket:
   - Check the solder joints where the socket connects to the board or wiring. Reflow if they look cracked or dull.
   - Gently wiggle the socket — it should be solidly mounted with no play.
4. If the socket's internal spring contacts are worn, replace the socket.

### Socket replacement:

1. Desolder the old barrel jack socket.
2. Solder in a replacement (standard 5.5mm × 2.1mm panel-mount or PCB-mount barrel jack socket).
3. Test with the PSU plugged in — verify solid connection with no intermittent behavior.

### Tools Required

- Phillips-head screwdriver
- Soldering iron + solder (for joint reflow or socket replacement)
- (Optional) Multimeter for continuity testing

### Parts Required

- **5.5mm × 2.1mm DC barrel jack socket** — commodity part, under $1. Available from Amazon, DigiKey, Mouser, or AliExpress.
- Or just replace the PSU if the issue is the plug side — see [PSU — No Output](psu-no-output.md).

### Estimated Difficulty

Easy (PSU replacement) to Moderate (socket reflow/replacement).

### Estimated Time

Under 1 minute (PSU swap) to 15–20 minutes (socket replacement).

## When to Contact Support

- If you're not comfortable with soldering.
- If the socket replacement doesn't fix the issue — the problem may be deeper in the power path (PCB trace, regulator, etc.).

## Design Mitigations

- **Standard connector** — 5.5mm × 2.1mm barrel jack is the most common DC power connector. Sockets and plugs are universally available.
- **External PSU** — the connector is on the mirror's enclosure, accessible without full disassembly.
- **Strain relief** — cable routing inside the enclosure includes a strain relief point so cable tugs stress the relief, not the solder joint.

---

## Related

- [PSU — No Output](psu-no-output.md) — if the issue is the PSU, not the connector
- [PSU — Voltage Instability](psu-voltage-instability.md) — if flickering is independent of cable movement
- [Wiring — Solder Joint Failure](wiring-solder-joint-failure.md) — if the socket's solder connection is the problem
