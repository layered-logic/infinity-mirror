---
title: ESP32 Module Shortlist
type: engineering
phase: 1
week: 1
date: 2026-04-06
status: complete
tags: [engineering, esp32, hardware, decision]
---

# ESP32 Module Shortlist — Strengths & Weaknesses
*Filtered for: small, cheap, available, Wi-Fi + BLE. Zigbee noted where applicable.*

## Decision (Apr 6, 2026)
**Selected: ESP32-C3-MINI-1** for v1.0 hardware.

Rationale: best combination of small footprint, lowest cost (~$2 LCSC), strong availability, and built-in USB Serial/JTAG (eliminates a discrete USB-to-UART chip from the PCB). Wi-Fi + BLE 5.0 is more than sufficient for app control and provisioning. Single-core 160 MHz is not a constraint for LED animation workloads.

**C6 variant flagged for future consideration:** The ESP32-C6-MINI-1 adds Zigbee/Thread/Matter in the same footprint for ~$0.80 more. Rather than adding this to v1.0 speculatively, the plan is to survey early customers on smart home ownership and Zigbee/Thread device usage. If demand is there, a C6-based v2 SKU is a natural, low-risk product line extension. The firmware and PCB layout would be largely the same — just swap the module.

---

*Strengths/weaknesses analysis below.*
*Not a recommendation — just the tradeoffs.*

---

## ESP32-WROOM-32E
**Size:** 25.5 × 18.0 × 3.1 mm | **Price:** ~$2.58 LCSC / ~$3.10–$3.50 Mouser | **Availability:** ★★★★★

**Strengths**
- The most documented ESP32 module on earth. Every forum post, tutorial, library, and Stack Overflow answer is probably written against this thing.
- Classic Bluetooth (BR/EDR) + BLE — if you ever want A2DP audio, BT serial, or legacy device pairing, this is the only current module that can do it.
- 26 GPIO, 2 I2C, 3 UART, 18 ADC channels — more headroom than you'll likely need for LED control.
- Dual-core at 240 MHz — substantial compute margin; you'll never be CPU-bound running NeoPixel animations.
- Available absolutely everywhere. Mouser, DigiKey, LCSC, Adafruit, SparkFun, Amazon, local electronics shops.

**Weaknesses**
- Largest footprint of the shortlist at 25.5 × 18 mm — meaningfully bigger than the others.
- No Zigbee. No Thread. No path to smart home without adding a separate hub or chip.
- ADC2 cannot be used while Wi-Fi is active — a silicon bug that will surprise you if you plan on analog inputs and Wi-Fi simultaneously.
- GPIO 6–11 are permanently reserved for the internal flash — easy to accidentally route to in a schematic.
- No native USB. Programming and debug require a separate USB-to-UART chip on your PCB.
- Classic BT is a feature you probably don't need — you're paying (in die area) for something unused.

---

## ESP32-MINI-1
**Size:** 13.2 × 19.0 × 2.4 mm | **Price:** ~$3.20 LCSC / ~$3.80–$4.20 Mouser | **Availability:** ★★★☆☆

**Strengths**
- Same LX6 dual-core silicon as the WROOM-32E — identical software compatibility, same toolchain, same libraries.
- Significantly narrower (13.2 mm vs 18.0 mm) — fits tighter PCB layouts.
- Thinner at 2.4 mm vs 3.1 mm — slightly easier in constrained enclosures.
- Classic BT + BLE, same as WROOM-32E.

**Weaknesses**
- More expensive than WROOM-32E for the same silicon — you pay a premium for the smaller footprint.
- Flash locked to 4 MB only — no 8 or 16 MB options.
- Still no Zigbee/Thread.
- Same ADC2/Wi-Fi limitation and GPIO6–11 reservation as the WROOM-32E.
- Fewer stockists — Mouser and DigiKey carry it, but less shelf presence than WROOM-32E. Harder to substitute in a pinch.
- No native USB.

---

## ESP32-PICO-MINI-02
**Size:** 13.2 × 16.6 × 2.4 mm | **Price:** ~$3.80 LCSC / ~$4.50–$5.00 Mouser | **Availability:** ★★☆☆☆

**Strengths**
- Smallest dual-core LX6 module available.
- 8 MB flash + 2 MB PSRAM are baked into the SiP (System-in-Package) — no external chips, so the compact footprint doesn't sacrifice memory.
- Same dual-core 240 MHz LX6 as WROOM-32E. Full software compatibility.
- Classic BT + BLE.

**Weaknesses**
- Most expensive of the LX6 modules — you're paying for the SiP packaging.
- No Zigbee/Thread.
- Lower stockist count — not as grab-able as WROOM-32E if supply gets tight.
- Same ADC2/Wi-Fi issue and reserved GPIO pins as the rest of the LX6 family.
- No native USB.
- The 2 MB PSRAM, while present, is probably unnecessary for LED control firmware.

---

