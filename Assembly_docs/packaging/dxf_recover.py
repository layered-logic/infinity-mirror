"""Recover parametric formulas for every coordinate in the CEFbox dielines.

For each unique x and y in the baseline geometry, find the matching
coordinate in each variation sample (by sorted-index position), then
solve the linear system c = a*L + b*W + c_h*H + d*t + e (mailer) or
c = a*L + b*W + c_h*H + d*t + slot_terms + e (insert).

Inputs varied one-at-a-time off a shared baseline, so each variation
sample isolates one parameter's coefficient.

Run:  python dxf_recover.py
Output: coords_<template>.json — formula per coordinate, used by the
        SVG generator that comes next.
"""

import json
import math
import re
from collections import defaultdict
from dataclasses import dataclass, asdict
from pathlib import Path

import ezdxf


REFS_DIR = Path(r"C:\Users\bowhi\Desktop\Independent_Study\Assembly_docs\Packaging_Templates")
OUT_DIR = Path(__file__).parent

DEDUPE_TOL = 0.05   # mm — merge coordinates within this distance


# ============================================================
# === Sample loading =========================================
# ============================================================

@dataclass
class Sample:
    template: str
    L: float; W: float; H: float; t: float
    SL: float = 0.0; SW: float = 0.0; SH: float = 0.0; IH: float = 0.0
    path: Path = None
    label: str = ""


def parse_filename(p: Path) -> Sample:
    name = p.stem
    if name.startswith("mailer_"):
        m = re.match(r"mailer_(\d+)x(\d+)x(\d+)_t([\d.]+)", name)
        return Sample("mailer", float(m.group(1)), float(m.group(2)),
                      float(m.group(3)), float(m.group(4)), path=p)
    if name.startswith("insert_"):
        m = re.match(
            r"insert_(\d+)x(\d+)x(\d+)_t([\d.]+)_slot(\d+)x(\d+)x(\d+)_IH(\d+)",
            name)
        return Sample("insert",
                      L=float(m.group(1)), W=float(m.group(2)),
                      H=float(m.group(3)), t=float(m.group(4)),
                      SL=float(m.group(5)), SW=float(m.group(6)),
                      SH=float(m.group(7)), IH=float(m.group(8)),
                      path=p)
    raise ValueError(f"unknown filename: {name}")


# ============================================================
# === Coordinate extraction ==================================
# ============================================================

def arc_points(arc, n=33):
    cx, cy = arc["center"]
    r = arc["radius"]
    sa = math.radians(arc["start_angle"])
    ea = math.radians(arc["end_angle"])
    if ea < sa:
        ea += 2 * math.pi
    return [(cx + r * math.cos(sa + (ea - sa) * i / (n - 1)),
             cy + r * math.sin(sa + (ea - sa) * i / (n - 1)))
            for i in range(n)]


def extract_all_endpoints(doc):
    """Return list of (x, y) for every distinct geometric vertex.

    EXCLUDES arc-sampled points (those drift between samples and would
    misalign sorted-index matching). Arcs are recovered separately by
    extracting (center, radius, start_angle, end_angle) per sample.
    """
    pts = []
    for e in doc.modelspace():
        et = e.dxftype()
        if et == "LINE":
            pts.append((e.dxf.start.x, e.dxf.start.y))
            pts.append((e.dxf.end.x, e.dxf.end.y))
        elif et == "LWPOLYLINE":
            pts.extend((p[0], p[1]) for p in e.get_points())
        elif et == "POINT":
            pts.append((e.dxf.location.x, e.dxf.location.y))
        # ARC endpoints (start/end of the arc itself) — these are exact
        elif et == "ARC":
            cx, cy, r = e.dxf.center.x, e.dxf.center.y, e.dxf.radius
            sa = math.radians(e.dxf.start_angle)
            ea = math.radians(e.dxf.end_angle)
            pts.append((cx + r * math.cos(sa), cy + r * math.sin(sa)))
            pts.append((cx + r * math.cos(ea), cy + r * math.sin(ea)))
    return pts


