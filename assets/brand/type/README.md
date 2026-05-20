---
title: Brand — Type Assets
type: asset-folder
status: scaffolded
tags: [brand, typography, assets]
---

# Brand — Type Assets

This folder holds **type specimens, hierarchy examples, and font license records** for the Layered Logic brand system.

The *decisions* (which faces, why, what roles) live in [docs/typography-decisions.md](../../../docs/typography-decisions.md). This folder holds the *visual artifacts* those decisions produce.

---

## What belongs here

| File | Purpose | Formats |
|---|---|---|
| `specimen-nhg.svg` | Neue Haas Grotesk specimen — roles, weights, italic/upright pairing, sample sizes | `.svg`, `.png` |
| `specimen-berkeley-mono.svg` | Berkeley Mono specimen — callout styles, tick-frame labels, code/technical samples | `.svg`, `.png` |
| `hierarchy.svg` | Full type ramp in context — display / subhead / body / caption / mono callout | `.svg`, `.png` |
| `LICENSE.md` | Note-of-record for font licenses purchased (holder, date, scope, seats) — **not the font files themselves** | `.md` |
| `*.woff2` | Web-licensed font files, **only if license permits redistribution in a public repo**. Commercial Type and Berkeley Graphics licenses typically do **not** — check each license before committing | `.woff2` |

## What does NOT belong here

- **Decision rationale** — why these faces, what was rejected, licensing strategy → [docs/typography-decisions.md](../../../docs/typography-decisions.md)
- **Purchased license PDFs / receipts** — these contain PII (name, address, payment info) and live in `Business/brand/type-licenses/` (the uncommitted Business folder). `LICENSE.md` here references them; it does not contain them.
- **`.otf` / `.ttf` source files** — commercial license terms prohibit redistribution. Source files live in the OS font folder locally and are served from a licensed CDN or self-hosted `.woff2` in production.

---

## Current status (May 7, 2026)

**Trial phase — no licenses purchased yet.** Decisions locked (see typography-decisions.md), specimens not produced. Purchase deferred until first public-facing surface; full plan and cost research in [font-license-plan.md](font-license-plan.md).

**To add:**
- [ ] `specimen-nhg.svg` — once license purchased
- [ ] `specimen-berkeley-mono.svg` — once license purchased
- [ ] `hierarchy.svg` — full type ramp
- [ ] `LICENSE.md` — created at purchase trigger (see font-license-plan.md for triggers + cost paths)

---

## Related

- [docs/typography-decisions.md](../../../docs/typography-decisions.md) — full rationale for these face choices
- [docs/brand-positioning.md §5](../../../docs/brand-positioning.md) — type roles and posture decisions
- [assets/README.md](../../README.md) — folder conventions across all design assets
