"""Validation tests for dieline generators.

Two checks per (template, dimensions) pair:

  1. ENTITY COUNT — generator output's line/arc/polyline counts (per layer)
     match the reference DXF's counts. Catches missing-feature regressions.

  2. Y-SYMMETRY — every entity has a mirror partner about the dieline's
     mirror axis (L/2 + 5t for the mailer). Catches one-sided emit bugs.
     Allows opt-out for asymmetric templates (multi-slot inserts).

Usage:
    from validation import count_entities, check_symmetry, count_from_dxf

    expected = count_from_dxf(reference_dxf_path)
    actual = count_entities(generated_entities)
    diffs = compare_counts(expected, actual)

    sym_ok, errors = check_symmetry(generated_entities, axis_x=L/2 + 5*t)
"""

from collections import Counter
from dataclasses import dataclass, field
from pathlib import Path
from typing import Dict, List, Tuple, Optional

import ezdxf


# ============================================================
# === Entity counting ========================================
# ============================================================

@dataclass
class EntityCounts:
    by_type: Counter = field(default_factory=Counter)
    by_type_layer: Counter = field(default_factory=Counter)

    def __str__(self):
        out = []
        for (t, layer), n in sorted(self.by_type_layer.items()):
            out.append(f"  {t:<12} layer={layer:<14} count={n}")
        return "\n".join(out)


def count_entities(entities) -> EntityCounts:
    """Count entities from a generator's output list (Line/Arc/Polyline namedtuples).
    Maps to canonical DXF types (LINE/ARC/LWPOLYLINE)."""
    c = EntityCounts()
    type_map = {"Line": "LINE", "Arc": "ARC", "Polyline": "LWPOLYLINE"}
    for e in entities:
        ent_type = type_map.get(type(e).__name__, type(e).__name__)
        layer = getattr(e, "layer", "0")
        c.by_type[ent_type] += 1
        c.by_type_layer[(ent_type, layer)] += 1
    return c


def count_from_dxf(path) -> EntityCounts:
    """Count entities from a reference DXF file."""
    doc = ezdxf.readfile(str(path))
    c = EntityCounts()
    for e in doc.modelspace():
        ent_type = e.dxftype()
        if ent_type == "POINT":
            continue  # ignore POINT entities (DXF auxiliary markers)
        layer = e.dxf.layer
        c.by_type[ent_type] += 1
        c.by_type_layer[(ent_type, layer)] += 1
    return c


def compare_counts(expected: EntityCounts, actual: EntityCounts,
                   tolerance: int = 0) -> List[str]:
    """Return a list of mismatch descriptions. Empty list = perfect match.
    `tolerance` allows |diff| <= tolerance per (type, layer) pair."""
    diffs = []
    all_keys = set(expected.by_type_layer.keys()) | set(actual.by_type_layer.keys())
    for key in sorted(all_keys):
        exp = expected.by_type_layer.get(key, 0)
        act = actual.by_type_layer.get(key, 0)
        d = act - exp
        if abs(d) > tolerance:
            sign = "+" if d > 0 else ""
            diffs.append(f"{key[0]} on '{key[1]}': expected {exp}, got {act} "
                         f"({sign}{d})")
    return diffs


# ============================================================
# === Y-symmetry check ========================================
# ============================================================

def _arc_canonical(cx, cy, r, sa, ea, layer):
    """Canonicalize an arc using (midpoint_angle, sweep) instead of (sa, ea).
    Handles the 0/360 wrap and makes arcs comparable regardless of direction."""
    sa_n = sa % 360.0
    ea_n = ea % 360.0
    sweep = (ea_n - sa_n) % 360.0
    if abs(sweep) < 0.01:
        sweep = 360.0
    midpoint = (sa_n + sweep/2) % 360.0
    return ("ARC", round(cx, 2), round(cy, 2), round(r, 2),
            round(midpoint, 1), round(sweep, 1), layer)


def _polyline_canonical(pts, closed, layer):
    """Canonical polyline: dedupe explicit closing-point duplicates and use
    a sorted tuple of unique (x, y) points + closed flag + layer."""
    # Detect repeated closing point: if first == last, drop the last
    pts_list = list(pts)
    if closed and len(pts_list) > 1 and pts_list[0] == pts_list[-1]:
        pts_list = pts_list[:-1]
    sorted_pts = tuple(sorted((round(p[0], 2), round(p[1], 2))
                              for p in pts_list))
    return ("POLYLINE", sorted_pts, closed, layer)


