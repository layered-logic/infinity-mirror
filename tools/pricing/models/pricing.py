"""Pricing engine — COGS calculation, markup, and platform fees."""

from __future__ import annotations

from dataclasses import dataclass, field

from .bom import BOM, Component, ScalingType
from .mirror import MirrorSpec


@dataclass
class LineItem:
    """One row in the BOM breakdown."""

    component_name: str
    quantity: float
    unit: str
    unit_price: float
    subtotal: float
    supplier_name: str = ""
    note: str = ""


@dataclass
class PricingResult:
    """Full pricing breakdown for a configured mirror."""

    # Inputs
    width_in: float
    height_in: float
    tier: str
    platform: str

    # BOM
    line_items: list[LineItem] = field(default_factory=list)
    material_cost: float = 0.0
    labor_cost: float = 0.0
    energy_cost: float = 0.0
    overhead_cost: float = 0.0

    # Derived
    cogs: float = 0.0
    markup_multiplier: float = 2.5
    sale_price: float = 0.0
    platform_fees: float = 0.0
    shipping_cost: float = 0.0
    customer_price: float = 0.0
    net_revenue: float = 0.0
    margin_pct: float = 0.0


def calculate_cogs(
    spec: MirrorSpec,
    bom: BOM,
    tier: str,
) -> tuple[list[LineItem], float]:
    """Calculate material COGS for a mirror spec and tier.

    Returns (line_items, total_material_cost).
    """
    components = bom.for_tier(tier)
    line_items: list[LineItem] = []
    total = 0.0

    for comp in components:
        qty, unit_price, supplier_name, note = _resolve_component(comp, spec)

        subtotal = qty * unit_price
        total += subtotal

        line_items.append(
            LineItem(
                component_name=comp.name,
                quantity=round(qty, 2),
                unit=comp.unit,
                unit_price=round(unit_price, 4),
                subtotal=round(subtotal, 2),
                supplier_name=supplier_name,
                note=note,
            )
        )

    return line_items, round(total, 2)


def _resolve_component(
    comp: Component, spec: MirrorSpec
) -> tuple[float, float, str, str]:
    """Determine quantity, unit price, supplier, and notes for a component.

    Returns (quantity, unit_price, supplier_name, note).
    """
    ref_w, ref_h = comp.ref_size

    if comp.scaling == ScalingType.FIXED:
        qty = comp.ref_quantity
        price = comp.best_price or 0.0
        return qty, price, "", ""

    elif comp.scaling == ScalingType.PERIMETER:
        scale = spec.perimeter_in / (2 * (ref_w + ref_h))
        qty = comp.ref_quantity * scale

        # LED strips: scale ref count by perimeter ratio, use per-LED supplier pricing
        if comp.suppliers and comp.unit == "LED":
            qty = round(qty)  # whole LEDs
            best = min(comp.suppliers, key=lambda s: s.unit_price)
            return qty, best.unit_price, best.name, ""

        price = comp.best_price or 0.0
        return qty, price, "", ""

    elif comp.scaling == ScalingType.AREA:
        # For sheet materials with suppliers, do sheet utilization
        if comp.suppliers:
            supplier, cost_per_piece, pieces = comp.best_supplier_for_sheet(
                spec.width_in, spec.height_in
            )
            if supplier:
                note = f"{pieces} pcs/sheet"
                return 1, round(cost_per_piece, 4), supplier.name, note

        # Fallback: simple area scaling
        scale = spec.area_sq_in / (ref_w * ref_h)
        qty = comp.ref_quantity * scale
        price = comp.best_price or 0.0
        return qty, price, "", ""

    return 0, 0, "", ""


def calculate_platform_fees(
    sale_price: float, platform_config: dict
) -> float:
    """Calculate total platform fees for a sale."""
    fees = 0.0
    fees += sale_price * platform_config.get("transaction_fee_pct", 0)
    fees += platform_config.get("transaction_fee_fixed", 0)
    fees += platform_config.get("listing_fee", 0)
    fees += sale_price * platform_config.get("payment_processing_pct", 0)
    fees += platform_config.get("payment_processing_fixed", 0)
    return round(fees, 2)


def full_pricing(
    spec: MirrorSpec,
    bom: BOM,
    tier: str,
    platform_key: str,
    platform_config: dict,
    markup_multiplier: float = 2.5,
    shipping_cost: float = 8.0,
) -> PricingResult:
    """Run the full pricing pipeline."""
    line_items, material_cost = calculate_cogs(spec, bom, tier)

    labor = bom.labor_per_unit
    energy = bom.energy_per_unit
    overhead = bom.overhead_per_unit
    cogs = material_cost + labor + energy + overhead

    sale_price = round(cogs * markup_multiplier, 2)
    fees = calculate_platform_fees(sale_price, platform_config)
    customer_price = round(sale_price + shipping_cost, 2)
    net_revenue = round(sale_price - fees - cogs, 2)
    margin_pct = round((net_revenue / sale_price) * 100, 1) if sale_price > 0 else 0.0

    return PricingResult(
        width_in=spec.width_in,
        height_in=spec.height_in,
        tier=tier,
        platform=platform_key,
        line_items=line_items,
        material_cost=material_cost,
        labor_cost=labor,
        energy_cost=energy,
        overhead_cost=overhead,
        cogs=round(cogs, 2),
        markup_multiplier=markup_multiplier,
        sale_price=sale_price,
        platform_fees=fees,
        shipping_cost=shipping_cost,
        customer_price=customer_price,
        net_revenue=net_revenue,
        margin_pct=margin_pct,
    )
