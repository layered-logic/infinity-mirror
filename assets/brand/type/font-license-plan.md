---
title: Font License Plan — Layered Logic
type: brand
phase: 2
week: 6
date: 2026-05-07
status: trial-phase
tags: [brand, typography, licensing, plan]
---

# Font License Plan

Pre-purchase planning record for the two locked typefaces — Neue Haas Grotesk and Berkeley Mono. The decisions are locked in [docs/typography-decisions.md](../../../docs/typography-decisions.md); this doc tracks **how to acquire them affordably** and what to do during the trial phase before any public launch.

When licenses are actually purchased, that goes in `LICENSE.md` (sibling file). This doc is the *plan* that informs purchase; `LICENSE.md` will be the *record* of what was bought.

---

## Current decision

**Trial both fonts. Defer purchase until a public-facing surface needs them.**

Trigger to buy: any of —
- First public marketing surface goes live (web hero, product page, listing)
- First product unit ships with packaging that uses these faces
- First press / portfolio piece publishes externally

Until then, mockups stay internal — Figma files, brand decks, this repo — which the trial-use terms below cover.

---

## Berkeley Mono (U.S. Graphics)

### License tiers (as of May 2026)

| Tier | Price | Use covered |
|---|---|---|
| **Developer / Personal** ([FX-102](https://usgraphics.com/catalog/FX-102)) | $75 one-time | Personal use, install on company devices. **Commercial use NOT covered.** |
| **Indie** ([FX-202](https://usgraphics.com/catalog/FX-202)) | $295 one-time, perpetual | Sole proprietor / freelancer / small business ≤5 employees. Commercial use OK. |
| Commercial (company-size based) | Higher (contact U.S. Graphics) | Larger orgs |

**Restriction to know:** the Indie commercial license is "UI elements only" and excludes building IDE/Terminal/Editor apps. For Layered Logic's use cases (product UI, marketing site, packaging captions, BOM tags) this is fine — a non-issue.

### Trial path

U.S. Graphics does not publish a free trial. Two options:

1. **Buy Personal at $75 now.** Use it for internal mockups (Figma, brand decks, this repo). Upgrade to Indie at $295 before any public commercial use. Total path cost = $370 if both bought. **Verify with U.S. Graphics whether Personal-tier files transfer to an upgraded Indie license without re-download / re-purchase.**
2. **Use a free placeholder for layout fidelity.** JetBrains Mono is the closest free analog (already rejected for the brand on character-feel grounds, but its metrics are similar enough for placement). Swap for Berkeley Mono once licensed. Total path cost = $295 at purchase, $0 now.

**Recommended:** Option 2. Don't pay $75 for a license that doesn't cover the eventual use case. Use JetBrains Mono in Figma as a layout placeholder; Berkeley Mono drops in cleanly when purchased.

### When to buy

Indie tier ($295) at the trigger above. Single payment, perpetual, no annual fee.

---

## Neue Haas Grotesk

### Two distribution channels — same typeface, different licensing

| Source | Family carrying ExtraLight Italic | Pricing visibility |
|---|---|---|
| **Commercial Type** (original foundry) | Text family includes ExtraLight | Not published — email `info@commercialtype.com` for quote |
| **Linotype / MyFonts** | Display Pro family only (`Pro 36 Extra Light` + `Pro 36 Extra Light Italic`) | $33.99 per single style desktop, $344.99 family of 16 |

A [TypeDrawers thread](https://typedrawers.com/discussion/2612/neue-haas-grotesks-pricing) notes the Linotype and Commercial Type versions are functionally identical files — the price difference is licensing terms, not file quality.

### Important: ExtraLight Italic availability gotcha

The locked spec calls for **ExtraLight Italic as the default body/display posture**. ExtraLight Italic is:
- ✅ Available in **Commercial Type's Text family**
- ❌ NOT available in **MyFonts/Linotype's Text family** (which only carries Roman/Italic in Regular/Medium/Bold — Pro 55/56/65/66/75/76)
- ✅ Available in **MyFonts/Linotype's Display Pro family** as `Pro 36 Italic`

**Implication:** if buying through MyFonts (cheapest path), you must use Display Pro, not Text. Display Pro 36 was drawn for headline use — there's a legibility tradeoff at 14–16px body sizes that should be tested before committing.

### Cheapest viable starter kit (MyFonts, desktop only)

| Style | Role | Price |
|---|---|---|
| Pro 36 Extra Light Italic | Default body / display | $33.99 |
| Pro 36 Extra Light (upright) | Emphasis pair | $33.99 |
| Pro 65 Medium *(optional)* | H3 / dense UI per spec | $33.99 |
| **Subtotal** (2 styles) | | **~$68** |
| **Subtotal** (3 styles) | | **~$102** |

**Web license is sold separately** on MyFonts. The desktop license covers Figma/Illustrator/print only — embedding in a website (font-face) requires a web license per style. Pricing is pageview-tiered; get a quote before buying desktop.

### Trial path

1. **Email Commercial Type at `info@commercialtype.com`** to request trial fonts. Commercial Type historically provides trial files on request — typically watermarked or time-limited, restricted to internal mockup use. Free.
2. **Use Inter as a free placeholder.** Closest free grotesk analog at small sizes; metrics differ but readable. Swap once licensed.

**Recommended:** Option 1 — contact Commercial Type. Two upsides: (a) you get trial files of the *exact* faces specced, including ExtraLight Italic in Text (not just Display); (b) opens the door to a Commercial Type quote that may compete with MyFonts and gives you the canonical-source story for the brand.

### When to buy

At the trigger above. Two paths to compare at that moment:

| Path | Likely cost | Notes |
|---|---|---|
| MyFonts Display Pro, 2 styles desktop + web | ~$200–400 est. | Cheapest, but Display family — body-size legibility caveat |
| Commercial Type Text, 2 styles desktop + web | TBD (likely higher) | Canonical source, ExtraLight in Text family (proper for body), brand story |

**Recommend re-deciding at purchase time** based on the Commercial Type quote — it might land close enough to MyFonts that the legibility + canonical-source upside is worth it.

---

## Cost summary (estimated, at purchase trigger)

| Item | Floor estimate | Realistic estimate |
|---|---|---|
| Berkeley Mono Indie | $295 | $295 |
| NHG ExtraLight Italic + ExtraLight (desktop, 2 styles) | $68 (MyFonts) | $200–400 (with web license) |
| NHG additional weights (Medium, etc.) | $0 (defer) | $34–100 |
| **Total** | **~$363** | **~$500–800** |

**Budget guidance:** plan for $500–800 when the trigger hits. Don't pre-pay; the trial paths above cover all internal work until launch.

---

## Action checklist

### During trial phase (now → first public surface)

- [x] Document this plan (this file, 2026-05-07)
- [ ] Email Commercial Type for NHG trial fonts + price quote on Pro 36 / 36 Italic (desktop + web, 1-user)
- [ ] Drop placeholder fonts into Figma / brand mockups (Inter for grotesk, JetBrains Mono for mono)
- [ ] Note in mockup files which font is placeholder vs. licensed-target so swap-in is mechanical later

### At purchase trigger

- [ ] Re-quote both NHG paths (MyFonts Display Pro vs. Commercial Type Text). Pick based on body-legibility test + price delta
- [ ] Buy Berkeley Mono Indie ($295) — confirm "UI elements only" restriction is fine for use cases
- [ ] Buy NHG selected styles (desktop + web)
- [ ] Move purchase records to `Business/brand/type-licenses/` (PDFs/receipts — uncommitted, has PII per `feedback_business_folder`)
- [ ] Create `LICENSE.md` in this folder as a public-safe note-of-record (foundry, license name, date, scope, seats — no PII)
- [ ] Update `assets/brand/type/README.md` to mark `LICENSE.md` as present

---

## Related

- [docs/typography-decisions.md](../../../docs/typography-decisions.md) — full rationale for the face choices
- [docs/brand-positioning.md §5](../../../docs/brand-positioning.md) — type roles
- [README.md](README.md) — folder index
- Future sibling: `LICENSE.md` (created at purchase)
