"""Render each CEFbox reference DXF to a PNG, 1:1, no transformations.

Pure passthrough — walks every modelspace entity and draws it at its actual
coordinates. Goal is to verify that ezdxf is reading the DXFs correctly
before any parametric work happens downstream.

  LINE        -> straight segment
  ARC         -> sampled curve (64 points)
  LWPOLYLINE  -> connected segments (closed if marked closed)
  POINT       -> small marker

Layer coloring matches what we expect from CEFbox dielines:
  cuttingLine  -> red, solid
  foldingLine  -> blue, dashed
  (any other layer is drawn in grey, mostly for safety)

Run:  python dxf_to_png.py
Output: passthrough_<filename>.png next to each DXF (in this script's dir).
"""

import math
from pathlib import Path

import ezdxf
import matplotlib.pyplot as plt


REFS_DIR = Path(r"C:\Users\bowhi\Desktop\Independent_Study\Assembly_docs\Packaging_Templates")
OUT_DIR = Path(__file__).parent

LAYER_STYLE = {
    "cuttingLine": {"color": "#d62728", "lw": 1.2, "ls": "-"},
    "foldingLine": {"color": "#1f4ec7", "lw": 1.0, "ls": (0, (5, 3))},
}
DEFAULT_STYLE = {"color": "#888", "lw": 0.8, "ls": "-"}


def style_for(layer):
    return LAYER_STYLE.get(layer, DEFAULT_STYLE)


def arc_sampled(cx, cy, r, sa_deg, ea_deg, n=64):
    sa = math.radians(sa_deg)
    ea = math.radians(ea_deg)
    if ea < sa:
        ea += 2 * math.pi
    return [(cx + r * math.cos(sa + (ea - sa) * i / n),
             cy + r * math.sin(sa + (ea - sa) * i / n))
            for i in range(n + 1)]


def render_dxf(dxf_path, out_path):
    doc = ezdxf.readfile(dxf_path)
    msp = doc.modelspace()

    counts = {"LINE": 0, "ARC": 0, "LWPOLYLINE": 0, "POINT": 0, "OTHER": 0}

    fig, ax = plt.subplots(figsize=(11, 11))
    ax.set_aspect("equal")
    ax.grid(True, linestyle=":", alpha=0.3)
    ax.set_xlabel("mm"); ax.set_ylabel("mm")

    for e in msp:
        et = e.dxftype()
        layer = e.dxf.layer
        st = style_for(layer)

        if et == "LINE":
            counts["LINE"] += 1
            ax.plot([e.dxf.start.x, e.dxf.end.x],
                    [e.dxf.start.y, e.dxf.end.y],
                    color=st["color"], linewidth=st["lw"], linestyle=st["ls"])
        elif et == "ARC":
            counts["ARC"] += 1
            pts = arc_sampled(e.dxf.center.x, e.dxf.center.y, e.dxf.radius,
                              e.dxf.start_angle, e.dxf.end_angle)
            xs = [p[0] for p in pts]; ys = [p[1] for p in pts]
            ax.plot(xs, ys, color=st["color"], linewidth=st["lw"],
                    linestyle=st["ls"])
        elif et == "LWPOLYLINE":
            counts["LWPOLYLINE"] += 1
            pts = [(p[0], p[1]) for p in e.get_points()]
            if e.closed:
                pts.append(pts[0])
            xs = [p[0] for p in pts]; ys = [p[1] for p in pts]
            ax.plot(xs, ys, color=st["color"], linewidth=st["lw"],
                    linestyle=st["ls"])
        elif et == "POINT":
            counts["POINT"] += 1
            ax.plot(e.dxf.location.x, e.dxf.location.y, marker="x",
                    color=st["color"], markersize=4)
        else:
            counts["OTHER"] += 1
            print(f"  ! skipped entity type {et}")

    ax.set_title(
        f"{dxf_path.name}\n"
        f"LINE={counts['LINE']}  ARC={counts['ARC']}  "
        f"LWPOLYLINE={counts['LWPOLYLINE']}  POINT={counts['POINT']}  "
        f"OTHER={counts['OTHER']}",
        fontsize=10
    )
    plt.tight_layout()
    plt.savefig(out_path, dpi=150, bbox_inches="tight")
    plt.close(fig)
    print(f"  wrote {out_path.name}  ({counts})")


def main():
    dxfs = sorted(REFS_DIR.glob("*.dxf"))
    print(f"Found {len(dxfs)} DXFs in {REFS_DIR}")
    for p in dxfs:
        out = OUT_DIR / f"passthrough_{p.stem}.png"
        print(f"{p.name}")
        render_dxf(p, out)


if __name__ == "__main__":
    main()
