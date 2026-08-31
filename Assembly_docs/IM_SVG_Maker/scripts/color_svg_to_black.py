"""CLI: colored SVG -> black SVG by fill-color selection.

List mode (no --color flag):
    python scripts/color_svg_to_black.py Washington_Huskies_logo.svg

Extract mode (one or more --color flags):
    python scripts/color_svg_to_black.py Washington_Huskies_logo.svg \
        --color "#E8D3A2" --out gold.svg
    python scripts/color_svg_to_black.py logo.svg --color "#FFFFFF" --invert -o nonbg.svg
    python scripts/color_svg_to_black.py logo.svg --color "gold" --tolerance 10 -o gold.svg

Colors can be hex (#RGB, #RRGGBB), named (gold, red), or rgb(r,g,b).
Tolerance is L-infinity over r/g/b channels in 0-255 units; 0 = exact match.
With --invert, everything that does NOT match the color list is kept.

The output is a black-filled SVG with one <path fill="#000000" fill-rule="evenodd"/>
per kept element — feed it straight into `python -m im_svg_maker`.
"""
from __future__ import annotations

import argparse
import sys
from pathlib import Path

# Allow running as `python scripts/color_svg_to_black.py ...` without an install.
sys.path.insert(0, str(Path(__file__).resolve().parent.parent))

from im_svg_maker.color_select import (
    list_colors,
    parse_color_spec,
    select_to_black,
)


def _format_color_table(rows) -> str:
    if not rows:
        return "(no solid-fill paths found)"
    header = f"{'color':<10} {'paths':>6} {'bbox area':>14}"
    sep = "-" * len(header)
    lines = [header, sep]
    for row in rows:
        lines.append(
            f"{row.color.hex():<10} {row.n_paths:>6} {row.bbox_area:>14.2f}"
        )
    return "\n".join(lines)


def main(argv: list[str] | None = None) -> int:
    p = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    p.add_argument("svg", type=Path, help="Input colored SVG.")
    p.add_argument(
        "--color", action="append", default=[],
        metavar="COLOR",
        help="Color to select (hex / named / rgb(...)). Pass repeatedly for multiple.",
    )
    p.add_argument(
        "--tolerance", type=int, default=0,
        help="L-infinity color-match tolerance in 0-255 units (default: 0 = exact).",
    )
    p.add_argument(
        "--invert", action="store_true",
        help="Keep paths that do NOT match the color list (e.g. drop a white background).",
    )
    p.add_argument(
        "--out", "-o", type=Path, default=None,
        help="Output SVG path. Required in extract mode; ignored in list mode.",
    )
    args = p.parse_args(argv)

    if not args.svg.exists():
        p.error(f"SVG not found: {args.svg}")

    if not args.color:
        rows = list_colors(args.svg)
        print(_format_color_table(rows))
        print(
            f"\n{len(rows)} distinct fill color(s). "
            f"Re-run with --color <hex> --out <file>.svg to extract."
        )
        return 0

    try:
        colors = [parse_color_spec(c) for c in args.color]
    except ValueError as exc:
        p.error(str(exc))

    out = args.out
    if out is None:
        suffix = "_black.svg"
        out = args.svg.with_name(args.svg.stem + suffix)

    n = select_to_black(
        args.svg, out, colors,
        tolerance=args.tolerance,
        invert=args.invert,
    )
    selected = ", ".join(c.hex() for c in colors)
    mode = "NOT matching" if args.invert else "matching"
    tol_note = f", tol +/-{args.tolerance}" if args.tolerance > 0 else ""
    print(
        f"Wrote {out} ({n} path(s) kept, fill {mode} {{ {selected} }}{tol_note})"
    )
    if n == 0:
        print(
            "  No paths matched. Run without --color to see what's actually in the file.",
            file=sys.stderr,
        )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
