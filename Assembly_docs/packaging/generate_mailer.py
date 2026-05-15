"""Generate a mailer dieline at arbitrary (L, W, H, t) using the recovered
parametric formulas from coord_formulas.json + arc-parameter recovery
across the 5 reference DXFs.

Walks the baseline DXF entity-by-entity:
  - LINE/LWPOLYLINE endpoints  -> evaluated via coord_formulas.json
  - ARC center/radius/angles   -> recovered here by matching arcs across
                                   samples (sorted by baseline (cy, cx))

Coords without clean formulas (mostly arc-tangent endpoints on lines that
attach to curves) fall back to baseline values and are drawn dashed in a
muted color so we can see what didn't scale.

Run:  python generate_mailer.py [L W H t]
Defaults: 225 225 60 1.5
"""

import json
import math
import re
import sys
from pathlib import Path

import ezdxf
import matplotlib.pyplot as plt


REFS_DIR = Path(r"C:\Users\bowhi\Desktop\Independent_Study\Assembly_docs\Packaging_Templates")
HERE = Path(__file__).parent
FORMULAS_PATH = HERE / "coord_formulas.json"

BASELINE_PARAMS = (300.0, 200.0, 80.0, 1.5)  # L, W, H, t at baseline


# ============================================================
# === Coord lookups from coord_formulas.json =================
# ============================================================

def load_formulas():
    with open(FORMULAS_PATH) as f:
        data = json.load(f)
    mailer = data["mailer"]
    x_by_baseline = {round(c["baseline"], 3): c for c in mailer["x_coords"]}
    y_by_baseline = {round(c["baseline"], 3): c for c in mailer["y_coords"]}
    return x_by_baseline, y_by_baseline


def eval_coord(formula_entry, target_params):
    """Evaluate the recovered formula at new (L, W, H, t).
    Returns (value, ok_flag) — ok_flag False if no clean formula."""
    if formula_entry is None:
        return None, False
    coefs = formula_entry.get("coefs")
    const = formula_entry.get("const")
    if coefs is None or const is None:
        return formula_entry["baseline"], False
    val = sum(c * p for c, p in zip(coefs, target_params)) + const
    return val, True


def lookup_coord(value, lookup, tol=0.05):
    rounded = round(value, 3)
    if rounded in lookup:
        return lookup[rounded]
    keys = list(lookup.keys())
    if not keys:
        return None
    best = min(keys, key=lambda k: abs(k - value))
    if abs(best - value) < tol:
        return lookup[best]
    return None


# ============================================================
# === Arc parameter recovery (per-arc, across samples) =======
# ============================================================

def parse_mailer_filename(p: Path):
    m = re.match(r"mailer_(\d+)x(\d+)x(\d+)_t([\d.]+)", p.stem)
    return (float(m.group(1)), float(m.group(2)), float(m.group(3)), float(m.group(4)))


def extract_arcs(doc):
    arcs = []
    for e in doc.modelspace():
        if e.dxftype() == "ARC":
            arcs.append({
                "cx": float(e.dxf.center.x),
                "cy": float(e.dxf.center.y),
                "r": float(e.dxf.radius),
                "sa": float(e.dxf.start_angle),
                "ea": float(e.dxf.end_angle),
                "layer": e.dxf.layer,
            })
    return arcs


def recover_arc_formulas():
    """For each baseline arc, recover (cx, cy, r, sa, ea) as a*L + b*W + c*H + d*t + const.

    Matches arcs across samples by sorting all arcs in each sample by
    (cy, cx). Assumes arc count and topology preserved across variations.
    """
    samples = []
    for p in sorted(REFS_DIR.glob("mailer_*.dxf")):
        doc = ezdxf.readfile(p)
        arcs = extract_arcs(doc)
        arcs.sort(key=lambda a: (a["cy"], a["cx"]))  # canonical order
        samples.append({"params": parse_mailer_filename(p), "arcs": arcs})

    baseline = next(s for s in samples if s["params"] == BASELINE_PARAMS)
    baseline_arcs = baseline["arcs"]

    # Sanity check: same arc count across all samples
    counts = {tuple(s["params"]): len(s["arcs"]) for s in samples}
    print(f"Arc counts per sample: {counts}")

    # For each baseline arc, recover formulas for each of (cx, cy, r, sa, ea)
    recovered = []
    for i, ba in enumerate(baseline_arcs):
        rec = {"baseline": ba, "formulas": {}}
        for key in ("cx", "cy", "r", "sa", "ea"):
            coefs = [0.0, 0.0, 0.0, 0.0]
            for s in samples:
                if s["params"] == BASELINE_PARAMS:
                    continue
                if i >= len(s["arcs"]):
                    continue
                sa_val = s["arcs"][i][key]
                L0, W0, H0, t0 = BASELINE_PARAMS
                L, W, H, t = s["params"]
                dL, dW, dH, dt = L-L0, W-W0, H-H0, t-t0
                if abs(dL) > 1e-9: coefs[0] = (sa_val - ba[key]) / dL
                elif abs(dW) > 1e-9: coefs[1] = (sa_val - ba[key]) / dW
                elif abs(dH) > 1e-9: coefs[2] = (sa_val - ba[key]) / dH
                elif abs(dt) > 1e-9: coefs[3] = (sa_val - ba[key]) / dt
            const = ba[key] - sum(c * p for c, p in zip(coefs, BASELINE_PARAMS))
            rec["formulas"][key] = (coefs, const)
        recovered.append(rec)
    return recovered


