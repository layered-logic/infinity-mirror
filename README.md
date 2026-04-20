---
title: Infinity Mirror — Project README
type: overview
status: active
tags: [readme, overview]
---

# Infinity Mirror

### A Human-Centered Design Engineering Venture by [Layered Logic](https://github.com/layered-logic)

---

> *Design a system, not just a gadget.*

An ESP32-powered infinity mirror built from the ground up — not just as a piece of hardware, but as a **complete product system**: interaction design, sustainable manufacturing, made-to-order operations, and a venture-ready business model.

This repository documents the full journey from prototype to pre-production, serving as both an engineering portfolio and a living product development case study.

---

## The Product

| | |
|---|---|
| **What** | App-connected LED infinity mirror with customizable patterns and colors |
| **How** | ESP32 microcontroller, WS2812B addressable LEDs, BLE/Wi-Fi, native mobile app |
| **Why** | Personalized ambient lighting that reflects the owner — standard designs or fully bespoke |

**Base model:** 6" x 6" — standard designs starting at $50
**Custom:** Bespoke shapes, sizes, patterns, and frame materials — designed with the customer

---

## What Makes This Different

This isn't just a build log. Every design decision is grounded in **human-centered design engineering**:

- **Stakeholder Mapping** — Understanding everyone who touches the product, from buyer to end-of-life
- **Sustainability Audit** — Full life cycle analysis. Replaceable LEDs. Recyclable frames. Documented environmental impact
- **Right-to-Repair** — A published repair guide so owners can maintain their own mirrors
- **Service Blueprint** — The complete user journey mapped from discovery to daily use to troubleshooting
- **User Research** — Real conversations with real people shaping every interface and business decision

---

## Project Structure

```
├── sprint_plan.md          # 11-week development sprint (Spring 2026)
├── sprint_log.md           # Progress tracker (done / in-progress / blocked)
├── tools/                  # Internal business utilities
│   └── pricing/            # Streamlit COGS/pricing calculator dashboard
├── hardware/               # Schematics, PCB layouts, BOM
├── firmware/               # ESP32 firmware (Wi-Fi, BLE, LED control)
├── app/                    # Mobile app (color/pattern control, custom orders)
├── docs/                   # HCDE deliverables, business model, case study
│   ├── stakeholder-map
│   ├── sustainability-audit
│   ├── repair-guide
│   ├── service-blueprint
│   └── venture-readiness
└── assets/                 # Product photography, brand identity, media
```

---

## Milestones

| Target | Milestone |
|--------|-----------|
| Apr 18 | Business foundation — competitive analysis, pricing model, stakeholder map, sustainability goals |
| May 9 | Brand identity finalized, app UI designed, PCB hardware validated, supply chain mapped |
| May 30 | Go-to-market ready — service blueprint, repair manual, media assets, pre-production mirror built |
| Jun 12 | Portfolio case study complete. Venture readiness delivered. **Launch-ready.** |

---

## Tech Stack

| Layer | Technology |
|-------|------------|
| Microcontroller | ESP32 (Wi-Fi + BLE) |
| LEDs | WS2812B (Neopixel-compatible) addressable RGB |
| PCB Design | KiCad |
| Firmware | C/C++ (PlatformIO) |
| Mobile App | TBD — native (iOS/Android) |
| 3D Visualizer | React + Three.js (React Three Fiber) |
| Prototyping | Figma (app UX/UI) |

---

## Background

This project evolved from several earlier experiments:

- An **STM8 LED controller** with button-driven color cycling and animations
- A **React Three.js visualizer** for previewing mirror designs in 3D
- An **ESP32 strip tester** for validating LED configurations
- **Custom KiCad PCB designs** for a purpose-built infinity mirror controller

The independent study takes these building blocks and asks: *What would it take to turn this into a real, sellable, supportable product — and do it responsibly?*

---

## License

TBD

---

## Navigating This Repo

- [**index.md**](index.md) — Map of Content / home note (start here for any session)
- [Sprint Plan](sprint_plan.md) · [Sprint Log](sprint_log.md) — 11-week plan and progress
- [docs/](docs/) — HCDE artifacts, business, engineering, meeting logs
- [Assembly_docs/](Assembly_docs/) — production-line ops
- [assets/](assets/) — design assets (logo, color, type, photos, Figma exports)
- [tools/](tools/) — internal utilities (pricing calculator)

---

<p align="center"><em>Layered Logic — Spring 2026</em></p>
