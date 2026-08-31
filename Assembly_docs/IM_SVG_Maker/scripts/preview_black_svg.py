"""Quick raster preview of a black-fill SVG as the pipeline reads it.

Renders the parsed cut polygon (post max_logo_dim scaling) to a PNG so you can
eyeball islands/holes before committing to a full fab run.

  python scripts/preview_black_svg.py inputs/Empowder/empowder_black.svg
"""
from __future__ import annotations

import argparse
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent.parent))

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
import numpy as np
from matplotlib.patches import PathPatch
from matplotlib.path import Path as MPath

from im_svg_maker.config import load_config
from im_svg_maker.svg_parse import load_cut_polygon


def main(argv: list[str] | None = None) -> int:
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("svg", type=Path)
    ap.add_argument("--out", type=Path, default=None)
    ap.add_argument("--config", type=Path, default=Path("variables.json"))
    args = ap.parse_args(argv)

    cfg = load_config(args.config)
    geom = load_cut_polygon(args.svg, cfg.max_logo_dim_mm)

    fig, ax = plt.subplots(figsize=(4, 5))
    for poly in geom.geoms:
        verts: list = []
        codes: list = []
        for ring in [poly.exterior, *poly.interiors]:
            c = list(ring.coords)
            verts += c
            codes += [MPath.MOVETO] + [MPath.LINETO] * (len(c) - 2) + [MPath.CLOSEPOLY]
        ax.add_patch(PathPatch(MPath(np.array(verts), codes),
                               facecolor="black", edgecolor="red", lw=0.5))
    ax.autoscale()
    ax.set_aspect("equal")
    ax.axis("off")
    out = args.out or args.svg.with_name(args.svg.stem + "_preview.png")
    plt.savefig(out, dpi=110, bbox_inches="tight")
    print(f"Wrote {out} ({len(geom.geoms)} islands, bounds={tuple(round(b,2) for b in geom.bounds)})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
