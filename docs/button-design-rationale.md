---
title: Button Design Rationale
type: hcde
phase: 2
week: 5
status: draft
tags: [hcde, ux, button, interface, portfolio, design-rationale]
---

# Button Design Rationale

**Author:** William White
**Date:** April 28, 2026
**Status:** Draft — pre-user-research. Will be revised once Week 5 interviews land.

This is the HCDE portfolio companion to [button-interface.md](button-interface.md). The spec doc *defines* the interface; this doc *explains* why it ended up that way — what alternatives were considered, what tensions had to be resolved, and what the choices commit me to.

---

## 1. The question I had to answer

The Layered Logic Infinity Mirror is two products at once:

- A piece of **lighting art** that lives on a wall and runs unattended for years
- A **configurable smart device** with patterns, colors, brightness, and (eventually) automations and custom designs

These two products want different control surfaces. Art wants to disappear — touch it once, then forget it. Smart devices want a screen, an app, a settings panel. The interface I ship has to serve both modes without making either feel compromised.

The question, narrowed: *what does a user touch on the device itself, and what lives in the app?*

---

## 2. Who the interface is designed for

From [brand-positioning §3](brand-positioning.md), the audience priority is:

1. **Interior Curator** — buys for fit and feel. Doesn't want to think about the device after install.
2. **Vibe-Seeker** — wants to set a mood, change with the season, occasionally tweak.
3. **Spec-Head** — wants every parameter exposed and addressable.

I weight design decisions by the priority order. Where a tradeoff favors Curator at the cost of Spec-Head ergonomics, Curator wins. Spec-Head can use the app — they will anyway. Curator may never open it.

This single ordering does most of the work in this doc.

---

## 3. The core tension

> **The mirror is a light first, a smart device second.**

The control interface has to make this true at the hardware level. If a buyer plugs in the mirror and the first thing it does is demand a Wi-Fi password, the brand promise is already broken.

Concretely, that means:

- The device must be **fully usable with no app, no account, and no network**. Every shipped pattern, every color, every brightness level — reachable with bare-hand interaction.
- Daily controls must be **readable without documentation**. If a guest visits and wants to dim the mirror, they should figure it out in under 30 seconds.
- Configuration that *changes the device's identity* (pairing, factory reset) must require **deliberate, non-accidental effort**. Someone bumping into the wall doesn't reset the firmware.

These three constraints set the shape of everything that follows.

---

## 4. The options I considered

### 4.1 App-only (no physical buttons beyond power)

The minimalist path. The mirror has a power cord and that's it; everything else lives in a phone app.

**Rejected.** Three problems:

1. **Setup friction** — to use the mirror at all, you have to install the app, create an account or pair, and connect the device to your network. That violates the "light first" principle. A user who never plugs in their phone can never use the product.
2. **Guest access** — a houseguest, roommate, or partner who doesn't have the app paired has no way to interact with the mirror. The device becomes single-user-locked, which is a strange thing for a piece of wall art to be.
3. **Long-tail control** — five years from now the app may not run on whatever phones exist. The device should outlive its app.

### 4.2 Single button (V0 prototype's model)

The V0 prototype used a single exposed button with a four-gesture grammar (single / double / triple / hold). Inherited from the STM8 Basic SKU and proven in the field.

**Kept for daily use, supplemented for rare ops.** The single-button grammar handles the daily control surface beautifully — it's tactile, requires no documentation, and the gesture mapping (single = next color, double = next pattern, triple = brightness, hold = off) is learnable in a few minutes.

The reason it's not enough on its own: the smart-device side of the product needs *occasional* destructive operations — pairing, factory reset, recovery. Putting those on the same button as daily controls means either:

- Adding a fifth or sixth gesture (long-long-hold, quadruple-press) that no one will discover, OR
- Having daily gestures accidentally enter destructive modes

Neither is acceptable. So the single-button grammar stays for daily, and a second control surface owns the rare stuff.

### 4.3 Capacitive touch (TTP223 evaluation)

The BOM analysis (Apr 9) flagged a TTP223 capacitive touch sensor as a $0.10 add-on that senses through 1–2mm PETG. Initially attractive: invisible button, no mechanical wear, fits the brand language of "tech disappears."

**Deferred, not rejected.** Two concerns held it back:

1. **No tactile feedback.** The V0 button gives a satisfying click that doubles as an "input received" signal. Capacitive needs a separate confirmation channel (LED feedback) to feel responsive — extra firmware work, and on some patterns the LED feedback could be ambiguous.
2. **Holds are awkward on capacitive.** Holding your finger against a smooth panel for 600ms+ is fine; doing it for 10 seconds (factory reset) is uncomfortable. Hold-based gestures are ergonomically tied to mechanical buttons.

I want to revisit this in v2 once the interaction grammar has been validated by real users. For v1, mechanical wins because it's known.

### 4.4 Knob / encoder

Considered briefly — a rotary encoder for brightness with a press-to-toggle would feel premium and physical.

**Rejected on cost and assembly time.** A through-hole encoder adds $1–2 to BOM and a soldering operation; the click-only switch is a single SMT placement. For a $65 retail product, the math doesn't work.

### 4.5 Two buttons (chosen)

One **exposed** primary button for daily UI; one **recessed pinhole** button for rare/destructive operations.

The recessed button is the load-bearing decision in this design. It comes from a specific precedent.

---

## 5. The Eero precedent

The Eero router (and AirPort, and most modern network gear) uses a recessed pinhole reset button — accessible only with a paperclip. It's a convention strong enough to be universally recognized: "this is the dangerous button."

