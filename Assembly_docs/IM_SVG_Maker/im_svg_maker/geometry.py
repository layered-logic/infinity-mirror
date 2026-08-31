"""Derive laser/inset/base geometries from the cut polygon.

All buffer operations use round joins so the offset is uniform everywhere — mitre
joins produce huge mitre extensions at sharp inner corners, which doesn't match
either physical kerf or 3D-printable corner behavior.
"""
from __future__ import annotations

from dataclasses import dataclass

from shapely.geometry import MultiPolygon, Polygon

from .config import Config


@dataclass
class Geometries:
    cut: MultiPolygon              # nozzle-rounded source cut region
    laser: MultiPolygon            # cut + kerf (the hole in the mirror)
    inset: MultiPolygon            # cut - clearance (3D plug top layer footprint)
    base: MultiPolygon             # cut + base_offset (3D plug support layer footprint)
    island_count: int
    base_offset_mm: float
    auto_bridge_gap_mm: float | None  # None if a single island


def round_to_nozzle(geom: MultiPolygon, r: float) -> MultiPolygon:
    """Round both inside and outside corners to radius r.

    Opening (rounds outside corners, removes thin protrusions) then closing
    (rounds inside corners, fills thin gaps).
    """
    if r <= 0:
        return geom
    opened = geom.buffer(-r, join_style="round").buffer(+r, join_style="round")
    closed = opened.buffer(+r, join_style="round").buffer(-r, join_style="round")
    return _as_multi(closed)


def _as_multi(geom) -> MultiPolygon:
    if isinstance(geom, Polygon):
        return MultiPolygon([geom])
    return geom


def _nearest_island_gap(islands: list[Polygon]) -> float | None:
    """Minimum shortest distance between any two islands (mm)."""
    if len(islands) < 2:
        return None
    best = float("inf")
    for i in range(len(islands)):
        for j in range(i + 1, len(islands)):
            d = islands[i].distance(islands[j])
            if d < best:
                best = d
    return best


def compute_base_offset(cfg: Config, islands: list[Polygon]) -> tuple[float, float | None]:
    n = len(islands)
    floor = cfg.base_offset_min_mm + max(0, n - 1) * cfg.base_offset_per_island_mm
    gap = _nearest_island_gap(islands)
    bridge = cfg.base_bridge_factor * (gap / 2.0) if gap is not None else 0.0
    return max(floor, bridge), gap


def build_geometries(cut_raw: MultiPolygon, cfg: Config) -> Geometries:
    cut = round_to_nozzle(cut_raw, cfg.nozzle_radius_mm)

    laser = _as_multi(cut.buffer(+cfg.laser_kerf_mm, join_style="round"))
    inset = _as_multi(cut.buffer(-cfg.inset_clearance_mm, join_style="round"))

    islands = list(cut.geoms)
    base_offset, gap = compute_base_offset(cfg, islands)
    base = _as_multi(cut.buffer(+base_offset, join_style="round"))

    return Geometries(
        cut=cut,
        laser=laser,
        inset=inset,
        base=base,
        island_count=len(islands),
        base_offset_mm=base_offset,
        auto_bridge_gap_mm=gap,
    )
