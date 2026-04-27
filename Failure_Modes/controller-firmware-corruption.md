---
title: "Controller — Firmware Corruption"
type: failure-mode
component: controller-pcb
severity: high
probability: very-low
user-repairable: partial
tags: [failure-mode, controller, firmware, ota]
---

# Controller — Firmware Corruption

The controller hardware is intact, but the firmware has become corrupted — the device won't boot correctly or behaves erratically.

---

## Symptoms

- Mirror powers on but LEDs show unexpected behavior — random flashing, frozen on a single color that isn't user-selected, or a brief flash followed by reboot loop.
- Buttons don't respond normally (but the mirror does *something* on power-up, unlike a hardware failure where it does nothing).
- (ESP32 variant) Device was working, received an OTA update, and now behaves incorrectly.
- (ESP32 variant) Device shows up briefly on the network then disappears — reboot loop.

## Likely Causes

- **Interrupted OTA update** — power loss or Wi-Fi drop during a firmware write. The most likely cause for ESP32 variants.
- **NVS corruption** — the flash region holding settings was corrupted by power loss during a write cycle. See also [NVS Corruption](controller-nvs-corruption.md) for the milder variant where only settings are lost.
- **Flash memory degradation** — flash has a finite write endurance (~100,000 cycles). NVS debouncing (500ms minimum between writes) prevents premature wear, but after many years of continuous state changes, the flash region could degrade.
- **Bitrot** — extremely rare, but possible: a single-bit error in the firmware partition. More likely in high-radiation environments (not typical for a home product).

## Probability

**Very low.** The ESP32's A/B partition scheme with automatic rollback is specifically designed to prevent bricked devices from bad OTA updates. If a new firmware fails its health check within 60 seconds of boot, the bootloader automatically reverts to the previous known-good partition. A complete firmware corruption requires a failure mode that defeats this rollback mechanism.

## Repair Path

### Step 1: Factory Reset (user-serviceable)

This clears NVS and returns to factory firmware state. Resolves most corruption scenarios.

1. With the mirror powered on (even if misbehaving), use a paperclip to press and hold the recessed button for **10 seconds**.
2. LED feedback: at 3 seconds you'll see an indigo pulse (pairing threshold — keep holding). At 10 seconds, a red flash followed by a green hold confirms factory reset.
3. The mirror reboots to out-of-box defaults: Indigo Signal color, no Wi-Fi credentials, unprovisioned.
4. If the mirror now works normally, re-pair through the app.

### Step 2: OTA Recovery (if factory reset doesn't help)

If the device can connect to Wi-Fi after factory reset and re-provisioning:

1. Re-provision the device via the app (recessed button 3s hold → pairing mode).
2. The device will check for OTA updates on boot and every 24 hours.
3. If a newer firmware is available, it will self-update.

### Step 3: Reflash Service (if the device can't boot at all)

If the device is in a hard boot loop that factory reset can't resolve:

1. Contact Layered Logic support.
2. Ship the mirror (or just the controller board) for reflash. Cost: shipping only.
3. The board is reflashed via USB/JTAG using the production flashing setup — same as Station S14 in the factory.

### Tools Required

- Paperclip or SIM tool (for factory reset)
- No tools needed for OTA recovery
- (Reflash service is handled by support)

### Parts Required

None — this is a software repair. The hardware is fine.

### Estimated Difficulty

Easy (factory reset) to N/A (reflash service handled by support).

### Estimated Time

Under 1 minute (factory reset). OTA recovery depends on network speed. Reflash service: shipping time + turnaround.

## When to Contact Support

- If factory reset doesn't resolve the issue (hard boot loop persists).
- If the device can't enter pairing mode after factory reset.
- The [reflash service](../docs/right-to-repair-philosophy.md#3-what-i-commit-to) is available at cost of shipping for the life of the product.

## Design Mitigations

- **A/B OTA partitions** — bad firmware automatically rolls back within 60 seconds. A bricked device from OTA should be nearly impossible.
- **Anti-rollback protection** — monotonic version counter in eFuse prevents firmware downgrades that might reintroduce known bugs.
- **Staged OTA rollout** — new builds go to 1% → 10% → 50% → 100% of devices. Opt-in telemetry cohort gets builds first (de facto beta channel). Issues are caught before they reach the full fleet.
- **NVS write debouncing** — state writes limited to once per 500ms, protecting flash endurance.
- **Factory reset path** — the recessed button factory reset is always available as a recovery mechanism, regardless of firmware state.

---

## Related

- [Controller — Hardware Failure](controller-hardware-failure.md) — if the hardware itself is dead
- [Controller — NVS Corruption](controller-nvs-corruption.md) — milder variant where only settings are lost
- [Right-to-Repair Philosophy](../docs/right-to-repair-philosophy.md) — §3 OTA commitment, §4 reflash service
