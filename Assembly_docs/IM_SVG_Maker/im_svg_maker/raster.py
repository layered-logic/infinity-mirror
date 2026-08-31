"""Raster image (JPG/PNG line art) -> black-filled SVG suitable for the pipeline.

The output SVG has all traced shapes as `<path fill="#000000" fill-rule="evenodd">`
elements with proper shell+hole nesting, so the main pipeline's `svg_parse` reads
the topology correctly via its symmetric-difference XOR.

Public entry point: `trace_raster_to_svg(image_path, out_svg, cfg)`.
"""
from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path

import numpy as np
from PIL import Image, ImageFilter
from potrace import Bitmap  # PyPI package name is `potracer`
from shapely.geometry import MultiPolygon, Polygon
from shapely.ops import unary_union

from .config import Config


@dataclass
class TraceResult:
    """Summary of what trace_raster_to_svg produced."""
    n_kept: int
    n_dropped_thin: int
    n_dropped_small: int

    def __int__(self) -> int:  # backwards-compat: old callers did `n = trace_raster_to_svg(...)`
        return self.n_kept


def _ring_to_d(coords) -> str:
    coords = list(coords)
    if not coords:
        return ""
    parts = [f"M{coords[0][0]:.3f},{coords[0][1]:.3f}"]
    for x, y in coords[1:]:
        parts.append(f"L{x:.3f},{y:.3f}")
    parts.append("Z")
    return " ".join(parts)


def _binarize_and_thicken(image_path: Path, thickness_px: int, threshold: int) -> np.ndarray:
    im = Image.open(image_path).convert("L")
    arr = np.array(im)
    binary = arr < threshold  # True where dark
    if thickness_px > 0:
        pil = Image.fromarray((binary.astype(np.uint8)) * 255)
        # Square dilation. Subsequent nozzle rounding in the main pipeline
        # smooths the slight corner artifacts this introduces.
        pil = pil.filter(ImageFilter.MaxFilter(thickness_px * 2 + 1))
        binary = np.array(pil) > 128
    return binary


def _trace_to_polygons(binary: np.ndarray, turdsize: int) -> MultiPolygon:
    """Run potrace and assemble its evenodd subpaths into a proper MultiPolygon."""
    # potracer treats True as "above blacklevel" (background); pass the inverse
    # so the dark/dilated regions become the traced foreground.
    bm = Bitmap((~binary).astype(bool))
    plist = bm.trace(turdsize=turdsize, opttolerance=0.2, alphamax=1.0)

    rings: list[list[tuple[float, float]]] = []
    for curve in plist:
        pts: list[tuple[float, float]] = []
        sp = curve.start_point
        pts.append((float(sp.x), float(sp.y)))
        for seg in curve:
            ep = seg.end_point
            if seg.is_corner:
                c = seg.c
                pts.append((float(c.x), float(c.y)))
                pts.append((float(ep.x), float(ep.y)))
            else:
                c1, c2 = seg.c1, seg.c2
                p0 = pts[-1]
                for t in np.linspace(0.0, 1.0, 10)[1:]:
                    u = 1.0 - t
                    x = (u**3 * p0[0]
                         + 3 * u**2 * t * c1.x
                         + 3 * u * t**2 * c2.x
                         + t**3 * ep.x)
                    y = (u**3 * p0[1]
                         + 3 * u**2 * t * c1.y
                         + 3 * u * t**2 * c2.y
                         + t**3 * ep.y)
                    pts.append((float(x), float(y)))
        if len(pts) >= 3:
            rings.append(pts)

    raw: list[Polygon] = []
    for r in rings:
        try:
            p = Polygon(r)
            if not p.is_valid:
                p = p.buffer(0)
            if isinstance(p, Polygon) and p.area > 0:
                raw.append(p)
        except Exception:
            continue

    raw.sort(key=lambda p: p.area, reverse=True)
    depths: list[int] = []
    for i, p in enumerate(raw):
        rep = p.representative_point()
        depths.append(sum(1 for q in raw[:i] if q.contains(rep)))

    shells: list[Polygon] = []
    holes_per_shell: list[list[list[tuple[float, float]]]] = []
    for i, p in enumerate(raw):
        if depths[i] % 2 == 0:
            shells.append(p)
            holes_per_shell.append([])
        else:
            rep = p.representative_point()
            for j in range(len(shells) - 1, -1, -1):
                if shells[j].contains(rep):
                    holes_per_shell[j].append(list(p.exterior.coords))
                    break

    polys = [Polygon(s.exterior.coords, h) for s, h in zip(shells, holes_per_shell)]
    polys = [p for p in polys if p.is_valid and p.area > 0]
    merged = unary_union(MultiPolygon(polys))
    if isinstance(merged, Polygon):
        merged = MultiPolygon([merged])
    return merged


