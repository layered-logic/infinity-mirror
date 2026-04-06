# ESP32 Module Comparison

All Espressif production modules — current recommended-for-new-design variants only (legacy WROOM-32/32D omitted).
Prices are LCSC single-unit (cheapest legitimate source) and Mouser/DigiKey single-unit (Western distributor), approximate as of early 2026.
✓ = present, ✗ = absent.

---

## Table 1 — Identity, Dimensions & Cost

| Module | SoC | CPU Arch | Cores | Clock | Dimensions (mm) | Flash | PSRAM | LCSC 1pc | Mouser/DigiKey 1pc |
|---|---|---|---|---|---|---|---|---|---|
| ESP32-WROOM-32E | ESP32-D0WD-V3 | Xtensa LX6 | 2 | 240 MHz | 25.5 × 18.0 × 3.1 | 4 / 8 / 16 MB | ✗ (✓ 2 MB on -N4R2 only) | ~$2.58 | ~$3.10–$3.50 |
| ESP32-WROOM-32UE | ESP32-D0WD-V3 | Xtensa LX6 | 2 | 240 MHz | 19.2 × 18.0 × 3.2 | 4 / 8 / 16 MB | ✗ | ~$2.65 | ~$3.20–$3.60 |
| ESP32-WROVER-E | ESP32-D0WD-V3 | Xtensa LX6 | 2 | 240 MHz | 18.0 × 31.4 × 3.3 | 4 / 8 / 16 MB | ✓ 8 MB | ~$3.23 | ~$4.50–$5.50 |
| ESP32-WROVER-IE | ESP32-D0WD-V3 | Xtensa LX6 | 2 | 240 MHz | 18.0 × 31.4 × 3.3 | 4 / 8 / 16 MB | ✓ 8 MB | ~$3.30 | ~$4.70–$5.70 |
| ESP32-MINI-1 | ESP32-U4WDH | Xtensa LX6 | 2 | 240 MHz | 13.2 × 19.0 × 2.4 | 4 MB | ✗ | ~$3.20 | ~$3.80–$4.20 |
| ESP32-MINI-1U | ESP32-U4WDH | Xtensa LX6 | 2 | 240 MHz | 13.2 × 13.5 × 2.4 | 4 MB | ✗ | ~$3.25 | ~$3.90–$4.30 |
| ESP32-PICO-MINI-02 | ESP32-PICO-V3-02 | Xtensa LX6 | 2 | 240 MHz | 13.2 × 16.6 × 2.4 | 8 MB (SiP) | ✓ 2 MB (SiP) | ~$3.80 | ~$4.50–$5.00 |
| ESP32-PICO-MINI-02U | ESP32-PICO-V3-02 | Xtensa LX6 | 2 | 240 MHz | 13.2 × 11.2 × 2.4 | 8 MB (SiP) | ✓ 2 MB (SiP) | ~$3.85 | ~$4.60–$5.10 |
| ESP32-S2-MINI-1 | ESP32-S2FH4 | Xtensa LX7 | 1 | 240 MHz | 20.0 × 15.4 × 2.4 | 4 MB | ✗ (✓ 2 MB on -N4R2) | ~$2.35 | ~$2.80–$3.20 |
| ESP32-S2-MINI-1U | ESP32-S2FH4 | Xtensa LX7 | 1 | 240 MHz | ~14.3 × 15.4 × 2.4 | 4 MB | ✗ | ~$2.38 | ~$2.90–$3.30 |
| ESP32-S2-SOLO-2 | ESP32-S2FH4 | Xtensa LX7 | 1 | 240 MHz | 25.5 × 18.0 × 3.1 | 4 / 8 / 16 MB | ✗ (✓ 2 MB on R2 variants) | ~$2.40 | ~$3.00–$3.50 |
| ESP32-S3-WROOM-1 | ESP32-S3 | Xtensa LX7 | 2 | 240 MHz | 25.5 × 16.5 × 3.1 | 4 / 8 / 16 MB | ✓ 0 / 2 / 8 / 16 MB | ~$3.64–$3.79 | ~$4.50–$5.50 |
| ESP32-S3-WROOM-1U | ESP32-S3 | Xtensa LX7 | 2 | 240 MHz | 19.2 × 16.5 × 3.2 | 4 / 8 / 16 MB | ✓ 0 / 2 / 8 / 16 MB | ~$3.70–$3.85 | ~$4.60–$5.60 |
| ESP32-S3-MINI-1 | ESP32-S3FH4R2 | Xtensa LX7 | 2 | 240 MHz | 20.5 × 15.4 × 2.4 | 4 / 8 MB | ✓ 0 / 2 MB | ~$3.26–$3.50 | ~$4.00–$5.00 |
| ESP32-S3-MINI-1U | ESP32-S3FH4R2 | Xtensa LX7 | 2 | 240 MHz | 15.4 × 15.4 × 2.4 | 4 / 8 MB | ✓ 0 / 2 MB | ~$3.30–$3.55 | ~$4.10–$5.10 |
| ESP32-C3-MINI-1 | ESP32-C3 | RISC-V 32-bit | 1 | 160 MHz | 13.2 × 16.6 × 2.4 | 4 MB | ✗ | ~$2.01 | ~$2.50–$3.00 |
| ESP32-C3-MINI-1U | ESP32-C3 | RISC-V 32-bit | 1 | 160 MHz | 13.2 × 12.5 × 2.4 | 4 MB | ✗ | ~$2.05 | ~$2.55–$3.05 |
| ESP32-C6-MINI-1 | ESP32-C6 | RISC-V 32-bit | 1 HP + 1 LP | 160 / 32 MHz | 13.2 × 16.6 × 2.4 | 4 / 8 MB | ✗ | ~$2.82 | ~$3.50–$4.50 |
| ESP32-C6-MINI-1U | ESP32-C6 | RISC-V 32-bit | 1 HP + 1 LP | 160 / 32 MHz | 13.2 × 12.5 × 2.4 | 4 / 8 MB | ✗ | ~$2.86 | ~$3.60–$4.60 |
| ESP32-H2-MINI-1 | ESP32-H2 | RISC-V 32-bit | 1 | 96 MHz | 13.2 × 16.6 × 2.4 | 2 / 4 MB | ✗ | ~$2.45 | ~$3.00–$3.80 |
| ESP32-H2-MINI-1U | ESP32-H2 | RISC-V 32-bit | 1 | 96 MHz | 13.2 × 12.5 × 2.4 | 2 / 4 MB | ✗ | ~$2.48 | ~$3.05–$3.85 |

