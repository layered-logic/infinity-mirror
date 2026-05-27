# IM_SVG_Maker

Generate the fabrication files for a Kintsugi-style infinity mirror from a single black SVG (or a raster line-art image).

A black SVG of a logo or drawing goes in. Four files come out, ready to fabricate:

- `laser.svg` + `laser.dxf` — vector cut paths for a 150 × 150 mm acrylic mirror tile. Hairline-red strokes, mirror outline + interior cutout.
- `plug.3mf` + `plug.stl` — 3D-printable clear-PETG plug. Two layers: a 3 mm **inset** that drops into the laser-cut hole flush with the mirror's front face, and a 1 mm **base** behind the mirror that overlaps each mirror fragment to hold it in place (the "kintsugi glue").

For the design rationale, geometry math, and every decision made along the way, see [PLAN.md](PLAN.md).

## Quick start

Requires Python 3.11+.

```bash
pip install -e .            # editable install from pyproject.toml
python -m im_svg_maker demo_W_black.svg
```

Outputs land in `out/<svg_stem>/` alongside a `run.log` that records island count, chosen base offset, and the active config.

### From a raster image (JPG/PNG line art)

```bash
python scripts/raster_to_black_svg.py fox.jpg       # writes fox.svg
python -m im_svg_maker fox.svg                       # runs the main pipeline
```

The raster preprocessor binarizes, dilates the line work, and traces with potrace. All knobs are pulled from `variables.json` (`raster_thickness_mm`, `raster_threshold`, `raster_turdsize`, plus the size-floor pair below) and overridable with CLI flags. See [scripts/raster_to_black_svg.py](scripts/raster_to_black_svg.py) for options.

Two optional post-trace size floors (default 0 = disabled):

- `--min-island-area-mm2 N` — drop traced islands smaller than N mm². Catches dust/specks that survive the pixel-units `turdsize` filter, in resolution-independent mm units.
- `--min-feature-width-mm N` — drop islands whose narrowest cross-section is below N mm. Implemented as an erosion test (`island.buffer(-N/2)`); if the erosion empties the island, it gets dropped. Keep-or-drop semantics — does not reshape surviving islands or fracture them at thin necks.

### From a colored / multi-fill SVG

If your art is multi-color, pick which fill is the cut region before running the main pipeline:

```bash
python scripts/color_svg_to_black.py Washington_Huskies_logo.svg
# -> lists distinct fills with element count + summed bbox area

python scripts/color_svg_to_black.py Washington_Huskies_logo.svg --color "#E8D3A2" -o gold.svg
python -m im_svg_maker gold.svg                                          # main pipeline
```

Colors accept hex (`#RGB`, `#RRGGBB`), named CSS colors (`gold`, `red`), or `rgb(r,g,b)`. `--color` is repeatable for a union of multiple fills. `--tolerance N` widens the match by N units of L-infinity distance over r/g/b (0-255 units; default 0 = exact). `--invert` keeps everything that *doesn't* match — useful when the simplest description is "drop the background." Output is a flat black SVG with one `<path fill="#000000" fill-rule="evenodd"/>` per kept element, ready for `python -m im_svg_maker`.

## Configuration

Everything is in [variables.json](variables.json). The most useful knobs:

| Variable | Default | Purpose |
| --- | --- | --- |
| `mirror_size_mm` | `[150, 150]` | Acrylic sheet dimensions. Logo is centered on this. |
| `max_logo_dim_mm` | `100` | Longest side of the logo, in mm. Both axes scale proportionally. |
| `nozzle_diameter_mm` | `0.6` | 3D printer nozzle. All cut polygon corners are pre-rounded to `nozzle/2` so nothing is sharper than the printer can produce. |
| `laser_kerf_mm` | `0.1` | Uniform outward offset of the laser cut. Compensates for material the laser vaporises. |
| `inset_clearance_mm` | `0.15` | Plug inset is shrunk by this for slide-in fit. |
| `inset_height_mm` / `base_height_mm` | `3.0` / `1.0` | Layer thicknesses. Inset = acrylic thickness, base = arbitrary support. |
| `base_offset_min_mm` | `2.0` | Minimum overlap between the base and each mirror fragment. |
| `base_offset_per_island_mm` | `0.5` | Extra offset per island beyond the first (small bonus; the bridge term does the heavy lifting). |
| `base_bridge_factor` | `1.0` | 0–1 scale on the auto-computed bridge requirement (`gap/2`). |
| `raster_thickness_mm` | `0.75` | Preprocessor only: dilate raster lines by this before tracing. |
| `raster_min_island_area_mm2` | `0` | Preprocessor only: drop traced islands below this area floor (0 = off). |
| `raster_min_feature_width_mm` | `0` | Preprocessor only: drop islands whose narrowest cross-section is below this (0 = off). |