def _apply_thresholds(
    geom: MultiPolygon, cfg: Config, pixels_per_mm: float
) -> tuple[MultiPolygon, int, int]:
    """Apply post-trace min-feature-width and min-island-area filters.

    Both thresholds default to 0 (disabled). The width filter erodes each island
    by width/2 as a test; if the erosion leaves nothing the whole island is below
    threshold and gets dropped, otherwise the original (untouched) island is kept.
    This drops-or-keeps semantics avoids morphological-opening's surprise of
    fracturing islands at thin necks. The area floor runs after, judging islands
    on their unmodified area. Returns (filtered_geom, n_dropped_thin, n_dropped_small).
    """
    polys: list[Polygon] = [p for p in geom.geoms if isinstance(p, Polygon) and p.area > 0]

    n_dropped_thin = 0
    if cfg.raster_min_feature_width_mm > 0:
        r_px = (cfg.raster_min_feature_width_mm * pixels_per_mm) / 2.0
        kept: list[Polygon] = []
        for p in polys:
            test = p.buffer(-r_px, join_style="round")
            if test.is_empty or test.area <= 0:
                continue
            kept.append(p)
        n_dropped_thin = len(polys) - len(kept)
        polys = kept

    n_dropped_small = 0
    if cfg.raster_min_island_area_mm2 > 0:
        threshold_px2 = cfg.raster_min_island_area_mm2 * (pixels_per_mm ** 2)
        kept = [p for p in polys if p.area >= threshold_px2]
        n_dropped_small = len(polys) - len(kept)
        polys = kept

    return MultiPolygon(polys), n_dropped_thin, n_dropped_small


def trace_raster_to_svg(image_path: Path, out_svg: Path, cfg: Config) -> TraceResult:
    """Trace `image_path` to a black SVG at `out_svg`, using cfg's raster knobs.

    Returns a TraceResult with kept-island count and per-threshold drop counts.
    int(result) gives the kept count for legacy callers.
    """
    im_size = Image.open(image_path).size  # (w, h)
    w, h = im_size
    pixels_per_mm = max(w, h) / cfg.max_logo_dim_mm
    thickness_px = max(0, int(round(cfg.raster_thickness_mm * pixels_per_mm)))

    binary = _binarize_and_thicken(image_path, thickness_px, cfg.raster_threshold)
    geom = _trace_to_polygons(binary, cfg.raster_turdsize)
    geom, n_dropped_thin, n_dropped_small = _apply_thresholds(geom, cfg, pixels_per_mm)

    parts = [
        f'<svg xmlns="http://www.w3.org/2000/svg" '
        f'width="{w}" height="{h}" viewBox="0 0 {w} {h}">\n'
    ]
    for poly in geom.geoms:
        d = _ring_to_d(poly.exterior.coords)
        for hole in poly.interiors:
            d += " " + _ring_to_d(hole.coords)
        parts.append(f'  <path d="{d}" fill="#000000" fill-rule="evenodd"/>\n')
    parts.append("</svg>\n")
    out_svg.write_text("".join(parts), encoding="utf-8")
    return TraceResult(
        n_kept=len(geom.geoms),
        n_dropped_thin=n_dropped_thin,
        n_dropped_small=n_dropped_small,
    )
