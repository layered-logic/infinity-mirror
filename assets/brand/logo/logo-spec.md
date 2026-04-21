---
title: Layered Logic Logo — Spec
type: brand
phase: 2
week: 4
date: 2026-04-21
status: wip
tags: [brand, logo, spec, generative]
---

# Layered Logic Logo — Spec

Specification for the Layered Logic logo. The mark is a **generative mouse-reactive sketch** built in P5.js with a **canonical static rest pose** that serves as the printable/etched/favicon reference frame.

Canonical source: [layered-logic/layered-logic-logo](https://github.com/layered-logic/layered-logic-logo) (private repo).

For code-level annotations, see [logo-code-notes.md](logo-code-notes.md). For the captured rest pose, see [logo-primary-dark.svg](logo-primary-dark.svg).

---

## Concept

**Two nested L-shapes** with histogram bars that radiate from each L's path in response to mouse proximity. At rest, the mark is two parallel L polylines. In motion, it behaves like a level meter reading the user's presence — a living diagram that resolves back to the static mark when attention moves away.

The two Ls literalize the "layered" meaning:
- Each L is a **layer**
- The nesting is the **logic**
- The histogram bars are the mark *responding* — the "Living" adjective from [brand-positioning §3](../../../docs/brand-positioning.md)

---

## What "Layered Logic" means (from the mark)

The name references:
- **Layers of 3D printing** (manufacturing process)
- **Layers of the mirror** (frame → LED → acrylic → mirror coating → front panel)
- **Layers of the design process** (research → sketch → prototype → iterate → ship)
- **Layers of data/analysis** (raw → cleaned → modeled → insight) — relevant to future consulting work

A nested-L mark puts the layering *in the mark itself*.

---

## Mark anatomy

| Element | Role | Parameters (current) |
|---|---|---|
| **Outer L** | Primary form. Bars radiate outward (left from vertical, down from horizontal) when triggered | Vertical arm 320px, horizontal arm 220px, stroke 7px |
| **Inner L** | Secondary form, ~20px inset from outer. Bars radiate inward (right from vertical, up from horizontal) — they meet and interleave with the outer L's bars | Vertical arm 288px (90% of outer), horizontal arm 180px, stroke 6px, gap 20px |
| **Angle** | Vertical arms are slightly tilted (4° off true vertical). Matches the italic-default posture from [typography](../../../docs/typography-decisions.md) — "the mark is in the same posture as the type" | `angleDeg = 86°` |

**System-level alignment:** the 86° tilt is deliberate — the italic-default type system leans, and the logo leans with it. Typography and logo share one posture.

---

## Rest pose (canonical static form)

The "rest pose" is the frame the mark returns to when no interaction is happening — mouse far from canvas, or no mouse at all (favicon, etched product, printed collateral).

**Definition:** at `d > histRadius` (150px from either L path), the sketch draws only the two L polylines. No bars. This is the canonical frozen state used anywhere the mark must be static.

The deterministic rest pose derived straight from the P5 parameters lives at [logo-primary-dark.svg](logo-primary-dark.svg). That SVG is the source-of-truth for any reproduction that must not run P5 (packaging, print, laser etch, vinyl cut, production PCB silkscreen, favicon).

---

## Current state (Apr 21, 2026)

The mark is **active WIP, not finalized**. Logo work is scoped out-of-sprint for Spring 2026 per [brand-positioning §7](../../../docs/brand-positioning.md) — this spec captures the current state and constraints; resolution of open items happens when Bill reopens the track.

**What's working:**
- `angleDeg = 86°` puts the mark in the same posture as the italic-default type system — system-level alignment
- Histogram-on-a-path with `barSteps = 5` quantization lands in the Field.io / IBM Quantum "measured sublime" grammar (not generic waveform)
- Mouse reactivity earns the *Living* adjective — the mark responds to presence
- Two nested Ls + bars radiating = "layered" literalized three times over (shape nesting, bar stacking, layer-of-layer)

**Resolved items:**
1. **Color** *(resolved 2026-04-21, revised same day)* — initial resolution pulled the mark into a magenta-violet `#BF00FF` / `#A020D0` pair; rendered artifacts read too pink, so the palette was shifted to indigo the same afternoon. **Final values:** outer L = `#4A25FF` (Indigo Reference), inner L = `#3214FF` (Indigo Signal). Matches the live P5 sketch's existing direction, kills the magenta cast, keeps the engineering-coded electric feel. Dark-mode colors held as-is on white for the light variant; a black-stroke monochrome variant covers one-color contexts. **P5 source** currently renders `#420AFF` / `#3214FF` — the outer bumps from `#420AFF` to `#4A25FF` when the P5 source is synced; inner is already correct.
2. **Canonical rest pose definition** *(resolved 2026-04-21)* — captured in [logo-primary-dark.svg](logo-primary-dark.svg). If the SVG ever drifts from the P5 code, regenerate it from the code params — geometry is driven from the sketch; color is now driven from the brand palette.

**Still parked:**
3. **Favicon variant** — 16×16 won't hold two Ls + gap. Needs a simplified single-L or fused ligature.
4. **`ampScale = 0.5` on horizontal arm** — bars on the horizontal arm are half-height. Deliberate asymmetry (reads as "settling down" toward the ground) or a balance hack? Document the intent either way.
5. **Indigo bloom layer behind the Ls for web hero** — activates signature devices #1 and #4 from [brand-research-notes §Five Visual Devices](../../../docs/brand-research-notes.md) for free. Zero code change on the mark itself; a CSS layer beneath the canvas.

---

## System constraints (any candidate mark must satisfy)

- **Favicon at 16×16** — must reduce to a glyph that holds at extreme small scale
- **Etched into product** — monochrome, high contrast, single-stroke-weight-friendly. The current mark works because it's line-only.
- **Over gradient bloom** — must work mask-style or with a solid variant over indigo-bloom backgrounds
- **Static export path** — the mark must have a canonical frozen state for print/etch/laser/small-format product application, even though the live mark is generative. *(Resolved for current state: logo-primary-dark.svg.)*
- **Wordmark + mark pair** — needs both a `layered logic` wordmark (set in Neue Haas Grotesk, probably italic per §5 system) and a standalone mark version

---

## Usage rules (v1, provisional)

*These are starting points. Refine when the track reopens.*

**Do:**
- Use the SVG rest pose wherever the mark is static (packaging, print, icons, product)
- Use the live P5 sketch for web hero and interactive surfaces
- Preserve the 86° posture (don't re-tilt to true vertical)
- Preserve the nested-L relationship (don't un-nest, don't collapse to a single L — except for favicon)

**Don't:**
- Don't recolor the mark without consulting the brand palette — any color decision is a §7 parked item that needs resolution first
- Don't put the mark over arbitrary photography. It's designed for near-black or indigo-bloom grounds
- Don't animate the mark with anything other than the P5 sketch. Secondary animations (fade, rotate, scale) dilute the histogram-bar move

---

## Product-line variations (future, parked)

Bill's note (Apr 21): "we can make variations of this per product line, but let's worry about that later."

**Deferred to post-Week-4 brand work.** The hook point for variation is the P5 sketch itself — the generative system can take different parameters per product line without losing brand coherence. Candidate axes for variation:
- Color (per product-line anchor — if a future line uses a different accent, the mark can carry it)
- Bar quantization level (`barSteps = 5` for parent; finer/coarser per product?)
- Number of Ls (three for three-tier products?)
- Bar direction grammar (outward/inward swap?)

Any variation must still pass the system constraints above (favicon, etch, over-gradient, static export).

---

## Files in this folder

| File | Purpose |
|---|---|
| [README.md](README.md) | Folder index + PNG capture instructions |
| [logo-spec.md](logo-spec.md) | This file — branding spec and constraints |
| [logo-code-notes.md](logo-code-notes.md) | Annotated companion to the P5 source |
| [logo-primary-dark.svg](logo-primary-dark.svg) | Canonical rest pose, dark ground, Indigo Reference + Signal |
| [logo-primary-light.svg](logo-primary-light.svg) | Canonical rest pose, white ground, Indigo Reference + Signal |
| [logo-primary-mono.svg](logo-primary-mono.svg) | Single-color variant (black strokes, transparent bg) for etch / one-color print / over photography |

**To add:**
- [ ] `logo-default.png` — raster screenshot from the running P5 sketch (capture instructions in README)
- [ ] `favicon.svg` + `favicon-16.png` — simplified mark for extreme small scale (blocked on parked item #3)
- [ ] `wordmark.svg` — "layered logic" set in Neue Haas Grotesk
- [ ] Sync P5 source to the locked indigo anchors — outer L needs `#420AFF` → `#4A25FF` (minor bump); inner L is already correct at `#3214FF`

---

## Related

- [Brand Positioning §7 Logo Direction](../../../docs/brand-positioning.md) — the brand worksheet section this spec expands
- [Typography Decisions](../../../docs/typography-decisions.md) — shared italic-default posture rationale
- [Brand Research Notes — Five Visual Devices](../../../docs/brand-research-notes.md) — indigo bloom layer (parked item #5) is device #1/#4 applied to the mark
- [logo-code-notes.md](logo-code-notes.md) — technical companion
- Canonical source: [layered-logic/layered-logic-logo](https://github.com/layered-logic/layered-logic-logo)
