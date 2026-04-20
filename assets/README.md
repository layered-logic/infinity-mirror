---
title: Design Assets
type: overview
status: active
tags: [assets, design, brand]
---

# Design Assets

Home for all visual and brand-facing files: logos, color specimens, type samples, product photography, moodboards, and design-system source files.

This folder is the counterpart to the written brand work in [docs/brand-positioning.md](../docs/brand-positioning.md). Decisions get documented there; visual artifacts land here.

---

## Folder structure

| Folder | Contents | Formats |
|---|---|---|
| `brand/logo/` | Final logo lockups, marks, wordmarks | `.svg`, `.png` (export), `.ai`/`.fig` (source) |
| `brand/color/` | Color palette specimens, swatches, usage examples | `.svg`, `.png` |
| `brand/type/` | Type specimens, hierarchy examples, font files if licensed for distribution | `.svg`, `.png`, `.woff2` |
| `brand/tokens/` | Design tokens (colors, type ramp, spacing) for import into Figma via Tokens Studio, or into code | `.json` (W3C design-tokens format) |
| `moodboards/` | Reference collections, adjacent-brand studies (per [brand-positioning §5](../docs/brand-positioning.md)) | `.png`, `.jpg`, `.md` (annotated boards) |
| `product-photos/` | Finished product photography for portfolio, listings, social (Week 9 deliverable) | `.jpg`, `.raw`/`.dng` in `source-files/` |
| `icons/` | Product UI and web icons | `.svg` |
| `figma/` | Figma exports, screenshots of frames, design system specs | `.png`, `.pdf` (no native `.fig` — lives in Figma cloud) |
| `source-files/` | Editable source: Illustrator, Figma exports, Photoshop, raw photos | `.ai`, `.psd`, `.xd`, `.dng`, `.raw` |

---

## Conventions

- **Naming:** `kebab-case`, no spaces. Include version/variant: `logo-primary-dark.svg`, `logo-primary-light.svg`.
- **Color in filenames:** prefer semantic names (`logo-primary`) over hex values — hex goes in `tokens/`.
- **Exports:** always export `.svg` for vector marks and a `@2x` `.png` fallback. Photos at both full-res and a `-web` variant.
- **Source files:** keep the editable source next to exports in `source-files/` so you can revise later. If a file came from Figma, note the Figma URL inside a sibling `.md`.
- **Licensed fonts:** do NOT commit `.otf`/`.ttf` for commercial licenses — commit a `LICENSE.md` in `brand/type/` noting the license and where the files are stored.

---

## Figma

`.fig` files cannot live in git (proprietary binary, no merge story). The Figma cloud file is authoritative. This folder holds:

- Exported frames as `.png` or `.pdf` for stable reference
- A `figma/README.md` with the Figma file URL(s) and what each file covers
- Design tokens exported to `brand/tokens/*.json` via the Tokens Studio plugin, so they can be version-controlled

---

## Related

- [Index / MOC](../index.md)
- [Brand Positioning](../docs/brand-positioning.md) — written brand direction this folder executes on
- [Sprint Plan — Week 4](../sprint_plan.md) — brand identity deliverable
- [Sprint Plan — Week 9](../sprint_plan.md) — product photography / media assets deliverable
