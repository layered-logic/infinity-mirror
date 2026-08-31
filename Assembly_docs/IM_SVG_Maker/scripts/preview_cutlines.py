"""Preview the cut-line geometry derived from a black SVG.

Step 1 view: mirror outline + filled black W (the region to be cut from the
mirror) + red kerf band on top (uniform width = the material the laser removes).
"""

from pathlib import Path
import matplotlib.pyplot as plt
from matplotlib.patches import Rectangle, PathPatch
from matplotlib.path import Path as MplPath
import numpy as np
from svgelements import SVG, Path as SvgPath
from shapely.geometry import Polygon, MultiPolygon
from shapely.affinity import scale, translate

# --- config (mirrors the plan's variables.json proposal) -------------------
MIRROR_SIZE_MM = (150, 150)
MAX_LOGO_DIM_MM = 100.0
NOZZLE_DIAMETER_MM = 0.6           # printer nozzle (tunable)
NOZZLE_RADIUS_MM = NOZZLE_DIAMETER_MM / 2  # min printable feature radius
LASER_KERF_MM = 0.5                # uniform outward offset (exaggerated for visibility)
INSET_CLEARANCE_MM = 0.15
BASE_OFFSET_MIN_MM = 2.0

SVG_PATH = Path(__file__).parent / "demo_W_black.svg"
OUT_PNG = Path(__file__).parent / "cut_lines_preview.png"


def svg_to_polygons(svg_path: Path) -> MultiPolygon:
    svg = SVG.parse(str(svg_path), reify=True)
    polys: list[Polygon] = []
    for element in svg.elements():
        if not isinstance(element, SvgPath):
            continue
        # Sample each subpath into a polyline
        for subpath in element.as_subpaths():
            seg_path = SvgPath(subpath)
            ts = np.linspace(0.0, 1.0, 400)
            raw_pts = seg_path.npoint(ts)
            pts = [(float(p[0]), float(p[1])) for p in raw_pts]
            # Remove degenerate consecutive duplicates
            cleaned = [pts[0]]
            for p in pts[1:]:
                if p != cleaned[-1]:
                    cleaned.append(p)
            if len(cleaned) >= 3:
                try:
                    poly = Polygon(cleaned)
                    if poly.is_valid and poly.area > 0:
                        polys.append(poly)
                except Exception:
                    pass
    # SVG even-odd: XOR overlapping polygons so inner subpaths become holes
    result = polys[0]
    for p in polys[1:]:
        result = result.symmetric_difference(p)
    if isinstance(result, Polygon):
        return MultiPolygon([result])
    return result


def normalize(geom):
    """Flip Y, center at origin, scale so max dimension equals MAX_LOGO_DIM_MM."""
    minx, miny, maxx, maxy = geom.bounds
    geom = scale(geom, xfact=1.0, yfact=-1.0, origin=(0, 0))
    minx, miny, maxx, maxy = geom.bounds
    cx, cy = (minx + maxx) / 2, (miny + maxy) / 2
    geom = translate(geom, xoff=-cx, yoff=-cy)
    minx, miny, maxx, maxy = geom.bounds
    longest = max(maxx - minx, maxy - miny)
    factor = MAX_LOGO_DIM_MM / longest
    geom = scale(geom, xfact=factor, yfact=factor, origin=(0, 0))
    return geom


def plot_outline(ax, geom, **kwargs):
    if isinstance(geom, Polygon):
        polys = [geom]
    else:
        polys = list(geom.geoms)
    for poly in polys:
        xs, ys = poly.exterior.xy
        ax.plot(xs, ys, **kwargs)
        for hole in poly.interiors:
            xs, ys = hole.xy
            ax.plot(xs, ys, **kwargs)


def _ring_to_path(ring):
    coords = list(ring.coords)
    verts = coords
    codes = [MplPath.MOVETO] + [MplPath.LINETO] * (len(coords) - 2) + [MplPath.CLOSEPOLY]
    return verts, codes


