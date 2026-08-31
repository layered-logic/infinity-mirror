"""Layered Logic mark -> black-fill SVG with frozen histogram flourish.

The animated P5 mark grows mouse-reactive histogram bars perpendicular off the L
arms (Gaussian envelope, stair-step quantized to `barSteps` levels). A laser cut
is static, so this freezes a pose: it reads the two L's from the canonical
stroked SVG, builds the arm bands, then adds a band of `bins` histogram ticks
along each *vertical* arm (the bars "grow off the sides of the vertical lines"),
each tick a line of the same stroke weight as its L. Everything is unioned into
one black-fill SVG ready for `python -m im_svg_maker`.

Bar profile matches the documented sketch: g = exp(-0.5*(u/sigma)^2) over the
window, quantized to `--steps` discrete levels, length scaled to `--max-bar`.

  python scripts/ll_flourish.py inputs/LayeredLogic/logo-primary-mono.svg \
      --out inputs/LayeredLogic/ll_flourish.svg --max-bar 60
"""
from __future__ import annotations

import argparse
import math
import sys
import xml.etree.ElementTree as ET
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent.parent))

from shapely.geometry import LineString, MultiPolygon, Polygon
from shapely.ops import unary_union


def _strip_ns(tag: str) -> str:
    return tag.split("}", 1)[1] if "}" in tag else tag


def _read_Ls(svg: Path) -> list[dict]:
    """Each stroked <g> -> one L: {width, vert:(p0,p1), horz:(p0,p1)} (p1 = shared corner)."""
    root = ET.parse(svg).getroot()
    Ls: list[dict] = []
    for g in root.iter():
        if _strip_ns(g.tag) != "g":
            continue
        sw = g.get("stroke-width")
        lines = [e for e in g if _strip_ns(e.tag) == "line"]
        if sw is None or len(lines) != 2:
            continue
        segs = [((float(l.get("x1")), float(l.get("y1"))),
                 (float(l.get("x2")), float(l.get("y2")))) for l in lines]
        # vertical arm = the one with the larger |dy|
        segs.sort(key=lambda s: abs(s[0][1] - s[1][1]), reverse=True)
        vert, horz = segs[0], segs[1]
        # orient vert so p1 is the corner (the endpoint shared with horz)
        hset = {horz[0], horz[1]}
        if vert[0] in hset:
            vert = (vert[1], vert[0])
        Ls.append({"width": float(sw), "vert": vert, "horz": horz})
    return Ls


def _hist_bars(p_top, p_corner, width, side, *,
               bins, steps, sigma, window_frac, center_frac, max_bar) -> list[Polygon]:
    """Histogram ticks perpendicular to the vertical arm p_top->p_corner."""
    (x0, y0), (x1, y1) = p_top, p_corner
    dx, dy = x1 - x0, y1 - y0
    L = math.hypot(dx, dy)
    if L == 0:
        return []
    ux, uy = dx / L, dy / L            # unit along arm (top -> corner)
    nx, ny = -uy * side, ux * side      # unit perpendicular, chosen side (+1/-1)

    half = (window_frac * L) / 2.0
    center_s = center_frac * L
    bars: list[Polygon] = []
    for i in range(bins):
        # position along arm within the window, bin-centered
        frac = (i + 0.5) / bins
        s = center_s + (frac * 2 - 1) * half
        if s < 0 or s > L:
            continue
        u = (frac * 2 - 1)             # -1..1 across the window
        g = math.exp(-0.5 * (u / sigma) ** 2)
        level = math.ceil(g * steps) / steps if g > 0 else 0.0
        bar_len = width / 2.0 + level * max_bar
        if bar_len <= width / 2.0:
            continue
        bx, by = x0 + ux * s, y0 + uy * s
        tip = (bx + nx * bar_len, by + ny * bar_len)
        bars.append(LineString([(bx, by), tip]).buffer(
            width / 2.0, cap_style="round", join_style="round"))
    return bars


def build(svg_in: Path, svg_out: Path, *, bins, steps, sigma,
          window_frac, center_fracs, max_bar, mode) -> int:
    Ls = _read_Ls(svg_in)
    if not Ls:
        raise SystemExit("No stroked L groups found in input SVG.")

    polys: list[Polygon] = []
    # arm bands
    for Ldef in Ls:
        w = Ldef["width"]
        for seg in (Ldef["vert"], Ldef["horz"]):
            polys.append(LineString(seg).buffer(w / 2.0, cap_style="round", join_style="round"))

    # flourish: bars on each L's vertical arm. mode picks which side each L grows.
    # outer L = Ls[0] (declared first in the file), inner L = Ls[1].
    for idx, Ldef in enumerate(Ls):
        if mode == "outward":
            side = -1 if idx == 0 else +1
        elif mode == "same":
            side = +1
        elif mode == "inward":
            side = +1 if idx == 0 else -1
        else:
            side = +1
        center_frac = center_fracs[idx] if idx < len(center_fracs) else center_fracs[-1]
        polys += _hist_bars(
            Ldef["vert"][0], Ldef["vert"][1], Ldef["width"], side,
            bins=bins, steps=steps, sigma=sigma,
            window_frac=window_frac, center_frac=center_frac, max_bar=max_bar)

    merged = unary_union(polys)
    if isinstance(merged, Polygon):
        merged = MultiPolygon([merged])

    minx, miny, maxx, maxy = merged.bounds
    parts = [f'<svg xmlns="http://www.w3.org/2000/svg" '
             f'viewBox="{minx} {miny} {maxx - minx} {maxy - miny}">\n']
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
    ap.add_argument("svg", type=Path, help="Canonical stroked LL SVG.")
    ap.add_argument("--out", type=Path, default=None)
    ap.add_argument("--bins", type=int, default=15)
    ap.add_argument("--steps", type=int, default=5)
    ap.add_argument("--sigma", type=float, default=0.35)
    ap.add_argument("--window-frac", type=float, default=0.5,
                    help="Histogram window length as a fraction of the arm (default 0.5).")
    ap.add_argument("--center-frac", type=float, nargs="+", default=[0.5],
                    help="Window center along the arm per L, 0=top 1=corner. "
                         "One value applies to all; two values = [outer, inner]. (default 0.5)")
    ap.add_argument("--max-bar", type=float, default=60.0,
                    help="Max bar length in SVG units (arm length ~320).")
    ap.add_argument("--mode", choices=["outward", "same", "inward"], default="outward",
                    help="Which side each L's bars grow (default outward).")
    args = ap.parse_args(argv)

    out = args.out or args.svg.with_name(args.svg.stem + "_flourish.svg")
    n = build(args.svg, out, bins=args.bins, steps=args.steps, sigma=args.sigma,
              window_frac=args.window_frac, center_fracs=args.center_frac,
              max_bar=args.max_bar, mode=args.mode)
    print(f"Wrote {out} ({n} islands, max_bar={args.max_bar}, mode={args.mode})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