---

## Table 2 — Radio & Connectivity Features

| Module | Wi-Fi Standard | Bluetooth | Classic BT (BR/EDR) | BLE | Zigbee / 802.15.4 | Thread | Matter | USB Native PHY | USB Type |
|---|---|---|---|---|---|---|---|---|---|
| ESP32-WROOM-32E | 802.11 b/g/n | ✓ v4.2 | ✓ | ✓ | ✗ | ✗ | ✗ | ✗ | — |
| ESP32-WROOM-32UE | 802.11 b/g/n | ✓ v4.2 | ✓ | ✓ | ✗ | ✗ | ✗ | ✗ | — |
| ESP32-WROVER-E | 802.11 b/g/n | ✓ v4.2 | ✓ | ✓ | ✗ | ✗ | ✗ | ✗ | — |
| ESP32-WROVER-IE | 802.11 b/g/n | ✓ v4.2 | ✓ | ✓ | ✗ | ✗ | ✗ | ✗ | — |
| ESP32-MINI-1 | 802.11 b/g/n | ✓ v4.2 | ✓ | ✓ | ✗ | ✗ | ✗ | ✗ | — |
| ESP32-MINI-1U | 802.11 b/g/n | ✓ v4.2 | ✓ | ✓ | ✗ | ✗ | ✗ | ✗ | — |
| ESP32-PICO-MINI-02 | 802.11 b/g/n | ✓ v4.2 | ✓ | ✓ | ✗ | ✗ | ✗ | ✗ | — |
| ESP32-PICO-MINI-02U | 802.11 b/g/n | ✓ v4.2 | ✓ | ✓ | ✗ | ✗ | ✗ | ✗ | — |
| ESP32-S2-MINI-1 | 802.11 b/g/n | ✗ | ✗ | ✗ | ✗ | ✗ | ✗ | ✓ | Full OTG |
| ESP32-S2-MINI-1U | 802.11 b/g/n | ✗ | ✗ | ✗ | ✗ | ✗ | ✗ | ✓ | Full OTG |
| ESP32-S2-SOLO-2 | 802.11 b/g/n | ✗ | ✗ | ✗ | ✗ | ✗ | ✗ | ✓ | Full OTG |
| ESP32-S3-WROOM-1 | 802.11 b/g/n | ✓ v5.0 | ✗ | ✓ | ✗ | ✗ | ✗ | ✓ | OTG + Serial/JTAG |
| ESP32-S3-WROOM-1U | 802.11 b/g/n | ✓ v5.0 | ✗ | ✓ | ✗ | ✗ | ✗ | ✓ | OTG + Serial/JTAG |
| ESP32-S3-MINI-1 | 802.11 b/g/n | ✓ v5.0 | ✗ | ✓ | ✗ | ✗ | ✗ | ✓ | OTG + Serial/JTAG |
| ESP32-S3-MINI-1U | 802.11 b/g/n | ✓ v5.0 | ✗ | ✓ | ✗ | ✗ | ✗ | ✓ | OTG + Serial/JTAG |
| ESP32-C3-MINI-1 | 802.11 b/g/n | ✓ v5.0 | ✗ | ✓ | ✗ | ✗ | ✗ | ✓ | Serial/JTAG (CDC only) |
| ESP32-C3-MINI-1U | 802.11 b/g/n | ✓ v5.0 | ✗ | ✓ | ✗ | ✗ | ✗ | ✓ | Serial/JTAG (CDC only) |
| ESP32-C6-MINI-1 | Wi-Fi 6 (802.11ax) | ✓ v5.3 | ✗ | ✓ | ✓ | ✓ | ✓ (via Wi-Fi or Thread) | ✓ | Serial/JTAG (CDC only) |
| ESP32-C6-MINI-1U | Wi-Fi 6 (802.11ax) | ✓ v5.3 | ✗ | ✓ | ✓ | ✓ | ✓ (via Wi-Fi or Thread) | ✓ | Serial/JTAG (CDC only) |
| ESP32-H2-MINI-1 | ✗ | ✓ v5.3 | ✗ | ✓ | ✓ | ✓ | ✓ (via Thread) | ✓ | Serial/JTAG (CDC only) |
| ESP32-H2-MINI-1U | ✗ | ✓ v5.3 | ✗ | ✓ | ✓ | ✓ | ✓ (via Thread) | ✓ | Serial/JTAG (CDC only) |