def unique_sorted(values, tol=DEDUPE_TOL):
    """Sort + dedupe close values."""
    out = []
    for v in sorted(values):
        if not out or abs(v - out[-1]) > tol:
            out.append(v)
    return out


# ============================================================
# === Formula recovery =======================================
# ============================================================

def recover_formula(baseline_val, baseline_params, variation_vals, variation_params,
                    param_names):
    """Solve c = a*L + b*W + c_h*H + d*t (+ slot terms if insert) + const.

    Each variation sample varies exactly ONE primary param (L, W, H, or t).
    The insert's slot sample varies multiple slot params at once — for any
    coord that DOES change in that sample, we report it as 'slot_dependent'
    with the raw delta, since 1 sample can't disambiguate 4 unknowns.
    """
    n_params = len(baseline_params)
    coefs = [0.0] * n_params
    slot_delta = None  # raw delta in the slot-variation sample
    slot_changed_params = []

    for sval, sparams in zip(variation_vals, variation_params):
        deltas = [sp - bp for sp, bp in zip(sparams, baseline_params)]
        changed = [i for i, d in enumerate(deltas) if abs(d) > 1e-9]
        if len(changed) == 0:
            continue
        # Single-parameter variation — clean coefficient recovery
        primary_changed = [i for i in changed if param_names[i] in ("L","W","H","t")]
        slot_changed = [i for i in changed if param_names[i] in ("SL","SW","SH","IH")]
        if len(primary_changed) == 1 and not slot_changed:
            i = primary_changed[0]
            coefs[i] = (sval - baseline_val) / deltas[i]
        elif slot_changed and not primary_changed:
            # The slot-variation sample. Just record the delta.
            if abs(sval - baseline_val) > 1e-6:
                slot_delta = sval - baseline_val
                slot_changed_params = [(param_names[i], deltas[i]) for i in slot_changed]
        # else: ambiguous — skip

    const = baseline_val - sum(c * p for c, p in zip(coefs, baseline_params))
    return coefs, const, slot_delta, slot_changed_params


def format_formula(coefs, const, param_names, tol=0.005):
    """Pretty-print a formula like '1.0*L + 2.0*t + 0.75'."""
    terms = []
    for c, name in zip(coefs, param_names):
        if abs(c) < tol:
            continue
        if abs(c - 1.0) < tol:
            terms.append(name)
        elif abs(c + 1.0) < tol:
            terms.append(f"-{name}")
        elif abs(c - 0.5) < tol:
            terms.append(f"{name}/2")
        elif abs(c + 0.5) < tol:
            terms.append(f"-{name}/2")
        elif abs(c - 2.0) < tol:
            terms.append(f"2*{name}")
        elif abs(c + 2.0) < tol:
            terms.append(f"-2*{name}")
        elif abs(c - round(c)) < tol:
            terms.append(f"{int(round(c))}*{name}")
        else:
            terms.append(f"{c:+.3f}*{name}")
    if abs(const) > tol or not terms:
        if abs(const - round(const)) < tol:
            terms.append(f"{int(round(const)):+d}")
        else:
            terms.append(f"{const:+.3f}")
    return " + ".join(terms).replace("+ -", "- ").lstrip("+ ")


# ============================================================
# === Per-template recovery ==================================
# ============================================================

