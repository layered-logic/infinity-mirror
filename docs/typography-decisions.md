---
title: Typography Decisions — Rationale
type: brand
phase: 2
week: 4
date: 2026-04-20
status: locked
tags: [brand, typography, decisions, rationale]
---

# Typography Decisions — Rationale

Companion doc to [Brand Positioning](brand-positioning.md). Section 5 of the worksheet carries the **decisions** (what faces, what roles, what weights). This file carries the **reasoning** for those decisions — why these faces, why this inversion, why not the alternatives.

Same pattern as [brand-research-notes.md](brand-research-notes.md): decisions live in the worksheet; rationale lives in a companion doc so the worksheet stays legible at a glance.

---

## The core move — italic is default, upright is emphasis

Most typographic systems reserve italic for emphasis inside running upright text. This system inverts that: **italic is the default posture; upright is the gear-shift**.

**Why this inversion is the signature move:**

- The five brand adjectives (§3) are *Ambient, Considered, Quiet, Crafted, Living*. Italic as default posture makes the reading surface feel **living** — the text leans into motion even when static.
- Most competitors in the lighting / smart-home / design-object space default to upright neo-grotesque (Helvetica, Inter, GT America, IBM Plex). Italic-default is immediately differentiated without resorting to display-quirk faces.
- It creates a built-in *gear-shift* mechanism. Any time the text goes upright — "Specs," "Repair Guide," "What's in the box" — the reader feels the posture change and knows they've entered the *functional voice*. This is the engagement hook the brand uses to move between ambient narrative and technical precision.
- It reinforces the brand-positioning pairing: "soft thing pinned by hard thing." Italic is the soft layer; upright Berkeley Mono is the hard layer.

**What this costs:**

- Readability research cautions against long-form italic body copy. Mitigation: keep italic runs short (editorial-length paragraphs, not article-length), use ExtraLight which reads as a *voice* rather than a weight wall, and allow upright for dense product-UI screens where italic fatigue would set in (per §5 table row "H3 / small heads").
- Italic-default is a strong opinion. It will read as *affected* to some readers. This is accepted — the brand is not trying to please the Spec-Head audience segment (§2); the Interior Curator will read italic as a voice choice, not an error.

---

## Why Neue Haas Grotesk (not Inter, not GT America, not Söhne)

**Neue Haas Grotesk** (Commercial Type, Christian Schwartz) is a digital restoration of the 1958 original that Helvetica was derived from. It's the *pre-Helvetica Helvetica* — the drawing before it got neutralized for phototypesetting.

**What it brings that Helvetica/Inter don't:**

- Slightly narrower, slightly more structured letterforms — reads as *composed* where Helvetica reads as *default*
- A genuinely designed ExtraLight that holds at display size without feeling fragile
- A proper italic companion (not an oblique). The italic has real construction — legible as a voice, not a shear
- Commercial Type's licensing is serious but clean — one-time purchase, no subscription, no usage metering

**Rejected alternatives:**

- **Inter** — free, excellent, but ubiquitous. Using it signals "tech startup using the default good-taste face." Cannot be the voice of a brand whose positioning is *considered*.
- **GT America** — a strong option, more geometric than NHG. Considered but rejected because GT America's italic is weaker than NHG's, and italic is the default posture here.
- **Söhne** (Klim) — beautiful, but leans editorial-swiss-magazine. Wrong temperature — too warm, too Kinfolk. NHG's colder precision matches the violet-on-near-black palette better.
- **ABC Diatype** — the display-face sibling (ABC Diatype Mono) was rejected below; the sans version wasn't strong enough on its own to pull Layered Logic away from NHG.

**Weights and postures used:**
| Role | Face | Weight | Posture |
|---|---|---|---|
| Body, subtitles, most UI | NHG Text | ExtraLight | **Italic (default)** |
| Display / headlines | NHG Display | ExtraLight or Light | **Italic (default)** |
| Marked emphasis *inside* italic | NHG Text | ExtraLight or Medium | Upright (the emphasis cue) |
| "Straight alignment" gear-shift sections | NHG Text | Medium or Bold | Upright, left-aligned, blocky |
| H3 / small heads in dense product UI | NHG Text | Medium | Upright permitted for legibility |

---

## Why Berkeley Mono (not JetBrains Mono, not IBM Plex Mono, not ABC Diatype Mono)

**Berkeley Mono** (Berkeley Graphics, Neil Panchal) is a contemporary commercial mono with characterful drawing — it has a point of view without becoming decorative.

**What it brings:**