def eval_arc_at_target(arc_rec, target_params):
    out = {}
    for key, (coefs, const) in arc_rec["formulas"].items():
        out[key] = sum(c * p for c, p in zip(coefs, target_params)) + const
    out["layer"] = arc_rec["baseline"]["layer"]
    return out


# ============================================================
# === Generate at target params ==============================
# ============================================================

def generate(target_params, out_png=None):
    L, W, H, t = target_params
    print(f"\nGenerating mailer at L={L} W={W} H={H} t={t}")

    x_by_b, y_by_b = load_formulas()
    arc_recs = recover_arc_formulas()

    baseline_doc = ezdxf.readfile(
        REFS_DIR / "mailer_300x200x80_t1.5.dxf"
    )

    cut_lines = []      # list of ((x0,y0), (x1,y1), ok_flag_combined)
    fold_lines = []
    polylines = []      # list of ([(x0,y0), ...], layer, ok_flag)
    fallbacks = 0
    total_endpoints = 0

    def resolve_x(val):
        nonlocal fallbacks, total_endpoints
        total_endpoints += 1
        entry = lookup_coord(val, x_by_b)
        v, ok = eval_coord(entry, target_params)
        if not ok:
            fallbacks += 1
            return val, False  # fallback to baseline
        return v, True

    def resolve_y(val):
        nonlocal fallbacks, total_endpoints
        total_endpoints += 1
        entry = lookup_coord(val, y_by_b)
        v, ok = eval_coord(entry, target_params)
        if not ok:
            fallbacks += 1
            return val, False
        return v, True

    for e in baseline_doc.modelspace():
        et = e.dxftype()
        layer = e.dxf.layer
        if et == "LINE":
            x0, ok1 = resolve_x(e.dxf.start.x)
            y0, ok2 = resolve_y(e.dxf.start.y)
            x1, ok3 = resolve_x(e.dxf.end.x)
            y1, ok4 = resolve_y(e.dxf.end.y)
            ok = ok1 and ok2 and ok3 and ok4
            entry = (cut_lines if layer == "cuttingLine" else fold_lines)
            entry.append(((x0, y0), (x1, y1), ok))
        elif et == "LWPOLYLINE":
            pts_resolved = []
            all_ok = True
            for (px, py, *_) in e.get_points():
                nx, okx = resolve_x(px)
                ny, oky = resolve_y(py)
                pts_resolved.append((nx, ny))
                all_ok = all_ok and okx and oky
            polylines.append((pts_resolved, layer, all_ok, e.closed))

    print(f"Endpoint resolution: {total_endpoints - fallbacks}/{total_endpoints} clean "
          f"(fallback to baseline for {fallbacks})")

    # ---- Render ----
    fig, ax = plt.subplots(figsize=(11, 11))
    ax.set_aspect("equal")
    ax.grid(True, linestyle=":", alpha=0.3)
    ax.set_xlabel("mm"); ax.set_ylabel("mm")

    CUT = "#d62728"; SCORE = "#1f4ec7"; FALLBACK = "#999999"

    for (p0, p1, ok) in cut_lines:
        color = CUT if ok else FALLBACK
        style = "-" if ok else (0, (4, 3))
        ax.plot([p0[0], p1[0]], [p0[1], p1[1]],
                color=color, linewidth=1.4, linestyle=style)

    for (p0, p1, ok) in fold_lines:
        color = SCORE if ok else FALLBACK
        style = (0, (6, 3)) if ok else (0, (2, 4))
        ax.plot([p0[0], p1[0]], [p0[1], p1[1]],
                color=color, linewidth=1.0, linestyle=style)

    for arc_rec in arc_recs:
        arc = eval_arc_at_target(arc_rec, target_params)
        cx, cy, r = arc["cx"], arc["cy"], arc["r"]
        sa, ea = arc["sa"], arc["ea"]
        if ea < sa: ea += 360
        thetas = [math.radians(sa + (ea - sa) * i / 64) for i in range(65)]
        xs = [cx + r * math.cos(th) for th in thetas]
        ys = [cy + r * math.sin(th) for th in thetas]
        color = CUT if arc["layer"] == "cuttingLine" else SCORE
        ax.plot(xs, ys, color=color, linewidth=1.4)

    for (pts, layer, ok, closed) in polylines:
        xs = [p[0] for p in pts]
        ys = [p[1] for p in pts]
        if closed:
            xs.append(xs[0]); ys.append(ys[0])
        color = (CUT if layer == "cuttingLine" else SCORE) if ok else FALLBACK
        style = "-" if ok else (0, (4, 3))
        ax.plot(xs, ys, color=color, linewidth=1.4, linestyle=style)

    ax.set_title(
        f"Generated mailer dieline — L={L:.0f} x W={W:.0f} x H={H:.0f} mm, "
        f"E-flute {t:.1f} mm\n"
        f"(red = cut, blue dashed = score, grey = fallback-to-baseline; "
        f"{total_endpoints - fallbacks}/{total_endpoints} endpoints scaled)",
        fontsize=11
    )

    if out_png is None:
        out_png = HERE / f"generated_mailer_{int(L)}x{int(W)}x{int(H)}_t{t}.png"
    plt.tight_layout()
    plt.savefig(out_png, dpi=150, bbox_inches="tight")
    print(f"Wrote {out_png}")


if __name__ == "__main__":
    if len(sys.argv) >= 5:
        target = (float(sys.argv[1]), float(sys.argv[2]),
                  float(sys.argv[3]), float(sys.argv[4]))
    else:
        target = (225.0, 225.0, 60.0, 1.5)
    generate(target)
