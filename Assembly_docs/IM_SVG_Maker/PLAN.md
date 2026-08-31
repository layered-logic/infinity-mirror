# Infinity Mirror SVG Processor — Plan

## Context

We're building a Python CLI that takes a **monochrome (black) SVG** — every path in the file represents the cut region — and emits the fabrication files for a Kintsugi-style infinity mirror:

1. **Laser-cut files** (SVG + DXF) for a 150×150 mm acrylic mirror tile — outer mirror outline plus an interior cutout matching the SVG.
2. **3D-printed plug** (3MF + STL) in clear PETG that fills the laser cutout. The plug has two stacked layers:
   - **Inset** (3 mm tall, matches acrylic thickness) — drops into the laser-cut hole, flush with the mirror's front face.
   - **Base** (1 mm tall) — sits behind the mirror, offset wider than the inset, overlapping each mirror fragment to hold them in place.

For multi-island logos, the base needs to grow wide enough to bridge islands into a single supporting piece while preferring to conserve filament.

**Out of scope for this project**: turning a multi-color SVG into the black input SVG. That's a separate future tool (it needs more user input — which color is the cut, how to handle overlaps, etc.). This program's contract is intentionally narrow: black in, fab files out.

**Demo input**: `demo_W_black.svg` (created during planning by extracting the gold halo path from `Washington_Huskies_logo.svg` and recoloring to black). The original colored SVG stays in the repo as reference but isn't consumed by the tool.

## Learnings from the prep / preview phase

