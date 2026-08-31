"""Build the 3D plug mesh and export as 3MF + STL.

Plug structure (z-axis = build direction):
  z = 0           : bottom of base
  z = base_h      : top of base / bottom of inset
  z = base_h + ins: top of inset (flush with mirror face when assembled)
"""
from __future__ import annotations

from pathlib import Path

import numpy as np
import trimesh
from shapely.geometry import MultiPolygon, Polygon

from .config import Config


def _extrude(poly_or_multi, height: float, z_offset: float) -> trimesh.Trimesh:
    """Extrude a (Multi)Polygon to a mesh and translate it to z_offset."""
    if isinstance(poly_or_multi, MultiPolygon):
        meshes = [_extrude(p, height, z_offset) for p in poly_or_multi.geoms]
        return trimesh.util.concatenate(meshes)
    poly: Polygon = poly_or_multi
    mesh = trimesh.creation.extrude_polygon(poly, height)
    mesh.apply_translation((0.0, 0.0, z_offset))
    return mesh


def build_plug(inset: MultiPolygon, base: MultiPolygon, cfg: Config) -> trimesh.Trimesh:
    base_mesh = _extrude(base, cfg.base_height_mm, z_offset=0.0)
    inset_mesh = _extrude(inset, cfg.inset_height_mm, z_offset=cfg.base_height_mm)
    # Union them into one watertight mesh so the slicer treats it as a single part.
    combined = trimesh.boolean.union([base_mesh, inset_mesh])
    if not combined.is_volume:
        # Fallback: simple concatenation. Slicers usually still handle this OK
        # because the two pieces touch flush.
        combined = trimesh.util.concatenate([base_mesh, inset_mesh])
    return combined


def export_plug(mesh: trimesh.Trimesh, out_dir: Path) -> tuple[Path, Path]:
    threemf = out_dir / "plug.3mf"
    stl = out_dir / "plug.stl"
    mesh.export(threemf)
    mesh.export(stl)
    return threemf, stl
