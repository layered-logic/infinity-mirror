---
title: Brand Positioning Worksheet
type: brand
phase: 2
week: 4
date: 2026-04-20
status: draft
tags: [brand, positioning, naming, voice]
---

# Brand Positioning Worksheet — Infinity Mirror Product Line

**Prepared by:** William White, Founder
**Date started:** April 20, 2026
**Status:** Working draft — Week 4 deliverable

---

## 0. Brand Architecture — Three-Tier Hierarchy

**Decision (Apr 20):** Layered Logic is a multi-line parent brand. Products sit two tiers below it.

```
Tier 1:   Layered Logic                    ← Parent company (WA LLC, filed Mar 23 2026)
          ├── Design consulting practice   ← One business arm
          └── [Product Mini-Brand]         ← The product arm ← NAMING TARGET TODAY
              ├── Infinity Mirror line     ← One product line
              ├── LED Nixie Clock line     ← Another product line (future)
              └── [Future display/LED products]
```

**What's locked**
| | |
|---|---|
| **Parent brand** | Layered Logic LLC (consulting + products) |
| **Product category (Tier 2 scope)** | Novel LED / light / display objects — infinity mirrors, Nixie clocks, future displays |
| **Price positioning (infinity mirror line)** | $200+ retail (Market Analysis — above Etsy $50–120 saturation) |

**What this document now names**
- **Primary goal:** Tier 2 name — the product mini-brand that covers *all* LED/display products, not just mirrors.
- **Not in scope today:** Individual product SKU names (e.g. specific mirror models). Those can be descriptive ("6×6 Classic," "Custom Series") under the mini-brand.

**Why this structure:** Layered Logic as parent keeps consulting and products under one legal and reputational roof. The Tier 2 mini-brand lets the product arm build its own design voice without every marketing decision being diluted by "also we do consulting." Each product line inherits the mini-brand without needing its own standalone identity.

**Implication for naming:** Mirror-specific candidates (Depthwise, Inwards, Threshold) are now **too narrow** — eliminated. The name needs to work equally well on an infinity mirror *and* a Nixie clock *and* a future LED wall panel.

---

## 1. Positioning Statement

Fill in this template — one sentence that captures the whole pitch. Draft it rough, then tighten.

> For **[who]**, who **[what they want / problem they have]**, **[product name]** is a **[category]** that **[key benefit]**. Unlike **[main alternative]**, we **[differentiator]**.

**Seeded from market analysis + stakeholder map:**

> For **design-conscious adults who want atmosphere over gadgetry**, who **are tired of sealed-unit electronics that break and become landfill**, **[product name]** is a **made-to-order LED infinity mirror** that **blends into a living space as ambient light, not a feature list — and that you can repair with a screwdriver ten years from now**. Unlike **Etsy hobbyist mirrors or sealed gaming decor**, we **design, build, and document every unit as a repairable system, not a disposable novelty**.

**Your edits:** _______________

---

## 2. Target Audience Priority

**Decision (Apr 20):**

| Rank | Archetype | Cares about |
|---|---|---|
| **1** | **Interior Curator** | Finish quality, wall mounting, cord management, how it looks *off* |
| **2** (very close to 1) | **Vibe-Seeker** | Mood lighting, app UX, color customization, ambient presence |
| **3** (far behind) | **Spec-Head** | ESP32, refresh rates, flashable firmware, open-source firmware |

**Founding principle:** *Technical specs should disappear in the face of function, quality, and overall vibes.* The user should never have to think about the ESP32, the refresh rate, or the LED driver. The tech exists to serve the experience, not to be the experience.

This is an anti-principle as much as a direction:
- **Don't:** lead with feature lists, bits/bytes, protocol names, or maker-culture jargon in public copy
- **Don't:** put specs in the hero of a product page
- **Do:** lead with how it feels in a room, what it does for the space, why it's *quiet* rather than *smart*
- **Do:** keep specs available for the Spec-Head who digs — just one click deeper, not on the surface

**How the tiers map to buying behavior:**
- **Curator** pays the premium on finish and presence
- **Vibe-Seeker** pays on mood + ease of use (app UX matters here)
- **Spec-Head** gets served "for free" via open-source firmware and technical transparency — they become evangelists, but the brand isn't aimed at them

---

## 3. Brand Personality — Five Adjectives

**Decision (Apr 20):**

