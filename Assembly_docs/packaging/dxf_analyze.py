"""Parse CEFbox reference DXFs and recover parametric formulas.

Reads the 10 reference dielines from Assembly_docs/Packaging_Templates/
(gitignored, paid CEFbox content), extracts geometry per DXF layer
('cuttingLine' = cuts, 'foldingLine' = scores), and diffs samples 2-5
against sample 1 for each template to recover ∂edge/∂param.

Run:  python dxf_analyze.py
"""

import os
import re
from collections import defaultdict, Counter
from dataclasses import dataclass
from pathlib import Path

import ezdxf


REFS_DIR = Path(r"C:\Users\bowhi\Desktop\Independent_Study\Assembly_docs\Packaging_Templates")


@dataclass
class DxfSample:
    template: str           # 'mailer' or 'insert'
    label: str              # 'baseline', 'vary_L', etc.
    L: float; W: float; H: float; t: float
    SL: float = 0.0; SW: float = 0.0; SH: float = 0.0; IH: float = 0.0
    path: Path = None

    @property
    def name(self) -> str:
        return f"{self.template}/{self.label}"


def parse_filename(p: Path) -> DxfSample:
    name = p.stem
    if name.startswith("mailer_"):
        # mailer_300x200x80_t1.5
        m = re.match(r"mailer_(\d+)x(\d+)x(\d+)_t([\d.]+)", name)
        L, W, H, t = int(m.group(1)), int(m.group(2)), int(m.group(3)), float(m.group(4))
        return DxfSample("mailer", "", L, W, H, t, path=p)
    elif name.startswith("insert_"):
        # insert_300x200x80_t1.5_slot150x150x25_IH55
        m = re.match(
            r"insert_(\d+)x(\d+)x(\d+)_t([\d.]+)_slot(\d+)x(\d+)x(\d+)_IH(\d+)",
            name,
        )
        return DxfSample(
            "insert", "",
            L=int(m.group(1)), W=int(m.group(2)), H=int(m.group(3)),
            t=float(m.group(4)),
            SL=int(m.group(5)), SW=int(m.group(6)), SH=int(m.group(7)),
            IH=int(m.group(8)),
            path=p,
        )
    raise ValueError(f"unknown filename: {name}")


def classify_sample(s: DxfSample, baseline: DxfSample) -> str:
    """Label a sample relative to the baseline."""
    if (s.L, s.W, s.H, s.t, s.SL, s.SW, s.SH) == (
        baseline.L, baseline.W, baseline.H, baseline.t,
        baseline.SL, baseline.SW, baseline.SH):
        return "baseline"
    diffs = []
    if s.L != baseline.L: diffs.append(f"L{baseline.L}->{s.L}")
    if s.W != baseline.W: diffs.append(f"W{baseline.W}->{s.W}")
    if s.H != baseline.H: diffs.append(f"H{baseline.H}->{s.H}")
    if s.t != baseline.t: diffs.append(f"t{baseline.t}->{s.t}")
    if s.SL != baseline.SL: diffs.append(f"SL{baseline.SL}->{s.SL}")
    if s.SW != baseline.SW: diffs.append(f"SW{baseline.SW}->{s.SW}")
    if s.SH != baseline.SH: diffs.append(f"SH{baseline.SH}->{s.SH}")
    return ",".join(diffs)


def extract_geometry(doc):
    """Return {layer_name: {'lines': [...], 'arcs': [...], 'polylines': [...]}}."""
    out = defaultdict(lambda: {"lines": [], "arcs": [], "polylines": [], "points": []})
    for e in doc.modelspace():
        layer = e.dxf.layer
        et = e.dxftype()
        if et == "LINE":
            out[layer]["lines"].append((
                (e.dxf.start.x, e.dxf.start.y),
                (e.dxf.end.x, e.dxf.end.y),
            ))
        elif et == "ARC":
            out[layer]["arcs"].append({
                "center": (e.dxf.center.x, e.dxf.center.y),
                "radius": e.dxf.radius,
                "start_angle": e.dxf.start_angle,
                "end_angle": e.dxf.end_angle,
            })
        elif et == "LWPOLYLINE":
            pts = [(p[0], p[1]) for p in e.get_points()]
            out[layer]["polylines"].append({"points": pts, "closed": e.closed})
        elif et == "POINT":
            out[layer]["points"].append((e.dxf.location.x, e.dxf.location.y))
    return out


def all_points(geom):
    """Flatten all geometry to a list of (x, y) for bbox / inspection."""
    pts = []
    for layer in geom.values():
        for (a, b) in layer["lines"]:
            pts.extend([a, b])
        for arc in layer["arcs"]:
            cx, cy, r = arc["center"][0], arc["center"][1], arc["radius"]
            # Sample 8 points around the arc for bbox purposes
            import math
            sa = math.radians(arc["start_angle"])
            ea = math.radians(arc["end_angle"])
            if ea < sa: ea += 2 * math.pi
            for i in range(9):
                ang = sa + (ea - sa) * i / 8
                pts.append((cx + r * math.cos(ang), cy + r * math.sin(ang)))
        for poly in layer["polylines"]:
            pts.extend(poly["points"])
        pts.extend(layer["points"])
    return pts


def bbox(points):
    if not points: return (0, 0, 0, 0)
    xs, ys = zip(*points)
    return (min(xs), min(ys), max(xs), max(ys))


