"""Load and validate variables.json."""
from __future__ import annotations

import json
from pathlib import Path

from pydantic import BaseModel, Field, field_validator


class Config(BaseModel):
    mirror_size_mm: tuple[float, float] = (150.0, 150.0)
    max_logo_dim_mm: float = Field(100.0, gt=0)
    nozzle_diameter_mm: float = Field(0.6, gt=0)
    laser_kerf_mm: float = Field(0.1, ge=0)
    inset_clearance_mm: float = Field(0.15, ge=0)
    inset_height_mm: float = Field(3.0, gt=0)
    base_height_mm: float = Field(1.0, gt=0)
    base_offset_min_mm: float = Field(2.0, ge=0)
    base_offset_per_island_mm: float = Field(0.5, ge=0)
    base_bridge_factor: float = Field(1.0, ge=0, le=1)
    # Raster-tracing preprocessor knobs (only used by scripts/raster_to_black_svg.py).
    raster_thickness_mm: float = Field(0.75, ge=0)
    raster_threshold: int = Field(128, ge=0, le=255)
    raster_turdsize: int = Field(8, ge=0)
    # Post-trace mm-units thresholds. 0 = disabled.
    # raster_min_island_area_mm2: drop traced islands below this area floor (deletes specks
    # that survive the pixel-units turdsize filter; resolution-independent).
    # raster_min_feature_width_mm: morphological-opening radius = width/2. Removes hairlines
    # and thin appendages that the printer can't reproduce as walls.
    raster_min_island_area_mm2: float = Field(0.0, ge=0)
    raster_min_feature_width_mm: float = Field(0.0, ge=0)

    @field_validator("mirror_size_mm", mode="before")
    @classmethod
    def _coerce_pair(cls, v):
        if isinstance(v, (list, tuple)) and len(v) == 2:
            return (float(v[0]), float(v[1]))
        raise ValueError("mirror_size_mm must be a 2-element [width, height] list")

    @property
    def nozzle_radius_mm(self) -> float:
        return self.nozzle_diameter_mm / 2


def load_config(path: Path) -> Config:
    with path.open() as f:
        data = json.load(f)
    return Config(**data)