- A distinctive lowercase `a` (double-story), lowercase `g` (double-story), and a genuinely designed `@` that reads as an object, not a glyph
- Terminals are *snipped* at an angle — this gives the mono a slight kinetic energy that sits next to the italic NHG without clashing
- Proportionally tuned — not a stretch of an existing sans, but drawn as a mono from the start
- Full mathematical symbol set, ligatures for code contexts

**Why Berkeley became the anchor:**

- It does the job the references (IBM Quantum, Intrinsic/Felix Faire) ask of their mono — becomes the "anchor of stability" against the soft gradients — without using IBM's Plex Mono (which would read as borrowed)
- Berkeley's character level is *just* high enough to feel like a brand decision without tipping into *novelty typeface*

**Rejected alternatives:**

- **JetBrains Mono** — excellent, free, ubiquitous in dev tools. Rejected for the same reason Inter was: too default. The mono is doing too much brand work here to be invisible.
- **IBM Plex Mono** — the obvious answer given how strongly the brand references IBM Quantum's visual work. Rejected specifically *because* it's the obvious answer — using Plex Mono would read as pastiche of a reference, not adoption of a grammar.
- **ABC Diatype Mono** — the closest runner-up. Rejected because its `R` has a small decorative squiggle on the tail that draws the eye every time the letter appears. For a brand that prizes *restraint*, that squiggle is one mannerism too many. It's the kind of detail that looks clever in the type specimen and gets tiring in product copy by month three.
- **GT Cinetype Mono / Pangram Pangram monos** — not evaluated in depth; the three faces above covered the decision space.

---

## No serif. No script. No display-novelty.

**No serif** — a serif would soften the technical-editorial axis the italic-default move is establishing. The "warm humanist" register belongs to other brands (Intrinsic itself uses a humanist sans; that's theirs). Layered Logic stays in the grotesque family all the way through.

**No script or hand-drawn face** — the brand is *crafted* (§3) but the crafting is in manufacturing and product, not in the typography. A script face would put the craft at the wrong layer.

**No display-novelty face** — no variable-width experiments, no reverse-contrast, no geometric neo-retro. The grammar is disciplined grotesque + mono. Anything else reads as trend.

---

## Licensing — acquire before any public-facing use

- **Neue Haas Grotesk** — Commercial Type. Web + desktop license purchased per-domain/per-seat. Current cost estimate: ~$200–400 for a full family license covering the weights above (ExtraLight, Medium, Bold, in Text + Display, with italics). Required before launching any public site or sending a licensed font to printers.
- **Berkeley Mono** — Berkeley Graphics. One-time purchase, ~$75 at the personal tier, ~$200+ commercial. Required before shipping any product with Berkeley Mono in firmware/UI or on public marketing surfaces.

**Where the license documentation lives:** `assets/brand/type/LICENSE.md` — a note-of-record (not the font files themselves) describing which licenses have been purchased, on which dates, for which seats/domains. The actual `.otf`/`.ttf` files are **not committed** to the repo (commercial license terms prohibit redistribution). Fonts live locally in the OS font folder and are pushed to production hosts via the licensed CDN or self-hosted `.woff2` per the license.

**Before any public-facing artifact is published:**
1. License purchased for both faces
2. Receipt / license PDF saved in `Business/brand/type-licenses/` (Business folder, not committed per [feedback](../.claude/memory/feedback_business_folder.md))
3. `assets/brand/type/LICENSE.md` updated with license holder, purchase date, scope (web/desktop), seat count

---

## Open questions / future work

- **Website font-loading strategy** — `.woff2` served from self-host vs. Commercial Type's CDN. Decide when the site is scaffolded.
- **Firmware/app UI face** — the mobile app and web app will need a UI type ramp. Neue Haas Grotesk at small sizes in an app UI may be too expensive per-install; evaluate whether a system fallback (SF Pro / Roboto italic) is acceptable for in-app text or whether the italic-default posture needs a custom-bundled lightweight face.
- **Print-ready variant** — any packaging, print collateral, or coffee-shop signage will need a print-safe font stack. NHG prints well; verify with first test proofs.

---

## Related

- [Brand Positioning §5 Typography](brand-positioning.md#5-visual-language--direction-before-execution) — the decisions this doc explains
- [Brand Research Notes](brand-research-notes.md) — the reference set that informed this typography direction (especially IBM Quantum's Plex Mono rigor and Intrinsic's two-type restraint)
- [Logo Spec](../assets/brand/logo/logo-spec.md) — the 86° logo tilt shares the italic-default posture specified here
- [assets/brand/type/](../assets/brand/type/) — where specimens, license notes, and `.woff2` files will land
- [Index / MOC](../index.md)
