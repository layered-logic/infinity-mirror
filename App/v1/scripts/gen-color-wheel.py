"""Generate a 256x256 HSV color wheel PNG for the LL Mirror app's color picker.

Hue maps to angle around the wheel (0° = red, sweeping counter-clockwise);
saturation maps to radius (center = white, edge = pure hue); value is fixed
at 1.0 so the wheel reads as a flat color gamut without brightness fade.
Outside the circular radius, pixels are transparent.

Output: App/v1/assets/color-wheel.png. Bundled into the RN app via require().

Run from anywhere; output path is relative to repo root.
"""
import colorsys
import math
import os

from PIL import Image


SIZE = 512  # 2x the original — RN <Image> samples cleanly at the larger phone-screen render size.
RADIUS = SIZE // 2


def main() -> None:
    img = Image.new("RGBA", (SIZE, SIZE), (0, 0, 0, 0))
    pixels = img.load()
    cx = cy = RADIUS

    for y in range(SIZE):
        for x in range(SIZE):
            dx = x - cx
            dy = y - cy
            r = math.sqrt(dx * dx + dy * dy)
            if r > RADIUS:
                continue  # outside the wheel — leave transparent
            # atan2 returns -pi..pi; shift to 0..2pi then to 0..1 hue.
            # +pi/2 so 0 deg (red) lands at the top — matches a clock-
            # face mental model that pairs better with rotation gestures.
            theta = math.atan2(dy, dx)
            hue = ((theta + math.pi / 2) / (2 * math.pi)) % 1.0
            sat = min(r / RADIUS, 1.0)
            val = 1.0
            rr, gg, bb = colorsys.hsv_to_rgb(hue, sat, val)
            pixels[x, y] = (
                int(round(rr * 255)),
                int(round(gg * 255)),
                int(round(bb * 255)),
                255,
            )

    out_dir = os.path.join(os.path.dirname(__file__), "..", "assets")
    os.makedirs(out_dir, exist_ok=True)
    out_path = os.path.join(out_dir, "color-wheel.png")
    img.save(out_path, "PNG")
    print(f"wrote {out_path} ({os.path.getsize(out_path)} bytes)")


if __name__ == "__main__":
    main()
