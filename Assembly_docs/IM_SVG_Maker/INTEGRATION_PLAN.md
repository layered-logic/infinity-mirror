# Visualizer Integration Plan

Scope for porting the IM_SVG_Maker preprocessing into the
[infinity_mirror_visualizer](https://github.com/bowbikes/infinity_mirror_visualizer)
React/Three.js app, so users can upload a JPG or a colored SVG and the 3D preview
shows the *actually-manufacturable* art (post-binarize, post-trace, post-size-floor,
post-nozzle-round).

## Goal

Today the visualizer accepts an already-prepped SVG. JPG is the format most users
actually have (photos, screenshots, marker drawings, downloaded logos). The
preview also doesn't currently reflect what manufacturing will do to the art —
the corner-rounding, the speckle removal, the dropping of unprintably thin
features. After the port:

1. Drop a JPG → see it binarized/dilated/traced live, with the size-floor and
   nozzle-rounding filters applied → 3D preview matches what Bill will fabricate.
2. Drop a colored SVG → click the fill that's "the cut" → same downstream
   preview.
3. The IM_SVG_Maker Python tool stays canonical and stays the engine for actual
   fab files at order time. The JS port is the **preview-time** layer.

## Boundary: what ports, what stays Python

| Module | Stays Python | Ports to JS | Why |
| --- | :---: | :---: | --- |
| `raster.py` (binarize, dilate, trace, size-floors) |  | ✅ | Browser needs JPG handling at upload time |
| `color_select.py` (fill listing + selection) |  | ✅ | Color-pick UX is inherently interactive |
| `svg_parse.py` (SVG → normalized polygons) |  | ✅ | Needed to drive the visualizer's 3D mesh |
| `geometry.py` nozzle rounding only |  | ✅ | The user-visible "cut shape" after manufacturing |
| `geometry.py` kerf/inset/base, `extrude.py`, `laser_export.py` | ✅ |  | Fab-only; runs once per order on Bill's machine |

The JS port stops at "manufacturable black silhouette." The 2D → 3D mesh step
already lives in the visualizer's `SvgIcon.jsx` (Three.js `SVGLoader`); the port
feeds the preprocessed black SVG into it unchanged.

## JS module layout (target)

```
infinity_mirror_visualizer/src/
  preprocess/
    index.js                # public API: preprocess({input, kind, opts}) → { svg, stats, warnings }
    raster.js               # JPG/PNG → black SVG (potrace.js + Canvas)
    colorSelect.js          # colored SVG → black SVG (DOM-walk + fill-match)
    sizeFloors.js           # min-island-area + min-feature-width filters (Clipper.js erosion test)
    nozzleRound.js          # opening-then-closing manufacturability rounding
    fixtures/               # synced from IM_SVG_Maker/tests/fixtures/
  components/
    UploadPanel.jsx         # new: dropzone accepting jpg/png/svg, replaces current SVG-only upload
    ColorPicker.jsx         # new: appears when uploaded SVG has multiple fills
    ManufacturabilityHints.jsx  # new: "feature X dropped — increase resolution or thicken stroke"
    SvgIcon.jsx             # unchanged downstream consumer
    ControlsPanel.jsx       # +1 section: "Manufacturability" with the two threshold sliders
```

Public API mirrors the Python tool's two CLI entry points so reasoning stays
consistent across implementations:

```js
// equivalent to `python scripts/raster_to_black_svg.py`
preprocessRaster(File | Blob, { thicknessMm, threshold, turdsize,
                                minIslandAreaMm2, minFeatureWidthMm,
                                maxLogoDimMm }) → { svgString, stats }

// equivalent to `python scripts/color_svg_to_black.py`
listColors(svgString) → ColorStat[]
selectByColor(svgString, { colors, tolerance, invert }) → { svgString, kept }

// after either of the above:
applyManufacturability(svgString, { minIslandAreaMm2, minFeatureWidthMm,
                                    nozzleDiameterMm }) → { svgString, droppedWarnings[] }
```

The visualizer's existing upload affordance becomes a thin shell over these.

## Library choices

| Need | Pick | Alternative considered | Why this one |
| --- | --- | --- | --- |
| Raster → vector trace | `potrace.js` (kilobtye port) | `imagetracerjs`, WASM potrace | Same algorithm family as the Python `potracer` → easiest parity. ~30 KB. No WASM. |
| Binarize + dilate | Canvas `ImageData` + 3×3 max-filter convolution | OpenCV.js | Self-written ~50 lines vs. a 10 MB WASM bundle. Same math. |
| Polygon offset / erosion (size floors + nozzle round) | `clipper-lib` (js-clipper) | `polygon-clipping`, `jsts` | Has `ClipperOffset` with round joins — exact `shapely.buffer(d, join_style='round')` equivalent. polygon-clipping has no offset. jsts is faithful but ~600 KB. |
| SVG path sampling | Three.js `SVGLoader` (already a dep) | `svg-path-parser`, hand-rolled | Already loaded; gives `ShapePath` objects we can flatten to polylines. |
| SVG fill resolution (color picker) | DOM + `getComputedStyle(el).fill` | `svgelements` port | Browser does CSS/inheritance resolution natively. No library. |
| Float-safe polygon comparison (parity tests) | `vitest` + `clipper-lib` symmetric-difference + area check | hand-rolled | Tooling Bill already uses in JS land. |

## Parity-test plan

Drift between the Python and JS pipelines is the central risk. Mitigation is a
small golden-input suite both pipelines must pass.

**Fixtures** (live in `IM_SVG_Maker/tests/fixtures/`, mirrored into the
visualizer at `src/preprocess/fixtures/` via a small `npm run sync-fixtures`
script — no submodule, just a copy):

| Fixture | Stresses |
| --- | --- |
| `demo_W_black.svg` | The locked baseline (single island, halo + interior). |
| `fox.jpg` | Raster trace, multi-island, dilation. |
| `Washington_Huskies_logo.svg` | Color picker (two fills). |
| `tiny_specks.jpg` | Speckle filtering — `raster_min_island_area_mm2`. |
| `hairline.svg` | Width-floor enforcement — `raster_min_feature_width_mm`. |
| `nested_holes.svg` | XOR / evenodd hole handling. |

**For each fixture, the Python tool emits the canonical black SVG** (committed
alongside the input as `<stem>.canonical.svg`). The JS port emits its own black
SVG at test time.

**Comparison metrics** (in `src/preprocess/__tests__/parity.test.js`):
1. **Island count** — must match exactly.
2. **Total polygon area** — within 2 % (tolerance for Bézier-sampling differences).
3. **Union bbox** — within 0.5 mm on each side.
4. **Symmetric difference area** — < 5 % of either input's area (catches shifted/rotated drift that area-match alone would miss).

A `python tools/regenerate_canonicals.py` script regenerates the `.canonical.svg`
files when the Python tool's behavior intentionally changes. Bill runs that and
commits; CI on the visualizer runs the JS-side parity tests.

## Order of work

Rough chunks, each independently shippable:

1. **JS module skeleton + parity harness.** Write the `preprocess/` directory
   with stub implementations that just pass through input → input. Wire the
   Vitest parity tests so they all fail loudly with clear diff output. Sync the
   golden fixtures from IM_SVG_Maker. (~½ day)

2. **Port `color_select.py`.** DOM walk + computed-fill matching + invert/tolerance.
   Easiest module, no math-library question. Gets the parity harness green on
   the Huskies fixture. (~½ day)

3. **Port `svg_parse.py` flatten.** Use Three.js SVGLoader to sample paths;
   normalize Y-flip + center + scale to `maxLogoDimMm`. Drives the rest.
   (~½ day)

4. **Port `raster.py` (binarize/dilate/trace).** Canvas pipeline + potrace.js +
   shell/hole assembly. Should get the fox parity test passing without the size
   floors. (~1 day)

5. **Port size-floor filters + nozzle rounding via Clipper.js.** Erosion-as-predicate
   for width floor (matches the May-26 decision), straight area filter, opening-
   then-closing for nozzle rounding. The last parity test goes green here. (~½ day)

6. **UI wire-up** — `UploadPanel`, `ColorPicker`, `ManufacturabilityHints`, the
   threshold sliders in `ControlsPanel`. Replaces the existing SVG-only upload
   with the new flow. (~1 day)

Total rough estimate: 3–4 working days. Sequential per chunk; each chunk leaves
the repo in a shippable state.

## Post-port follow-ups (notes only, not in scope of this port)

### Migrate hosting Netlify → Vercel

Trigger: Bill's preference; no technical blocker on Netlify.

Concrete steps when we get to it:
1. Add `vercel.json` with a Vite SPA preset (rewrite all to `/index.html` for
   client-side routing if any; Vite defaults otherwise).
2. Strip Netlify-specific config (`netlify.toml`, `_redirects`, build-image
   pins) — verify what's actually there first.
3. Connect the GitHub repo at vercel.com with framework preset "Vite", build
   command `npm run build`, output `dist/`.
4. Move custom domain. If the visualizer lives at `billwhite.me/infinity` or
   `layeredlogic.cc/infinity`, that's a Cloudflare DNS change to a Vercel CNAME
   ([reference_domain_layeredlogic](.claude/projects/C--Users-bowhi-Desktop-Independent-Study/memory/reference_domain_layeredlogic.md)
   covers the Cloudflare setup).
5. Test the preview deployments + the production deployment side-by-side
   against Netlify before flipping DNS. Decommission Netlify after a week of
   parallel operation.

### New UX modes

Bill flagged additional UX modes as a post-port follow-up but has not yet
specified them. Things this port should leave room for without locking in:

- The `preprocess/` module is *side-effect-free* and pure-function-shaped, so
  any mode (gallery, side-by-side compare, before/after slider, batch upload,
  saved-design library) plugs in without touching the algorithms.
- `UploadPanel`, `ColorPicker`, `ManufacturabilityHints` are siblings of
  `ControlsPanel`, not nested inside it — easy to hide/show per mode.
- The 3D scene takes a black-SVG string regardless of how it was produced —
  so swapping the upload-and-preprocess flow for, e.g., a "browse designs"
  flow doesn't touch the renderer.

**Action item for Bill:** enumerate the UX modes you have in mind so we can
sanity-check the module boundaries above and add concrete component sketches
before the port ships. Right now this is "design with optionality but no
concrete commitments."
