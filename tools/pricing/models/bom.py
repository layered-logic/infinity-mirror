"""BOM data model — components, suppliers, and scaling types."""

from __future__ import annotations

import math
from dataclasses import dataclass, field
from enum import Enum
from pathlib import Path

import yaml


class ScalingType(Enum):
    FIXED = "fixed"
    PERIMETER = "perimeter"
    AREA = "area"


@dataclass
class Supplier:
    name: str
    url: str
    unit_price: float
    sheet_size: tuple[float, float] | None = None  # (W, H) inches — for area-scaled sheet materials
    scrape_enabled: bool = False
    last_scraped: str | None = None


@dataclass
class Component:
    name: str
    scaling: ScalingType
    tier: str  # "basic", "pro", or "both"
    ref_quantity: float
    ref_size: tuple[float, float]  # (W, H) inches
    unit: str
    unit_price: float | None = None  # simple fixed price (no suppliers list)
    suppliers: list[Supplier] = field(default_factory=list)

    @property
    def best_price(self) -> float | None:
        """Return the cheapest available price (direct or supplier)."""
        if self.unit_price is not None:
            return self.unit_price
        if self.suppliers:
            return min(s.unit_price for s in self.suppliers)
        return None

    def best_supplier_for_sheet(
        self, target_w: float, target_h: float
    ) -> tuple[Supplier | None, float, int]:
        """For area-scaled sheet materials, find the supplier with lowest per-piece cost.

        Returns (supplier, cost_per_piece, pieces_per_sheet).
        """
        if not self.suppliers:
            return None, 0.0, 0

        best: tuple[Supplier | None, float, int] = (None, float("inf"), 0)
        for s in self.suppliers:
            if s.sheet_size:
                sw, sh = s.sheet_size
                pieces = _pieces_from_sheet(target_w, target_h, sw, sh)
                if pieces > 0:
                    cost_per_piece = s.unit_price / pieces
                    if cost_per_piece < best[1]:
                        best = (s, cost_per_piece, pieces)
        return best


def _pieces_from_sheet(
    piece_w: float, piece_h: float, sheet_w: float, sheet_h: float, kerf: float = 0.0
) -> int:
    """How many piece_w x piece_h rectangles fit in a sheet_w x sheet_h sheet.

    Tries both orientations and picks the better one.
    Kerf defaults to 0 — laser kerf is negligible (~0.2mm) and piece dimensions
    are designed to account for it.
    """
    def _grid(pw: float, ph: float, sw: float, sh: float) -> int:
        if pw <= 0 or ph <= 0:
            return 0
        cols = math.floor((sw + kerf) / (pw + kerf)) if kerf > 0 else math.floor(sw / pw)
        rows = math.floor((sh + kerf) / (ph + kerf)) if kerf > 0 else math.floor(sh / ph)
        return cols * rows

    option_a = _grid(piece_w, piece_h, sheet_w, sheet_h)
    option_b = _grid(piece_h, piece_w, sheet_w, sheet_h)
    return max(option_a, option_b)


@dataclass
class BOM:
    components: list[Component]
    labor_per_unit: float
    energy_per_unit: float
    overhead_per_unit: float

    def for_tier(self, tier: str) -> list[Component]:
        """Return components applicable to a given tier."""
        return [c for c in self.components if c.tier == tier or c.tier == "both"]


def load_config(config_path: Path) -> dict:
    """Load and return the raw YAML config."""
    with open(config_path, "r") as f:
        return yaml.safe_load(f)


def build_bom(config: dict) -> BOM:
    """Build a BOM from parsed YAML config."""
    components = []
    for c in config["components"]:
        suppliers = []
        for s in c.get("suppliers", []):
            suppliers.append(
                Supplier(
                    name=s["name"],
                    url=s.get("url", ""),
                    unit_price=s["unit_price"],
                    sheet_size=tuple(s["sheet_size"]) if "sheet_size" in s else None,
                    scrape_enabled=s.get("scrape_enabled", False),
                )
            )
        components.append(
            Component(
                name=c["name"],
                scaling=ScalingType(c["scaling"]),
                tier=c["tier"],
                ref_quantity=c["ref_quantity"],
                ref_size=tuple(c["ref_size"]),
                unit=c["unit"],
                unit_price=c.get("unit_price"),
                suppliers=suppliers,
            )
        )

    return BOM(
        components=components,
        labor_per_unit=config.get("labor_per_unit", 10.0),
        energy_per_unit=config.get("energy_per_unit", 1.0),
        overhead_per_unit=config.get("overhead_per_unit", 3.0),
    )