---

## Table 3 — Peripherals & I/O

| Module | GPIO (exposed) | ADC Ch | DAC | I2C | SPI (GP) | UART | I2S | CAN/TWAI | AI Accel | Temp Range | Antenna | FCC/CE |
|---|---|---|---|---|---|---|---|---|---|---|---|---|
| ESP32-WROOM-32E | 26 | 18 (12-bit) | ✓ 2ch | ✓ 2 | ✓ 2 | ✓ 3 | ✓ 1 | ✓ | ✗ | –40 to 85°C (N) / 105°C (H) | PCB trace | ✓ |
| ESP32-WROOM-32UE | 26 | 18 (12-bit) | ✓ 2ch | ✓ 2 | ✓ 2 | ✓ 3 | ✓ 1 | ✓ | ✗ | –40 to 85°C (N) / 105°C (H) | U.FL ext. | ✓ |
| ESP32-WROVER-E | 24 (–2 for PSRAM) | 18 (12-bit) | ✓ 2ch | ✓ 2 | ✓ 2 | ✓ 3 | ✓ 1 | ✓ | ✗ | –40 to 85°C | PCB trace | ✓ |
| ESP32-WROVER-IE | 24 (–2 for PSRAM) | 18 (12-bit) | ✓ 2ch | ✓ 2 | ✓ 2 | ✓ 3 | ✓ 1 | ✓ | ✗ | –40 to 85°C | U.FL ext. | ✓ |
| ESP32-MINI-1 | 28 | 18 (12-bit) | ✓ 2ch | ✓ 2 | ✓ 2 | ✓ 3 | ✓ 1 | ✓ | ✗ | –40 to 85°C (N) / 105°C (H) | PCB trace | ✓ |
| ESP32-MINI-1U | 28 | 18 (12-bit) | ✓ 2ch | ✓ 2 | ✓ 2 | ✓ 3 | ✓ 1 | ✓ | ✗ | –40 to 85°C (N) / 105°C (H) | U.FL ext. | ✓ |
| ESP32-PICO-MINI-02 | 27 | 18 (12-bit) | ✓ 2ch | ✓ 2 | ✓ 3 | ✓ 3 | ✓ 1 | ✓ | ✗ | –40 to 85°C | PCB trace | ✓ |
| ESP32-PICO-MINI-02U | 27 | 18 (12-bit) | ✓ 2ch | ✓ 2 | ✓ 3 | ✓ 3 | ✓ 1 | ✓ | ✗ | –40 to 85°C | U.FL ext. | ✓ |
| ESP32-S2-MINI-1 | 27 | 20 (12-bit) | ✓ 2ch | ✓ 2 | ✓ 2 | ✓ 2 | ✓ 1 | ✓ | ✗ | –40 to 85°C | PCB trace | ✓ |
| ESP32-S2-MINI-1U | 27 | 20 (12-bit) | ✓ 2ch | ✓ 2 | ✓ 2 | ✓ 2 | ✓ 1 | ✓ | ✗ | –40 to 85°C | U.FL ext. | ✓ |
| ESP32-S2-SOLO-2 | 40 | 20 (12-bit) | ✓ 2ch | ✓ 2 | ✓ 2 | ✓ 2 | ✓ 1 | ✓ | ✗ | –40 to 85°C | PCB trace | ✓ |
| ESP32-S3-WROOM-1 | 36 | 20 (12-bit) | ✗ | ✓ 2 | ✓ 2 | ✓ 3 | ✓ 2 | ✓ | ✓ (SIMD/NN) | –40 to 85°C | PCB trace | ✓ |
| ESP32-S3-WROOM-1U | 36 | 20 (12-bit) | ✗ | ✓ 2 | ✓ 2 | ✓ 3 | ✓ 2 | ✓ | ✓ (SIMD/NN) | –40 to 85°C | U.FL ext. | ✓ |
| ESP32-S3-MINI-1 | 39 | 20 (12-bit) | ✗ | ✓ 2 | ✓ 2 | ✓ 3 | ✓ 2 | ✓ | ✓ (SIMD/NN) | –40 to 85°C | PCB trace | ✓ |
| ESP32-S3-MINI-1U | 39 | 20 (12-bit) | ✗ | ✓ 2 | ✓ 2 | ✓ 3 | ✓ 2 | ✓ | ✓ (SIMD/NN) | –40 to 85°C | U.FL ext. | ✓ |
| ESP32-C3-MINI-1 | 15 | 6 (12-bit) | ✗ | ✓ 1 | ✓ 1 | ✓ 2 | ✓ 1 | ✓ | ✗ | –40 to 85°C (N) / 105°C (H) | PCB trace | ✓ |
| ESP32-C3-MINI-1U | 15 | 6 (12-bit) | ✗ | ✓ 1 | ✓ 1 | ✓ 2 | ✓ 1 | ✓ | ✗ | –40 to 85°C (N) / 105°C (H) | U.FL ext. | ✓ |
| ESP32-C6-MINI-1 | 22 | 7 (12-bit) | ✗ | ✓ 2 | ✓ 1 | ✓ 3 | ✓ 1 | ✓ | ✗ | –40 to 85°C (N) / 105°C (H) | PCB trace | ✓ |
| ESP32-C6-MINI-1U | 22 | 7 (12-bit) | ✗ | ✓ 2 | ✓ 1 | ✓ 3 | ✓ 1 | ✓ | ✗ | –40 to 85°C (N) / 105°C (H) | U.FL ext. | ✓ |
| ESP32-H2-MINI-1 | 19 | 5 (12-bit) | ✗ | ✓ 2 | ✓ 1 | ✓ 2 | ✓ 1 | ✓ | ✗ | –40 to 105°C (all) | PCB trace | ✓ |
| ESP32-H2-MINI-1U | 19 | 5 (12-bit) | ✗ | ✓ 2 | ✓ 1 | ✓ 2 | ✓ 1 | ✓ | ✗ | –40 to 105°C (all) | U.FL ext. | ✓ |

