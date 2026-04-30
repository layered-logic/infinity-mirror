---
title: Repair-Driven Design Decisions
type: hcde
phase: 2
week: 5
status: draft
tags: [hcde, repair, design-rationale, portfolio, decision-matrix]
---

# Repair-Driven Design Decisions

**Author:** William White
**Date:** April 30, 2026
**Status:** Draft — companion artifact to [right-to-repair-philosophy.md](right-to-repair-philosophy.md) and the [Failure_Modes/](../Failure_Modes/) inventory.

The [philosophy doc](right-to-repair-philosophy.md) states the repair *commitments*. The [failure mode inventory](../Failure_Modes/README.md) enumerates *what can break* and how the buyer fixes it. This doc closes the loop in the other direction: it walks through the **specific design decisions** made during v1.0 development and traces each one back to the repair value(s) it serves and the failure mode(s) it pre-empts. It is the portfolio artifact that proves repairability wasn't retrofitted onto the design — it shaped the design from the start.

The structure is deliberately a matrix rather than prose. Repair claims are easy to make in narrative form; harder to make in a table where every row has to terminate at a specific commitment, a specific failure mode, and a specific cross-link.

---

## 1. Repair values, defined

The matrix below cross-references each design decision against five repair values. They're listed here so the column abbreviations in §2 are unambiguous.

| Code | Value | One-line meaning |
|---|---|---|
| **R** | Reopenable | The product can be disassembled with standard tools, without destruction, indefinitely. |
| **C** | Commodity parts | Replacement parts are available from multiple independent suppliers — not gated by Layered Logic's warehouse. |
| **S** | Self-recovery | Common faults can be cleared by the owner without contacting Layered Logic. |
| **L** | Long service life | The decision actively extends the product's expected lifetime in service. |
| **O** | OTA-fixable | Software-side issues are correctable without anyone opening the enclosure. |

A decision can serve multiple values; the matrix marks each one that applies.

---

## 2. The decision matrix

Every row of this matrix terminates at: (a) a specific repair value, (b) a specific failure mode in [Failure_Modes/](../Failure_Modes/) it pre-empts, and (c) a cross-link to the spec or scoping doc where the decision is recorded.

