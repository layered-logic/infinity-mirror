"""Convert a solid black-fill SVG into an outline (ribbon) black SVG.

For logos that are solid silhouettes (e.g. the empowder flower + mountain), this
turns each filled island into a constant-width ribbon that follows its contour:

    ribbon = island - erode(island, band_width)

The ribbon hugs the outer boundary and every interior hole, so a solid blob
becomes a readable outline instead of a large material cutout. Band width is
given directly in millimetres of the final logo (the input is normalized to the
pipeline's `max_logo_dim_mm` first, so the width is true-to-output).

Output is a flat black SVG (`fill-rule="evenodd"`) ready for `python -m
im_svg_maker`. Coordinates are emitted Y-down so the pipeline's own Y-flip
restores the original orientation.

  python scripts/outline_black_svg.py inputs/Empowder/empowder_black.svg --band-mm 4
"""
from __future__ import annotations

import argparse
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent.parent))

from shapely.geometry import MultiPolygon, Polygon

from im_svg_maker.config import load_config
from im_svg_maker.svg_parse import load_cut_polygon


def _ring_d(coords) -> str:
    # emit Y-down (negate) so svg_parse's flip restores math orientation
    pts = list(coords)
    return "M " + " L ".join(f"{x:.4f},{-y:.4f}" for x, y in pts) + " Z"


def outline(in_svg: Path, out_svg: Path, band_mm: float, cfg_path: Path) -> int:
    cfg = load_config(cfg_path)
    geom = load_cut_polygon(in_svg, cfg.max_logo_dim_mm)

    ribbons: list[Polygon] = []
    for island in geom.geoms:
        eroded = island.buffer(-band_mm, join_style="round")
        ribbon = island.difference(eroded)
        if ribbon.is_empty:
            continue
        if isinstance(ribbon, Polygon):
            ribbons.append(ribbon)
        else:
            ribbons.extend(p for p in ribbon.geoms if isinstance(p, Polygon) and p.area > 0)

    if not ribbons:
        raise SystemExit("Outline empty — band width likely too large for every island.")

    merged = MultiPolygon(ribbons)
    minx, miny, maxx, maxy = merged.bounds
    # bounds are in math-Y; emitted coords are -Y, so viewBox spans [-maxy, -miny]
    vb = f"{minx:.4f} {-maxy:.4f} {maxx - minx:.4f} {maxy - miny:.4f}"
    parts = [f'<svg xmlns="http://www.w3.org/2000/svg" viewBox="{vb}">\n']
    for poly in merged.geoms:
        d = _ring_d(poly.exterior.coords)
        for hole in poly.interiors:
            d += " " + _ring_d(hole.coords)
        parts.append(f'  <path d="{d}" fill="#000000" fill-rule="evenodd"/>\n')
    parts.append("</svg>\n")
    out_svg.write_text("".join(parts), encoding="utf-8")
    return len(merged.geoms)


def main(argv: list[str] | None = None) -> int:
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("svg", type=Path, help="Input solid black-fill SVG.")
    ap.add_argument("--out", type=Path, default=None,
                    help="Output ribbon SVG (default: <stem>_outline.svg).")
    ap.add_argument("--band-mm", type=float, default=4.0,
                    help="Ribbon width in mm of the final logo (default 4).")
    ap.add_argument("--config", type=Path, default=Path("variables.json"))
    args = ap.parse_args(argv)

    out = args.out or args.svg.with_name(args.svg.stem + "_outline.svg")
    n = outline(args.svg, out, args.band_mm, args.config)
    print(f"Wrote {out} ({n} ribbon islands, band={args.band_mm}mm)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
