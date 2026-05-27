"""Regenerate the .canonical.svg files for every fixture in tests/fixtures/.

These canonicals are the source-of-truth output the JS port in
infinity_mirror_visualizer must match (within tolerance). Run this whenever
the Python preprocessor's behavior intentionally changes; commit the diffs.

Each fixture's recipe is encoded inline below. Add a new entry when adding
a new fixture.
"""
from __future__ import annotations

import sys
from pathlib import Path

# Allow running as `python tools/regenerate_canonicals.py` from repo root.
HERE = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(HERE))

from im_svg_maker.color_select import parse_color_spec, select_to_black
from im_svg_maker.config import load_config
from im_svg_maker.raster import trace_raster_to_svg

FIXTURES_DIR = HERE / "tests" / "fixtures"
CONFIG_PATH = HERE / "variables.json"


def _canonical(fixture: str) -> Path:
    return FIXTURES_DIR / (Path(fixture).stem + ".canonical.svg")


def _passthrough(src: Path, dst: Path) -> None:
    """Black-SVG passthrough — copy bytes. This is the trivial canonical for
    a fixture that is already pre-prepped for the main pipeline."""
    dst.write_bytes(src.read_bytes())


def regenerate() -> int:
    cfg = load_config(CONFIG_PATH)
    recipes = [
        ("demo_W_black.svg", "passthrough", {}),
        ("fox.jpg", "raster", {}),
        ("Washington_Huskies_logo.svg", "color_select", {"color": "#E8D3A2"}),
        ("nested_holes.svg", "passthrough", {}),
        ("tiny_specks.svg", "passthrough", {}),
        ("hairline.svg", "passthrough", {}),
    ]

    n_ok = 0
    for fixture, recipe, opts in recipes:
        src = FIXTURES_DIR / fixture
        if not src.exists():
            print(f"  SKIP {fixture} (not found)")
            continue
        dst = _canonical(fixture)
        if recipe == "passthrough":
            _passthrough(src, dst)
            print(f"  OK   {fixture} -> {dst.name} (passthrough)")
        elif recipe == "raster":
            result = trace_raster_to_svg(src, dst, cfg)
            print(
                f"  OK   {fixture} -> {dst.name} "
                f"(raster, {result.n_kept} islands, "
                f"-{result.n_dropped_thin} thin, -{result.n_dropped_small} small)"
            )
        elif recipe == "color_select":
            color_key = parse_color_spec(opts["color"])
            n = select_to_black(src, dst, [color_key])
            print(
                f"  OK   {fixture} -> {dst.name} "
                f"(color_select {opts['color']}, {n} path(s) kept)"
            )
        else:
            print(f"  FAIL {fixture}: unknown recipe {recipe!r}")
            continue
        n_ok += 1

    print(f"\nRegenerated {n_ok} canonical(s).")
    return 0 if n_ok == len(recipes) else 1


if __name__ == "__main__":
    raise SystemExit(regenerate())