| # | Decision | R | C | S | L | O | Failure mode pre-empted | Source |
|---|---|---|---|---|---|---|---|---|
| 1 | **Phillips-head fasteners only.** No proprietary bits, no glue, no ultrasonic welds. The enclosure opens with a tool every household owns. | ✓ |  | ✓ |  |  | All cases that require accessing internal components — see [solder joint](../Failure_Modes/wiring-solder-joint-failure.md), [PSU swap](../Failure_Modes/psu-no-output.md), etc. | [right-to-repair-philosophy §2](right-to-repair-philosophy.md) |
| 2 | **Friction-fit + screwed assembly, glued nowhere.** Every panel that joins another can be separated and rejoined without destroying either side. | ✓ |  | ✓ | ✓ |  | [Acrylic crack/scratch](../Failure_Modes/acrylic-crack-scratch.md), [frame physical damage](../Failure_Modes/frame-physical-damage.md) | [right-to-repair-philosophy §2](right-to-repair-philosophy.md), [LED Mirror Project Brief](../external_chats/LED_Mirror_Project_Brief.md) |
| 3 | **WS2812B 5M/60 commodity LED strip** (not a custom or proprietary lighting module). |  | ✓ |  |  |  | [LED dead pixel](../Failure_Modes/led-dead-pixel.md), [strip total failure](../Failure_Modes/led-strip-total-failure.md), [color degradation](../Failure_Modes/led-color-degradation.md) | [BOM breakdown](bom-breakdown-basic-6x6.md) |
| 4 | **5V barrel-jack PSU brick** (industry-standard plug, replaceable from any electronics retailer). |  | ✓ | ✓ |  |  | [PSU no output](../Failure_Modes/psu-no-output.md), [voltage instability](../Failure_Modes/psu-voltage-instability.md) | [BOM breakdown](bom-breakdown-basic-6x6.md), [sustainability-audit §2](sustainability-audit.md) |
| 5 | **PETG (printable) for all internal printed parts.** Owner can reprint from STL on request. The geometry is deliberately printable on consumer FDM, not just industrial machines. | ✓ | ✓ | ✓ |  |  | [PETG breakage](../Failure_Modes/petg-breakage.md) | [right-to-repair-philosophy §3](right-to-repair-philosophy.md), [LED Mirror Project Brief](../external_chats/LED_Mirror_Project_Brief.md) |
| 6 | **Replaceable controller module** (the ESP32-C3 sits on a separate PCB module rather than being embedded in the main board). | ✓ | ✓ | ✓ |  |  | [Controller hardware failure](../Failure_Modes/controller-hardware-failure.md), [Wi-Fi failure](../Failure_Modes/controller-wifi-failure.md) | [firmware-architecture-scoping §6](firmware-architecture-scoping.md) |
| 7 | **OTA firmware updates with self-hosted infrastructure** (`ota.layeredlogic.cc`, Cloudflare Worker + R2). Software bugs ship as binary updates that arrive without any owner action. |  |  |  | ✓ | ✓ | [Firmware corruption](../Failure_Modes/controller-firmware-corruption.md), all classes of post-ship software regression | [firmware-architecture-scoping §5.4](firmware-architecture-scoping.md), [firmware-security.md](firmware-security.md) |
| 8 | **Self-hosted OTA on `ota.layeredlogic.cc`** (not on a third-party platform). Software updates remain available even if a third-party vendor goes EOL. |  |  |  | ✓ | ✓ | Indirect: protects against supply-chain shutdown of update infrastructure | [firmware-security.md](firmware-security.md), [reference_domain_layeredlogic memory](../../../.claude/projects/C--Users-bowhi-Desktop-Independent-Study/memory/reference_domain_layeredlogic.md) |
| 9 | **NVS-stored settings** (Wi-Fi creds, color, pattern, brightness). State persists across reboots; a power cut doesn't strand the owner in setup mode. |  |  | ✓ | ✓ |  | [NVS data loss](../Failure_Modes/controller-nvs-corruption.md) (handled gracefully — first-boot defaults restore device to a usable state) | [firmware-spec.md](firmware-spec.md), [project_firmware_status memory](../../../.claude/projects/C--Users-bowhi-Desktop-Independent-Study/memory/project_firmware_status.md) |
| 10 | **Recessed factory-reset button.** Owner can clear bricked Wi-Fi state, paired credentials, or stale settings without contacting Layered Logic. |  |  | ✓ |  |  | [Wi-Fi failure](../Failure_Modes/controller-wifi-failure.md), [NVS corruption](../Failure_Modes/controller-nvs-corruption.md), provisioning-loop edge cases | [button-interface §3](button-interface.md), [button-design-rationale §3](button-design-rationale.md) |
| 11 | **App-driven provisioning + control** (no firmware-flashing for setup or daily use). |  |  | ✓ | ✓ | ✓ | Stale firmware bugs that would otherwise require user-facing flashing | [App spec](app-spec.md), [webapp-spec.md](webapp-spec.md) |
| 12 | **Reflash service for dead controllers** (cost-of-shipping, not a paid repair). The closed-binary firmware is the philosophy doc's [§4 honest limit](right-to-repair-philosophy.md#4-the-honest-limits) — this is the mitigation. |  |  | ✓ | ✓ |  | Replacement controllers shipped without firmware can be made functional via the reflash service | [right-to-repair-philosophy §4](right-to-repair-philosophy.md) |
| 13 | **Versioned PCB silkscreen** — every shipped board carries its revision letter visibly. An owner reading a schematic can match it to their physical board. | ✓ |  | ✓ |  |  | All controller-side failure modes that require schematic reference | [right-to-repair-philosophy §3](right-to-repair-philosophy.md) |
| 14 | **STL files supplied on request** for any owner reporting a printable-part repair need. Not published by default — service-driven distribution. |  | ✓ | ✓ |  |  | [PETG breakage](../Failure_Modes/petg-breakage.md), edge cases involving custom modifications | [right-to-repair-philosophy §3](right-to-repair-philosophy.md) |
| 15 | **Public schematic references on GitHub** (per-revision folder). Anyone can read along with the design that's inside their unit. | ✓ |  |  | ✓ |  | Indirect: enables independent repair by hobbyists or repair shops | [right-to-repair-philosophy §3](right-to-repair-philosophy.md) |
| 16 | **7-year minimum replacement-part availability** for the PCB and controller module. Commodity parts (LED strip, PSU, screws, acrylic) outlive this on their own. |  | ✓ |  | ✓ |  | All controller-side failures across the documented service window | [right-to-repair-philosophy §3](right-to-repair-philosophy.md) |
| 17 | **No bonded enclosure.** Acrylic panels sit in milled channels, not adhesive. Replaceable as separate parts. | ✓ |  | ✓ | ✓ |  | [Acrylic crack/scratch](../Failure_Modes/acrylic-crack-scratch.md), [coating degradation](../Failure_Modes/acrylic-coating-degradation.md) | [LED Mirror Project Brief](../external_chats/LED_Mirror_Project_Brief.md), [sustainability-audit §2](sustainability-audit.md) |
| 18 | **Open-mode auth as the default.** Roommates and guests can use the mirror without authentication friction; reduces "I can't control my own light" support burden. |  |  | ✓ |  |  | Pairing-state edge cases that would otherwise need a factory reset | [firmware-security.md](firmware-security.md) |
| 19 | **Three-tier product line shares a common firmware core.** Basic (STM8), Pro (ESP32), Pro+ (ESP32 + Matter) all draw from `Firmware/v1/core/`. A bug fix on one tier benefits all tiers. |  |  |  | ✓ | ✓ | Indirect: amortizes the maintenance burden across the product line | [firmware-architecture-scoping §6](firmware-architecture-scoping.md) |
| 20 | **LED strip pre-tested at receiving** before the build. Catches DOA strips before they enter the unit; reduces field-failure rate. |  |  |  | ✓ |  | [LED total failure](../Failure_Modes/led-strip-total-failure.md) (frequency reduction, not elimination) | [Assembly_docs/LED_Mirror_Project_Brief](../Assembly_docs/LED_Mirror_Project_Brief.md) |

