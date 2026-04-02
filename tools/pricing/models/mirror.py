"""Mirror geometry — converts dimensions into material quantities."""

from __future__ import annotations

import math
from dataclasses import dataclass


@dataclass
class MirrorSpec:
    """Computed material requirements for a given mirror size."""

    width_in: float
    height_in: float
    perimeter_in: float
    perimeter_mm: float
    area_sq_in: float
    led_count: int
    psu_amps: float
    psu_watts: float

    @property
    def perimeter_scale(self) -> float:
        """Scale factor vs. a 6x6 reference perimeter."""
        return self.perimeter_in / (6 * 4)

    @property
    def area_scale(self) -> float:
        """Scale factor vs. a 6x6 reference area."""
        return self.area_sq_in / (6 * 6)


def compute_mirror_spec(
    width_in: float,
    height_in: float,
    led_density_per_meter: int = 60,
    led_power_draw_amps: float = 0.06,
    led_voltage: float = 5.0,
) -> MirrorSpec:
    """Compute all derived quantities from mirror width and height."""
    perimeter_in = 2 * (width_in + height_in)
    perimeter_mm = perimeter_in * 25.4
    area_sq_in = width_in * height_in

    led_pitch_mm = 1000.0 / led_density_per_meter
    led_count = math.ceil(perimeter_mm / led_pitch_mm)

    psu_amps = led_count * led_power_draw_amps
    psu_watts = psu_amps * led_voltage

    return MirrorSpec(
        width_in=width_in,
        height_in=height_in,
        perimeter_in=perimeter_in,
        perimeter_mm=perimeter_mm,
        area_sq_in=area_sq_in,
        led_count=led_count,
        psu_amps=psu_amps,
        psu_watts=psu_watts,
    )
