---
title: Repair Ecosystem — Index
type: moc
phase: 2
week: 5
status: active
tags: [moc, repair, hcde, portfolio, navigation]
---

# Repair Ecosystem — Index

> Sub-MOC for the Layered Logic repair story. Linked from the home note at [index.md](../index.md). Walk top-to-bottom for the narrative; jump by section for reference.

The repair commitment for the Infinity Mirror v1.0 spans five connected documents. None of them is a complete picture by itself — values without inventory is a slogan, inventory without rationale is a parts list, and a scorecard without philosophy is just numbers. This index ties them into one navigable artifact.

---

## 1. Start here — the value framework

[**Right-to-Repair Philosophy**](right-to-repair-philosophy.md)

The first-person values document. Why repair matters (pro-consumer conviction, environmental honesty, regulatory tailwind), what repair concretely means for v1.0, eight specific commitments, the honest limits, and what the buyer is asked to do in return. **Read first.** Everything else is downstream of this document.

---

## 2. The technical evidence

[**Failure Mode Inventory**](../Failure_Modes/README.md)

Seventeen enumerated failure modes across eight component categories: LED strip, PSU, controller/PCB, acrylic panels, wood frame, PETG printed parts, wiring, buttons. Each failure mode has its own file with symptoms, causes, probability, repair path, parts sourcing, and design mitigations. This is the answer to "okay, but what does *user-repairable* actually mean for this product?" — twelve fully user-repairable, five partial, zero non-repairable.

Per-failure-mode files (linked from the inventory README):

- **LED strip:** [dead pixel](../Failure_Modes/led-dead-pixel.md) · [total failure](../Failure_Modes/led-strip-total-failure.md) · [color degradation](../Failure_Modes/led-color-degradation.md)
- **PSU:** [no output](../Failure_Modes/psu-no-output.md) · [voltage instability](../Failure_Modes/psu-voltage-instability.md)
- **Controller / PCB:** [hardware failure](../Failure_Modes/controller-hardware-failure.md) · [firmware corruption](../Failure_Modes/controller-firmware-corruption.md) · [Wi-Fi failure](../Failure_Modes/controller-wifi-failure.md) · [NVS corruption](../Failure_Modes/controller-nvs-corruption.md)
- **Acrylic panels:** [crack/scratch](../Failure_Modes/acrylic-crack-scratch.md) · [coating degradation](../Failure_Modes/acrylic-coating-degradation.md)
- **Wood frame:** [physical damage](../Failure_Modes/frame-physical-damage.md)
- **PETG printed parts:** [breakage](../Failure_Modes/petg-breakage.md)
- **Wiring:** [solder joint failure](../Failure_Modes/wiring-solder-joint-failure.md) · [barrel jack intermittent](../Failure_Modes/barrel-jack-intermittent.md)
- **Buttons:** [primary unresponsive](../Failure_Modes/button-primary-failure.md) · [recessed unresponsive](../Failure_Modes/button-recessed-failure.md)

---

## 3. The design rationale

[**Repair-Driven Design Decisions**](repair-design-decisions.md)

Twenty design decisions made during v1.0 development, each one cross-referenced against the repair values it serves and the failure modes it pre-empts. This is the artifact that demonstrates repair wasn't retrofitted — it shaped the choices from the start. Format is matrix-first: every row terminates at a specific failure mode and a specific source citation, not at narrative.

---

## 4. The numbers

[**Repairability Scorecard**](repairability-scorecard.md)

Quantitative companion to the philosophy doc. Top-line: 17 failure modes, 12 fully user-repairable (71%), 0 non-repairable (0%), Phillips screwdriver as the single tool requirement, 7-year minimum replacement-part commitment. Per-failure-mode breakdown of severity, repair tier, tools, parts source, and estimated repair time. Several time-cells are placeholders pending golden-sample measurement; flagged in the doc.

---

## 5. The supporting context

These docs aren't part of the repair ecosystem proper but are load-bearing context for the repair claims:

- [**Sustainability Audit**](sustainability-audit.md) — lifecycle data behind the "long-service-life" claim. Stage 6 directly addresses repairability; Stage 7 covers end-of-life paths and the acrylic-coating recyclability gap that bounds the philosophy's promises.
- [**Brand Positioning**](brand-positioning.md) — the "Living" brand adjective is the values bridge between the repair philosophy and the consumer-facing brand promise. §4 voice is the first-person framing this whole repair-doc set inherits.
- [**BOM Breakdown**](bom-breakdown-basic-6x6.md) — component-by-component cost and sourcing data backing the parts-availability claims.
- [**Firmware Architecture Scoping**](firmware-architecture-scoping.md) — where the OTA / closed-binary / reflash-service architecture is technically defined.
- [**Firmware Security Model**](firmware-security.md) — auth modes, OTA signing, telemetry. Adjacent to repair: a closed binary that is *also* signed and rolled out responsibly is a different thing than a closed binary that is opaque about its update process.
- [**Button Interface Design**](button-interface.md) and [**Button Design Rationale**](button-design-rationale.md) — the recessed factory-reset button is one of the most-cited self-recovery mechanisms in the design-decisions matrix; its rationale lives here.

---

## 6. Pending — the buyer-facing wrapper

[**User Repair Guide**](user-repair-guide.md) *(pending — Week 4 → Week 8 final)*

The customer-facing how-to companion. Currently blocked on the assembly guide, which lives upstream in the operations/manufacturing track. The guide will translate the failure-mode files into procedural instructions a non-technical buyer can follow. Specifically: which screwdriver, which strip part number, where to email for the STL, how to use the recessed button to recover from a wedged Wi-Fi state.

The philosophy + inventory + decisions + scorecard are the *technical* and *values* foundation; the User Repair Guide is the *experiential* surface that lands in the buyer's hand. They share the same backing data; they differ in voice and detail level.

---

## 7. Pending — packaging integration

[**Packaging QR code**](packaging-qr-code.md) *(blocked — Apr 28)*

Per [philosophy §6](right-to-repair-philosophy.md#6-what-this-asks-of-the-buyer), the buyer reaches the repair docs via a QR code laser-etched into the outer cardboard. Blocked on final packaging design (stock, dimensions, cutting process). Once packaging is locked, the QR code pass adds zero material — the laser cutter etches the link in the same pass that scores the box.

---

## How to read this set

For different readers, in order of expected utility:

| If you are… | Read in this order |
|---|---|
| **A buyer wondering "is this thing fixable?"** | Repair Index (this doc, §1–4 summary) → User Repair Guide (when ready) → Failure Mode Inventory README |
| **An advisor reviewing the HCDE portfolio** | Right-to-Repair Philosophy → Repair-Driven Design Decisions → Repairability Scorecard → Sustainability Audit Stage 6–7 |
| **A regulator checking compliance** | Right-to-Repair Philosophy §3 (commitments) + §5 (compliance) → Failure Mode Inventory → Repairability Scorecard §4 (parts independence) |
| **A future Layered Logic engineer** | Repair-Driven Design Decisions matrix → Firmware Architecture Scoping → Failure Mode Inventory per-component files |
| **A repair shop or hobbyist** | Right-to-Repair Philosophy §3 (commitments) → Failure Mode Inventory → Public schematic refs (when published per §3) |

---

## Maintenance

This index is intended to stay current with the repair-ecosystem doc set. If a new repair doc lands, link it from §3 (rationale) or §5 (supporting context) by topic. If the User Repair Guide ships, replace the §6 placeholder with a real link and trim the "pending" framing. The philosophy doc itself is treated as load-bearing — changes to it cascade through the matrix and scorecard, and a deliberate edit pass on all three should follow any revision to it.

---

## Related

- [Home Note (index.md)](../index.md) — repository top-level MOC; this sub-MOC is linked from there
- [Right-to-Repair Philosophy](right-to-repair-philosophy.md)
- [Failure Mode Inventory](../Failure_Modes/README.md)
- [Repair-Driven Design Decisions](repair-design-decisions.md)
- [Repairability Scorecard](repairability-scorecard.md)
- [Sustainability Audit](sustainability-audit.md)
- [Sprint Plan](../sprint_plan.md) — Week 4 deliverables, Week 8 follow-ups