def recover_template(samples):
    """Returns a dict with x and y formula tables, validation summary."""
    template = samples[0].template
    if template == "mailer":
        baseline = next(s for s in samples
                        if (s.L, s.W, s.H, s.t) == (300, 200, 80, 1.5))
        params_of = lambda s: (s.L, s.W, s.H, s.t)
        param_names = ["L", "W", "H", "t"]
    else:
        baseline = next(s for s in samples
                        if (s.L, s.W, s.H, s.t, s.SL, s.SW, s.SH) ==
                           (300, 200, 80, 1.5, 150, 150, 25))
        params_of = lambda s: (s.L, s.W, s.H, s.t, s.SL, s.SW, s.SH, s.IH)
        param_names = ["L", "W", "H", "t", "SL", "SW", "SH", "IH"]

    print(f"\n{'#'*70}")
    print(f"# {template.upper()}")
    print(f"#  baseline = {params_of(baseline)}")
    print(f"#  params   = {param_names}")
    print(f"{'#'*70}\n")

    # Load all samples' endpoint clouds
    clouds = {}
    for s in samples:
        doc = ezdxf.readfile(s.path)
        pts = extract_all_endpoints(doc)
        xs = unique_sorted([p[0] for p in pts])
        ys = unique_sorted([p[1] for p in pts])
        clouds[s.path.stem] = {"xs": xs, "ys": ys, "sample": s,
                               "params": params_of(s)}

    # Sanity-check: do all samples have matching coord counts?
    bx_count = len(clouds[baseline.path.stem]["xs"])
    by_count = len(clouds[baseline.path.stem]["ys"])
    print(f"baseline coord count: {bx_count} unique x, {by_count} unique y")
    matched_all = True
    for stem, c in clouds.items():
        if stem == baseline.path.stem:
            continue
        diff_x = len(c["xs"]) - bx_count
        diff_y = len(c["ys"]) - by_count
        marker = "OK " if (diff_x == 0 and diff_y == 0) else "*** "
        print(f"  {marker}{stem}: {len(c['xs'])}x / {len(c['ys'])}y  "
              f"(diff vs baseline: {diff_x:+d}x / {diff_y:+d}y)")
        if diff_x != 0 or diff_y != 0:
            matched_all = False

    # Order variation samples (everything except baseline)
    var_clouds = [c for stem, c in clouds.items() if stem != baseline.path.stem]
    var_params = [c["params"] for c in var_clouds]

    # Prefer simpler hypotheses: k=0 first, then ±1, then ±0.5, then ±2.
    LWHt_K_ORDER = [0.0, 1.0, -1.0, 0.5, -0.5, 2.0, -2.0]
    T_K_ORDER = [0.0, 1.0, -1.0, 2.0, -2.0, 1.5, -1.5, 3.0, -3.0,
                 0.5, -0.5, 4.0, -4.0, 4.5, -4.5, 5.0, -5.0,
                 6.0, -6.0, 8.0, -8.0, 10.0, -10.0]
    MATCH_TOL = 0.05  # tight — only accept clean matches

    def match_coord_in_sample(c0, baseline_params, sample_coords, sample_params):
        """Find the coord in sample_coords that matches c0 under some clean
        formula. Tries simpler k first; returns first within MATCH_TOL."""
        L0, W0, H0, t0 = baseline_params[:4]
        L, W, H, t = sample_params[:4]
        dL, dW, dH, dt = L - L0, W - W0, H - H0, t - t0

        # Slot-only sample (no primary change) — return nearest if very close,
        # else flag as unchanged (k=0). The caller handles slot delta separately.
        if dL == dW == dH == dt == 0:
            best = min(sample_coords, key=lambda v: abs(v - c0))
            return best

        # The varying param decides which k-set we search
        if abs(dL) > 1e-9:
            k_list, delta = LWHt_K_ORDER, dL
        elif abs(dW) > 1e-9:
            k_list, delta = LWHt_K_ORDER, dW
        elif abs(dH) > 1e-9:
            k_list, delta = LWHt_K_ORDER, dH
        else:
            k_list, delta = T_K_ORDER, dt

        for k in k_list:
            pred = c0 + k * delta
            best = min(sample_coords, key=lambda v: abs(v - pred))
            if abs(best - pred) < MATCH_TOL:
                return best
        return None

    def recover_axis(axis):
        baseline_coords = clouds[baseline.path.stem][axis]
        results = []
        for i, c0 in enumerate(baseline_coords):
            variation_vals = []
            for vc in var_clouds:
                matched = match_coord_in_sample(
                    c0, params_of(baseline), vc[axis], vc["params"])
                variation_vals.append(matched)
            if any(v is None for v in variation_vals):
                results.append({"index": i, "baseline": c0,
                                "formula": "?(no clean match)",
                                "variations": variation_vals})
                continue
            coefs, const, slot_delta, slot_changed = recover_formula(
                c0, params_of(baseline), variation_vals, var_params, param_names)
            formula_str = format_formula(coefs, const, param_names)
            if slot_delta is not None:
                slot_info = f" [slot_d={slot_delta:+.2f}]"
                formula_str += slot_info
            results.append({
                "index": i,
                "baseline": c0,
                "coefs": list(coefs),
                "const": const,
                "slot_delta": slot_delta,
                "slot_changed_params": slot_changed,
                "formula": formula_str,
                "variations": variation_vals,
            })
        return results

    x_results = recover_axis("xs")
    y_results = recover_axis("ys")

    return {
        "template": template,
        "baseline_params": dict(zip(param_names, params_of(baseline))),
        "param_names": param_names,
        "x_coords": x_results,
        "y_coords": y_results,
        "matched_all": matched_all,
    }