That convention does three things at once:

1. **It gates destructive actions behind a physical barrier.** You can't bump into a paperclip-only button. You can't sit on it.
2. **It signals consequence to the user before the press.** Nobody finds a pinhole and casually pokes it — finding the paperclip is itself a deliberate decision.
3. **It separates daily UI from configuration UI** so the daily UI can stay clean. The exposed button doesn't have to grow a fifth gesture for "factory reset"; that role is offloaded entirely.

I lean on this precedent intentionally. Users — even non-technical ones — already know what a pinhole button means, because they've reset their router or their phone's SIM tray or a pair of headphones. Inheriting that mental model means I don't have to teach it.

---

## 6. Why I preserved V0's gesture grammar

The primary button uses the same gesture mapping as the V0 prototype's single-button STM8 firmware:

- **Single press** — toggle on (when off) or advance base color (when on)
- **Double press** — change pattern
- **Triple press** — advance brightness
- **Hold** — turn off

Three reasons:

1. **It works.** V0 has been in the field on the prototype unit for months. The grammar is intuitive after a few minutes; once learned, it's muscle memory.
2. **Continuity across SKUs.** The Basic STM8 product (already in production) and the Pro/Pro+ ESP32 product share an interaction grammar. A user who upgrades from Basic to Pro doesn't have to relearn the device.
3. **Cost of redesign is unjustified.** I considered alternatives — adding a long-press for pattern, separating "next color" from "previous color" with double-press direction, etc. None of them were obviously better. Changing for the sake of changing isn't a design decision, it's churn.

The one place I pushed back on continuity: the parked clarification in [button-interface §6](button-interface.md#parked-clarification) about self-palette patterns swallowing single-press feedback. This is the kind of edge case user testing is meant to surface. Documented but not solved — Week 5 interviews are the test.

---

## 7. What I gave up

Honest accounting of the trade-offs this design accepts:

- **Spec-Head users get a sparse hardware UI.** Want to reorder patterns? Set a brightness ceiling? Bind a custom gesture? Use the app. Hardware is for daily essentials only.
- **No multi-button combos.** I'm not adding "press primary while holding recessed = enter dev mode." Combos are unguessable, undocumented, and create accidental triggers. Whatever a power user wants, the app handles it.
- **Discoverability is bounded by the grammar.** A user who never tries triple-press won't find brightness control on the device. The unboxing card has to teach the gestures explicitly. (See: packaging QR code concept — laser-etched into cardboard, gestures listed alongside.)
- **Five-year app survival is a real risk.** If I rely on the app for any non-trivial functionality and the app stops working, that functionality is gone. Mitigation: keep the device's standalone capability honest, don't put basic features behind app-only flows. (See: [right-to-repair §3](right-to-repair-philosophy.md#3-what-i-commit-to) — every device-level feature must be reachable without an app.)

---

## 8. What I want to learn from user research

This rationale is principle-driven. The Week 5 interviews ([user-interview-outline.md](user-interview-outline.md)) are the first chance to pressure-test the assumptions.

Specifically, I want to know:

| Hypothesis | Question that tests it |
|---|---|
| Curators don't open apps for wall art | "When was the last time you opened an app for a piece of decor?" |
| Single-press color advance is intuitive | Watch them try it. Don't pre-explain. |
| Hold-to-off is intuitive | Same — watch the first attempt. |
| Recessed button is recognized as "the reset" | Show photos of the back; ask what they'd assume the pinhole does. |
| Brightness needs more than 4 steps | Ask if 4 brightness levels feels like enough. |
| App-vs-button preference correlates with audience type | Compare answers across interview subjects. |

If the interviews refute any of the strong claims here, this doc gets revised. The point of writing it before user research is to make the hypothesis legible — so the research can falsify it cleanly, rather than just confirming what I already wanted to hear.

---

## 9. Open questions and parked items

Pulled forward from [button-interface.md](button-interface.md) and the firmware architecture work:

- **Recessed button discoverability.** A first-time user shouldn't *need* the recessed button — out-of-box the device runs without it. But they need to find it when something goes wrong. The unboxing flow and Repair Guide both have to surface it.
- **Self-palette pattern feedback.** Single-press on Rainbow advances base color in firmware state but produces no visual change. Possible refinement: ~200ms acknowledgement flash. Defer until user testing.
- **Capacitive touch revisit for v2.** Once the gesture grammar is validated and the visible-button interaction model is locked, capacitive becomes interesting again as a "tech disappears" upgrade.
- **Matter commissioning gesture.** If the Pro+ SKU ships, the recessed button needs a third gesture (tentative: 6s hold) for Matter commissioning. Doesn't affect Pro or Basic.

---

## 10. The one-sentence summary

> *Two buttons because the mirror is a light first and a smart device second — and the difference between those two roles deserves a physical seam.*

---

## Related

- [Button Interface Design](button-interface.md) — the spec this rationale explains
- [Firmware Architecture Scoping §4](firmware-architecture-scoping.md#4-design-principles-locked-apr-20) — the design principles this inherits
- [Brand Positioning §3](brand-positioning.md) — audience priority that drives the trade-offs
- [Right-to-Repair Philosophy §3](right-to-repair-philosophy.md#3-what-i-commit-to) — the standalone-capability commitment
- [User Interview Outlines](user-interview-outline.md) — Week 5 research that will pressure-test this
- [Pattern Dictionary](pattern-dictionary.md) — the visual contract the gestures advance through