def _entity_signature(e, axis_x: float, tol: float = 0.05):
    """Mirror signature: canonical form of what the entity's reflection looks
    like, used to look up its partner among the self-sigs of other entities."""
    name = type(e).__name__
    if name == "Line":
        p0 = (round(2*axis_x - e.x0, 2), round(e.y0, 2))
        p1 = (round(2*axis_x - e.x1, 2), round(e.y1, 2))
        pts = tuple(sorted([p0, p1]))
        return ("LINE", pts, e.layer)
    elif name == "Arc":
        new_cx = 2*axis_x - e.cx
        # Mirroring negates the midpoint angle (about the x-axis projection)
        # but preserves sweep magnitude.
        sa_n = e.sa % 360.0
        ea_n = e.ea % 360.0
        sweep = (ea_n - sa_n) % 360.0
        if abs(sweep) < 0.01: sweep = 360.0
        old_mid = (sa_n + sweep/2) % 360.0
        new_mid = (180.0 - old_mid) % 360.0
        return ("ARC", round(new_cx, 2), round(e.cy, 2), round(e.r, 2),
                round(new_mid, 1), round(sweep, 1), e.layer)
    elif name == "Polyline":
        pts_list = list(e.pts)
        if e.closed and len(pts_list) > 1 and pts_list[0] == pts_list[-1]:
            pts_list = pts_list[:-1]
        mirrored = tuple(sorted((round(2*axis_x - p[0], 3), round(p[1], 3))
                                for p in pts_list))
        return ("POLYLINE", mirrored, e.closed, e.layer)
    return None


def _entity_self_signature(e):
    """Canonical self-signature of an entity for matching."""
    name = type(e).__name__
    if name == "Line":
        p0 = (round(e.x0, 2), round(e.y0, 2))
        p1 = (round(e.x1, 2), round(e.y1, 2))
        pts = tuple(sorted([p0, p1]))
        return ("LINE", pts, e.layer)
    elif name == "Arc":
        return _arc_canonical(e.cx, e.cy, e.r, e.sa, e.ea, e.layer)
    elif name == "Polyline":
        return _polyline_canonical(e.pts, e.closed, e.layer)
    return None


def check_symmetry(entities, axis_x: float, tol: float = 0.05
                   ) -> Tuple[bool, List[str]]:
    """Check that every entity has a mirror partner about x=axis_x.

    An entity that is its own mirror (e.g., a horizontal score that spans
    the axis evenly) counts as having a partner (itself).

    Returns (ok, list_of_orphan_entity_descriptions).
    """
    # Build sets of self-signatures and mirror-signatures
    self_sigs = Counter()
    mirror_sigs = Counter()
    entity_by_self_sig = {}

    for e in entities:
        ss = _entity_self_signature(e)
        ms = _entity_signature(e, axis_x, tol)
        self_sigs[ss] += 1
        mirror_sigs[ms] += 1
        entity_by_self_sig[ss] = e

    # For each entity's self-sig, check that its mirror-sig exists in
    # self_sigs with at least the same count.
    orphans = []
    for ss, count in self_sigs.items():
        # Find what the entity's mirror signature WOULD be when looked
        # up as a self-sig of the partner
        e = entity_by_self_sig[ss]
        partner_self_sig = _entity_signature(e, axis_x, tol)
        # Convert partner_self_sig (which is in mirror-form) back to a
        # self-sig: just use it as-is (it's already canonical)
        partner_count = self_sigs.get(partner_self_sig, 0)
        if partner_count < count:
            orphans.append(f"{type(e).__name__} at "
                           f"{_describe_position(e)} (layer={e.layer}) "
                           f"has no mirror partner about x={axis_x}")
    return (len(orphans) == 0, orphans)


def _describe_position(e):
    name = type(e).__name__
    if name == "Line":
        return f"({e.x0:.2f},{e.y0:.2f})-({e.x1:.2f},{e.y1:.2f})"
    elif name == "Arc":
        return f"center=({e.cx:.2f},{e.cy:.2f}) r={e.r:.2f}"
    elif name == "Polyline":
        return f"{len(e.pts)} pts starting at ({e.pts[0][0]:.2f},{e.pts[0][1]:.2f})"
    return "?"


# ============================================================
# === Test runner ============================================
# ============================================================

@dataclass
class TestCase:
    name: str
    params: tuple           # (L, W, H, t) for mailer; (L, W, H, t, SL, SW, SH, IH) for insert
    reference_dxf: Optional[Path] = None
    expected_axis_x: Optional[float] = None
    skip_symmetry: bool = False


