"""Zoom into the outer edge vs an inner V to compare offset widths."""
import matplotlib.pyplot as plt
from preview_cutlines import svg_to_polygons, normalize, SVG_PATH, fill_geom
from shapely.geometry import Polygon, MultiPolygon

cut = normalize(svg_to_polygons(SVG_PATH))
laser = cut.buffer(0.5, join_style="round")
# The "ring" between original and kerf-grown — this is the material the laser vaporises
kerf_ring = laser.difference(cut)

fig, axes = plt.subplots(1, 3, figsize=(15, 6))

regions = [
    ("Outer edge (top of W)", (-20, 20, 30, 40)),
    ("Inner V (bottom-middle)", (-8, 8, -18, -8)),
    ("Inner top edge of W", (-25, -10, 18, 28)),
]
for ax, (title, (xlo, xhi, ylo, yhi)) in zip(axes, regions):
    fill_geom(ax, cut, color="black")
    fill_geom(ax, kerf_ring, color="red", alpha=0.7)
    ax.set_xlim(xlo, xhi)
    ax.set_ylim(ylo, yhi)
    ax.set_aspect("equal")
    ax.set_title(title)
    ax.grid(True, alpha=0.3)
    # mm scale bar
    ax.plot([xlo + 1, xlo + 2], [ylo + 0.5, ylo + 0.5], color="blue", linewidth=3)
    ax.text(xlo + 1.5, ylo + 0.7, "1mm", color="blue", fontsize=8, ha="center")

plt.suptitle("0.5mm kerf ring (red) — should be uniform width everywhere")
plt.tight_layout()
out = SVG_PATH.parent / "cut_lines_zoom.png"
plt.savefig(out, dpi=150)
print(f"Wrote {out}")
