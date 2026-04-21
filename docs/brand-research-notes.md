---
title: Brand Reference Analysis — Research Notes
type: brand
phase: 2
week: 4
date: 2026-04-20
status: draft
tags: [brand, research, moodboard, references]
---

# Brand Reference Analysis — Research Notes

Companion doc to [Brand Positioning](brand-positioning.md). This file holds the full analysis of Bill's five moodboard references. The worksheet carries the *decisions*; this doc carries the *reasoning* for those decisions.

> **Method caveat:** WebFetch was denied in the environment where this analysis was run. Descriptions below are reconstructed from the research agent's prior familiarity with these well-documented projects. Hex values are approximations — verify with an eyedropper against live case study imagery before finalizing any palette.

---

## Reference 1 — Paris Electronic Week 2022 (Diplomatie Studios)

[the-brandidentity.com case study](https://the-brandidentity.com/project/diplomatie-studios-identity-for-paris-electronic-week-2022-is-a-kaleidoscopic-collision-of-colour)

- **Palette:** High-saturation CMYK collisions — electric magenta `~#FF2E88`, acid yellow `~#F2FF00`, cyan `~#00E0FF`, deep violet-black ground. Colors overprint and bleed into secondary hues.
- **Type:** Single condensed grotesque at display scale, mixed-case, tight kerning. Technical mono captions in fine print.
- **Layout:** Asymmetric, poster-first. Big title block anchored to a corner; image plate dominates. Loose grid, flush-to-edge alignment.
- **Motion / pattern:** Kaleidoscopic mirrored imagery, lens-flare photography, gradient meshes that read as "color leaking."
- **Signature device:** Kaleidoscopic symmetry of electronic/light photography set against flat type blocks — soft luminous chaos pinned by rigid typographic frames.
- **Pull:** The "light bleed pinned under a hard type lockup" move.
- **Avoid:** Loud rainbow palette — Bill's indigo anchor gets diluted if we chase CMYK chaos.

## Reference 2 — SPRING Performing Arts Festival (LAVA)

[lava.nl case study](https://lava.nl/progress/spring-performing-arts-festivals-new-identity/)

- **Palette:** Pastel-to-saturated gradient ramps — peach/coral into lavender into teal. Off-white paper ground; black type.
- **Type:** Neo-grotesque in a single weight, tight leading, italic for titling; captions same family, small. Type sits directly on imagery without a plate.
- **Layout:** Editorial grid, generous margins, text-forward. Imagery cropped to soft organic shapes floating in the page.
- **Motion / pattern:** Core move is a soft, photographic gradient blob — blurred color forms resembling long-exposure light or fabric. Slow morphing motion.
- **Signature device:** A blurred, amorphous color-field shape (the "spring bloom") used as a recurring container — always soft-edged, always paired with crisp typographic hierarchy.
- **Pull:** The soft color-field shape as a recurring container.
- **Avoid:** The pastel warmth and book-like whitespace — wrong temperature for a dark-mode product brand.

## Reference 3 — Intrinsic (Pentagram identity)

[pentagram.com case study](https://www.pentagram.com/work/intrinsic)

- **Palette:** Near-black ground `~#0A0A0A`, warm off-white `~#F4F0E8`, a single restrained accent (coral or blue) in data viz.
- **Type:** Custom wordmark with distinctive lowercase "i"; body in humanist sans. One display, one text — tightly controlled.
- **Layout:** Disciplined 12-column, generous negative space, left-aligned. Mono captions sit at edges as a technical frame.
- **Motion / pattern:** Restrained in identity; motion lives in the generative work by Felix Faire.
- **Signature device:** The pairing of warm neutral type against deep technical imagery — humanist voice on engineering substrate.
- **Pull:** The disciplined two-type system + near-black + one accent restraint.
- **Avoid:** The warm-neutral palette — Layered Logic's system is indigo, not parchment.

## Reference 4 — Intrinsic (Felix Faire generative visuals)

[felixfaire.com case study](https://felixfaire.com/work/intrinsic)

- **Palette:** Deep blacks to indigo/violet, with neon accent strokes in cyan, magenta, warm amber. Everything photographed-feeling.
- **Type:** When type appears it's mono/technical, small, as callouts or coordinates — never decorative.
- **Layout:** Full-bleed 3D scenes; UI-style overlays with thin rules and mono labels.
- **Motion / pattern:** **The core "gradients + hard outlines" reference.** Volumetric soft gradients (fog, light fields) sit *behind* razor-thin wireframe geometry. Point clouds, isolines, lidar-style dot meshes animate slowly over the gradient. Hard 1px lines against infinitely soft color.
- **Signature device:** Wireframe-over-volumetric-light. A precise geometric skeleton floating in a cloud of soft color.
- **Pull:** The *visual logic* — wireframe over gradient, two layers never blended.
- **Avoid:** The heavy 3D rendering workload — adopt the grammar, don't try to out-render a CG studio.

## Reference 5 — IBM Quantum (Field.io)

[field.io case study](https://field.io/work/ibm-quantum-brand-visuals)

- **Palette:** IBM Plex cool neutrals + signature IBM blue `~#0F62FE`, extended into deep teal, violet, electric cyan. Dark ground dominant.
- **Type:** IBM Plex Sans + Plex Mono throughout. Mono carries all technical annotation.
- **Layout:** Scientific-instrument framing — tick marks, axis labels, measurement ticks at the edges. Centered hero compositions with annotated peripheries.
- **Motion / pattern:** Flowing ribbon-like gradients representing qubit states — smooth, volumetric, photographic, but *always* diagrammed on top of with crisp mono labels and thin rules.
- **Signature device:** The "annotated sublime" — a beautiful fluid gradient form treated like a specimen, with mono captions and tick marks pinning it down.
- **Pull:** The annotation layer — mono labels, tick marks, thin rules that turn a soft gradient into a "measured" object.
- **Avoid:** IBM blue — Layered Logic's anchor is indigo (cooler and more saturated than IBM's signature blue; the distinction matters).

---

## Synthesis — Shared Visual Grammar

**The grammar:** soft volumetric color (gradient, fog, bloom, light field) treated as the *subject*, pinned and measured by a hard, precise typographic/geometric overlay. The soft thing is ambient and emotional; the hard thing is technical and first-person. **Tension between the two is the brand.**

**"Gradients + hard outlines" concretely means:**
1. A blurred, luminous color field — not a CSS linear-gradient, but a photographed or volumetrically-rendered bloom — sits in the back layer
2. A crisp front layer of 1px rules, mono coordinates, wireframe geometry, or thin italic display type
3. The two layers never blend — the hard layer is always at full opacity and sharp; the soft layer is always out-of-focus

**A credible Layered Logic identity** would be:
- Dark-mode ground `~#0B0A0F` (not pure black)
- A single indigo `#4A25FF` gradient bloom as the recurring "hero" element (varying in scale, never in hue)
- Neue Haas Grotesk ExtraLight Italic for first-person voice at display size
- A mono (JetBrains Mono / Berkeley Mono / IBM Plex Mono) for every technical moment — serial numbers, coordinates, BOM IDs, timestamps
- A 1px indigo or white rule system that frames the bloom like a measurement

---

## Five Concrete Visual Devices to Adopt

1. **Indigo bloom behind ExtraLight Italic display type.** A single soft `#4A25FF` radial gradient, ~40% of canvas, sits behind a left-aligned italic "I" statement. Type is at full opacity; bloom is blurred 80–120px. No other color.
2. **Mono tick-mark frame.** Thin 1px white rules with mono labels (e.g., `LL-001 / 6x6 / REV-A / 2026`) at the four edges of every key image — IBM Quantum's annotation move, applied to product photography.
3. **Wireframe over bloom for product reveals.** A 1px indigo wireframe of the mirror (exploded axonometric) floats over a volumetric indigo bloom — Felix Faire's move, executed in 2D vector so it stays cheap to produce.
4. **The soft-shape container.** LAVA's color-field blob, recolored to an indigo-to-black gradient, used as a recurring crop mask for photography and a container for short-form copy. Always soft-edged; type that sits on it is always sharp.
5. **Overprint moments, sparingly.** One borrowed Diplomatie move: let the indigo bloom pass *through* a mono caption so the caption inverts where it overlaps the bloom — a single permitted "collision" per composition, signaling the product is light-based.

## Anti-Moves

- No pastels
- No warm neutrals (except as a single deliberate light-mode base)
- No multi-hue rainbows
- No heavy 3D renders
- No sans beyond Neue Haas + one mono
- No upright non-italic display voice *(open decision — italic-only display is a strong move but needs confirmation; see worksheet §5 Typography)*

---

## Related

- [Index / MOC](../index.md)
- [Brand Positioning](brand-positioning.md) — decisions this research informs
- [Typography Decisions](typography-decisions.md) — IBM Quantum's Plex Mono and Intrinsic's two-type restraint feed directly into the NHG + Berkeley Mono choice
- [Logo Spec](../assets/brand/logo/logo-spec.md) — Intrinsic / IBM Quantum / Field.io generative systems are the precedent for the P5-driven logo
- [Moodboards index](../assets/moodboards/README.md) — links + summaries for the five references analyzed here