def fill_geom(ax, geom, **kwargs):
    """Fill a (Multi)Polygon respecting holes via a single compound matplotlib Path."""
    if geom.is_empty:
        return
    if isinstance(geom, Polygon):
        polys = [geom]
    else:
        polys = list(geom.geoms)
    all_verts, all_codes = [], []
    for poly in polys:
        v, c = _ring_to_path(poly.exterior)
        all_verts.extend(v)
        all_codes.extend(c)
        for hole in poly.interiors:
            v, c = _ring_to_path(hole)
            all_verts.extend(v)
            all_codes.extend(c)
    patch = PathPatch(MplPath(all_verts, all_codes), **kwargs)
    ax.add_patch(patch)


def round_to_nozzle(geom, r):
    """Round both inside and outside corners to radius r (printable by nozzle).

    Opening (rounds outside corners, removes thin protrusions),
    then closing (rounds inside corners, fills thin gaps).
    """
    opened = geom.buffer(-r, join_style="round").buffer(+r, join_style="round")
    closed = opened.buffer(+r, join_style="round").buffer(-r, join_style="round")
    return closed


def main():
    raw = svg_to_polygons(SVG_PATH)
    cut_raw = normalize(raw)

    # Apply nozzle rounding to the cut polygon so no feature is sharper than
    # what the printer can reproduce.
    cut = round_to_nozzle(cut_raw, NOZZLE_RADIUS_MM)
    laser = cut.buffer(LASER_KERF_MM, join_style="round")
    kerf_band = laser.difference(cut)  # the material the laser removes

    # Plug inset: drops into the laser hole, shrunk by clearance for slide-in fit.
    inset = cut.buffer(-INSET_CLEARANCE_MM, join_style="round")
    clearance_band = cut.difference(inset)  # the air gap between plug wall and cut

    # Plug base: sits behind the mirror, expanded outward to overlap mirror fragments.
    # For a single-island shape, offset = base_offset_min_mm.
    base_offset = BASE_OFFSET_MIN_MM
    base = cut.buffer(base_offset, join_style="round")
    # Where the base overlaps mirror material (the support region, visible only
    # from behind in real life — it's the "kintsugi glue").
    base_support = base.difference(cut)

    fig, ax = plt.subplots(figsize=(9, 9))

    mw, mh = MIRROR_SIZE_MM
    ax.add_patch(Rectangle((-mw / 2, -mh / 2), mw, mh,
                           fill=False, edgecolor="black", linewidth=1.0,
                           label=f"mirror {mw}x{mh}mm"))

    # Draw base support FIRST (it sits behind everything else)
    fill_geom(ax, base_support, facecolor="#2ca02c", edgecolor="none", alpha=0.45,
              label=f"plug base support ({base_offset:.1f}mm overlap onto mirror)")
    fill_geom(ax, cut, facecolor="black", edgecolor="none",
              label="cut region (nozzle-rounded)")
    fill_geom(ax, kerf_band, facecolor="red", edgecolor="none",
              label=f"laser kerf band ({LASER_KERF_MM:.2f}mm wide)")
    fill_geom(ax, inset, facecolor="#3a8fff", edgecolor="none",
              label=f"plug inset (cut shrunk by {INSET_CLEARANCE_MM:.2f}mm clearance)")

    ax.set_aspect("equal")
    ax.set_xlim(-mw / 2 - 10, mw / 2 + 10)
    ax.set_ylim(-mh / 2 - 10, mh / 2 + 10)
    ax.set_xlabel("mm")
    ax.set_ylabel("mm")
    ax.set_title(f"Step 3: + plug base support (green) — overlaps mirror behind the plug\n"
                 f"nozzle={NOZZLE_DIAMETER_MM}mm  kerf={LASER_KERF_MM}mm  "
                 f"clearance={INSET_CLEARANCE_MM}mm  base_offset={base_offset}mm")
    ax.grid(True, alpha=0.3)

    handles, labels = ax.get_legend_handles_labels()
    seen = {}
    for h, l in zip(handles, labels):
        seen.setdefault(l, h)
    ax.legend(seen.values(), seen.keys(), loc="lower right", fontsize=9)

    plt.tight_layout()
    plt.savefig(OUT_PNG, dpi=150)
    print(f"Wrote {OUT_PNG}")


if __name__ == "__main__":
    main()