1. **Ambient** — part of the room, not fighting for attention
2. **Considered** — every decision made deliberately; nothing there by accident
3. **Quiet** — confident enough not to shout; quality through restraint
4. **Crafted** — honest about being handmade; aesthetic doesn't undersell the work
5. **Living** — dynamic, responsive, adaptive

**On "Living" — the expansive definition:**

This adjective does more work than the others. It covers three product-level ideas:

- **Dynamic now:** LED patterns that shift, fade, cycle — the mirror is never static
- **Adaptive later:** firmware that learns user preferences, schedules, routines
- **Sensing later:** optional proximity/temperature/ambient sensors via the I2C breakouts already on the STM8 and ESP32 (hardware-ready, firmware-pending)

This becomes a long-term product thesis, not just a copy word. It also creates a **brand commitment** to the user: adaptive and sensing features imply data collection, which requires trust. The brand voice must be *explicit about user control* — no hidden telemetry, no cloud lock-in, clear opt-in for any learning/sensing behavior. This is an HCDE-aligned differentiator against "smart home" brands that hoover data by default.

**Anti-list (what the brand is NOT):** gamer, flashy, edgy, disruptive, "smart" (in the Ring/Nest/Alexa sense), RGB-gaming, cheap, mass-market, loud, trendy, spec-sheet-driven.

**Note on "crafted" vs. operational reality:** For initial runs, production *is* garage-scale / made-to-order — that's a strength to own, not hide. The brand doesn't need to look like MUJI from day one; it needs to look honest and intentional at whatever scale it's at. Etsy as a sales channel is fine; looking like generic Etsy-hobbyist output is not.

---

## 4. Voice & Tone

**Decisions (Apr 20):**

- **Person: first-person singular "I."** Bill is the designer, engineer, and maker — the brand voice *is* his voice. This is a solo founder's authenticity card: every "I" is true. As the business grows, this can transition to "we," but while Layered Logic is one person, "we" would feel inflated. "I" also makes every claim verifiable: *I* made this, *I* designed this, *I* stand behind the repair.
- **Formality:** TBD — leaning considered-conversational (not formal, not casual-bro)
- **Humor:** TBD — likely dry and sparing, never cheeky
- **Technical depth:** hide the ESP32/WS2812B jargon in public copy. Specs live on a "technical details" page that the Spec-Head can dig for, not on the hero. The headline is never "ESP32-powered."

### Tagline — Decision (Apr 20)

**Product-line tagline (infinity mirror):** *"Light that layers."*

**The move — tagline as motif, not fixed string.** The `"[X] that layers"` pattern becomes a parent-brand device. Each product line or service arm gets its own noun, but inherits the cadence:

| Arm / line | Tagline |
|---|---|
| Infinity mirror line | *"Light that layers."* |
| Consulting practice (future) | *"Data that layers."* (working; pattern-confirming) |
| Nixie clock line (future) | *"Time that layers."* (working) |
| Future LED/display products | `"[X] that layers."` — choose the noun per product |

This does three useful things at once:
1. Every product/service reads as "by Layered Logic" without saying it — the *cadence* is the endorsement
2. The tagline family explains the parent-brand name retroactively — "Layered Logic" becomes self-evident once you've heard two of these
3. Product-line differentiation happens in the *noun*, not a whole new tagline — lightweight to extend

**Usage rules:**
- Always the lowercase, declarative form. No exclamation, no capitalization for drama.
- Set in Neue Haas Grotesk ExtraLight Italic at display size — the italic *is* the brand voice.
- Pairs with `by Layered Logic` set in Berkeley Mono, when endorsement is needed.

**Parent-brand tagline (Layered Logic itself):** deferred — let the product lines prove the cadence first, then back-formulate a parent line if one is still needed.

---

## 5. Visual Language — Direction Before Execution

**Decisions (Apr 20):**

- [x] **Sans** (not serif)
- [x] **Cool palette** (not warm)
- [x] **High contrast** — driven by dark-mode-primary + gradient moments against near-black
- [x] **Product-only photography for now** (Q8: Option C); dual photography system deferred as future-state
- [ ] Grid/geometric · [ ] Organic/hand-drawn  *(TBD — references point toward structured grid underlying kinetic/gradient overlays — see Layout & Motion below)*

### Light / Dark Mode Strategy

**Dual-mode with system-preference default.**

