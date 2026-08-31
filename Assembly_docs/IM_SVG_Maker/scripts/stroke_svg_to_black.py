"""Stroke-based SVG -> black-filled SVG for the IM_SVG_Maker pipeline.

The main pipeline wants solid black fills. Logos drawn as strokes
(`fill="none"` + `stroke-width`, e.g. the Layered Logic nested-L mark) carry no
fill, so the color/raster preprocessors skip them. This converts stroked
geometry into filled ribbons by buffering each stroked segment outward by
`stroke-width / 2` with round caps and joins (matching `stroke-linecap="round"`),
then emits one black `<path>` per connected band.

Scope: `<line>`, `<polyline>`, and `<path>` (via svgelements sampling). Stroke
width is read from the element, else inherited from an ancestor `<g>`, else
`--default-width`. Coordinates are passed through untouched in the SVG's own
user units, so the main pipeline's `max_logo_dim_mm` scaling still applies.

Usage:
  python scripts/stroke_svg_to_black.py logo-primary-mono.svg
  python scripts/stroke_svg_to_black.py logo.svg --out ll_black.svg --default-width 7
"""
from __future__ import annotations

import argparse
import sys
import xml.etree.ElementTree as ET
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent.parent))

from shapely.geometry import LineString, MultiPolygon, Polygon
from shapely.ops import unary_union
from svgelements import Path as SvgPath

_SVG_NS = "{http://www.w3.org/2000/svg}"


def _strip_ns(tag: str) -> str:
    return tag.split("}", 1)[1] if "}" in tag else tag


def _stroke_width(el: ET.Element, inherited: float) -> float:
    sw = el.get("stroke-width")
    if sw is None:
        # also accept it inside a `style="..."` blob
        style = el.get("style", "")
        for chunk in style.split(";"):
            if chunk.strip().startswith("stroke-width"):
                sw = chunk.split(":", 1)[1].strip()
                break
    if sw is None:
        return inherited
    try:
        return float(sw.replace("px", "").strip())
    except ValueError:
        return inherited


def _band(coords: list[tuple[float, float]], width: float) -> Polygon | None:
    if len(coords) < 2 or width <= 0:
        return None
    line = LineString(coords)
    if line.length == 0:
        return None
    return line.buffer(width / 2.0, cap_style="round", join_style="round")


def _sample_path(d: str, n: int = 200) -> list[list[tuple[float, float]]]:
    """Sample an SVG path into one polyline per subpath."""
    p = SvgPath(d)
    subpaths: list[list[tuple[float, float]]] = []
    for sub in p.as_subpaths():
        sp = SvgPath(*list(sub))
        length = sp.length()
        if length == 0:
            continue
        steps = max(2, min(n, int(length)))
        pts = []
        for i in range(steps + 1):
            pt = sp.point(i / steps)
            pts.append((pt.x, pt.y))
        subpaths.append(pts)
    return subpaths


def _walk(el: ET.Element, inherited_width: float, bands: list[Polygon]) -> None:
    tag = _strip_ns(el.tag)
    width = _stroke_width(el, inherited_width)

    if tag == "line":
        coords = [
            (float(el.get("x1", 0)), float(el.get("y1", 0))),
            (float(el.get("x2", 0)), float(el.get("y2", 0))),
        ]
        b = _band(coords, width)
        if b is not None:
            bands.append(b)
    elif tag == "polyline":
        raw = el.get("points", "").replace(",", " ").split()
        nums = [float(v) for v in raw]
        coords = list(zip(nums[0::2], nums[1::2]))
        b = _band(coords, width)
        if b is not None:
            bands.append(b)
    elif tag == "path":
        for coords in _sample_path(el.get("d", "")):
            b = _band(coords, width)
            if b is not None:
                bands.append(b)

    for child in el:
        _walk(child, width, bands)


def convert(svg_in: Path, svg_out: Path, default_width: float) -> int:
    tree = ET.parse(svg_in)
    root = tree.getroot()

    bands: list[Polygon] = []
    _walk(root, default_width, bands)
    if not bands:
        raise SystemExit("No stroked geometry found (line/polyline/path with stroke-width).")

    merged = unary_union(bands)
    if isinstance(merged, Polygon):
        merged = MultiPolygon([merged])

    minx, miny, maxx, maxy = merged.bounds
    parts = [
        f'<svg xmlns="http://www.w3.org/2000/svg" '
        f'viewBox="{minx} {miny} {maxx - minx} {maxy - miny}">\n'
    ]
    for poly in merged.geoms:
        d = "M " + " L ".join(f"{x:.4f},{y:.4f}" for x, y in poly.exterior.coords) + " Z"
        for hole in poly.interiors:
            d += " M " + " L ".join(f"{x:.4f},{y:.4f}" for x, y in hole.coords) + " Z"
        parts.append(f'  <path d="{d}" fill="#000000" fill-rule="evenodd"/>\n')
    parts.append("</svg>\n")
    svg_out.write_text("".join(parts), encoding="utf-8")
    return len(merged.geoms)


def main(argv: list[str] | None = None) -> int:
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("svg", type=Path, help="Input stroked SVG.")
    ap.add_argument("--out", type=Path, default=None,
                    help="Output black SVG (default: <stem>_black.svg next to input).")
    ap.add_argument("--default-width", type=float, default=1.0,
                    help="Stroke width to assume when an element/group declares none.")
    args = ap.parse_args(argv)

    out = args.out or args.svg.with_name(args.svg.stem + "_black.svg")
    n = convert(args.svg, out, args.default_width)
    print(f"Wrote {out} ({n} islands)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
