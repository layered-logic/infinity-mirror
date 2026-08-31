"""Colored SVG -> black SVG by fill-color selection.

Bridges arbitrary multi-color art into the main pipeline, which expects a
monochrome SVG where every <path> is part of the cut region. Two modes:

- list_colors(svg) -> enumerate the distinct fill colors present, ranked by
  visual presence (summed path bbox area), so the user can see what's in the
  file before deciding what to keep.
- select_to_black(svg, out, colors, ...) -> keep elements whose resolved fill
  matches (or, with invert=True, doesn't match) the given color list, then
  emit a black-filled SVG ready for `python -m im_svg_maker`.

All transform / CSS / inheritance fill resolution is delegated to svgelements
via `SVG.parse(..., reify=True)`. The emitted SVG mirrors raster.py's format
(`<path fill="#000000" fill-rule="evenodd"/>` only) so the main pipeline's
existing parser reads it without changes.

Stroke-only paths (`fill="none"`) and gradient/pattern fills are skipped — the
"which color is the cut" UX problem only has a well-defined answer for solid
fills.
"""
from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
from typing import Iterator

from svgelements import SVG, Color, Path as SvgPath, Shape


@dataclass(frozen=True)
class ColorKey:
    """RGB triple keyed for grouping + tolerance-based matching."""
    r: int
    g: int
    b: int

    @classmethod
    def from_color(cls, color) -> "ColorKey | None":
        """Try to extract an (r,g,b) triple from an svgelements Color-like value."""
        if color is None:
            return None
        if isinstance(color, str):
            if color.lower() in ("none", "transparent"):
                return None
            try:
                color = Color(color)
            except Exception:
                return None
        r = getattr(color, "red", None)
        g = getattr(color, "green", None)
        b = getattr(color, "blue", None)
        if r is None or g is None or b is None:
            return None
        return cls(int(r), int(g), int(b))

    def hex(self) -> str:
        return f"#{self.r:02X}{self.g:02X}{self.b:02X}"


@dataclass
class ColorStats:
    color: ColorKey
    n_paths: int
    bbox_area: float


def parse_color_spec(spec: str) -> ColorKey:
    """Parse a user-supplied color spec (hex, named, rgb(...)) into a ColorKey."""
    try:
        c = Color(spec)
    except Exception as exc:
        raise ValueError(f"Could not parse color {spec!r}: {exc}") from exc
    key = ColorKey.from_color(c)
    if key is None:
        raise ValueError(f"Color {spec!r} did not resolve to an RGB triple")
    return key


def _iter_filled_paths(svg: SVG) -> Iterator[tuple[SvgPath, ColorKey]]:
    """Yield (path-with-baked-transforms, color-key) for each solid-fill shape."""
    for el in svg.elements():
        if not isinstance(el, Shape):
            continue
        fill = getattr(el, "fill", None)
        key = ColorKey.from_color(fill)
        if key is None:
            continue
        if isinstance(el, SvgPath):
            path = el
        else:
            try:
                path = SvgPath(el)
            except Exception:
                continue
        yield path, key


def _resolve_viewport(svg: SVG) -> tuple[float, float, float, float]:
    """Return (min_x, min_y, width, height) for the output viewBox."""
    vb = getattr(svg, "viewbox", None)
    if vb is not None:
        try:
            return (float(vb.x), float(vb.y), float(vb.width), float(vb.height))
        except Exception:
            pass
    try:
        w = float(getattr(svg, "width", 0) or 0)
        h = float(getattr(svg, "height", 0) or 0)
        if w > 0 and h > 0:
            return (0.0, 0.0, w, h)
    except Exception:
        pass
    # Fallback: union of all element bboxes.
    minx = miny = float("inf")
    maxx = maxy = float("-inf")
    for el in svg.elements():
        if not isinstance(el, Shape):
            continue
        bbox = el.bbox()
        if bbox is None:
            continue
        x0, y0, x1, y1 = bbox
        minx = min(minx, x0); miny = min(miny, y0)
        maxx = max(maxx, x1); maxy = max(maxy, y1)
    if minx == float("inf"):
        raise ValueError("SVG has no resolvable extent (no viewBox, no width/height, no geometry).")
    return (minx, miny, maxx - minx, maxy - miny)


def list_colors(svg_path: Path) -> list[ColorStats]:
    """Enumerate distinct solid fills in `svg_path`, ranked by summed bbox area."""
    svg = SVG.parse(str(svg_path), reify=True)
    accum: dict[ColorKey, tuple[int, float]] = {}
    for path, key in _iter_filled_paths(svg):
        bbox = path.bbox()
        if bbox is None:
            area = 0.0
        else:
            x0, y0, x1, y1 = bbox
            area = max(0.0, (x1 - x0) * (y1 - y0))
        n, a = accum.get(key, (0, 0.0))
        accum[key] = (n + 1, a + area)
    return [
        ColorStats(color=k, n_paths=n, bbox_area=a)
        for k, (n, a) in sorted(accum.items(), key=lambda kv: -kv[1][1])
    ]


def _matches(key: ColorKey, targets: list[ColorKey], tolerance: int) -> bool:
    if not targets:
        return False
    for t in targets:
        if max(abs(key.r - t.r), abs(key.g - t.g), abs(key.b - t.b)) <= tolerance:
            return True
    return False


def select_to_black(
    svg_path: Path,
    out_svg: Path,
    colors: list[ColorKey],
    *,
    tolerance: int = 0,
    invert: bool = False,
) -> int:
    """Select solid-fill shapes by color, emit a black SVG, return paths kept.

    `colors` is the match set. `invert=True` keeps the complement (everything
    that doesn't match) — useful when the simplest description of the design
    is "everything that isn't the background." `tolerance` is L-infinity over
    the (r,g,b) channels in 0-255 units.
    """
    svg = SVG.parse(str(svg_path), reify=True)
    vx, vy, vw, vh = _resolve_viewport(svg)

    d_strings: list[str] = []
    for path, key in _iter_filled_paths(svg):
        hit = _matches(key, colors, tolerance)
        if hit == invert:
            continue
        d = path.d()
        if d:
            d_strings.append(d)

    parts = [
        f'<svg xmlns="http://www.w3.org/2000/svg" '
        f'width="{vw}" height="{vh}" viewBox="{vx} {vy} {vw} {vh}">\n'
    ]
    for d in d_strings:
        parts.append(f'  <path d="{d}" fill="#000000" fill-rule="evenodd"/>\n')
    parts.append("</svg>\n")
    out_svg.write_text("".join(parts), encoding="utf-8")
    return len(d_strings)