- **Dark mode (primary identity):** techier, nerdier, Teenage Engineering-adjacent vibe. Near-black backgrounds, violet glow accents, generous negative space.
- **Light mode:** approachable home-goods vibe, closer to Crate & Barrel / interior-design editorial. Cream or off-white base, deep violet accents connecting back to the parent brand.
- The violet motif connects the two modes — it's what says "Layered Logic" regardless of which mode the viewer is in.
- Default behavior on website/app: detect `prefers-color-scheme` and serve the user's system preference. Manual override available.

**Hypothesis to test (future user research):** technical / design-literate users will self-select into dark mode at higher rates; home/lifestyle buyers will trend light. If true, this gives the brand a natural self-segmenting audience experience without us having to ask who they are. *(Add to user-research sprint, Weeks 4–5.)*

### Color Anchor

**Violet** (user calls it violet, not magenta) at `#BF00FF` as an anchor / motif across the **Layered Logic parent brand**, not just the product mini-brand. Slightly desaturated in practice.

Candidate desaturations (to explore as swatches in `assets/brand/color/`):

| Name (working) | Hex | Use |
|---|---|---|
| Violet Reference | `#BF00FF` | Glow / pure reference — probably too hot for large areas |
| Violet Signal | `#A020D0` | ~70% sat — still electric, less aggressive; accent on dark mode |
| Violet Stone | `#8E3EB0` | ~55% sat — dusty electric violet, works at scale |
| Violet Deep | `#4A1065` | Near-black violet for text/backgrounds in dark mode |

**Pairing:** looks strong against greyscale. Secondary/tertiary palette direction TBD (see Question Q7 below).

### Typography — Decision (Apr 20)

Neue Haas Grotesk ExtraLight Italic is the default display and body voice. Berkeley Mono is the technical accent. No serif.

**The inversion:** in this system, *italic is default and upright is emphasis* — the opposite of most typographic systems. This is the novelty move. Upright weights aren't forbidden; they're deployed deliberately to create a gear-shift moment.

| Role | Face | Weight | Posture |
|---|---|---|---|
| Body, subtitles, most UI | Neue Haas Grotesk Text | ExtraLight | **Italic (default)** |
| Display / headlines | Neue Haas Grotesk Display | ExtraLight or Light | **Italic (default)** |
| Marked emphasis *inside* italic text | Neue Haas Grotesk Text | ExtraLight or Medium | Upright (the emphasis cue) |
| Section-level "straight alignment" moments | Neue Haas Grotesk Text | Medium or Bold | Upright, left-aligned, blocky — used sparingly to break the flow and re-engage the reader |
| Technical callouts | **Berkeley Mono** | Regular | Upright (always) |
| H3 / small heads (product UI, dense screens) | Neue Haas Grotesk Text | Medium | Upright permitted for legibility where italic gets tiring |

**Why Berkeley Mono (not JetBrains Mono, not ABC Diatype Mono):**
- Berkeley Mono has the most characterful drawing without becoming decorative
- ABC Diatype Mono was close but its "R" has a small squiggle/tail that draws the eye — wrong for a brand that prizes restraint
- JetBrains is excellent and free but slightly generic in this lineup

**Reading rhythm this produces:** flowing italic for narrative and product copy (ambient, living), punctuated by upright Berkeley Mono for any technical/measurement moment (serial numbers, versions, coordinates, BOM IDs). The upright mono becomes the "anchor of stability" Felix Faire's work calls for — the hard outline against the soft gradient.

**When to use a *straight-aligned upright block* as a device:** a section that needs to snap the reader to attention — "Specs," "Repair Guide," "What's in the box" — can be set upright to signal a tonal shift from the italic narrative voice into the functional voice. The gear-shift is the engagement mechanism.

**Licensing note:** Neue Haas Grotesk is commercial (Commercial Type). Berkeley Mono is commercial (Berkeley Graphics). Purchase both before any public-facing use. Document in `assets/brand/type/LICENSE.md`.

### Moodboard References

Provided by Bill (Apr 20):