def unique_axis_positions(lines, axis: str, eps: float = 0.5):
    """For LINES that are horizontal (axis='h') or vertical (axis='v'),
    return the set of unique constant-axis coordinates."""
    coords = []
    for ((x0, y0), (x1, y1)) in lines:
        if axis == "h" and abs(y0 - y1) < eps:
            coords.append(round((y0 + y1) / 2, 2))
        elif axis == "v" and abs(x0 - x1) < eps:
            coords.append(round((x0 + x1) / 2, 2))
    # Dedupe close values
    out = []
    for c in sorted(coords):
        if not out or abs(c - out[-1]) > eps:
            out.append(c)
    return out


def summarize_sample(s: DxfSample):
    doc = ezdxf.readfile(s.path)
    geom = extract_geometry(doc)
    pts = all_points(geom)
    bb = bbox(pts)
    bw = bb[2] - bb[0]
    bh = bb[3] - bb[1]

    cut_lines = geom["cuttingLine"]["lines"]
    fold_lines = geom["foldingLine"]["lines"]
    cut_arcs = geom["cuttingLine"]["arcs"]
    cut_polys = geom["cuttingLine"]["polylines"]

    fold_h = unique_axis_positions(fold_lines, "h")
    fold_v = unique_axis_positions(fold_lines, "v")

    return {
        "sample": s,
        "bbox": bb,
        "bbox_w": bw,
        "bbox_h": bh,
        "n_cut_lines": len(cut_lines),
        "n_cut_arcs": len(cut_arcs),
        "n_cut_polys": len(cut_polys),
        "n_fold_lines": len(fold_lines),
        "fold_h_y": fold_h,
        "fold_v_x": fold_v,
        "cut_lines": cut_lines,
        "fold_lines": fold_lines,
        "cut_arcs": cut_arcs,
        "cut_polys": cut_polys,
    }


def print_template_summary(template: str, samples_data):
    print(f"\n{'='*70}")
    print(f"= {template.upper()} — {len(samples_data)} samples")
    print(f"{'='*70}")

    base = samples_data[0]
    print(f"\nBaseline label: {base['sample'].label}")
    print(f"  inputs: L={base['sample'].L} W={base['sample'].W} H={base['sample'].H}"
          f" t={base['sample'].t}", end="")
    if template == "insert":
        s = base['sample']
        print(f" SL={s.SL} SW={s.SW} SH={s.SH} IH={s.IH}", end="")
    print()
    print(f"  bbox: {base['bbox_w']:.1f} x {base['bbox_h']:.1f} mm")
    print(f"  cut entities: {base['n_cut_lines']} lines, "
          f"{base['n_cut_arcs']} arcs, {base['n_cut_polys']} polylines")
    print(f"  fold lines: {base['n_fold_lines']}  "
          f"(horizontal scores at y={base['fold_h_y']})")
    print(f"                       "
          f"(vertical scores at x={base['fold_v_x']})")

    # Diff table — bbox and fold-line positions vs baseline
    print(f"\n{'sample':<35} {'bbox W':>9} {'dw':>7} {'bbox H':>9} {'dh':>7}"
          f"  {'n_cut':>6} {'n_fold':>7}")
    print("-" * 90)
    for d in samples_data:
        s = d['sample']
        dw = d['bbox_w'] - base['bbox_w']
        dh = d['bbox_h'] - base['bbox_h']
        label = s.label or "?"
        print(f"  {label:<33} {d['bbox_w']:>9.1f} {dw:>+7.1f} "
              f"{d['bbox_h']:>9.1f} {dh:>+7.1f}  "
              f"{d['n_cut_lines']:>6} {d['n_fold_lines']:>7}")

    # Show how fold-line positions shift relative to baseline
    print(f"\nFold-line position deltas vs baseline:")
    for d in samples_data[1:]:
        s = d['sample']
        bh_diff = [round(a - b, 2) for a, b in zip(d['fold_h_y'], base['fold_h_y'])]
        bv_diff = [round(a - b, 2) for a, b in zip(d['fold_v_x'], base['fold_v_x'])]
        same_count_h = len(d['fold_h_y']) == len(base['fold_h_y'])
        same_count_v = len(d['fold_v_x']) == len(base['fold_v_x'])
        print(f"  {s.label}")
        if same_count_h:
            print(f"    dy horizontal fold positions: {bh_diff}")
        else:
            print(f"    horizontal fold count changed: {len(base['fold_h_y'])} -> {len(d['fold_h_y'])}")
            print(f"    new y positions: {d['fold_h_y']}")
        if same_count_v:
            print(f"    dx vertical fold positions:   {bv_diff}")
        else:
            print(f"    vertical fold count changed: {len(base['fold_v_x'])} -> {len(d['fold_v_x'])}")
            print(f"    new x positions: {d['fold_v_x']}")


def main():
    files = sorted(REFS_DIR.glob("*.dxf"))
    print(f"Found {len(files)} DXF files in {REFS_DIR}")

    samples = [parse_filename(p) for p in files]

    # Group by template, pick baseline (the 300x200x80 t1.5 standard slot)
    by_template = defaultdict(list)
    for s in samples:
        by_template[s.template].append(s)

    for template, samps in by_template.items():
        # Baseline = the sample with L=300, W=200, H=80, t=1.5 (and standard slot for inserts)
        if template == "mailer":
            baseline = next(s for s in samps
                            if (s.L, s.W, s.H, s.t) == (300, 200, 80, 1.5))
        else:
            baseline = next(s for s in samps
                            if (s.L, s.W, s.H, s.t, s.SL, s.SW, s.SH) ==
                               (300, 200, 80, 1.5, 150, 150, 25))

        for s in samps:
            s.label = classify_sample(s, baseline)

        # Put baseline first, others ordered by their label
        samps.sort(key=lambda s: (s.label != "baseline", s.label))

        samples_data = [summarize_sample(s) for s in samps]
        print_template_summary(template, samples_data)


if __name__ == "__main__":
    main()
