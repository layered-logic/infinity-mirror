"""Nest four 150x150 tile cut files into one 300x300 cutting sheet.

Each IM_SVG_Maker `laser.dxf` is a single mirror tile whose geometry is centered
on the origin (the 150x150 outer square spans -75..+75 in both axes, with the
interior laser cuts inside it). This wrapper places four such tiles into a 2x2
grid on the real stock sheet, separated by a small gap, and writes a combined
`sheet.dxf` (+ `sheet.svg` for a visual check). No outer sheet boundary is cut.

Tile -> quadrant mapping (in argument order):
    1: top-left      2: top-right
    3: bottom-left   4: bottom-right

Each tile lands on its own DXF layer (tile_1..tile_4) so the laser software can
isolate them. Footprint = 150*2 + gap in each axis; a warning prints if that
exceeds --sheet (the stock you're actually cutting).

  python scripts/tile_sheet.py out/fox/laser.dxf out/demo_W_black/laser.dxf \
      out/ll_black/laser.dxf out/empowder_outline_3mm/laser.dxf \
      --gap 0.5 --out out/sheet
"""
from __future__ import annotations

import argparse
from pathlib import Path

import ezdxf

TILE_MM = 150.0  # each tile's outer square edge

STROKE_COLOR = "#FF0000"
STROKE_WIDTH_MM = 0.01


def _read_rings(dxf_path: Path) -> list[list[tuple[float, float]]]:
    """All closed LWPOLYLINE rings from a tile DXF, as (x, y) point lists."""
    doc = ezdxf.readfile(dxf_path)
    msp = doc.modelspace()
    rings: list[list[tuple[float, float]]] = []
    for pl in msp.query("LWPOLYLINE"):
        pts = [(p[0], p[1]) for p in pl.get_points("xy")]
        if pts:
            rings.append(pts)
    return rings


def _quadrant_offsets(gap: float) -> list[tuple[float, float]]:
    step = (TILE_MM + gap) / 2.0  # tile-center distance from sheet center
    return [
        (-step, step),   # 1 top-left
        (step, step),    # 2 top-right
        (-step, -step),  # 3 bottom-left
        (step, -step),   # 4 bottom-right
    ]


def _square_ring() -> list[tuple[float, float]]:
    h = TILE_MM / 2.0
    return [(-h, -h), (h, -h), (h, h), (-h, h)]


def build_sheet(tiles: list[Path], out_stem: Path, gap: float, sheet_mm: float,
                squares_only: bool = False) -> tuple[Path, Path, float]:
    offsets = _quadrant_offsets(gap)

    doc = ezdxf.new(setup=True)
    doc.units = ezdxf.units.MM
    msp = doc.modelspace()

    svg_paths: list[str] = []
    for i, (ox, oy) in enumerate(offsets, start=1):
        layer = f"tile_{i}"
        lyr = doc.layers.add(layer)
        lyr.color = 1            # ACI red
        lyr.rgb = (255, 0, 0)    # exact FF0000
        # squares-only: just the 150x150 frame; otherwise all rings from the tile DXF.
        rings = [_square_ring()] if squares_only else _read_rings(tiles[i - 1])
        for ring in rings:
            moved = [(x + ox, y + oy) for x, y in ring]
            msp.add_lwpolyline(moved, close=True,
                               dxfattribs={"layer": layer, "color": 1, "true_color": 0xFF0000})
            d = "M " + " L ".join(f"{x:.4f} {y:.4f}" for x, y in moved) + " Z"
            svg_paths.append(d)

    out_stem.parent.mkdir(parents=True, exist_ok=True)
    dxf_out = out_stem.with_suffix(".dxf")
    doc.saveas(dxf_out)

    footprint = TILE_MM * 2 + gap
    # SVG: Y-up DXF coords -> flip for SVG display; viewBox covers the stock sheet.
    half = sheet_mm / 2.0
    svg_out = out_stem.with_suffix(".svg")
    parts = [
        f'<svg xmlns="http://www.w3.org/2000/svg" '
        f'width="{sheet_mm}mm" height="{sheet_mm}mm" '
        f'viewBox="{-half} {-half} {sheet_mm} {sheet_mm}">\n',
        f'<g fill="none" stroke="{STROKE_COLOR}" stroke-width="{STROKE_WIDTH_MM}" '
        f'vector-effect="non-scaling-stroke" transform="scale(1,-1)">\n',
    ]
    for d in svg_paths:
        parts.append(f'  <path d="{d}" />\n')
    parts.append("</g></svg>\n")
    svg_out.write_text("".join(parts), encoding="utf-8")

    return dxf_out, svg_out, footprint


def main(argv: list[str] | None = None) -> int:
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("tiles", type=Path, nargs="*",
                    help="Four tile DXF files, in TL TR BL BR order. "
                         "Omit when using --squares-only.")
    ap.add_argument("--squares-only", action="store_true",
                    help="Emit just the four 150mm frame squares, no logo geometry "
                         "(no tile inputs needed).")
    ap.add_argument("--gap", type=float, default=0.5,
                    help="Gap between tiles in mm (default 0.5).")
    ap.add_argument("--sheet", type=float, default=300.0,
                    help="Stock sheet edge in mm, for the SVG frame + overhang check (default 300).")
    ap.add_argument("--out", type=Path, default=Path("out/sheet"),
                    help="Output path stem; writes <stem>.dxf and <stem>.svg (default out/sheet).")
    args = ap.parse_args(argv)

    if not args.squares_only:
        if len(args.tiles) != 4:
            ap.error("Provide exactly 4 tile DXFs (or use --squares-only).")
        for t in args.tiles:
            if not t.exists():
                ap.error(f"Tile DXF not found: {t}")

    dxf_out, svg_out, footprint = build_sheet(
        args.tiles, args.out, args.gap, args.sheet, squares_only=args.squares_only)
    print(f"Wrote {dxf_out}")
    print(f"Wrote {svg_out}")
    print(f"Footprint: {footprint:.2f} x {footprint:.2f} mm (4x {TILE_MM:.0f}mm tiles, {args.gap}mm gap)")
    if footprint > args.sheet + 1e-6:
        over = (footprint - args.sheet) / 2.0
        print(f"WARNING: footprint exceeds {args.sheet:.0f}mm stock by {footprint - args.sheet:.2f}mm "
              f"({over:.2f}mm overhang per side). Reduce --gap or use larger stock.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