1. **Paris Electronic Week 2022** — Diplomatie Studios — [the-brandidentity.com](https://the-brandidentity.com/project/diplomatie-studios-identity-for-paris-electronic-week-2022-is-a-kaleidoscopic-collision-of-colour)
2. **SPRING Performing Arts Festival** — Lava.nl — [lava.nl](https://lava.nl/progress/spring-performing-arts-festivals-new-identity/)
3. **Adam Ho** — personal site + Archetype interview — [adamho.com](https://www.adamho.com/) · [archetype.fund](https://www.archetype.fund/media/in-conversation-with-adam-ho)
4. **Intrinsic** — Pentagram identity, Felix Faire motion system — [pentagram.com](https://www.pentagram.com/work/intrinsic) · [felixfaire.com](https://felixfaire.com/work/intrinsic)
5. **IBM Quantum** — Field.io brand visuals — [field.io](https://field.io/work/ibm-quantum-brand-visuals)

**Common themes Bill identified across these references:**

- **Gradients** — as a primary brand device, not decoration
- **Soft colors** — saturated but not aggressive; light-as-pigment feeling
- **Repeating patterns** — grids, interference, moiré, particle systems
- **Blending paired with hard outlines** — a signature tension. Blurred luminous fields contrasted against sharp geometric lines, wireframe type, or crisp mask edges.

**Updated brand lineage (supersedes earlier Teenage Engineering-only read):**

The references point to a more kinetic/generative school: contemporary European cultural/festival identity work crossed with tech-spinoff branding (Intrinsic, IBM Quantum, SPRING, Paris Electronic Week). Studios in this lineage: Field.io, Pentagram (Pentagram's experiential group), Lava, Diplomatie, Sons & Daughters, Bureau Borsche, Zak Group.

The meta-taste: **structured editorial typography + kinetic color/pattern surfaces**. Not minimalism. Not maximalism. A disciplined grammar that makes color and motion *legible*.

*Full per-reference analysis, signature devices, pulls, and anti-moves are in [brand-research-notes.md](brand-research-notes.md). Five concrete visual devices distilled below.*

### Five Signature Visual Devices

From the common grammar across all five references — adopted as core brand moves:

1. **Violet bloom behind ExtraLight Italic display type.** A single soft `#BF00FF` radial gradient (~40% canvas, blurred 80–120px) behind left-aligned italic "I" statements. Type at full opacity, bloom out-of-focus. No other color.
2. **Mono tick-mark frame.** Thin 1px white rules with mono labels — e.g. `LL-001 / 6×6 / REV-A / 2026` — at the edges of key images. Annotation-as-branding (IBM Quantum move).
3. **Wireframe over bloom for product reveals.** 1px violet wireframe of the product (exploded axonometric) floating over a volumetric violet bloom. Executed as 2D vector so it's cheap to produce — no render farm required (Felix Faire move, simplified).
4. **Soft-shape container.** An amorphous violet-to-black gradient blob as recurring crop mask for photography or container for short-form copy. Always soft-edged, type on it always sharp (LAVA move, recolored).
5. **Overprint collision, sparingly.** Violet bloom passes *through* a mono caption so the caption inverts where it overlaps. One permitted "collision" per composition. Signals the product is light-based (Diplomatie move, restrained).

### Anti-Moves

- No pastels
- No warm-neutral-only palette (single cream-base in light mode is fine; magazine-beige is not)
- No multi-hue rainbows
- No heavy 3D renders
- No sans beyond Neue Haas Grotesk + Berkeley Mono
- No making italic "special" — italic is default, upright is emphasis. Treating italic as the rare-case is the default behavior this system rejects.

### Layout & Motion Direction

Flowing from the references:

- **Wide margins, generous line-height** — the layout is spacious, not dense
- **Baseline grid visible as quiet structure** — grid as organizing principle, not decoration
- **Information-dense corners** — small technical captions, timestamps, version numbers (mono type) in the margins, signaling craft
- **Gradient surfaces as brand device** — not just for buttons and dividers; whole-frame gradient fields used as backgrounds or hero moments
- **Hard outlines as counterpoint** — wireframe geometry, crisp mask edges, precise type contrast against soft gradient backgrounds. This tension is the signature.
- **Motion is content, not ornament** — website/product pages should treat motion as the medium (per IBM Quantum / Intrinsic), not as a hover polish

Open design decisions that fall out of this:
- [ ] Does the website use a WebGL/shader-driven hero surface? (Significant dev cost, high brand payoff.)
- [ ] Is the brand system *generative* — a set of rules that produces variations — or *composed* (fixed assets)?
- [ ] How does motion survive at product-scale vs. social-media-scale vs. print?

---

## 6. Palette — Decision (Apr 20)

**Locked: Hybrid.** See swatch at [assets/brand/color/palette-hybrid-recommended.svg](../assets/brand/color/palette-hybrid-recommended.svg).

Options B (pure LED spectrum) and C (pure editorial warm) were considered and rejected in favor of the layered system below, which takes the restraint of C for structural use and the saturation of B for moments of light/motion only. The analyses of each pure option are preserved below for reference.

### Option B — LED Spectrum Duotone

Violet anchor + one or two sibling glow colors (electric cyan, soft pink, amber).

**Pros:**
- Differentiates product lines naturally (Infinity Mirror = violet, Nixie Clock = amber/warm, future panel = cyan)
- Reinforces the "these colors exist as light" idea — colors that feel synthetic, not pigment
- Enables multi-hue gradient blooms directly (violet → cyan is a classic LED-spectrum fade)
- Aligns with the Intrinsic / IBM Quantum / Paris Electronic Week references
- Easy to evolve — every new product line can claim a color

**Cons:**
- Risk of looking like Govee / Nanoleaf if handled without restraint
- More palette decisions to keep in sync across product lines
- The violet anchor gets weaker with each additional saturated hue

### Option C — Editorial Warm Counterweight

Violet + warm cream base (`~#F4EFE6`) + deep near-black (`~#0E0D10`). No other saturated colors.

**Pros:**
- Maximum restraint — aligns with "quiet" and "considered"
- Warm cream + cool violet is an intentional contrast (not accidental)
- Works beautifully in light mode (cream base) and dark mode (near-black base), with violet doing the bridging
- Closer to post-Dieter-Rams lineage; safer at scale
- Exactly one saturated color to maintain → visual discipline enforced by construction

**Cons:**
- Product lines can't differentiate by color (need to differentiate by shape, motif, or type)
- Less generative potential for gradients (violet-to-cream-to-near-black is limited palette for bloom work)
- May feel static against references that lean kinetic

### Hybrid (Locked)

Use both, but for different roles. Most references (IBM Quantum especially) operate this way.

```
Layered Logic — Layered Palette (proposed)

Structural / editorial layer (inherited from Option C):
  Near-black ground        #0B0A0F   — dark mode base
  Warm off-white           #F4EFE6   — light mode base, also used for type on dark
  Mid grey                 #8A8A8E   — secondary type, disabled states, rules
  Violet Deep              #2E0D3F   — deep violet for text emphasis on cream, shadows

Brand anchor (present everywhere, unmissable):
  Violet Signal            #A020D0   — primary violet accent, UI
  Violet Reference         #BF00FF   — pure violet, used only inside blooms/glows

Glow spectrum (Option B, restricted to gradients/blooms/motion — never flat UI):
  Violet → Cyan            #BF00FF → #7DD3FC   — primary bloom gradient
  Violet → Pink            #BF00FF → #F472B6   — secondary bloom (for softer moments)

Per-product-line accent (future, Tier 3):
  Infinity Mirror line     Violet Signal       — inherits the parent anchor
  Nixie Clock line         Warm amber          — TBD hex, chosen to be "the nixie color"
  Future display line      TBD                 — stays in the glow-spectrum family
```

**Key rule:** saturated colors only show up inside *gradient blooms* and *motion*. Static UI and typography use the editorial layer (near-black, cream, mid grey, violet deep). This is the discipline that keeps the brand from collapsing into Govee.

**Swatch files (in `assets/brand/color/`):**
- [palette-hybrid-recommended.svg](../assets/brand/color/palette-hybrid-recommended.svg) — **LOCKED**
- [palette-option-b-led-spectrum.svg](../assets/brand/color/palette-option-b-led-spectrum.svg) — reference only
- [palette-option-c-editorial-warm.svg](../assets/brand/color/palette-option-c-editorial-warm.svg) — reference only

## 7. Logo Direction

**Concept (Bill, Apr 20):** Two nested "L"s. **Bill is developing this independently in P5.js** — work lives in a separate GitHub repo (link pending). Generative/code-driven mark exploration, not static vector sketches.

The name "Layered Logic" is literal — it references:
- Layers of 3D printing (manufacturing process)
- Layers of the mirror (frame → LED → acrylic → mirror coating → front panel)
- Layers of the design process (research → sketch → prototype → iterate → ship)
- Layers of data/analysis (raw → cleaned → modeled → insight) — relevant to future consulting work

A nested-L mark puts the layering *in the mark itself*. Each L is a layer; the nesting is the logic.

**Status:** logo exploration is happening in P5.js — under active, not-yet-finalized development. Bill will share the repo link when he's ready for feedback. Until then, no parallel static sketches are being produced here — this doc captures only the constraints any candidate mark will need to satisfy.

**Logo system constraints (any P5 candidate will be tested against these):**
- Works as a favicon at 16×16 (extreme constraint)
- Works etched into a physical product (monochrome, high contrast)
- Works over a gradient bloom (mask or solid variant)
- Has a static-export path — the mark must have a canonical frozen state for use in print, etching, laser, and small-format product application, even if the "live" mark is generative
- Needs both wordmark (`layered logic`, set in Neue Haas Grotesk) and standalone mark versions

**Generative-mark implications (if the P5 work becomes the primary mark):**
- Aligns strongly with the references — Intrinsic, IBM Quantum, Field.io all use generative brand systems
- Requires a "reference frame" — one canonical still of the generative mark that becomes the legal/printable logo
- Opens an animated-logo path for the website hero (the motion *is* the mark)

## 8. What "Layered Logic" Means

Bill's etymology (captured verbatim, Apr 20):

> "Layered Logic" unifies all the aspects of the products and services I want to make/offer. The layers are of 3D printing, of the mirror, of design processes, and of data science projects. It was something that I realized unified all the aspects of the products and services I want to make/offer.

**Implication for brand narrative:** this origin story is a real one — it's worth telling in the "about" page, on packaging inserts, and in founder-voice posts. It's also a good test for future work: if a product or service doesn't have a "layered" character, it probably isn't Layered Logic work.

**Usage in copy (tagline/about candidates that lean on the meaning):**
- "Layers of light, layers of logic."
- "Everything I make is built in layers."
- "The name is literal."

---

## 9. What Comes Out of This Session

**Done today (Apr 20):**
- §0 Brand architecture — three-tier hierarchy (Layered Logic → Product mini-brand → Lines)
- §1 Positioning statement — seeded, needs Bill's final edit pass
- §2 Audience priority — Curator > Vibe-Seeker >> Spec-Head, with "tech disappears" founding principle
- §3 Five adjectives — Ambient, Considered, Quiet, Crafted, Living (+ Living expansion)
- §4 Voice — first-person "I", tagline directions captured
- §5 Visual — sans, cool, high-contrast, product-only photography for now; dual light/dark mode with system default; Neue Haas Grotesk ExtraLight Italic body, mono accent; five visual devices identified
- §6 Palette — B vs C analyzed, hybrid proposal on the table
- §7 Logo — nested-L concept; directions to mock up
- §8 Etymology — Bill's "layers" meaning captured
- Companion research doc: [brand-research-notes.md](brand-research-notes.md) with full reference analysis

**Decisions locked (Apr 20, second pass):**
- Typography: **ExtraLight Italic as default, upright as emphasis** (the inversion). Berkeley Mono for technical accent.
- Palette: **Hybrid** (three-tier system: structural/editorial + brand anchor + glow spectrum)
- Mono finalist: **Berkeley Mono** (ABC Diatype's "R" tail was a no-go; JetBrains felt generic next to Berkeley)
- Tagline: ***"Light that layers."*** for the infinity mirror line; `"[X] that layers."` as a parent-brand motif extending to other products and services

**Still open:**
- §1 Positioning statement — needs Bill's final edit pass
- Logo direction — Bill is developing in P5.js; repo link pending. No parallel sketches being produced here.
- Mini-brand naming (Tier 2) — deferred until visual identity lands
- Parent-brand tagline (Layered Logic umbrella) — deferred; let product-line cadence prove itself first

**Next concrete work:**
- [x] Produce 3 SVG palette swatches in `assets/brand/color/`
- [ ] Fix overlapping text in palette-hybrid-recommended.svg (done Apr 20 2nd pass)
- [ ] Save reference images to `assets/moodboards/`
- [ ] Type specimen mockup — Berkeley Mono "LL-001 / 6×6 / REV-A / 2026" + Neue Haas Grotesk ExtraLight Italic "Light that layers." on dark and light grounds
- [ ] Berkeley Mono + Neue Haas Grotesk license purchase (before any public use)
- [ ] Right-to-Repair philosophy doc (inherits voice from §4)
- [ ] Review Bill's P5 logo work when repo link is shared

---

## Open Questions (log as they come up)

- _______
- _______
- _______

---

## Related

- [Index / MOC](../index.md)
- [Stakeholder Map](stakeholder-map.md) — archetypes (§3.1) that Section 2 audience ranking draws from
- [Market Analysis Report](Market%20Analysis%20Report.md) — 4-segment bifurcation positioning is built against
- Right-to-Repair Philosophy *(pending)* — inherits voice/tone from this doc
- [Sprint Plan](../sprint_plan.md) — Week 4 deliverables