def print_results(res):
    template = res["template"]
    print(f"\n=== X-coordinate formulas ({template}) ===")
    print(f"{'idx':>4} {'baseline':>10}  formula")
    for r in res["x_coords"]:
        print(f"  {r['index']:>2}  {r['baseline']:>10.3f}  {r['formula']}")

    print(f"\n=== Y-coordinate formulas ({template}) ===")
    print(f"{'idx':>4} {'baseline':>10}  formula")
    for r in res["y_coords"]:
        print(f"  {r['index']:>2}  {r['baseline']:>10.3f}  {r['formula']}")

    # Coefficient frequency report
    print(f"\n=== Coefficient frequency ({template}) ===")
    coef_counts = defaultdict(int)
    for r in res["x_coords"] + res["y_coords"]:
        if "coefs" not in r:
            continue
        sig = tuple(round(c, 3) for c in r["coefs"])
        coef_counts[sig] += 1
    print(f"{'count':>6}  ({' '.join(res['param_names'])}) coefs")
    for sig, count in sorted(coef_counts.items(), key=lambda kv: -kv[1]):
        sig_str = " ".join(f"{c:+.3f}" for c in sig)
        print(f"  {count:>4}  ({sig_str})")


# ============================================================
# === Main ===================================================
# ============================================================

def main():
    files = sorted(REFS_DIR.glob("*.dxf"))
    samples = [parse_filename(p) for p in files]
    by_template = defaultdict(list)
    for s in samples:
        by_template[s.template].append(s)

    results = {}
    for template, samps in by_template.items():
        res = recover_template(samps)
        print_results(res)
        results[template] = res

    # Save formulas to JSON for the next stage (SVG generator)
    out_path = OUT_DIR / "coord_formulas.json"
    # Strip unserializable fields
    serializable = {}
    for k, r in results.items():
        serializable[k] = {
            "baseline_params": r["baseline_params"],
            "param_names": r["param_names"],
            "x_coords": [{"index": x.get("index"), "baseline": x["baseline"],
                          "coefs": x.get("coefs"), "const": x.get("const"),
                          "slot_delta": x.get("slot_delta"),
                          "formula": x["formula"]}
                         for x in r["x_coords"]],
            "y_coords": [{"index": y.get("index"), "baseline": y["baseline"],
                          "coefs": y.get("coefs"), "const": y.get("const"),
                          "slot_delta": y.get("slot_delta"),
                          "formula": y["formula"]}
                         for y in r["y_coords"]],
            "matched_all": r["matched_all"],
        }
    with open(out_path, "w") as f:
        json.dump(serializable, f, indent=2)
    print(f"\nWrote {out_path}")


if __name__ == "__main__":
    main()