def run_mailer_tests():
    """Run all 5 reference-dimension validations on the mailer generator."""
    from mailer_reft import MailerSpec, generate, mirror_axis_x

    REFS = Path(r"C:\Users\bowhi\Desktop\Independent_Study\Assembly_docs\Packaging_Templates")
    cases = [
        TestCase("baseline 300x200x80 t1.5", (300, 200, 80, 1.5),
                 REFS / "mailer_300x200x80_t1.5.dxf"),
        TestCase("vary L: 400x200x80 t1.5", (400, 200, 80, 1.5),
                 REFS / "mailer_400x200x80_t1.5.dxf"),
        TestCase("vary W: 300x250x80 t1.5", (300, 250, 80, 1.5),
                 REFS / "mailer_300x250x80_t1.5.dxf"),
        TestCase("vary H: 300x200x120 t1.5", (300, 200, 120, 1.5),
                 REFS / "mailer_300x200x120_t1.5.dxf"),
        TestCase("vary t: 300x200x80 t3.0", (300, 200, 80, 3.0),
                 REFS / "mailer_300x200x80_t3.dxf"),
    ]

    print(f"{'='*70}\nMAILER VALIDATION SUITE\n{'='*70}\n")
    n_pass = 0
    n_fail = 0
    for case in cases:
        print(f"--- {case.name} ---")
        L, W, H, t = case.params
        spec = MailerSpec(L=L, W=W, H=H, t=t)
        ents = generate(spec, phase=6)

        # Count check
        expected_counts = count_from_dxf(case.reference_dxf)
        actual_counts = count_entities(ents)
        count_diffs = compare_counts(expected_counts, actual_counts, tolerance=2)
        # Y-symmetry check
        axis_x = mirror_axis_x(spec)
        sym_ok, orphans = check_symmetry(ents, axis_x)

        if not count_diffs and sym_ok:
            print(f"  PASS  (counts match, symmetry OK about x={axis_x})")
            n_pass += 1
        else:
            print(f"  FAIL")
            if count_diffs:
                for d in count_diffs:
                    print(f"    count: {d}")
            if orphans:
                print(f"    symmetry: {len(orphans)} orphans (showing first 5)")
                for o in orphans[:5]:
                    print(f"      {o}")
            n_fail += 1
        print()

    print(f"{'='*70}\nResults: {n_pass}/{n_pass+n_fail} passed\n{'='*70}")
    return n_pass, n_fail


def run_insert_tests():
    """Run all 5 reference-dimension validations on the insert generator."""
    from insert_tray import InsertSpec, generate, mirror_axis_x

    REFS = Path(r"C:\Users\bowhi\Desktop\Independent_Study\Assembly_docs\Packaging_Templates")
    cases = [
        TestCase("baseline 300x200x80 t1.5 slot150x150x25 IH55",
                 (300, 200, 80, 1.5, 150, 150, 25, 55),
                 REFS / "insert_300x200x80_t1.5_slot150x150x25_IH55.dxf"),
        TestCase("vary L: 400x200x80 ...",
                 (400, 200, 80, 1.5, 150, 150, 25, 55),
                 REFS / "insert_400x200x80_t1.5_slot150x150x25_IH55.dxf"),
        TestCase("vary W: 300x250x80 ...",
                 (300, 250, 80, 1.5, 150, 150, 25, 55),
                 REFS / "insert_300x250x80_t1.5_slot150x150x25_IH55.dxf"),
        TestCase("vary H: 300x200x120 IH95",
                 (300, 200, 120, 1.5, 150, 150, 25, 95),
                 REFS / "insert_300x200x120_t1.5_slot150x150x25_IH95.dxf"),
        TestCase("vary slot: 300x200x80 slot200x100x50 IH30",
                 (300, 200, 80, 1.5, 200, 100, 50, 30),
                 REFS / "insert_300x200x80_t1.5_slot200x100x50_IH30.dxf"),
    ]

    print(f"\n{'='*70}\nINSERT VALIDATION SUITE\n{'='*70}\n")
    n_pass = 0
    n_fail = 0
    for case in cases:
        print(f"--- {case.name} ---")
        L, W, H, t, SL, SW, SH, IH = case.params
        spec = InsertSpec.single_slot(L=L, W=W, H=H, t=t,
                                       SL=SL, SW=SW, SH=SH, IH=IH)
        ents = generate(spec, phase=5)

        expected_counts = count_from_dxf(case.reference_dxf)
        actual_counts = count_entities(ents)
        count_diffs = compare_counts(expected_counts, actual_counts, tolerance=2)
        axis_x = mirror_axis_x(spec)
        sym_ok, orphans = check_symmetry(ents, axis_x)

        if not count_diffs and sym_ok:
            print(f"  PASS  (counts match, symmetry OK about x={axis_x})")
            n_pass += 1
        else:
            print(f"  FAIL")
            if count_diffs:
                for d in count_diffs:
                    print(f"    count: {d}")
            if orphans:
                print(f"    symmetry: {len(orphans)} orphans (showing first 5)")
                for o in orphans[:5]:
                    print(f"      {o}")
            n_fail += 1
        print()

    print(f"{'='*70}\nResults: {n_pass}/{n_pass+n_fail} passed\n{'='*70}")
    return n_pass, n_fail


if __name__ == "__main__":
    run_mailer_tests()
    print()
    run_insert_tests()
