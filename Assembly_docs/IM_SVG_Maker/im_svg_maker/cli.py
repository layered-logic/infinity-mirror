"""CLI: black SVG -> laser.svg + laser.dxf + plug.3mf + plug.stl + run.log."""
from __future__ import annotations

import argparse
import sys
from pathlib import Path

from .config import load_config
from .extrude import build_plug, export_plug
from .geometry import build_geometries
from .laser_export import write_dxf, write_svg
from .svg_parse import load_cut_polygon


def _format_log(svg: Path, cfg, geo) -> str:
    minx, miny, maxx, maxy = geo.cut.bounds
    return (
        f"Input SVG: {svg}\n"
        f"Mirror size: {cfg.mirror_size_mm[0]} x {cfg.mirror_size_mm[1]} mm\n"
        f"Logo bounds (mm): x=[{minx:.2f}, {maxx:.2f}]  y=[{miny:.2f}, {maxy:.2f}]\n"
        f"Logo size (mm): {maxx - minx:.2f} x {maxy - miny:.2f}\n"
        f"Nozzle diameter: {cfg.nozzle_diameter_mm} mm (rounding radius {cfg.nozzle_radius_mm} mm)\n"
        f"Laser kerf offset: {cfg.laser_kerf_mm} mm (uniform outward)\n"
        f"Inset clearance: {cfg.inset_clearance_mm} mm\n"
        f"Heights: inset={cfg.inset_height_mm} mm, base={cfg.base_height_mm} mm\n"
        f"Island count: {geo.island_count}\n"
        f"Auto-bridge gap: {geo.auto_bridge_gap_mm if geo.auto_bridge_gap_mm is not None else 'n/a (single island)'}\n"
        f"Base offset chosen: {geo.base_offset_mm:.3f} mm\n"
    )


def run(svg_path: Path, config_path: Path, out_root: Path) -> int:
    cfg = load_config(config_path)
    cut_raw = load_cut_polygon(svg_path, cfg.max_logo_dim_mm)
    geo = build_geometries(cut_raw, cfg)

    out_dir = out_root / svg_path.stem
    out_dir.mkdir(parents=True, exist_ok=True)

    write_svg(geo.laser, cfg, out_dir / "laser.svg")
    write_dxf(geo.laser, cfg, out_dir / "laser.dxf")

    mesh = build_plug(geo.inset, geo.base, cfg)
    threemf, stl = export_plug(mesh, out_dir)

    log_text = _format_log(svg_path, cfg, geo)
    (out_dir / "run.log").write_text(log_text, encoding="utf-8")

    print(log_text)
    print(f"Wrote: {out_dir / 'laser.svg'}")
    print(f"Wrote: {out_dir / 'laser.dxf'}")
    print(f"Wrote: {threemf}")
    print(f"Wrote: {stl}")
    print(f"Wrote: {out_dir / 'run.log'}")
    return 0


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(
        prog="im_svg_maker",
        description="Generate infinity-mirror fab files from a black SVG.",
    )
    parser.add_argument("svg", type=Path, help="Path to a monochrome (black) SVG.")
    parser.add_argument(
        "--config", type=Path, default=Path("variables.json"),
        help="Path to variables.json (default: ./variables.json)",
    )
    parser.add_argument(
        "--out", type=Path, default=Path("out"),
        help="Output root directory (default: ./out)",
    )
    args = parser.parse_args(argv)

    if not args.svg.exists():
        parser.error(f"SVG not found: {args.svg}")
    if not args.config.exists():
        parser.error(f"Config not found: {args.config}")

    return run(args.svg, args.config, args.out)


if __name__ == "__main__":
    sys.exit(main())
