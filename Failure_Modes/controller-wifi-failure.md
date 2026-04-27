---
title: "Controller — Wi-Fi Connectivity Loss"
type: failure-mode
component: controller-pcb
severity: medium
probability: low
user-repairable: yes
tags: [failure-mode, controller, wifi, esp32, network]
---

# Controller — Wi-Fi Connectivity Loss

The ESP32 controller can't connect to the home Wi-Fi network. The mirror still works locally (buttons, patterns, colors) but is unreachable from the app and can't receive OTA updates.

**Applies to:** ESP32 Pro and Pro+ variants only. The STM8 Basic model has no Wi-Fi.

---

## Symptoms

- The app shows the device as "offline" or "not found."
- The mirror works fine from the physical buttons — LEDs respond, patterns change, brightness adjusts.
- mDNS discovery (`_layeredlogic._tcp.local.`) returns no results.
- The mirror was previously connected and working on the network.

## Likely Causes

- **Router change or password change** — the saved Wi-Fi credentials in NVS no longer match the network.
- **Router firmware update** changed Wi-Fi settings (channel, band, security protocol).
- **2.4GHz band disabled on router** — the ESP32-C3 only supports 2.4GHz Wi-Fi. If the router was reconfigured to 5GHz-only, the device can't connect.
- **IP conflict or DHCP exhaustion** — the router ran out of DHCP leases or assigned a conflicting IP.
- **Distance/interference** — the mirror was moved to a location with weak 2.4GHz signal.
- **ESP32 Wi-Fi radio failure** — hardware-level radio failure. Rare but possible.

## Probability

**Low for device-side issues, moderate for network-change issues.** The most common scenario is a changed router password or a router replacement — the device itself is almost never the problem. The ESP32's Wi-Fi radio is robust and industrial-grade.

## Repair Path

### If the network changed (most common):

1. Use a paperclip to hold the recessed button for **3 seconds** → mirror enters pairing mode (slow indigo pulse).
2. Open the Layered Logic app → follow the provisioning flow to enter new Wi-Fi credentials.
3. The mirror connects to the new network and saves the credentials to NVS.

### If the network hasn't changed:

1. **Restart the mirror** — unplug power, wait 5 seconds, plug back in. The ESP32 re-attempts Wi-Fi connection on boot.
2. **Restart the router** — clears DHCP state and any temporary bans.
3. **Check 2.4GHz is enabled** on the router — the ESP32-C3 doesn't support 5GHz.
4. **Move the mirror closer to the router** temporarily to rule out signal strength.
5. If none of the above helps, **re-provision**: recessed button 3s hold → pairing mode → re-enter credentials via the app. This clears and re-saves the Wi-Fi config.

### Tools Required

- Paperclip or SIM tool (for recessed button)

### Parts Required

None — this is a configuration issue.

### Estimated Difficulty

Easy.

### Estimated Time

2–5 minutes.

## When to Contact Support

- If the device won't enter pairing mode at all (recessed button unresponsive). See [Button — Recessed Failure](button-recessed-failure.md).
- If re-provisioning repeatedly fails — the Wi-Fi radio may have a hardware issue.
- If you need help identifying your 2.4GHz network name or checking router settings.

## Design Mitigations

- **Radios dark at rest** — Wi-Fi only activates after provisioning. No passive advertising means no phantom connectivity issues from boot.
- **User-initiated re-provisioning** — the 3s recessed hold makes it trivial to re-pair to a new network without opening the device.
- **Standalone operation** — Wi-Fi loss degrades app control and OTA, but **the mirror continues to work perfectly from the physical buttons.** No connectivity is required for core product function.
- **mDNS discovery** — the app finds the device automatically by service name. No manual IP entry required.

---

## Related

- [Controller — Hardware Failure](controller-hardware-failure.md) — if the radio is physically dead
- [Controller — NVS Corruption](controller-nvs-corruption.md) — if Wi-Fi credentials were lost from NVS
- [Button — Recessed Failure](button-recessed-failure.md) — if you can't enter pairing mode
