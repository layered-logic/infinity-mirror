"""Diagnose the polygon topology coming out of svg_to_polygons."""
from preview_cutlines import svg_to_polygons, normalize, SVG_PATH
from shapely.geometry import Polygon, MultiPolygon

raw = svg_to_polygons(SVG_PATH)
cut = normalize(raw)

def describe(label, geom):
    print(f"--- {label} ---")
    print(f"  type: {type(geom).__name__}")
    if isinstance(geom, MultiPolygon):
        polys = list(geom.geoms)
    else:
        polys = [geom]
    print(f"  polygon count: {len(polys)}")
    for i, p in enumerate(polys):
        print(f"  poly {i}: area={p.area:.2f} bounds={[round(b,2) for b in p.bounds]} ext_ccw={p.exterior.is_ccw} holes={len(p.interiors)}")
        for j, h in enumerate(p.interiors):
            poly_hole = Polygon(h)
            print(f"    hole {j}: area={poly_hole.area:.2f} bounds={[round(b,2) for b in poly_hole.bounds]} ring_ccw={h.is_ccw}")

describe("ORIGINAL", cut)
print()
describe("buffer(+0.5, round)", cut.buffer(0.5, join_style="round"))
