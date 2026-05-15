"""Render a list of generated entities to PNG. Solid lines, color-only
differentiation between cut (red) and score (blue) per Bill's spec.

Usage:
    from cefbox_render import render_entities
    render_entities(entities, out_path='foo.png', title='...')
"""

import math
from pathlib import Path
from typing import List

import matplotlib.pyplot as plt


CUT_COLOR = "#d62728"
SCORE_COLOR = "#1f4ec7"
LW = 1.4


def render_entities(entities: List, out_path, title: str = "",
                    bbox: tuple = None):
    """Render entities to PNG. entities is a list of Line/Arc/Polyline
    namedtuples (from mailer_reft.py or insert_tray.py)."""
    fig, ax = plt.subplots(figsize=(11, 11))
    ax.set_aspect("equal")
    ax.grid(True, linestyle=":", alpha=0.3)
    ax.set_xlabel("mm"); ax.set_ylabel("mm")

    for e in entities:
        kind = type(e).__name__
        layer = e.layer
        color = CUT_COLOR if layer == "cuttingLine" else SCORE_COLOR

        if kind == "Line":
            ax.plot([e.x0, e.x1], [e.y0, e.y1],
                    color=color, linewidth=LW, linestyle="-")
        elif kind == "Arc":
            sa = math.radians(e.sa); ea = math.radians(e.ea)
            if ea < sa: ea += 2 * math.pi
            n = 64
            ts = [sa + (ea - sa) * i / n for i in range(n + 1)]
            xs = [e.cx + e.r * math.cos(th) for th in ts]
            ys = [e.cy + e.r * math.sin(th) for th in ts]
            ax.plot(xs, ys, color=color, linewidth=LW)
        elif kind == "Polyline":
            xs = [p[0] for p in e.pts]
            ys = [p[1] for p in e.pts]
            if e.closed and (xs[0] != xs[-1] or ys[0] != ys[-1]):
                xs.append(xs[0]); ys.append(ys[0])
            ax.plot(xs, ys, color=color, linewidth=LW)

    if title:
        ax.set_title(title, fontsize=10)
    if bbox:
        ax.set_xlim(bbox[0], bbox[2])
        ax.set_ylim(bbox[1], bbox[3])

    plt.tight_layout()
    plt.savefig(out_path, dpi=150, bbox_inches="tight")
    plt.close(fig)
    return out_path