---

## 3. Reading the matrix

A few patterns the matrix surfaces, none of which are coincidence:

- **Every controller-side failure mode terminates at either a self-recovery path (recessed button, OTA) or a swap path (replaceable module, reflash service).** No controller failure requires Layered Logic to physically touch the unit; either the owner clears it, the OTA fixes it, or a board ships out and back. This is the fundamental architectural decision behind the closed-binary acknowledgement in [philosophy §4](right-to-repair-philosophy.md#4-the-honest-limits) — the binary is closed, but its consequences are not stranded.
- **Commodity parts dominate the BOM.** Of the 17 enumerated failure modes in the [inventory](../Failure_Modes/README.md), 14 are resolved by parts available from multiple independent suppliers — i.e., the buyer's repair path doesn't depend on Layered Logic continuing to exist. The 3 that do depend on Layered Logic (the PCB, the controller module, and the firmware reflash service) are exactly the cases where [§4 of the philosophy](right-to-repair-philosophy.md#4-the-honest-limits) names the dependency explicitly.
- **OTA-fixability protects against the failure mode that doesn't appear in the inventory: bugs I haven't shipped yet.** A static enumeration of failure modes is a snapshot; software regressions that arrive after ship are continuous. OTA is the design lever that keeps post-ship surface area small.
- **Long-service-life value (L) clusters around decisions that make the product *adaptive*, not just *durable*.** Reprintable parts, replaceable modules, OTA-updatable firmware, schematic availability — these don't reduce wear, they reduce the cost of recovery from wear. The product is designed to outlive its components by replacing them, not to outlive its components by lasting forever.

---

## 4. What this matrix doesn't cover

For honesty, the categories of decision this doc deliberately leaves out:

- **Cost-driven decisions** that don't terminate at a repair value. The matrix is filtered to entries where repair was a *driver*, not a side effect.
- **The user-facing repair guide itself** — that's a separate Week 8 deliverable, blocked on the assembly guide.
- **Quantitative assessment** of how the decisions perform in practice. That's the [repairability scorecard](repairability-scorecard.md).
- **End-of-life paths** other than repair. The acrylic-coating recyclability gap is documented in [sustainability-audit §7](sustainability-audit.md) — it's a stated limit of the philosophy, not a design decision this doc claims credit for solving.

---

## Related

- [Right-to-Repair Philosophy](right-to-repair-philosophy.md) — the values framework this matrix substantiates
- [Failure Mode Inventory](../Failure_Modes/README.md) — every cell in the "Failure mode pre-empted" column links here
- [Repairability Scorecard](repairability-scorecard.md) — quantitative companion
- [Repair Index](repair-index.md) — entry point for the repair-ecosystem docs
- [Sustainability Audit](sustainability-audit.md) — lifecycle context, including the EOL gap that bounds the philosophy
- [Brand Positioning](brand-positioning.md) — "Living" adjective that this matrix operationalizes
- [Button Design Rationale](button-design-rationale.md) — paired-format companion (button-side decisions; this doc covers repair-side decisions)
- [Sprint Plan](../sprint_plan.md) — Week 4 deliverables, Week 8 follow-ups