---

## Table 4 — Use Cases, Availability & Notes

| Module | Popularity | Availability | Common Uses | Key Notes / Gotchas |
|---|---|---|---|---|
| ESP32-WROOM-32E | ★★★★★ Very popular | Everywhere — Mouser, DigiKey, LCSC, Amazon, Adafruit, SparkFun | General IoT, dev boards, hobbyist projects, Wi-Fi+BT sensors, gateways | Industry-standard. GPIO 6–11 always reserved (flash). ADC2 dead while Wi-Fi is on. Supersedes -32D and original -32. |
| ESP32-WROOM-32UE | ★★★★☆ Popular | Wide — same as 32E | Same as 32E but where external antenna or enclosure placement requires it | Identical to 32E but with U.FL connector instead of PCB trace antenna. Use when antenna placement matters (metal enclosures, deep in product). |
| ESP32-WROVER-E | ★★★★☆ Popular | Wide — Mouser, DigiKey, LCSC | Camera streams, audio buffering, large UIs (LVGL), JSON-heavy apps | PSRAM uses GPIO16/17 — those pins unavailable. 3.3 mm tall — check enclosure clearance. Longer board due to stacked PSRAM chip. |
| ESP32-WROVER-IE | ★★★☆☆ Moderate | Wide | Same as WROVER-E for enclosures needing external antenna | Same gotchas as WROVER-E. U.FL antenna variant. |
| ESP32-MINI-1 | ★★★☆☆ Moderate | Moderate — DigiKey, Mouser, LCSC | Compact dual-core ESP32 where WROOM-32E footprint is too large | Same LX6 silicon as WROOM-32E, narrower (13.2 mm). Flash locked to 4 MB. Same ADC2/Wi-Fi caveat. |
| ESP32-MINI-1U | ★★☆☆☆ Moderate-niche | Moderate | Same as MINI-1 for enclosures needing external antenna | Shortest body of the MINI series (13.5 mm). U.FL. |
| ESP32-PICO-MINI-02 | ★★☆☆☆ Moderate-niche | Moderate — DigiKey, Mouser, LCSC | Tiniest dual-core + PSRAM module; SiP wearable/compact designs | Flash and PSRAM are inside the SiP die stack — no discrete chips. Smallest dual-core option with integrated memory. |
| ESP32-PICO-MINI-02U | ★★☆☆☆ Moderate-niche | Moderate | Extremely compact (11.2 mm body) with SiP memory | Smallest body of any dual-core ESP32 module. U.FL. |
| ESP32-S2-MINI-1 | ★★★☆☆ Moderate | Moderate — DigiKey, Mouser, LCSC | USB HID gadgets, USB CDC serial, USB mass storage, Wi-Fi IoT with no BT needed | **No Bluetooth whatsoever.** Native USB OTG is the entire reason to use this. Popular with MicroPython USB projects. ADC2 unusable with Wi-Fi. |
| ESP32-S2-MINI-1U | ★★☆☆☆ Moderate | Moderate | Same as S2-MINI-1 for external antenna | Same S2 — shorter body with U.FL. |
| ESP32-S2-SOLO-2 | ★★☆☆☆ Niche | Moderate | Drop-in WROOM-32E footprint replacement when USB OTG is needed | Same 25.5 × 18 mm footprint as WROOM-32E. More GPIO breakout than MINI-1. No Bluetooth. |
| ESP32-S3-WROOM-1 | ★★★★★ Very popular | Wide — DigiKey, Mouser, LCSC, Adafruit | Camera + display (ESP-WHO), ML/AI inference, audio (I2S mic/speaker), large LVGL UIs, USB HID + Wi-Fi + BLE combo | **No Classic Bluetooth.** **No DAC.** Octal PSRAM variants (R8/R16V) consume GPIO36/37. USB OTG shares GPIO19/20 with JTAG. Best all-around pick for complex new designs. Huge variant matrix — check -NxRy suffix carefully. |
| ESP32-S3-WROOM-1U | ★★★★☆ Popular | Wide | Same as WROOM-1 for external antenna placement | U.FL. All same S3 gotchas apply. |
| ESP32-S3-MINI-1 | ★★★★☆ Popular & growing | Wide | Compact S3 designs needing USB + dual-core + AI accel; wearables, audio badge, BLE+Wi-Fi portables | Same LX7 silicon as WROOM-1. PSRAM capped at 2 MB (no 8/16 MB option). Good where WROOM-1 footprint is too large. |
| ESP32-S3-MINI-1U | ★★★☆☆ Moderate | Wide | Same as MINI-1 for external antenna | Extremely compact at 15.4 × 15.4 mm. |
| ESP32-C3-MINI-1 | ★★★★★ Very popular | Wide — Mouser, DigiKey, LCSC | Low-cost BLE + Wi-Fi sensor nodes, battery IoT, ESP-NOW mesh, simple smart home devices | **Only 15 GPIO** — plan carefully. Only 1 I2C, 1 general-purpose SPI. Flash embedded in SoC. USB is CDC-only (no host/HID/OTG). Cheapest dual-radio module. ADC2 has 1 channel and is unreliable with Wi-Fi. |
| ESP32-C3-MINI-1U | ★★★★☆ Popular | Wide | Same as C3-MINI-1 for external antenna | Shorter body (12.5 mm). U.FL. |
| ESP32-C6-MINI-1 | ★★★☆☆ Growing fast | Wide & growing — DigiKey, Mouser, LCSC | Matter end devices, Zigbee/Thread sensors, smart home nodes, Wi-Fi 6 low-latency IoT | First Espressif Wi-Fi 6 module. Zigbee AND Thread AND Wi-Fi in one chip (time-division). Only 7 ADC channels and 1 SPI. LP core enables deep sleep with peripheral monitoring. Best for Matter ecosystem. |
| ESP32-C6-MINI-1U | ★★★☆☆ Growing | Wide & growing | Same as C6-MINI-1 for external antenna | U.FL. |
| ESP32-H2-MINI-1 | ★★☆☆☆ Niche but growing | Moderate — DigiKey, Mouser, LCSC, Adafruit | Zigbee end devices, Thread end nodes, Matter-over-Thread sensors, 802.15.4 mesh, BLE beacons | **No Wi-Fi at all.** Designed as a 802.15.4 + BLE endpoint — pairs with an ESP32-C6 or ESP32 as gateway. Slowest clock (96 MHz). Fewest GPIO (19) and ADC (5). -N2 variant has only 2 MB flash — tight for complex firmware. Best temp rating of all (–40 to 105°C standard). |
| ESP32-H2-MINI-1U | ★★☆☆☆ Niche | Moderate | Same as H2-MINI-1 for external antenna | U.FL. |

