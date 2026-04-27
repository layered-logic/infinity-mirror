---
title: "PSU — Voltage Instability"
type: failure-mode
component: power-supply
severity: medium
probability: low
user-repairable: yes
tags: [failure-mode, psu, power, flickering]
---

# PSU — Voltage Instability

The power supply is providing inconsistent voltage, causing visible flickering, dimming, or erratic LED behavior.

---

## Symptoms

- LEDs flicker or pulse irregularly (not in sync with any pattern).
- Overall brightness dips and recovers on its own.
- The mirror resets or reboots spontaneously — the controller brown-outs when voltage drops below its minimum operating threshold.
- Colors appear incorrect at high brightness (LEDs starved of voltage show dimmer-than-expected output, especially in white).

## Likely Causes

- **Aging PSU** — electrolytic capacitors in commodity switch-mode supplies degrade over years, increasing output ripple and reducing voltage regulation.
- **Undersized PSU** — if the original was replaced with a lower-rated supply, it may sag under load. The mirror draws up to ~2.8W at full white; a 5V/500mA (2.5W) supply would be marginal.
- **Loose barrel jack** — a partially seated connector causes intermittent contact. See [Barrel Jack — Intermittent Connection](barrel-jack-intermittent.md).
- **Long or thin DC cable** — voltage drop over a low-gauge extension cable can pull the delivered voltage below 4.5V at the controller.
- **Shared power strip with high-draw appliances** — rare, but a heavily loaded circuit can cause voltage dips at the wall outlet.

## Probability

**Low.** The 25W PSU running at 11% load has enormous headroom. Voltage instability would require either a defective/aged PSU or an environmental factor (bad outlet, extension cord, etc.).

## Repair Path

1. **Check the barrel jack connection first.** Unplug and firmly reseat the DC barrel connector on the mirror. If the issue is intermittent, see [Barrel Jack — Intermittent Connection](barrel-jack-intermittent.md).
2. **Try a different wall outlet** — rule out the outlet/circuit as the source.
3. **Remove any extension cables** between the PSU and mirror — test with a direct connection.
4. **Replace the PSU** if the above doesn't help. Same replacement process as [PSU — No Output](psu-no-output.md) — just swap the brick.

### Tools Required

- None for PSU swap
- (Optional) Multimeter to measure output voltage under load — should be 4.8–5.2V DC

### Parts Required

- **5V DC power supply, barrel jack (5.5×2.1mm), 2A or higher** — ~$3–$8 from any electronics supplier.

### Estimated Difficulty

Trivial — plug swap.

### Estimated Time

Under 1 minute (PSU swap). 5 minutes if troubleshooting outlet and cables first.

## When to Contact Support

- If the flickering persists with a new PSU — the issue may be inside the mirror (bad solder joint on the power input, failing controller). See [Wiring — Solder Joint Failure](wiring-solder-joint-failure.md).
- If you measure correct voltage at the PSU output but the mirror still flickers — controller-side issue.

## Design Mitigations

- **PSU headroom** — 25W supply for a 2.8W max load. Even a 50% PSU degradation would still deliver sufficient power.
- **Bulk capacitor on the LED strip** — the 47µF capacitor on the controller PCB smooths brief voltage dips.
- **Firmware current limiting** — the LED drive never exceeds ~29% of rated current, keeping instantaneous power draw predictable and well within PSU capacity.

---

## Related

- [PSU — No Output](psu-no-output.md) — complete PSU failure
- [Barrel Jack — Intermittent Connection](barrel-jack-intermittent.md) — mechanical connection issue
- [Controller — Hardware Failure](controller-hardware-failure.md) — if the controller is browning out
