"""Generate Android launcher PNGs from the Layered Logic logo geometry.

The SVG at assets/brand/logo/logo-primary-dark.svg is just two stroked
polyline L shapes; we re-render them directly with PIL so the build
doesn't need cairosvg/inkscape/imagemagick on the dev machine.

Run from the App/v1/ directory.
"""
from __future__ import annotations

from pathlib import Path
from PIL import Image, ImageDraw

# Brand-locked geometry from assets/brand/logo/logo-primary-dark.svg.
# Original SVG units; we scale to fit the icon canvas. The canonical
# inner L is inset 20 SVG units from the outer; for an app-icon the gap
# reads cramped, so we re-derive the inner L with a larger inset by
# scaling each inner offset by INNER_INSET_FACTOR.
OUTER = [(22.32, -319.22), (0.0, 0.0), (220.0, 0.0)]
_INNER_CANON = [(40.05, -305.91), (19.95, -18.60), (199.95, -18.60)]
INNER_INSET_FACTOR = 1.55  # 1.0 = canonical 20-unit gap; >1.0 widens the channel
INNER = [
    (
        OUTER[i][0] + (_INNER_CANON[i][0] - OUTER[i][0]) * INNER_INSET_FACTOR,
        OUTER[i][1] + (_INNER_CANON[i][1] - OUTER[i][1]) * INNER_INSET_FACTOR,
    )
    for i in range(3)
]

OUTER_COLOR = "#4A25FF"   # Indigo Reference (brand primary)
INNER_COLOR = "#3214FF"   # Indigo Signal (brand secondary)
# Transparent so the launcher (Pixel 9 themed icons) paints its own
# tile background. Indigo strokes read on light or dark launcher tiles.
BG_COLOR = (0, 0, 0, 0)

# Stroke widths in SVG units. Canonical 7/6 reads spindly at icon
# sizes; bumped 2.3x for the legacy mipmap, then nudged 15% bolder.
SW_OUTER = 16.0
SW_INNER = 14.0

# Bounding box of the geometry (covers both L's).
ALL_PTS = OUTER + INNER
MIN_X = min(p[0] for p in ALL_PTS)
MAX_X = max(p[0] for p in ALL_PTS)
MIN_Y = min(p[1] for p in ALL_PTS)
MAX_Y = max(p[1] for p in ALL_PTS)
GEOM_W = MAX_X - MIN_X
GEOM_H = MAX_Y - MIN_Y


# Standard Android launcher icon densities.
SIZES = {
    "mipmap-mdpi": 48,
    "mipmap-hdpi": 72,
    "mipmap-xhdpi": 96,
    "mipmap-xxhdpi": 144,
    "mipmap-xxxhdpi": 192,
}

# Padding ratio — leave breathing room so the icon doesn't crowd the
# launcher mask. Adaptive icons require ~1/3 keepout on each side, but
# we're producing legacy square icons; 8% padding gives the geometry
# ~10% more presence than the prior 12% setting.
PADDING_RATIO = 0.08


def render_icon(size: int) -> Image.Image:
    img = Image.new("RGBA", (size, size), BG_COLOR)
    draw = ImageDraw.Draw(img)

    inner = size * (1 - 2 * PADDING_RATIO)
    # Pick the smaller of the two scales so the geometry fits in the
    # padded box on whichever axis is tightest. Then center.
    scale = min(inner / GEOM_W, inner / GEOM_H)
    geom_w_px = GEOM_W * scale
    geom_h_px = GEOM_H * scale
    offset_x = (size - geom_w_px) / 2 - MIN_X * scale
    offset_y = (size - geom_h_px) / 2 - MIN_Y * scale

    def project(p):
        return (p[0] * scale + offset_x, p[1] * scale + offset_y)

    # Stroke widths scale with the geometry. PIL stroke joins look
    # best when we pass the polyline as a single sequence to .line()
    # with width and rounded join (joint="curve").
    sw_outer = max(2, round(SW_OUTER * scale))
    sw_inner = max(2, round(SW_INNER * scale))

    outer_pts = [project(p) for p in OUTER]
    inner_pts = [project(p) for p in INNER]

    # Order matters — draw outer first, inner on top, so the brighter
    # signal indigo reads as forward.
    draw.line(outer_pts, fill=OUTER_COLOR, width=sw_outer, joint="curve")
    draw.line(inner_pts, fill=INNER_COLOR, width=sw_inner, joint="curve")

    return img


def main():
    res_dir = Path(__file__).resolve().parent.parent / "android" / "app" / "src" / "main" / "res"
    if not res_dir.is_dir():
        raise SystemExit(f"expected android res dir at {res_dir}")

    for folder, size in SIZES.items():
        img = render_icon(size)
        for fname in ("ic_launcher.png", "ic_launcher_round.png"):
            out = res_dir / folder / fname
            out.parent.mkdir(parents=True, exist_ok=True)
            img.save(out, format="PNG")
            print(f"  {out.relative_to(res_dir.parent.parent.parent.parent)}: {size}x{size}")

    # Adaptive-icon foreground (API 26+, optional but nicer on modern launchers).
    # We skip ic_launcher_background.xml — leaving the legacy square icon as
    # the canonical surface for now.

if __name__ == "__main__":
    main()
