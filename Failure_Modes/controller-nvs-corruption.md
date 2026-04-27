---
title: "Controller — NVS Data Loss"
type: failure-mode
component: controller-pcb
severity: low
probability: very-low
user-repairable: yes
tags: [failure-mode, controller, nvs, settings, esp32]
---

# Controller — NVS Data Loss

User settings stored in non-volatile storage (NVS) are lost — the mirror reverts to factory defaults unexpectedly.

**Applies to:** Both STM8 Basic (EEPROM) and ESP32 Pro/Pro+ (NVS flash partition) variants, with different symptoms.

---

## Symptoms

- The mirror powers on to the default color (Indigo Signal `#3214FF`) and default brightness instead of the user's last setting.
- (ESP32 variant) Wi-Fi credentials are lost — the device is unprovisioned and needs to be re-paired.
- Saved pattern selection is reset to the first pattern.
- This happens after a power outage, power surge, or plug pull — not during normal button use.

## Likely Causes

- **Power loss during a write cycle** — NVS/EEPROM corruption occurs when power is cut in the narrow window (milliseconds) during an active flash write. This is the primary cause.
- **Flash wear** — NVS flash sectors have a write endurance of ~100,000 cycles. If the debouncing mechanism failed or was bypassed, frequent writes could exhaust the sector.
- **Power surge** — a grid event that corrupts the flash contents.
- **Firmware bug** — a software defect that writes invalid data to NVS. Resolved via OTA update.

## Probability

**Very low.** The firmware debounces NVS writes to once per 500ms minimum, protecting against rapid state-change wear. Power-loss-during-write requires hitting a millisecond window during an infrequent write — realistic over years of use, but unlikely to happen often. STM8 EEPROM (128 bytes, 10,000 cycle endurance) is more vulnerable than ESP32 flash (~100,000 cycles with wear leveling).

## Repair Path

This is a self-resolving failure — the mirror works fine, it just lost its settings.

1. **Set your preferences again** using the primary button — color (single press), pattern (double press), brightness (triple press).
2. The new settings are saved to NVS automatically.
3. (ESP32 variant) If Wi-Fi credentials were lost, **re-provision**: recessed button 3s hold → pairing mode → enter Wi-Fi credentials via the app.

That's it. No hardware work needed.

### Tools Required

- Paperclip or SIM tool (only if re-provisioning Wi-Fi)

### Parts Required

None.

### Estimated Difficulty

Trivial.

### Estimated Time

Under 2 minutes.

## When to Contact Support

- If settings are lost **repeatedly** (more than once or twice) — may indicate flash wear or a firmware bug. Report the behavior so it can be investigated and potentially patched via OTA.
- If the mirror doesn't save new settings at all after resetting them — the NVS partition may be damaged. Factory reset (10s recessed hold) reformats NVS and may resolve it.

## Design Mitigations

- **NVS write debouncing** — state writes limited to once per 500ms, preventing rapid cycling from button mashing or pattern changes.
- **Soft-off vs. hard-off distinction** — soft-off (hold button) saves state; hard-off (power loss) resumes to a known default. This design choice means NVS corruption from power loss produces a *safe* outcome (factory defaults) rather than an *undefined* one.
- **ESP32 wear leveling** — the NVS library automatically distributes writes across the flash sector to maximize endurance.
- **Factory reset as recovery** — if NVS is badly corrupted, the 10-second recessed hold reformats the partition cleanly.

---

## Related

- [Controller — Firmware Corruption](controller-firmware-corruption.md) — more severe corruption affecting boot
- [Controller — Wi-Fi Failure](controller-wifi-failure.md) — if only Wi-Fi credentials were lost
- [PSU — No Output](psu-no-output.md) — if power loss events are frequent, address the root cause
