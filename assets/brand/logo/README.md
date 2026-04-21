---
title: Brand — Logo Assets
type: asset-folder
status: active
tags: [brand, logo, assets]
---

# Brand — Logo Assets

Home for the Layered Logic logo — a **generative P5.js sketch** with a **canonical static rest pose**. This folder holds the brand-facing artifacts that accompany the P5 source (which lives in its own repo: [layered-logic/layered-logic-logo](https://github.com/layered-logic/layered-logic-logo)).

---

## Files

| File | Purpose |
|---|---|
| [logo-spec.md](logo-spec.md) | Branding spec — what the mark means, usage rules, constraints, future product-line variations |
| [logo-code-notes.md](logo-code-notes.md) | Annotated companion to the P5.js source — parameters, architecture, dev recipes |
| [logo-primary-dark.svg](logo-primary-dark.svg) | Canonical rest pose on near-black ground, Indigo Reference + Signal |
| [logo-primary-light.svg](logo-primary-light.svg) | Canonical rest pose on white ground, same indigo anchors held as-is |
| [logo-primary-mono.svg](logo-primary-mono.svg) | Single-color variant (black strokes, transparent background). For etch, one-color print, embossing, vinyl cut, over-photography. Swap the stroke to `#FFFFFF` for white-on-dark contexts |

All three SVGs are deterministic — derived from the P5 parameters — and serve as the source of truth for any non-P5 reproduction.

---

## To add

- [ ] `logo-default.png` — raster screenshot from the running P5 sketch (see capture instructions below)
- [ ] `favicon.svg` + `favicon-16.png` — simplified mark for extreme small scale (blocked on [parked item #3](logo-spec.md))
- [ ] `wordmark.svg` — "layered logic" set in Neue Haas Grotesk ExtraLight Italic
- [ ] Sync the P5 source to the locked indigo anchors — outer L needs `#420AFF` → `#4A25FF` (minor bump); inner L is already correct at `#3214FF`

---

## How to capture `logo-default.png` from the running sketch

1. Clone the P5 repo: `git clone git@github.com:layered-logic/layered-logic-logo.git`
2. Open `index.html` in a browser (any modern browser — no build step)
3. **Move the mouse off the canvas** — anywhere outside the browser window, or hover over the URL bar. The mark settles into its rest pose (two static L polylines, no histogram bars).
4. Press `S` — this triggers `saveCanvas('layered-logic', 'png')` in the sketch. A PNG will download.
5. Rename to `logo-default.png` and drop it in this folder.

**For a reproducible frame at a fixed canvas size**, resize the browser to the target dimensions before pressing `S` — the sketch uses `windowWidth` / `windowHeight` for canvas size, so the export matches whatever the window is at capture time. Recommended: 1920×1080 for marketing; 512×512 or 1024×1024 for square product-surface use.

---

## SVG vs PNG — when to use which

- **Use `logo-primary-dark.svg`** on near-black / dark UI grounds (web hero fallback, product packaging, dark-mode interfaces).
- **Use `logo-primary-light.svg`** on white / paper / light-mode grounds. The indigo anchors hold their identity against white with less strident loudness than the earlier violet pair — still electric, but without the magenta cast.
- **Use `logo-primary-mono.svg`** anywhere single-color is required or preferred: laser etch, embossing, foil, vinyl cut, photocopy, one-color print, over photography, or subdued placements where the indigo would compete.
- **Use `logo-default.png`** (once captured) only when a raster is explicitly needed — social media post thumbnails, PNG-only CMS fields, embed contexts where SVG isn't supported.

The live P5 sketch is the authoritative form for web/interactive surfaces. SVG and PNG are fallbacks for contexts where the sketch can't run.

---

## Related

- [assets/README.md](../../README.md) — folder conventions across all design assets
- [docs/brand-positioning.md §7](../../../docs/brand-positioning.md) — logo direction section of the brand worksheet
- [docs/typography-decisions.md](../../../docs/typography-decisions.md) — the 86° angle shares posture with italic-default type
- Canonical source: [layered-logic/layered-logic-logo](https://github.com/layered-logic/layered-logic-logo)