- **Kerf must be a mm-offset, not a scale.** The original `scale: 1.05` was a multiplicative scale about the origin, which produces zero offset at the centroid and grows with distance — wrong physical model for a laser kerf (which is a fixed-width material gap). Replaced with `laser_kerf_mm` (uniform outward offset via `shapely.Polygon.buffer(dist)`).
- **All `buffer()` calls use `join_style="round"`.** Mitre joins extend wildly at sharp inner corners (the W's V notches are 30–45°, which would push a mitre out by 4–8× the offset distance). Round joins give a true uniform offset everywhere.
- **Nozzle rounding is applied to the cut polygon *before* deriving any output.** Both inside and outside corners are rounded to `nozzle_diameter_mm / 2` via opening-then-closing: `cut.buffer(-r).buffer(+r).buffer(+r).buffer(-r)`. Ensures no feature is sharper than the printer can reproduce, and the laser cut matches what gets printed (so the plug fits).
- **SVG → polygon topology** for the W: one outer subpath + one inner W-silhouette subpath, combined via `symmetric_difference` to produce a single polygon with one hole. Works for typical halo-style logos. For SVGs with many nested subpaths we'll need shell/hole sorting by area + containment.
- **SVG path sampling** uses `svgelements.Path.npoint(np.linspace(0, 1, 400))` — npoint requires a numpy array (passing a Python list raises `TypeError`).
- **Visual verification trick**: render the *difference* polygons (`laser - cut` for kerf band, `cut - inset` for clearance, `base - cut` for support overlap) as filled regions. Comparing two thin outlines produces misleading perception of width at concave corners; filled bands make uniformity obvious. Worth keeping for any visual debugging.
- **Base support engulfs small features.** When `base_offset_min_mm >= half-width-of-feature`, that interior mirror fragment is fully covered by the base — no glass shows, just plastic. Acceptable for v1; future option to detect and skip tiny fragments noted in Alternatives.
- **Matplotlib polygon-with-holes rendering** must use a single compound `PathPatch` with `MOVETO`/`LINETO`/`CLOSEPOLY` codes — naïvely "punching holes" by overdrawing white wipes out anything underneath.
- **Preview scripts to keep** as part of the repo (under `scripts/`) for future visual debugging: `preview_cutlines.py`, `preview_zoom.py`, `diag.py`. They drive the same pipeline functions as the CLI.
- **Raster preprocessor** lives in the package as `im_svg_maker.raster.trace_raster_to_svg(image, out_svg, cfg)`. `scripts/raster_to_black_svg.py` is a thin CLI wrapper around it. Takes raster_thickness_mm / raster_threshold / raster_turdsize from `variables.json` (overridable via CLI flags). Uses potracer for trace + a shell/hole assembly pass (sort by area desc, depth parity from `representative_point` containment) so the resulting SVG has proper topology for the main pipeline's XOR parser.
- **STRtree.nearest returns numpy.int64**, not a Python int, and doesn't reliably skip self. Replaced with a straight O(N²) pairwise `distance()` in `_nearest_island_gap` — fine for the island counts we'll see (typically <50).
- **`base_offset_per_island_mm` default lowered to 0.5** (from 3.0). With more than a couple islands, 3.0 produced runaway offsets (fox at 3 islands → 8 mm base, swallowing the whole logo as a blob). 0.5 keeps the base hugging the contour; the bridge-factor term still picks up the slack when islands are genuinely far apart.
- **Width-floor: erosion-as-test, not morphological opening.** First implementation of `raster_min_feature_width_mm` did `island.buffer(-w/2).buffer(+w/2)` (open) intending to prune thin parts in-place. On the fox at width=2.5mm this *fractured* the islands at every thin neck and the count went from 5 to 34 — the opposite of "filter out thin junk." Switched to using erosion only as a keep-or-drop predicate: if `island.buffer(-w/2)` is empty the whole island is too thin and gets dropped, otherwise the original (unmodified) island is kept. No reshaping, no fracturing, predictable behavior. Thin appendages on otherwise-thick islands still ride through — that's the main pipeline's nozzle-rounding job to clean up.
- **`color_select.py` matches on resolved fill via svgelements `reify=True`.** SVG fill resolution has to follow CSS/inheritance/transform paths (e.g. the Huskies SVG has its colors on `<path fill=...>` directly but real-world exports often inherit from a `<g style="fill:..."/>`). Doing this from raw XML is hand-rolled CSS; doing it via svgelements is one call. The emitted black SVG uses the `.d()` of the *transformed* path so the output coordinates are in viewport space, ready for the main pipeline's existing parser to re-XOR via evenodd.

## Inputs

- A monochrome SVG path file (e.g. `demo_W_black.svg`). All `<path>` elements are treated as the cut region; fill color is ignored. Convention is black fill so source files are visually obvious.
- `variables.json` — configuration. Will be expanded from the current `scale: 1.05` to:

```jsonc
{
  "mirror_size_mm": [150, 150],      // acrylic sheet size (centered)
  "max_logo_dim_mm": 100,            // longest side of logo fits within this
  "nozzle_diameter_mm": 0.6,         // 3D printer nozzle; all corners pre-rounded to radius = this/2
  "laser_kerf_mm": 0.1,              // uniform outward offset of the laser cut (mm, NOT a scale)
  "inset_clearance_mm": 0.15,        // plug inset perimeter shrinks by this for slide-in fit
  "inset_height_mm": 3.0,            // matches acrylic thickness
  "base_height_mm": 1.0,             // support layer thickness
  "base_offset_min_mm": 2.0,         // floor overlap each fragment must get
  "base_offset_per_island_mm": 0.5,  // extra offset per island beyond the first (small bonus; bridge handles the heavy lifting)
  "base_bridge_factor": 1.0,         // 0..1 — scales the auto-bridge requirement
  "raster_thickness_mm": 0.75,       // (preprocessor) dilate traced raster lines by this thickness
  "raster_threshold": 128,           // (preprocessor) grayscale threshold (0-255) for foreground detection
  "raster_turdsize": 8               // (preprocessor) ignore speckles smaller than this many pixels
}
```

Final base offset formula: `max(base_offset_min_mm + (N-1)*base_offset_per_island_mm, base_bridge_factor * (nearest_island_gap_mm / 2))`. Logged at run time so the user can see why a given offset was chosen.

## Outputs (written to `out/<svg-stem>/`)

- `laser.svg` — 150×150 mm outer rectangle + kerf-scaled gold cut as an interior hole. Strokes set to hairline red (`#FF0000`, 0.01 mm) — common laser convention.
- `laser.dxf` — same geometry as DXF (via `ezdxf`).
- `plug.3mf` — primary 3D file (carries mm units, more modern).
- `plug.stl` — STL fallback for slicers that prefer it.
- `run.log` — chosen parameters, island count, computed base offset, bounding boxes.

## Pipeline

1. **Parse SVG** — `svgelements` to load all `<path>` elements with their resolved cumulative transforms applied (matrix, scale, translate). Color/fill is ignored.
2. **Flatten paths to polygons** — segmentize Béziers via `svgelements`, build shapely `Polygon`/`MultiPolygon` honoring the SVG fill-rule so subpath holes become real holes.
3. **Normalize coordinates** — flip Y to standard math orientation, translate so logo bbox is centered at origin, scale uniformly so `max(width, height) == max_logo_dim_mm`.
4. **Build polygon set** — convert each closed path (with holes) into a `shapely.MultiPolygon`. Resolve fill-rule (likely evenodd for the halo — its interior W counts as a hole).
5. **Round to nozzle limit** — apply opening then closing with `r = nozzle_diameter_mm / 2` to the cut polygon. All subsequent geometry derives from this rounded cut.
6. **Compute geometries** (all `shapely` ops use `join_style="round"`):
   - `inset_poly` = `cut.buffer(-inset_clearance_mm)`.
   - `laser_poly` = `cut.buffer(+laser_kerf_mm)` (uniform mm offset, NOT a scale).
   - Detect **islands**: number of polygons in the `MultiPolygon` (top-level, holes don't count).
   - Auto-bridge gap = min nearest-neighbor distance between island bounds (use `shapely.STRtree`).
   - `base_offset` per formula above.
   - `base_poly` = `cut.buffer(+base_offset)` (expands outward AND shrinks holes inward — exactly the support overlap we want for both the inner W mirror fragment and the surrounding mirror).
7. **Extrude to 3D**:
   - `trimesh.creation.extrude_polygon(inset_poly, inset_height_mm)` → translate up by `base_height_mm`.
   - `trimesh.creation.extrude_polygon(base_poly, base_height_mm)` → sits at z=0.
   - Union with `trimesh.boolean.union` (manifold3d backend) to make one watertight mesh.
8. **Emit files** — `mesh.export('plug.3mf')`, `mesh.export('plug.stl')`. Laser SVG built by hand (small XML) so we control stroke styling; DXF via `ezdxf` polylines.

## Key files & libraries

New project layout (all new — no existing code to reuse):

```
IM_SVG_Maker/
  pyproject.toml              # uv/pip deps
  im_svg_maker/
    __init__.py
    cli.py                    # argparse entrypoint: `python -m im_svg_maker <svg>`
    config.py                 # load+validate variables.json (pydantic)
    svg_parse.py              # SVG → shapely MultiPolygon (all paths, color ignored)
    geometry.py               # nozzle rounding, kerf/inset/base offsets, islands, base formula
    extrude.py                # shapely → trimesh meshes, union, export
    laser_export.py           # SVG + DXF writers
    raster.py                 # raster image -> black SVG (potracer + shell/hole assembly)
  scripts/
    preview_cutlines.py       # filled-band 2D preview of cut/laser/inset/base
    preview_zoom.py           # zoomed kerf-band view for visual debugging
    diag.py                   # polygon topology / area / winding diagnostic
    raster_to_black_svg.py    # CLI wrapper around im_svg_maker.raster.trace_raster_to_svg
  Washington_Huskies_logo.svg # original colored reference, not consumed
  demo_W_black.svg            # demo input (already created)
  variables.json              # extended config
  out/                        # generated
```

Dependencies: `svgelements`, `shapely>=2`, `trimesh`, `manifold3d`, `ezdxf`, `pydantic`, `numpy`.

## Alternatives logged (not implemented)

Per the question round, these alternatives for the base shape were considered and rejected for v1 but worth recording:

- **Convex hull of islands** — always bridges, but loses the organic outline. Could be a future `base_strategy: "hull"` option.
- **Bounding rectangle** — simplest, ugliest, ruled out.
- **Per-island independent plugs** — would make the kintsugi look fragmented but break the "single plug" assumption.
- **Skip-tiny-fragment base** — detect interior mirror fragments smaller than `2 * base_offset` and stop the base from closing over them (so even tiny features keep a sliver of mirror visible). Adds complexity; defer until we see a logo where this matters. **(Implemented 2026-05-26 at a different layer than originally planned: `raster_min_island_area_mm2` and `raster_min_feature_width_mm` ship in [raster.py](im_svg_maker/raster.py) — i.e. in the jpg→svg preprocessor, since that's the path where ratty sub-threshold geometry actually enters the pipeline. A clean black SVG from the new color selector doesn't need the filter. Width-floor uses an erosion test (`island.buffer(-w/2)`) as a keep-or-drop predicate rather than a morphological opening — opening fractured islands at thin necks instead of pruning them. The main-pipeline-level skip-tiny-fragment idea remains a future option for hand-authored SVGs with deliberately small interior features.)**

## Verification

End-to-end manual test using the demo black SVG:

1. `python -m im_svg_maker demo_W_black.svg`
2. Confirm `out/demo_W_black/` contains all 5 files.
3. Open `laser.svg` in a browser / Inkscape — verify:
   - 150×150 mm outer rectangle.
   - Interior W-halo cut, centered, longest dim ≈ 100 mm.
   - Cut shape is visibly slightly larger than the plug inset (kerf scale).
4. Open `plug.3mf` in a slicer (PrusaSlicer / Bambu / Cura):
   - Inset layer (3 mm tall) is the gold halo shape.
   - Base layer (1 mm tall) sits below, wider than the inset, with the inner hole around the W slightly shrunken vs the inset's inner hole.
   - Mesh is manifold (slicer reports no errors).
5. `run.log` shows island count = 1, base offset = `base_offset_min_mm`.
6. Sanity-check on a synthetic multi-island SVG (two disconnected black circles, ~10 mm apart) — confirm base offset grows to bridge them into one polygon.

Physical verification (post-fab) is on the user — the program's job is geometric correctness and clean file output.
