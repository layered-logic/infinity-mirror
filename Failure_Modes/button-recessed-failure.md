---
title: "Button — Recessed Unresponsive"
type: failure-mode
component: button
severity: medium
probability: very-low
user-repairable: partial
tags: [failure-mode, button, recessed, pairing, factory-reset]
---

# Button — Recessed Unresponsive

The recessed pinhole button stops working — pairing mode (3s hold) and factory reset (10s hold) can't be triggered.

---

## Symptoms

- Inserting a paperclip into the pinhole and pressing produces no response.
- The mirror doesn't enter pairing mode (no indigo pulse at 3s).
- Factory reset can't be triggered (no red flash at 10s).
- All primary button functions still work normally.
- (ESP32 variant) The device is stuck on its current Wi-Fi credentials — can't re-provision to a new network via the button path.

## Likely Causes

- **Misaligned pinhole** — the paperclip isn't reaching the button. The enclosure hole may not line up perfectly with the button center, especially after reassembly from a previous repair.
- **Button switch failure** — same as [Primary Button Failure](button-primary-failure.md), but lower probability since the recessed button is used far less frequently (a few times in the product's lifetime vs. daily use).
- **Solder joint failure** — the button's leads have cracked loose from the PCB.
- **Debris in the pinhole** — dust, paint, or a foreign object is blocking the pin from reaching the button.
- **GPIO failure** — the recessed button's GPIO pin is damaged.

## Probability

**Very low.** The recessed button is used rarely — perhaps a handful of times over the product's entire life (initial pairing, occasional re-provisioning, maybe one factory reset). Mechanical wear is essentially zero. Failure would be a manufacturing defect or alignment issue.

## Repair Path

### Step 1: Check the pinhole

1. Look into the pinhole with a flashlight — can you see the button inside?
2. Try a thinner pin or a different paperclip — the standard paperclip diameter is ~0.8mm; ensure it fits the pinhole.
3. Press firmly — the recessed mounting means you need slightly more force than the exposed primary button.

### Step 2: Check alignment

1. If you recently reassembled the mirror, the PETG enclosure may be slightly shifted. Open the back panel and verify the pinhole aligns with the button center.
2. Adjust the enclosure position if needed — it's friction-fit.

### Step 3: Button replacement

Same process as [Primary Button Failure](button-primary-failure.md) Step 3 — desolder the failed switch, solder in a replacement.

### Tools Required

- Paperclip, SIM tool, or thin pin
- Phillips-head screwdriver (if opening)
- Flashlight (for inspection)
- Soldering iron (if replacing the switch)

### Parts Required

- **6mm × 6mm tactile momentary switch** — same commodity part as the primary button. Pennies each from any electronics supplier.

### Estimated Difficulty

Easy (alignment fix) to Moderate (button replacement).

### Estimated Time

2 minutes (alignment) to 20 minutes (replacement).

## When to Contact Support

- If you can't reach the button at all and the alignment looks correct.
- If you need to re-provision Wi-Fi but can't enter pairing mode — support may be able to assist via an alternative provisioning path.
- If you need a factory reset but can't trigger it — support can reflash the board to achieve the same result.

## Design Mitigations

- **Hold-only semantics** — no tap gestures on the recessed button. This means the button only needs to register sustained contact, not precise timing. Even a marginal button that's hard to press still works if you can hold it.
- **LED threshold feedback** — the indigo pulse at 3s and red flash at 10s confirm the button *is* registering. If you see the feedback, the button works — the issue is elsewhere.
- **Recessed by design** — the pinhole placement prevents accidental activation, but the tradeoff is that the button is harder to reach. This is intentional per the [Button Interface Design §5](../docs/button-interface.md#5-recessed-button--rare-operations).

---

## Related

- [Button — Primary Failure](button-primary-failure.md) — if the daily-use button is the one that's dead
- [Controller — Wi-Fi Failure](controller-wifi-failure.md) — if you need to re-provision but the recessed button is dead
- [Controller — Firmware Corruption](controller-firmware-corruption.md) — if factory reset is needed but the button path is blocked