## ESP32-C3-MINI-1
**Size:** 13.2 × 16.6 × 2.4 mm | **Price:** ~$2.01 LCSC / ~$2.50–$3.00 Mouser | **Availability:** ★★★★★

**Strengths**
- Cheapest module on the shortlist by a clear margin (~$2 LCSC).
- Small footprint at 13.2 × 16.6 mm — same as PICO-MINI-02, but cheaper.
- Wi-Fi + BLE 5.0 — plenty capable for app control and BLE provisioning.
- Built-in USB Serial/JTAG — no external USB-to-UART chip needed on your PCB for programming/debug.
- Flash embedded in the SoC package — fewer components to fail.
- Wide availability — nearly as ubiquitous as the WROOM-32E.
- RISC-V architecture is well-supported in ESP-IDF and Arduino; the ecosystem has fully matured.

**Weaknesses**
- Single-core at 160 MHz — half the compute of the LX6/LX7 modules. Likely still more than enough for LED animation, but headroom is tighter.
- Only 15 GPIO exposed on the module — the lowest of the shortlist. Pin budget requires discipline.
- Only 1 I2C port, 1 general-purpose SPI port, 6 ADC channels — peripheral count is lean.
- No Classic Bluetooth.
- No Zigbee/Thread — no smart home path without a separate chip.
- ADC2 (only 1 channel on C3) is unreliable with Wi-Fi active.
- USB is CDC/JTAG only — no USB host, no HID device emulation.

---

## ESP32-C6-MINI-1
**Size:** 13.2 × 16.6 × 2.4 mm | **Price:** ~$2.82 LCSC / ~$3.50–$4.50 Mouser | **Availability:** ★★★☆☆ (growing)

**Strengths**
- **The only small/cheap module with Zigbee + Thread + Matter built in** — if smart home integration is a goal, this is the path that doesn't require a second chip.
- Zigbee, Thread, and Wi-Fi can run simultaneously (time-division; not true concurrent, but works in practice).
- Wi-Fi 6 (802.11ax) — backward compatible with b/g/n; not a feature you need, but it's not a downside either.
- BLE 5.3 — solid for provisioning and app connectivity.
- Low-power (LP) core (32 MHz RISC-V) runs independently while the main core sleeps — good for battery-conscious designs or always-on sensor polling.
- Built-in USB Serial/JTAG — same benefit as C3, no external UART chip needed.
- Availability is growing fast as Matter ecosystem demand pulls it into more distributor stock.

**Weaknesses**
- More expensive than the C3 for similar base capability if you don't use Zigbee.
- Only 7 ADC channels and 1 general-purpose SPI port — the most constrained peripheral set on the shortlist after the H2.
- No Classic Bluetooth.
- No DAC.
- USB is CDC/JTAG only — same as C3.
- Single-core HP at 160 MHz (same as C3) — no dual-core option.
- Slightly newer silicon — smaller community and fewer real-world deployment examples than C3 or WROOM-32E.

---

## ESP32-H2-MINI-1
**Size:** 13.2 × 16.6 × 2.4 mm | **Price:** ~$2.45 LCSC / ~$3.00–$3.80 Mouser | **Availability:** ★★☆☆☆

**Strengths**
- Native IEEE 802.15.4 (Zigbee 3.0 + Thread 1.4 + Matter-over-Thread) — purpose-built for mesh/smart home.
- BLE 5.3 for direct device pairing.
- Best operating temperature of the shortlist (–40 to 105°C standard across all variants).
- Small and relatively cheap.
- Built-in USB Serial/JTAG.

**Weaknesses**
- **No Wi-Fi at all.** This is not an oversight — the H2 was designed to be a mesh endpoint, not a standalone connected device. Without Wi-Fi, you cannot do direct app control over a local network.
- Slowest clock of the shortlist at 96 MHz.
- Only 19 GPIO and 5 ADC channels — the most constrained of the group.
- To use H2 as the controller with any Wi-Fi functionality, you'd need to pair it with a second chip (e.g., a C6 or WROOM-32E acting as a Wi-Fi/Thread border router). That adds cost, complexity, and PCB space.
- Niche — smaller community, fewer examples, less mature tooling compared to C3 or WROOM-32E.

---

## Dropped from Shortlist (and Why)

| Module | Reason Dropped |
|---|---|
| ESP32-WROVER-E/IE | Bigger (31.4 mm long) and more expensive — PSRAM is overkill for LED control |
| ESP32-S2-MINI-1 | No Bluetooth at all — not viable for BLE app pairing |
| ESP32-S3-WROOM-1 / MINI-1 | AI acceleration, higher cost — more module than needed here |
| All -U antenna variants | Same silicon as their PCB-antenna counterparts; only relevant if your enclosure blocks the onboard antenna |

---

## Related

- [Index / MOC](../index.md)
- [ESP32 Module Comparison](esp32-module-comparison.md) — full-field comparison this shortlist was drawn from
- [BOM Breakdown](bom-breakdown-basic-6x6.md) — where the ~$2 module cost lands
- [Sprint Plan](../sprint_plan.md)