Base offset formula: `max(base_offset_min_mm + (N-1)*base_offset_per_island_mm, base_bridge_factor * nearest_island_gap_mm / 2)`. Logged at run time.

## Project layout

```
IM_SVG_Maker/
  im_svg_maker/             # main package
    cli.py                  # entrypoint (python -m im_svg_maker)
    config.py               # pydantic loader for variables.json
    svg_parse.py            # SVG -> normalized shapely MultiPolygon
    geometry.py             # nozzle rounding, kerf/inset/base offsets, island formula
    extrude.py              # shapely -> trimesh, 3MF/STL export
    laser_export.py         # SVG + DXF writers (red hairline laser convention)
    raster.py               # raster -> black SVG via potracer + shell/hole assembly + size floors
    color_select.py         # colored SVG -> black SVG by fill-color selection
  scripts/
    raster_to_black_svg.py  # CLI wrapper around im_svg_maker.raster
    color_svg_to_black.py   # CLI wrapper around im_svg_maker.color_select
    preview_cutlines.py     # filled-band 2D preview (cut/laser/inset/base layers)
    preview_zoom.py         # zoomed kerf-band view for visual debugging
    diag.py                 # polygon topology / area / winding diagnostic
  Washington_Huskies_logo.svg  # original colored reference (not consumed)
  demo_W_black.svg          # the W example used throughout planning
  fox.jpg                   # raster line-art example
  variables.json            # all tunable knobs
  pyproject.toml
  out/                      # generated per-input subdirs
  PLAN.md                   # full design doc & decision log
```

## Verifying a run

Open `out/<svg_stem>/`:

- `laser.svg` — view in a browser or Inkscape. Should show the 150 × 150 mm outer rectangle and the interior cut, both as red hairlines.
- `plug.3mf` / `plug.stl` — open in a slicer (PrusaSlicer, Bambu, Cura). Confirm: total height = `base_height_mm + inset_height_mm`, base layer wider than inset, mesh reports as manifold/watertight.
- `run.log` — quick-read summary of params, island count, computed base offset.

For visual debugging of the 2D geometry, the `scripts/preview_*.py` tools render filled-band overlays on the mirror sheet — handy for sanity-checking new SVGs before printing.

## Notes for next hands

- **Color-SVG → black-SVG** is now handled by `scripts/color_svg_to_black.py` (a list-fills + pick-one(or-more) flow). It's intentionally minimal: solid fills only, no gradient/pattern handling, overlap-handling is whatever the main pipeline's evenodd XOR does to nested subpaths. The fox preprocessor still handles raster line-art.
- **Tracing topology**: potracer returns evenodd subpaths. `raster.py` rebuilds proper shell+hole nesting by sorting polygons by area and counting containment depth — required so the main pipeline's XOR-based SVG parser reads holes correctly.
- **Round joins everywhere**: every `shapely.buffer()` in this codebase uses `join_style="round"`. Mitre joins extend wildly at sharp inner corners and aren't physically meaningful for either laser kerf or printable curvature. Don't switch.
- **STRtree gotcha**: shapely's STRtree.nearest can return `numpy.int64` and may include self in results. `_nearest_island_gap` does a straight O(N²) pairwise instead. Fine for typical island counts (<50).
- See **Learnings** section of [PLAN.md](PLAN.md) for the full list of things we figured out while building this.
