---
title: Layered Logic Logo — Code Notes
type: brand
phase: 2
week: 4
date: 2026-04-21
status: current
tags: [brand, logo, p5js, code]
---

# Layered Logic Logo — Code Notes

Annotated companion to the P5.js source at [layered-logic/layered-logic-logo](https://github.com/layered-logic/layered-logic-logo) (private repo). This doc explains *how the sketch works* in plain English, documents the parameters, and records dev recipes for common tweaks.

The repo itself has its own short `README.md` (quick start) and `CLAUDE.md` (file map). This doc is the deeper commentary — what the code is *doing* and *why the choices matter*. Read alongside [logo-spec.md](logo-spec.md) for the brand side.

**Canonical source:** the P5 repo. If this doc and the code disagree, the code wins — update this doc.

---

## Stack

- [P5.js 1.9.0](https://p5js.org/) loaded from cdnjs CDN
- No build step, no dependencies. Open `index.html` in a browser.
- Two files do all the work:
  - `index.html` — 23 lines — sets `<body>` background `#0a0a0a`, loads p5 and the sketch
  - `p5_code.js` — 267 lines — the entire sketch

---

## Parameter reference

All parameters live at the top of `p5_code.js` (lines 3–19). Tweak here, reload, see the effect.

### Geometry

| Param | Default | What it does |
|---|---|---|
| `angleDeg` | `86` | Angle of the vertical arm (degrees, measured from horizontal). `90` would be true vertical; `86` gives a 4° tilt that matches the italic-default type posture. |
| `lenVert` | `320` | Length of the outer L's vertical arm (px) |
| `lenHorz` | `220` | Length of the outer L's horizontal arm (px) |
| `innerLen` | `180` | Length of the inner L's horizontal arm (px). The inner vertical arm is `lenVert * 0.9 = 288px` — hardcoded in the `drawUnifiedL` call, not a top-level param. |
| `gap` | `20` | Spacing between the inner and outer L, in pixels perpendicular to each arm |
| `wOuter` | `7` | Outer L stroke weight (px) |
| `wInner` | `6` | Inner L stroke weight (px) |

### Histogram behavior

| Param | Default | What it does |
|---|---|---|
| `histRadius` | `150` | Mouse proximity required to trigger the effect (px from path). Beyond this, the L draws static — **this is the rest pose trigger**. |
| `histWindow` | `150` | Length of the histogram "window" along the L path (px). The bars appear within a `histWindow`-wide band centered on the mouse projection. |
| `bins` | `15` | Number of histogram bars in the window |
| `maxBarLen` | `150` | Maximum bar length (px). Bars scale between `weight/2` and this. |
| `sigma` | `0.35` | Gaussian width controlling how sharply bars peak at the mouse position. Lower = sharper peak; higher = wider spread. |
| `barSteps` | `5` | Bar heights quantized to this many discrete levels — creates the stair-step / "measured" feel. Raise for smoother, lower for more stepped. |
| `barRound` | `false` | If true, bars have rounded caps; if false, square caps. Current: square (reads more technical). |
| `cornerBlend` | `50` | Px zone near the L corner where bar direction smoothly blends from vertical-perpendicular to horizontal-perpendicular. |
| `verticalBias` | `25` | Px preference for the vertical segment when projecting the mouse. Biases ambiguous mouse positions near the corner toward the vertical arm. |

---

## Architecture

### Entry points (standard P5)

- **`setup()`** — creates a full-window canvas, sets `angleMode(DEGREES)`, `strokeCap(ROUND)` default, `noFill()`.
- **`draw()`** — runs every frame:
  1. Paints background `#0a0a0a`
  2. Translates origin to `(width * 0.45, height * 0.6)` — this is the logo's anchor point, slightly left-of-center and below vertical center
  3. Builds `vDir` (along vertical arm), `hDir` (along horizontal), and perpendicular unit vectors `nV` / `nH`
  4. Calls `drawUnifiedL` twice — once for the outer L, once for the inner L with bar directions reversed

### Core function — `drawUnifiedL({ corner, vDir, vLen, hDir, hLen, vBarDir, hBarDir, col, weight, id })`

This is the whole drawing system. It treats each L as a 1D path parameterized by `s ∈ [0, vLen+hLen]`:
- `s = 0` is the top of the vertical arm (V0)
- `s = vLen` is the corner (V1)
- `s = vLen + hLen` is the end of the horizontal arm (V2)

**Steps:**
1. **Project mouse onto path** — `projectToPath()` finds closest point on the L, returning `s` (position along the path) and `d` (perpendicular distance). Applies `verticalBias` so ambiguous projections near the corner favor the vertical arm.
2. **Early-out** — if `d > histRadius` (mouse is far), just draw the L as two static lines. **This is the rest pose branch.** Returns immediately.
3. **Hover intensity** — compute `hover = smoothstep(1, 0, d / histRadius)`, which is `1` when mouse is on the path and `0` at the edge of the active radius. Used to fade bars in/out smoothly.
4. **Smooth + snap the window center** — `state.center` (stored per-L in `_pathState`) lerps toward the mouse projection, then snaps to the nearest bin grid position. This is what gives the stair-step propagation feel: the window doesn't slide continuously, it jumps bin-by-bin.
5. **Clamp window to path bounds** — so the histogram can't spill off the ends.
6. **Draw L segments outside the window** — the portions of the L path not inside the histogram window are drawn as normal lines.
7. **Draw spine through the window** — connects the bars at their bases.
8. **Draw each bar** — for `i = 0 .. bins-1`:
   - Compute bar position `s` along the path
   - Get bar direction (blended near corner via `getBlendedBarDir`)
   - Compute Gaussian intensity `g = exp(-0.5 * (u/sigma)^2)` where `u` is distance from window center
   - Quantize to `barSteps` levels
   - Halve amplitude on the horizontal section (`ampScale = 0.5` if past the corner) — the §7 parked question
   - Draw the bar as a `line()` from the spine outward

### Helpers

| Function | Responsibility |
|---|---|
| `getPointOnPath(V0, V1, V2, vLen, s)` | Converts path parameter `s` back to a `(x, y)` point on either the vertical or horizontal segment |
| `projectToPath(mouse, V0, V1, V2, vLen, hLen)` | Projects mouse onto the path, returns `{s, d}` with vertical-segment bias |
| `drawPathSegment(V0, V1, V2, vLen, hLen, s0, s1)` | Draws the portion of the L path between two parameters, handling the corner transition |
| `getBlendedBarDir(s, vLen, vBarDir, hBarDir)` | Smoothly blends bar direction from vertical-perpendicular to horizontal-perpendicular across the `cornerBlend` zone |
| `distToSegment(P, A, B)` | Standard point-to-line-segment distance (returns `[distance, t]` where `t` is projected length along AB) |
| `smoothstep(e0, e1, x)` | Standard smoothstep easing |

### Per-L state (`_pathState`)

A `Map` keyed by `id` ("outer" / "inner") holding `{ center }` — the smoothed, snapped window center for each L. Keeping state per-L means the two histogram windows propagate independently when the mouse is near both.

---

## Controls

| Input | Action |
|---|---|
| Move mouse | Reveal histogram on whichever L is closest |
| Move mouse outside `histRadius` | Return to rest pose (static L polylines) |
| Press `S` | Save current canvas as `layered-logic.png` via `saveCanvas()` |

---

## Dev recipes

**Change colors.** Edit `outerCol` and `innerCol` inside `draw()` (lines 44–45). Currently `color(66, 10, 255)` (`#420AFF`) and `color(50, 20, 255)` (`#3214FF`). Locked brand palette (Apr 21): Indigo Reference `#4A25FF` / Indigo Signal `#3214FF`. Minor bump needed on the outer: `color(66, 10, 255)` → `color(74, 37, 255)`. Inner is already correct. The SVG exports in this folder already use the final locked values.

**Adjust sensitivity.** `histRadius` controls how far the mouse can be and still trigger; `sigma` controls how sharply bars peak. For a subtler effect, drop `histRadius` to 80 and raise `sigma` to 0.5.

**Change bar count / resolution.** `bins` sets count. `barSteps` sets vertical quantization. For a finer read, try `bins = 25, barSteps = 8`.

**Swap corner-rounding on caps.** Flip `barRound = true` for rounded bar ends. Leaves the L spine still `ROUND` (set in `setup()`).

**Add a third L.** Call `drawUnifiedL()` a third time in `draw()` with its own corner, dirs, and `id: "third"`. The per-L state map (`_pathState`) handles independent propagation.

**Export a frame.** Run in browser, move mouse far from the canvas (so the mark is at rest), press `S`. Exports `layered-logic.png`.

**Export a SPECIFIC frame (e.g., an animation moment).** The `S` key saves whatever frame is active. If you want a reproducible non-rest frame, temporarily hardcode `mouseX`/`mouseY` inside `draw()` before the `projectToPath` call.

---

## Notes and observations

- **The mark doesn't animate on its own.** All motion is mouse-driven. On a touchscreen with no hover, the mark sits at rest — this is fine for product surfaces but means the web hero should probably ship a subtle idle animation (scheduled on a timer, fake a mouse projection) to show the mark's character before a user interacts.

- **`ampScale = 0.5` on the horizontal arm (line 151) is hardcoded.** This makes horizontal bars half-height. Reads as "settling toward the ground." Parked: is this deliberate or a balance hack? Decide and document in logo-spec.md.

- **Performance is unconstrained.** Full-window canvas, 60fps, no throttling. Fine on desktop; may need a reduced-canvas or `frameRate(30)` for mobile web hero.

- **Private repo.** Currently at [layered-logic/layered-logic-logo](https://github.com/layered-logic/layered-logic-logo). Decide visibility before public web launch — open-sourcing the mark is consistent with the right-to-repair posture; keeping it private is consistent with closed-binary firmware. This is a brand decision, not a code decision.

---

## Related

- [logo-spec.md](logo-spec.md) — brand side: what the mark means, usage rules, constraints
- [logo-primary-dark.svg](logo-primary-dark.svg) — rest pose derived deterministically from the parameters above
- [Brand Positioning §7](../../../docs/brand-positioning.md) — parent worksheet
- [Typography Decisions](../../../docs/typography-decisions.md) — the 86° angle shares the italic-default posture
- Canonical source: [layered-logic/layered-logic-logo](https://github.com/layered-logic/layered-logic-logo)
