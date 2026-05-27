"""Parse a monochrome SVG into a normalized shapely MultiPolygon.

Every <path> element is treated as part of the cut region (fill color is ignored).
Subpaths are sampled into polylines, combined via symmetric_difference so that
nested subpaths become holes (matches SVG even-odd fill rule).

The result is normalized: Y-flipped, centered at origin, scaled so the longest
axis equals `max_logo_dim_mm`.
"""
from __future__ import annotations

from pathlib import Path

import numpy as np
from shapely.affinity import scale as shp_scale, translate as shp_translate
from shapely.geometry import MultiPolygon, Polygon
from svgelements import SVG
from svgelements import Path as SvgPath

# Number of samples used to discretize each subpath. 400 is enough for the
# W demo; complex curves may want more.
SAMPLES_PER_SUBPATH = 400


def _subpath_to_polygon(seg_path: SvgPath) -> Polygon | None:
    ts = np.linspace(0.0, 1.0, SAMPLES_PER_SUBPATH)
    raw = seg_path.npoint(ts)
    pts = [(float(p[0]), float(p[1])) for p in raw]
    cleaned: list[tuple[float, float]] = [pts[0]]
    for p in pts[1:]:
        if p != cleaned[-1]:
            cleaned.append(p)
    if len(cleaned) < 3:
        return None
    poly = Polygon(cleaned)
    if not poly.is_valid:
        poly = poly.buffer(0)
        if not isinstance(poly, Polygon) or not poly.is_valid or poly.area == 0:
            return None
    if poly.area == 0:
        return None
    return poly


def _svg_to_multipolygon(svg_path: Path) -> MultiPolygon:
    svg = SVG.parse(str(svg_path), reify=True)
    polys: list[Polygon] = []
    for element in svg.elements():
        if not isinstance(element, SvgPath):
            continue
        for subpath in element.as_subpaths():
            poly = _subpath_to_polygon(SvgPath(subpath))
            if poly is not None:
                polys.append(poly)
    if not polys:
        raise ValueError(f"No usable <path> geometry found in {svg_path}")

    # Even-odd fill: XOR overlapping polygons so contained subpaths become holes.
    result = polys[0]
    for p in polys[1:]:
        result = result.symmetric_difference(p)

    if isinstance(result, Polygon):
        return MultiPolygon([result])
    return result


def _normalize(geom: MultiPolygon, max_logo_dim_mm: float) -> MultiPolygon:
    """Flip Y, center at origin, scale so longest axis == max_logo_dim_mm."""
    # SVG Y points down; flip to standard math orientation.
    flipped = shp_scale(geom, xfact=1.0, yfact=-1.0, origin=(0, 0))
    minx, miny, maxx, maxy = flipped.bounds
    cx, cy = (minx + maxx) / 2, (miny + maxy) / 2
    centered = shp_translate(flipped, xoff=-cx, yoff=-cy)
    minx, miny, maxx, maxy = centered.bounds
    longest = max(maxx - minx, maxy - miny)
    if longest == 0:
        raise ValueError("Logo has zero extent after parsing")
    factor = max_logo_dim_mm / longest
    scaled = shp_scale(centered, xfact=factor, yfact=factor, origin=(0, 0))
    if isinstance(scaled, Polygon):
        return MultiPolygon([scaled])
    return scaled


def load_cut_polygon(svg_path: Path, max_logo_dim_mm: float) -> MultiPolygon:
    """Top-level entry: SVG file -> normalized cut MultiPolygon (mm)."""
    return _normalize(_svg_to_multipolygon(svg_path), max_logo_dim_mm)
