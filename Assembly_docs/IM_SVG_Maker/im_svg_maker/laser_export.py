"""Write the laser-cut artifacts: SVG (red hairline) and DXF."""
from __future__ import annotations

from pathlib import Path
from xml.sax.saxutils import escape

import ezdxf
from shapely.geometry import MultiPolygon, Polygon

from .config import Config

# Red hairline stroke — common convention for vector laser cutters (Epilog,
# Glowforge, Trotec all recognize a red 0-width stroke as a vector cut).
STROKE_COLOR = "#FF0000"
STROKE_WIDTH_MM = 0.01


def _ring_to_d(ring) -> str:
    coords = list(ring.coords)
    if not coords:
        return ""
    parts = [f"M {coords[0][0]:.4f} {coords[0][1]:.4f}"]
    for x, y in coords[1:]:
        parts.append(f"L {x:.4f} {y:.4f}")
    parts.append("Z")
    return " ".join(parts)


def _polygon_to_d(poly: Polygon) -> str:
    segs = [_ring_to_d(poly.exterior)]
    for hole in poly.interiors:
        segs.append(_ring_to_d(hole))
    return " ".join(segs)


def write_svg(laser_geom: MultiPolygon, cfg: Config, out: Path) -> None:
    mw, mh = cfg.mirror_size_mm
    # Mirror is centered at origin in our working coordinates; convert to an
    # SVG with origin at top-left and Y pointing down.
    # SVG world: (0,0) top-left, viewBox = mirror full sheet.
    parts: list[str] = []
    parts.append(
        f'<svg xmlns="http://www.w3.org/2000/svg" '
        f'width="{mw}mm" height="{mh}mm" '
        f'viewBox="{-mw/2} {-mh/2} {mw} {mh}">'
    )
    parts.append(
        f'<g fill="none" stroke="{STROKE_COLOR}" '
        f'stroke-width="{STROKE_WIDTH_MM}" vector-effect="non-scaling-stroke" '
        f'transform="scale(1,-1)">'  # flip Y so polygon coords render correctly
    )
    # Outer mirror rectangle.
    parts.append(
        f'<rect x="{-mw/2}" y="{-mh/2}" width="{mw}" height="{mh}" />'
    )
    # Interior laser cut(s).
    for poly in laser_geom.geoms:
        parts.append(f'<path d="{escape(_polygon_to_d(poly))}" />')
    parts.append("</g></svg>\n")

    out.write_text("\n".join(parts), encoding="utf-8")


def write_dxf(laser_geom: MultiPolygon, cfg: Config, out: Path) -> None:
    doc = ezdxf.new(setup=True)
    doc.units = ezdxf.units.MM
    msp = doc.modelspace()
    mw, mh = cfg.mirror_size_mm

    # Mirror rectangle as a closed LWPOLYLINE.
    msp.add_lwpolyline(
        [(-mw/2, -mh/2), (mw/2, -mh/2), (mw/2, mh/2), (-mw/2, mh/2)],
        close=True,
    )

    for poly in laser_geom.geoms:
        msp.add_lwpolyline(list(poly.exterior.coords), close=True)
        for hole in poly.interiors:
            msp.add_lwpolyline(list(hole.coords), close=True)

    doc.saveas(out)
