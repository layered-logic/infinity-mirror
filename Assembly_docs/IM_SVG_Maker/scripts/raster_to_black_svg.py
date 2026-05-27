"""CLI wrapper around `im_svg_maker.raster.trace_raster_to_svg`.

Usage:
  python scripts/raster_to_black_svg.py fox.jpg
  python scripts/raster_to_black_svg.py fox.jpg --thickness-mm 1.0 --out fox_thin.svg

All knobs default to whatever is in variables.json (raster_thickness_mm,
raster_threshold, raster_turdsize, max_logo_dim_mm). CLI flags override.
"""
from __future__ import annotations

import argparse
import sys
from pathlib import Path

# Allow running as `python scripts/raster_to_black_svg.py ...` without an install.
sys.path.insert(0, str(Path(__file__).resolve().parent.parent))

from im_svg_maker.config import load_config
from im_svg_maker.raster import trace_raster_to_svg


def main(argv: list[str] | None = None) -> int:
    p = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    p.add_argument("image", type=Path, help="Input raster (JPG/PNG).")
    p.add_argument("--out", type=Path, default=None,
                   help="Output SVG path (default: <image stem>.svg next to input).")
    p.add_argument("--config", type=Path, default=Path("variables.json"),
                   help="Path to variables.json (default: ./variables.json).")
    p.add_argument("--thickness-mm", type=float, default=None,
                   help="Override raster_thickness_mm.")
    p.add_argument("--threshold", type=int, default=None,
                   help="Override raster_threshold (0-255).")
    p.add_argument("--turdsize", type=int, default=None,
                   help="Override raster_turdsize (speckle filter, pixels).")
    p.add_argument("--min-island-area-mm2", type=float, default=None,
                   help="Override raster_min_island_area_mm2 (drop islands smaller than this; 0=off).")
    p.add_argument("--min-feature-width-mm", type=float, default=None,
                   help="Override raster_min_feature_width_mm (prune thinner-than-this features; 0=off).")
    args = p.parse_args(argv)

    cfg = load_config(args.config)
    if args.thickness_mm is not None:
        cfg = cfg.model_copy(update={"raster_thickness_mm": args.thickness_mm})
    if args.threshold is not None:
        cfg = cfg.model_copy(update={"raster_threshold": args.threshold})
    if args.turdsize is not None:
        cfg = cfg.model_copy(update={"raster_turdsize": args.turdsize})
    if args.min_island_area_mm2 is not None:
        cfg = cfg.model_copy(update={"raster_min_island_area_mm2": args.min_island_area_mm2})
    if args.min_feature_width_mm is not None:
        cfg = cfg.model_copy(update={"raster_min_feature_width_mm": args.min_feature_width_mm})

    out = args.out or args.image.with_suffix(".svg")
    result = trace_raster_to_svg(args.image, out, cfg)
    dropped_bits: list[str] = []
    if result.n_dropped_thin > 0:
        dropped_bits.append(
            f"{result.n_dropped_thin} dropped <{cfg.raster_min_feature_width_mm}mm wide"
        )
    if result.n_dropped_small > 0:
        dropped_bits.append(
            f"{result.n_dropped_small} dropped <{cfg.raster_min_island_area_mm2}mm² area"
        )
    dropped_str = (" — " + ", ".join(dropped_bits)) if dropped_bits else ""
    print(
        f"Wrote {out} ({result.n_kept} islands, thickness={cfg.raster_thickness_mm}mm "
        f"@ {cfg.max_logo_dim_mm}mm target){dropped_str}"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