---

## Cross-Cutting Gotchas (Read Before Choosing)

| Gotcha | Affects |
|---|---|
| **ADC2 cannot be used while Wi-Fi is active** — silicon-level limitation, not a software bug | ESP32 (LX6), S2, S3 |
| **No Classic Bluetooth (BR/EDR)** — only BLE | S3, C3, C6, H2 |
| **No Bluetooth at all** | S2 |
| **No Wi-Fi at all** | H2 |
| **No DAC** — PWM or external DAC required for analog audio out | S3, C3, C6, H2 |
| **GPIO 6–11 always reserved for flash** | All original ESP32 (LX6) modules |
| **GPIO 16–17 reserved for PSRAM** on WROVER | WROVER-E, WROVER-IE |
| **GPIO 36–37 consumed by Octal PSRAM** | S3-WROOM-1 -N4R8 / -N16R16V variants |
| **USB on C3/C6/H2 is CDC/JTAG only** — no USB host, no HID, no mass storage | C3, C6, H2 |
| **Full USB OTG** (host + device, HID, mass storage) | S2, S3 only |
| **Module height is 3.3 mm** (vs 2.4 / 3.1) — check enclosure clearance | WROVER-E, WROVER-IE |
| **All modules are FCC/CE pre-certified** — can simplify end-product certification if RF section is unmodified | All |
| **LCSC is the cheapest legitimate source** — typically 20–40% below Mouser/DigiKey for Espressif modules | All |

---

## Quick Selection Guide (for LED Art Controller Context)

| Need | Pick |
|---|---|
| Standard Wi-Fi + BLE + max ecosystem support | **ESP32-S3-WROOM-1-N8** |
| Compact footprint, same S3 capability | **ESP32-S3-MINI-1-N8** |
| Classic BT audio (A2DP) needed | **ESP32-WROOM-32E** (only current LX6 modules have Classic BT) |
| Wi-Fi only, native USB, no BT | **ESP32-S2-MINI-1** |
| Lowest cost dual-radio node | **ESP32-C3-MINI-1** |
| Matter / Zigbee / Thread | **ESP32-C6-MINI-1** |
| 802.15.4 mesh endpoint, no Wi-Fi | **ESP32-H2-MINI-1** |
| Maximum RAM for camera / large buffers | **ESP32-S3-WROOM-1-N16R8** or **ESP32-WROVER-E-N16R8** |

---

*Sources: Espressif official datasheets; LCSC, DigiKey, Mouser listings; espboards.dev SoC comparison; Espressif FCC/CE certification page.*
