"""Roll End Front Tuck (REFT) mailer parametric part library.

Built as a graph of NAMED PARTS. Y-axis symmetry across x = L/2 enforced
as a hard constraint: every horizontal-extent part is defined for x in
[L/2, ...] and mirrored to produce x in [..., L/2].

Spine layout (bottom to top, y direction):
    FRONT_TUCK   y in [-(H+t),         +t/2]            front wall + closing tuck
    BOTTOM       y in [+t/2,           +(W + 1.5t)]     floor (with side walls + finger holes)
    BACK_WALL    y in [+(W+1.5t),      +(W + H + 3t)]   back wall (with dust flaps)
    LID          y in [+(W+H+3t),      +(2W + H + 4t)]  top of box
    LID_TUCK     y in [+(2W+H+4t),     +(2W + 2H + 5t)] closing tuck mirror

Roll-end side walls attach to BOTTOM's left/right edges. Outer x at -(2H +
9.25), inner x at -(2H + 6.75) — the "2H" reflects the roll-end's doubled
fold construction.

Build order in this file matches the build/verify cadence:
    Phase 1: spine_outline() + spine_scores()  [outer rectangle + 4 scores]
    Phase 2: + left_side_wall() + mirror       [roll-ends with teeth pattern]
    Phase 3: + dust_flaps()                    [back/front wall side flaps]
    Phase 4: + ear_lock_caps()                 [curved tuck ends with horns]
    Phase 5: + finger_holes()                  [side wall holes, scaled with W]
"""

from __future__ import annotations
from dataclasses import dataclass
from typing import List, NamedTuple, Tuple

LAYER_CUT = "cuttingLine"
LAYER_SCORE = "foldingLine"


# ============================================================
# === Entity records =========================================
# ============================================================

class Line(NamedTuple):
    x0: float; y0: float; x1: float; y1: float
    layer: str = LAYER_CUT


class Arc(NamedTuple):
    cx: float; cy: float; r: float
    sa: float; ea: float
    layer: str = LAYER_CUT


class Polyline(NamedTuple):
    pts: Tuple[Tuple[float, float], ...]
    closed: bool = True
    layer: str = LAYER_CUT


# ============================================================
# === Spec ===================================================
# ============================================================

@dataclass
class MailerSpec:
    L: float
    W: float
    H: float
    t: float = 1.5

    # CEFbox-derived constants (extracted from 5 reference DXFs covering H, W, t variations).
    # All offsets are parametric in t (confirmed at t=1.5 and t=3.0 samples).
    #   side wall outer x:        -(2H + 4.833*t + 2)    -> -169.25 @ t=1.5, -176.5 @ t=3
    #   side wall inner x:        -(2H + 4.5*t)          -> -166.75 @ t=1.5, -173.5 @ t=3
    #   side-wall fold inner x:   -(H + 4*t)             -> -86 @ t=1.5, -92 @ t=3
    #   inner score x (panel):    -(H + t)               -> -81.5 @ t=1.5, -83 @ t=3
    #   dust flap outer x:        -(100 - 1.5*t)         -> -97.75 @ t=1.5, -95.5 @ t=3
    #   shoulder chamfer angle:   ~9 degrees (rise/run = 0.158)
    DUST_OUTER_BASE: float = 100.0           # outer x = -(DUST_OUTER_BASE - 1.5*t)
    SHOULDER_RISE_RATIO: float = 2.574 / 16.25  # ≈ 0.1584

    # Tooth pattern on side wall outer edge (alternating segments)
    TOOTH_OUT_LEN: float = 38.1              # outer-x segment length
    TOOTH_IN_END_LEN: float = 39.1           # inner-x segment length at end
    TOOTH_IN_MID_LEN: float = 40.6           # inner-x segment length in middle
    TOOTH_TRANSITION: float = 1.25           # diagonal transition height

    # Finger hole — small rounded rectangle in BOTTOM panel near each side
    FINGER_HOLE_W: float = 5.25
    FINGER_HOLE_H: float = 40.6

    # Ear-lock end cap arcs (curved tuck-flap ends)
    EAR_ARC_RADIUS: float = 80.75
    EAR_HORN_RADIUS: float = 3.0

    # Empirical: number of finger holes per side scales with W
    def n_finger_holes(self) -> int:
        if self.W <= 220: return 2
        if self.W <= 290: return 3
        return max(2, round(self.W / 90))

    # Geometric constraints — derived from formula breakdown points and
    # CEFbox-validated ranges (t=1.5 baseline + t=3.0 sample).
    MIN_W: float = 180.0     # tooth pattern needs W >= ~160 to avoid negative middle segment; +20 margin
    MIN_L: float = 60.0      # 4.5t inset on both sides leaves at least 30mm of panel
    MIN_H: float = 20.0      # below this the side wall (2H wide) is too thin to fold
    MAX_H: float = 200.0     # untested above this
    MIN_T: float = 0.5
    MAX_T: float = 5.0       # untested above this

    def validate(self) -> None:
        """Raise ValueError if dimensions violate geometric constraints.
        Called automatically by generate()."""
        issues = []
        if self.t < self.MIN_T or self.t > self.MAX_T:
            issues.append(f"t={self.t} outside validated range "
                          f"[{self.MIN_T}, {self.MAX_T}]")
        if self.L < self.MIN_L:
            issues.append(f"L={self.L} < MIN_L={self.MIN_L} (spine score insets "
                          f"of 4.5t={4.5*self.t:.1f} on each side leave too little panel)")
        if self.W < self.MIN_W:
            issues.append(f"W={self.W} < MIN_W={self.MIN_W} (tooth pattern's middle "
                          f"inner segment becomes negative below this)")
        if self.H < self.MIN_H:
            issues.append(f"H={self.H} < MIN_H={self.MIN_H} (side wall is 2H={2*self.H:.0f}mm "
                          f"wide; too thin to fold properly)")
        if self.H > self.MAX_H:
            issues.append(f"H={self.H} > MAX_H={self.MAX_H} (untested; arc tangent formulas "
                          f"may not extrapolate cleanly)")
        if self.H < 2 * self.t:
            issues.append(f"H={self.H} < 2t={2*self.t} (dust flap shoulder chamfer rise "
                          f"could exceed H, making case-A geometry degenerate)")
        if issues:
            raise ValueError("MailerSpec invalid:\n  - " + "\n  - ".join(issues))


# ============================================================
# === Geometry helpers =======================================
# ============================================================

def mirror_x(entities: List, axis_x: float) -> List:
    """Reflect entities across vertical line x = axis_x."""
    out = []
    for e in entities:
        if isinstance(e, Line):
            out.append(Line(2*axis_x - e.x1, e.y1,
                            2*axis_x - e.x0, e.y0, e.layer))
        elif isinstance(e, Arc):
            new_sa = (180.0 - e.ea) % 360.0
            new_ea = (180.0 - e.sa) % 360.0
            out.append(Arc(2*axis_x - e.cx, e.cy, e.r, new_sa, new_ea, e.layer))
        elif isinstance(e, Polyline):
            new_pts = tuple((2*axis_x - x, y) for (x, y) in reversed(e.pts))
            out.append(Polyline(new_pts, e.closed, e.layer))
    return out


# ============================================================
# === Phase 1: spine outline + scores =========================
# ============================================================

# Y-coordinates of the 5 spine-panel boundaries.
# Panel heights, derived from baseline (W=200, H=80, t=1.5):
#   FRONT_TUCK : H + 0.5t  (= 80.75)
#   BOTTOM     : W + t     (= 201.5)
#   BACK_WALL  : H + t     (= 81.5)
#   LID        : W + 2t    (= 203)
#   LID_TUCK   : H + t     (= 81.5)
# Sum from y=0: starts at -H (front-tuck bottom), reaches 2W+2H+5.5t (top).
def y_bot(s):                return -s.H                        # -80
def y_score_tuck_bottom(s):  return s.t * 0.5                   # +0.75
def y_score_bottom_back(s):  return s.W + s.t * 1.5             # +202.25
def y_score_back_lid(s):     return s.W + s.H + s.t * 2.5       # +283.75
def y_score_lid_tuck(s):     return 2*s.W + s.H + s.t * 4.5     # +486.75
def y_top(s):                return 2*s.W + 2*s.H + s.t * 5.5   # +568.25


def spine_scores(s: MailerSpec) -> List[Line]:
    """4 horizontal score lines between the 5 spine panels.

    Score insets at baseline (t=1.5):
      tuck/bottom score:  x in [3, 312]      inset = 3   = 2*t
      bottom/back score:  x in [1.5, 313.5]  inset = 1.5 = t
      back/lid score:     x in [6.75, 308.25] inset = 6.75 = 4.5*t (LID corner)
      lid/lid-tuck score: same as back/lid    inset = 6.75 = 4.5*t
    """
    L, t = s.L, s.t
    inset_outer = 4.5 * t   # back/lid + lid/lid-tuck (LID dust flap corner)
    inset_inner = 2 * t
    inset_minor = t
    return [
        Line(inset_inner, y_score_tuck_bottom(s),
             x_mirror(s, inset_inner), y_score_tuck_bottom(s), LAYER_SCORE),
        Line(inset_minor, y_score_bottom_back(s),
             x_mirror(s, inset_minor), y_score_bottom_back(s), LAYER_SCORE),
        Line(inset_outer, y_score_back_lid(s),
             x_mirror(s, inset_outer), y_score_back_lid(s), LAYER_SCORE),
        Line(inset_outer, y_score_lid_tuck(s),
             x_mirror(s, inset_outer), y_score_lid_tuck(s), LAYER_SCORE),
    ]


def spine_outline(s: MailerSpec) -> List[Line]:
    """Just the 4 horizontal cut edges of the spine: front-tuck bottom, etc.

    Phase 1 placeholder — 4 horizontal cut lines marking the top/bottom of
    each panel where it's visible (most of the perimeter is shared with
    side walls / dust flaps and gets emitted by those parts).

    For now, just emit the FRONT_TUCK bottom edge and LID_TUCK top edge
    (the only spine-only horizontal cuts).
    """
    L = s.L
    return [
        Line(0, y_bot(s), L, y_bot(s), LAYER_CUT),    # FRONT_TUCK bottom (will be split later)
        Line(0, y_top(s), L, y_top(s), LAYER_CUT),    # LID_TUCK top
    ]


# ============================================================
# === Phase 2: roll-end side walls with tooth pattern =========
# ============================================================
#
# LEFT side wall geometry, attached to BOTTOM panel's left edge (x=0).
# Outer x at -(2H + 9.25), inner x at -(2H + 6.75). Spans y in [t, W+t]
# (matches BOTTOM panel's vertical extent).
#
# Tooth pattern (alternating outer/inner verticals connected by short
# diagonals) ranges from N=2 outer teeth at small W to more at larger W.
# At baseline W=200: 2 outer teeth, 3 inner segments.
# At W=250: 3 outer teeth, 4 inner segments.

def _tooth_pattern_y(s: MailerSpec):
    """Build the (kind, y_start, y_end) sequence for the side wall outer
    edge teeth pattern. kind = 'I' (inner segment) or 'O' (outer tooth).

    Returns the list. Adjusts inner-segment lengths so the total spans
    exactly y in [t, W+t].
    """
    n_outer = s.n_finger_holes()        # outer teeth = number of finger holes
    n_inner = n_outer + 1                # inner segments = teeth + 1

    inner_short = s.TOOTH_IN_END_LEN     # 39.1 — used at the two ends
    inner_long = s.TOOTH_IN_MID_LEN      # 40.6 — used in the middle
    outer_len = s.TOOTH_OUT_LEN          # 38.1
    transition = s.TOOTH_TRANSITION      # 1.25

    # Total at default lengths:
    #   2 short ends + (n_inner - 2) long mids + n_outer outer + 2*n_outer transitions
    n_long = max(0, n_inner - 2)
    default_total = (2 * inner_short
                     + n_long * inner_long
                     + n_outer * outer_len
                     + 2 * n_outer * transition)
    target_total = s.W                   # span is W (from y=t to y=W+t)
    slack = target_total - default_total
    long_adj = (inner_long + slack / n_long) if n_long > 0 else inner_long

    pattern = []
    y = s.t  # start at side wall bottom
    for i in range(n_inner):
        seg_len = inner_short if (i == 0 or i == n_inner - 1) else long_adj
        pattern.append(('I', y, y + seg_len))
        y += seg_len
        if i < n_outer:
            y += transition
            pattern.append(('O', y, y + outer_len))
            y += outer_len + transition
    return pattern


def x_side_outer(s):   return -(2*s.H + 4.833*s.t + 2)   # -169.25 @ t=1.5
def x_side_inner(s):   return -(2*s.H + 4.5*s.t)          # -166.75 @ t=1.5
def x_side_fold(s):    return -(s.H + 4*s.t)              # -86.0 @ t=1.5
def x_panel_inner(s):  return -(s.H + s.t)                # -81.5 @ t=1.5
def x_dust_outer(s):   return -(s.DUST_OUTER_BASE - 1.5*s.t)  # -97.75 @ t=1.5

# CRITICAL: the dieline is mirror-symmetric about x = L/2 + 5t (= 157.5 at
# baseline), NOT about x = L/2. All right-side x positions are reflections
# of left-side positions about this axis.
def mirror_axis_x(s):  return s.L / 2 + 5 * s.t           # 157.5 @ baseline
def x_mirror(s, x):    return 2 * mirror_axis_x(s) - x


def left_side_wall(s: MailerSpec) -> List:
    """LEFT roll-end side wall: outer edge teeth + inner edge horizontal caps."""
    L, W, H, t = s.L, s.W, s.H, s.t
    outer_x = x_side_outer(s)
    inner_x = x_side_inner(s)

    ents = []
    pattern = _tooth_pattern_y(s)

    # Vertical segments per pattern step
    for kind, ys, ye in pattern:
        x = inner_x if kind == 'I' else outer_x
        ents.append(Line(x, ys, x, ye, LAYER_CUT))

    # Diagonal connectors between consecutive segments
    for i in range(len(pattern) - 1):
        kind1, _, ye1 = pattern[i]
        kind2, ys2, _ = pattern[i + 1]
        x1 = inner_x if kind1 == 'I' else outer_x
        x2 = inner_x if kind2 == 'I' else outer_x
        ents.append(Line(x1, ye1, x2, ys2, LAYER_CUT))

    # Top + bottom horizontal caps (close the side wall outline at y=t and y=W+t).
    # Run from inner_x to where the dust-flap corner-chamfer ends at x = -(H + 4t).
    fold_x = x_side_fold(s)
    panel_inner = x_panel_inner(s)
    ents.append(Line(inner_x, t, fold_x, t, LAYER_CUT))
    ents.append(Line(inner_x, W + t, fold_x, W + t, LAYER_CUT))

    # Side wall fold SCORES (doubled-fold roll-end construction):
    # - x = -(H + t)  = panel_inner: inner fold where BOTTOM panel hinges to side wall
    # - x = -(H + 4t) = fold_x:      outer fold within the side wall (between layers)
    ents.append(Line(panel_inner, 0, panel_inner, W + 2*t, LAYER_SCORE))    # baseline -81.5, y=0..203
    ents.append(Line(fold_x, t, fold_x, W + t, LAYER_SCORE))                # baseline -86, y=1.5..201.5

    return ents


# ============================================================
# === Phase 3: dust flaps with shoulder chamfer + corner chamfer
# ============================================================
#
# Each panel that becomes a side-of-the-box (FRONT_TUCK below the floor,
# BACK_WALL above the floor) has a dust flap on each end. The dust flap
# is a protrusion extending OUTWARD (-x for left side) past the floor
# edge.
#
# Two geometric cases depending on H+t vs DUST_OUTER:
#   CASE A (H+t < DUST_OUTER):  dust flap wider at the outer edge of the
#                               panel than at the spine fold. A diagonal
#                               "shoulder chamfer" narrows it as it
#                               approaches the score line.
#   CASE B (H+t >= DUST_OUTER): dust flap is the same width as the spine
#                               fold inset, no shoulder chamfer needed.
#                               y=0 horizontal extends past the outer
#                               vertical's top end (creating a step).
#
# Each flap also has a small CORNER CHAMFER at the (inner_panel, 0)
# vertex connecting to the side wall fold strip — present in both cases.


def front_tuck_dust_flap_left(s: MailerSpec) -> List:
    """Left dust flap on the FRONT_TUCK panel.

    Geometry (cuttingLine, looking at left half before mirror):
      Outer vertical at outer_x going up from y=-H
      Shoulder chamfer (Case A only)
      Inner vertical at inner_x (height t in Case A, or part of panel in Case B)
      y=0 inner horizontal (extends to 0 or past outer_x depending on case)
      Corner chamfer (inner_x, 0) -> (fold_x, t)
    """
    L, W, H, t = s.L, s.W, s.H, s.t
    outer_x = x_dust_outer(s)
    inner_x = x_panel_inner(s)
    fold_x  = x_side_fold(s)

    ents = []
    shoulder_width = inner_x - outer_x   # positive in Case A (inner > outer when both negative); negative in Case B

    if shoulder_width > 0:
        # CASE A: shoulder chamfer present
        rise = shoulder_width * s.SHOULDER_RISE_RATIO
        # Outer vertical: y=-H to y=-(t+rise) at outer_x
        ents.append(Line(outer_x, -H, outer_x, -(t + rise), LAYER_CUT))
        # Shoulder chamfer
        ents.append(Line(outer_x, -(t + rise), inner_x, -t, LAYER_CUT))
        # Inner vertical: y=-t to y=0 at inner_x
        ents.append(Line(inner_x, -t, inner_x, 0, LAYER_CUT))
        # y=0 horizontal: from inner_x to 0
        ents.append(Line(inner_x, 0, 0, 0, LAYER_CUT))
    else:
        # CASE B: no shoulder chamfer; outer vertical runs all the way to y=0.
        # Matches CEFbox's representation: split at y=-t into two segments.
        ents.append(Line(outer_x, -H, outer_x, -t, LAYER_CUT))
        ents.append(Line(outer_x, -t, outer_x, 0, LAYER_CUT))
        # y=0 horizontal: from inner_x to 0 (passes through outer_x)
        ents.append(Line(inner_x, 0, 0, 0, LAYER_CUT))

    # Corner chamfer to side wall: (inner_x, 0) -> (fold_x, t)
    ents.append(Line(inner_x, 0, fold_x, t, LAYER_CUT))

    return ents


def back_wall_dust_flap_left(s: MailerSpec) -> List:
    """Left dust flap on the BACK_WALL panel.

    Mirror of front_tuck_dust_flap_left about y = (W/2 + t):
      FRONT_TUCK  y = 0           -> BACK_WALL  y = W + 2t  (inner cut, 203 baseline)
      FRONT_TUCK  y = -t          -> BACK_WALL  y = W + 3t  (top of inner vert, 204.5)
      FRONT_TUCK  y = -(t+rise)   -> BACK_WALL  y = W + 3t + rise  (top of chamfer, 207.074)
      FRONT_TUCK  y = -H          -> BACK_WALL  y = W + 2t + H  (= y_back_cut, 283 baseline)
      FRONT_TUCK  side-wall-chamfer (inner_x, 0) -> (fold_x, t)
                                  -> BACK_WALL  (inner_x, W+2t) -> (fold_x, W+t)
    """
    L, W, H, t = s.L, s.W, s.H, s.t
    outer_x = x_dust_outer(s)
    inner_x = x_panel_inner(s)
    fold_x  = x_side_fold(s)
    y_inner_cut = W + 2*t       # mirror of y=0 (the y=203 cut at baseline)
    y_top_cut   = W + 2*t + H   # mirror of y=-H (the y=283 cut at baseline)

    ents = []
    shoulder_width = inner_x - outer_x

    if shoulder_width > 0:
        rise = shoulder_width * s.SHOULDER_RISE_RATIO
        # Outer vertical: y=y_top_cut to y=y_inner_cut + (t + rise)
        ents.append(Line(outer_x, y_top_cut,
                         outer_x, y_inner_cut + (t + rise), LAYER_CUT))
        # Shoulder chamfer (outer_x, ...) -> (inner_x, y_inner_cut + t)
        ents.append(Line(outer_x, y_inner_cut + (t + rise),
                         inner_x, y_inner_cut + t, LAYER_CUT))
        # Inner vertical: y=y_inner_cut+t to y=y_inner_cut
        ents.append(Line(inner_x, y_inner_cut + t,
                         inner_x, y_inner_cut, LAYER_CUT))
        # Inner-cut horizontal: y=y_inner_cut, x=inner_x to 0
        ents.append(Line(inner_x, y_inner_cut, 0, y_inner_cut, LAYER_CUT))
    else:
        # CASE B back wall: split outer vertical at y_inner_cut + t for symmetry
        # with the FRONT_TUCK case-B split. Mirror of (outer_x, -H to -t)+(outer_x, -t to 0).
        ents.append(Line(outer_x, y_top_cut, outer_x, y_inner_cut + t, LAYER_CUT))
        ents.append(Line(outer_x, y_inner_cut + t, outer_x, y_inner_cut, LAYER_CUT))
        ents.append(Line(inner_x, y_inner_cut, 0, y_inner_cut, LAYER_CUT))

    # Side-wall corner chamfer: (inner_x, y_inner_cut=W+2t) -> (fold_x, W+t)
    ents.append(Line(inner_x, y_inner_cut, fold_x, W + t, LAYER_CUT))

    return ents


def panel_outer_edges(s: MailerSpec) -> List:
    """Outer horizontal cuts at panel bottoms/tops where the dust flaps live.

    Three segments at y=-H (FRONT_TUCK bottom) — left flap, panel span, right
    flap — because the FRONT_TUCK is one continuous panel across the bottom.
    Two segments at y=y_back_cut (BACK_WALL top) — left flap, right flap —
    no middle, because the LID occupies the middle above the BACK_WALL.
    """
    L, W, H, t = s.L, s.W, s.H, s.t
    outer_x = x_dust_outer(s)
    y_back_cut = W + 2*t + H   # = 283 at baseline

    return [
        # FRONT_TUCK bottom edge (3 segments)
        Line(outer_x,      -H, 2*t,           -H, LAYER_CUT),  # left dust flap
        Line(2*t,          -H, x_mirror(s, 2*t),       -H, LAYER_CUT),  # main panel span
        Line(x_mirror(s, 2*t),      -H, x_mirror(s, outer_x),   -H, LAYER_CUT),  # right dust flap
        # BACK_WALL top edge (2 segments, no middle because LID is above)
        Line(outer_x, y_back_cut, t,          y_back_cut, LAYER_CUT),
        Line(x_mirror(s, t),   y_back_cut, x_mirror(s, outer_x), y_back_cut, LAYER_CUT),
    ]


# ============================================================
# === Phase 4: ear-lock arcs on the LID_TUCK and LID corners ==
# ============================================================
#
# 10 arcs at baseline, all on the left/right symmetric LID + LID_TUCK area.
# FRONT_TUCK has NO arcs (it's a rectangular panel).
#
# 4 LID corner arcs (r ~ H/2): concave indents at each of the 4 LID corners
# 2 LID_TUCK main arcs (r ~ H): the ear-lock curved cap at the top
# 2 LID_TUCK horn arcs (r=3): small horns at the outer ear-lock tips
# 2 LID_TUCK score-tangent arcs (r ~ 2): tiny rounds near the score line
#
# Plus 4 tangent diagonals (2 mirrored pairs) connecting arc tangents.
#
# The ear-lock cap is geometrically: LID_TUCK main arc (internally tangent
# to a horn arc at angle 171.57 deg at baseline), then horn arc, then a
# diagonal tangent line down to a score-tangent arc near the LID_TUCK
# score, which then runs back into the score area.

import math

# Horn arc relative position (offset from LID_TUCK main arc center)
# Baseline: horn center at (-74.659, 498.154), main center at (2.25, 486.75)
# Offset: (-76.909, +11.404) at baseline
# At H=120: offset (-116.619, +16.279)
# At t=3:   offset (-77.345, +13.418)
# At W=250: same as baseline (W doesn't affect)
#
# horn_dy = horn_cy - main_cy: 11.404 -> 16.279 with delta_H=40 -> coef ~ 0.122
#                            : 11.404 -> 13.418 with delta_t=1.5 -> coef ~ 1.343
def horn_offset_y(s):
    """Vertical offset from LID_TUCK main arc center to horn arc center."""
    return 0.122 * s.H + 1.343 * s.t - 0.391    # empirical fit

def horn_offset_x(s):
    """Horizontal offset from LID_TUCK main arc center to horn arc center.

    Horn arc is INTERNALLY tangent to main arc, so distance between centers
    = r_main - r_horn. Therefore offset_x = -sqrt((r_main - r_horn)^2 - offset_y^2).
    """
    r_main = s.H + 0.5 * s.t
    r_horn = 3.0
    dy = horn_offset_y(s)
    return -math.sqrt(max(0, (r_main - r_horn)**2 - dy**2))

# Score-tangent arc (small, r~2) sits near the LID_TUCK score line.
# Baseline: (4.618, 486.228) r=2.132. At t=3: (9.227, 492.453) r=4.273.
# r appears to scale with t: r = 2.132 + a*(t-1.5). Slope ~ (4.273-2.132)/1.5 = 1.427.
# So r_score = 1.427*t + 0 (since at t=0, r=0?). Hmm let's check at baseline t=1.5: 1.427*1.5 = 2.14 ≈ 2.132 ✓
# At t=3: 1.427*3 = 4.28 ≈ 4.273 ✓
def score_tangent_r(s):
    return 1.427 * s.t

def score_tangent_offset_x(s):
    """Offset from LID_TUCK main arc center to score-tangent arc center."""
    # baseline 4.618 - 2.25 = 2.368. At t=3: 9.227 - 4.5 = 4.727.
    # Coef in t: (4.727-2.368)/1.5 = 1.573
    return 1.573 * s.t

def score_tangent_offset_y(s):
    """Offset from LID_TUCK main arc center to score-tangent arc center (y)."""
    # baseline 486.228 - 486.75 = -0.522. At t=3: 492.453 - 493.5 = -1.047. coef = -0.35/t
    return -0.348 * s.t

# Upper LID corner arc y position: empirical from samples
def lid_corner_upper_y(s):
    # baseline: 434.682; H=120: 440.400; W=250: 536.848; t=3: 438.570
    # ΔH=40 → +5.718 (coef 0.143)
    # ΔW=50 → +102.166 (coef 2.043)
    # Δt=1.5 → +3.888 (coef 2.59)
    return 0.143 * s.H + 2.043 * s.W + 2.59 * s.t + 19.357 - 414.682  # ugh empirical
    # = 0.143H + 2.043W + 2.59t - 395.325
    # check baseline: 0.143*80 + 2.043*200 + 2.59*1.5 - 395.325 = 11.44+408.6+3.885-395.325 = 28.6
    # That's not right. Let me redo:
    # baseline output: 434.682
    # 0.143*80 + 2.043*200 + 2.59*1.5 + d = 434.682
    # 11.44 + 408.6 + 3.885 + d = 434.682
    # 423.925 + d = 434.682
    # d = 10.757

# (I'll just hardcode the empirical formula below in lid_corner_arcs.)


def lid_outer_edges(s: MailerSpec) -> List[Line]:
    """LID outer vertical edges connecting the lower and upper corner arcs.
    Baseline: x=-74 and x=L+74, y from 325.625 to 434.682. Length 109.057."""
    L, W, H, t = s.L, s.W, s.H, s.t
    x_lid_left = -(H - 4*t)          # = -74 at H=80, t=1.5
    x_lid_right = x_mirror(s, x_lid_left)
    y_lower = W + 1.5*H + 3.75*t     # = 325.625 at baseline
    y_upper = 0.143*H + 2.043*W + 2.59*t + 10.757  # = 434.682 at baseline
    return [
        Line(x_lid_left,  y_lower, x_lid_left,  y_upper, LAYER_CUT),
        Line(x_lid_right, y_lower, x_lid_right, y_upper, LAYER_CUT),
    ]


def lid_corner_arcs(s: MailerSpec) -> List[Arc]:
    """4 concave arcs at LID corners (lower-left/right, upper-left/right).

    Formulas:
      cx_left  = -0.5*H + 4.25*t      (cx_right = x_mirror(s, cx_left))
      r        = 0.5*H + 0.25*t
      cy_lower = W + 1.5*H + 3.75*t   (= y_score_back_lid + H/2 + 1.25t)
      cy_upper = 0.143*H + 2.043*W + 2.59*t + 10.757   (empirical fit)
      sa/ea: lower-left  sa=180  ea=270
             lower-right sa=270  ea=360 (=0)
             upper-left  sa=ea_top  ea=180
             upper-right sa=0      ea=ea_top_mirror
    """
    L, W, H, t = s.L, s.W, s.H, s.t
    cx_left = -0.5*H + 4.25*t
    cx_right = x_mirror(s, cx_left)
    r = 0.5*H + 0.25*t
    cy_lower = W + 1.5*H + 3.75*t
    cy_upper = 0.143*H + 2.043*W + 2.59*t + 10.757

    # Upper arc angles: at baseline sa=101.95, ea=180 (left). Means the tangent
    # diagonal leaves the arc at 101.95 deg. At t=3: sa=102.12, H=120: sa=108.65.
    # Approximation: sa_upper depends on tangent geometry.
    # Use baseline value with simple scaling.
    sa_upper_left = 101.95 + (s.H - 80) * 0.168    # rough
    ea_upper_right = 180 - sa_upper_left

    sa_lower_right = 270 + (90 - 90)  # constant 270
    return [
        # lower-left (concave, sweeps -x to -y quadrant): sa=180, ea=270
        Arc(cx_left,  cy_lower, r, 180, 270, LAYER_CUT),
        # lower-right: sa=270, ea=360
        Arc(cx_right, cy_lower, r, 270, 360, LAYER_CUT),
        # upper-left: sa=sa_upper, ea=180
        Arc(cx_left,  cy_upper, r, sa_upper_left, 180, LAYER_CUT),
        # upper-right: sa=0, ea=180-sa_upper
        Arc(cx_right, cy_upper, r, 0, ea_upper_right, LAYER_CUT),
    ]


def lid_tuck_main_arcs(s: MailerSpec) -> List[Arc]:
    """2 main ear-lock arcs (r = H + 0.5t) centered at the LID_TUCK score."""
    L, W, H, t = s.L, s.W, s.H, s.t
    cx_left = 1.5 * t
    cx_right = x_mirror(s, cx_left)
    cy = y_score_lid_tuck(s)
    r = H + 0.5 * t

    # ea_left = angle from main center to horn center (internally tangent)
    horn_dx = horn_offset_x(s)
    horn_dy = horn_offset_y(s)
    ea_left = math.degrees(math.atan2(horn_dy, horn_dx)) % 360.0
    # Right arc is mirror: sa from horn-mirror angle to 90
    sa_right = (180 - ea_left) % 360.0

    return [
        Arc(cx_left,  cy, r, 90, ea_left, LAYER_CUT),
        Arc(cx_right, cy, r, sa_right, 90, LAYER_CUT),
    ]


def lid_tuck_horn_arcs(s: MailerSpec) -> List[Arc]:
    """2 small horn arcs (r=3) at the outer tips of the ear-locks."""
    L, W, H, t = s.L, s.W, s.H, s.t
    main_cx_left = 1.5 * t
    main_cy = y_score_lid_tuck(s)

    horn_cx_left = main_cx_left + horn_offset_x(s)
    horn_cy = main_cy + horn_offset_y(s)
    horn_cx_right = x_mirror(s, horn_cx_left)

    r_horn = 3.0
    # sa = continues from where main arc left off; ea = goes back inward by ~93.5 deg
    # baseline horn sa=171.57, ea=265.11 -> sweep = 93.54
    horn_dx = horn_offset_x(s)
    horn_dy = horn_offset_y(s)
    sa_horn_left = math.degrees(math.atan2(horn_dy, horn_dx)) % 360.0
    ea_horn_left = (sa_horn_left + 93.54) % 360.0
    return [
        Arc(horn_cx_left,  horn_cy, r_horn, sa_horn_left, ea_horn_left, LAYER_CUT),
        Arc(horn_cx_right, horn_cy, r_horn,
            (180 - ea_horn_left) % 360.0,
            (180 - sa_horn_left) % 360.0, LAYER_CUT),
    ]


def lid_tuck_score_arcs(s: MailerSpec) -> List[Arc]:
    """2 small arcs (r ~ 2t scaled) near the LID_TUCK score line on each side."""
    L, W, H, t = s.L, s.W, s.H, s.t
    main_cx_left = 1.5 * t
    main_cy = y_score_lid_tuck(s)
    cx_left = main_cx_left + score_tangent_offset_x(s)
    cy = main_cy + score_tangent_offset_y(s)
    cx_right = x_mirror(s, cx_left)
    r = score_tangent_r(s)

    # baseline sa=-78.05 ea=85.12 -> sweep ~163
    # at H=120: sa=-71.35 ea=84.42 -> sweep ~156
    # Approximation: keep baseline angles with small H/t adjustments
    sa_left = -78.05 + (s.H - 80) * 0.168
    ea_left = 85.12 + (s.t - 1.5) * (-0.083)
    return [
        Arc(cx_left,  cy, r, sa_left, ea_left, LAYER_CUT),
        Arc(cx_right, cy, r,
            (180 - ea_left) % 360.0,
            (180 - sa_left) % 360.0, LAYER_CUT),
    ]


def lid_tuck_tangent_diagonals(s: MailerSpec) -> List[Line]:
    """Tangent line segments connecting the LID corner arc to score-tangent arc,
    and horn arc to LID_TUCK score-tangent path.

    Two pairs of diagonals at baseline:
      A: (-74.915, 495.165) -> (4.799, 488.353)   horn->score tangent
      B: (-41.988, 474.181) -> (5.059, 484.142)   LID-corner-upper->score-tangent
    """
    L, W, H, t = s.L, s.W, s.H, s.t

    # Horn-end tangent diagonal A
    main_cx = 1.5 * t
    main_cy = y_score_lid_tuck(s)
    horn_cx_left = main_cx + horn_offset_x(s)
    horn_cy = main_cy + horn_offset_y(s)
    # Tangent point on horn arc (at its ea angle pointing back inward)
    horn_sa = math.degrees(math.atan2(horn_offset_y(s), horn_offset_x(s)))
    horn_ea = (horn_sa + 93.54)
    pA_horn_x = horn_cx_left + 3.0 * math.cos(math.radians(horn_ea))
    pA_horn_y = horn_cy + 3.0 * math.sin(math.radians(horn_ea))
    # Tangent point on score-tangent arc (at angle ea_left)
    score_cx_left = main_cx + score_tangent_offset_x(s)
    score_cy = main_cy + score_tangent_offset_y(s)
    score_r = score_tangent_r(s)
    score_ea = 85.12 + (s.t - 1.5) * (-0.083)
    pA_score_x = score_cx_left + score_r * math.cos(math.radians(score_ea))
    pA_score_y = score_cy + score_r * math.sin(math.radians(score_ea))

    # Upper-LID-corner tangent diagonal B
    lid_cx_left = -0.5*H + 4.25*t
    lid_cy_upper = 0.143*H + 2.043*W + 2.59*t + 10.757
    lid_r = 0.5*H + 0.25*t
    lid_sa = 101.95 + (s.H - 80) * 0.168
    pB_lid_x = lid_cx_left + lid_r * math.cos(math.radians(lid_sa))
    pB_lid_y = lid_cy_upper + lid_r * math.sin(math.radians(lid_sa))
    score_sa = -78.05 + (s.H - 80) * 0.168
    pB_score_x = score_cx_left + score_r * math.cos(math.radians(score_sa))
    pB_score_y = score_cy + score_r * math.sin(math.radians(score_sa))

    return [
        # Diagonal A (left)
        Line(pA_horn_x, pA_horn_y, pA_score_x, pA_score_y, LAYER_CUT),
        Line(x_mirror(s, pA_horn_x), pA_horn_y, x_mirror(s, pA_score_x), pA_score_y, LAYER_CUT),
        # Diagonal B (left)
        Line(pB_lid_x, pB_lid_y, pB_score_x, pB_score_y, LAYER_CUT),
        Line(x_mirror(s, pB_lid_x), pB_lid_y, x_mirror(s, pB_score_x), pB_score_y, LAYER_CUT),
    ]


# ============================================================
# === Phase 5: finger holes ===================================
# ============================================================
#
# One rectangular finger hole per outer tooth, placed in the BOTTOM panel
# right against the side-wall fold (x in [0, FINGER_HOLE_W] on the left,
# mirrored on the right).
#
# Baseline (W=200, n_outer=2):
#   hole 1: x [0, 5.25], y [40.6, 81.2]    (matches outer tooth 1 at y=41.85-79.95)
#   hole 2: x [0, 5.25], y [121.8, 162.4]  (matches outer tooth 2 at y=123.05-161.15)
#
# Each hole extends by TOOTH_TRANSITION (= 1.25) above and below the outer
# tooth's y range, so hole_height = tooth_length + 2*transition = 38.1 + 2.5 = 40.6.


def finger_holes(s: MailerSpec) -> List[Polyline]:
    """Closed-rectangle finger holes, one per outer tooth, mirrored to both sides."""
    L = s.L
    fh_w = s.FINGER_HOLE_W
    pattern = _tooth_pattern_y(s)
    holes = []
    for kind, ys, ye in pattern:
        if kind != 'O':
            continue
        hole_y_bot = ys - s.TOOTH_TRANSITION   # = ys - 1.25
        hole_y_top = ye + s.TOOTH_TRANSITION   # = ye + 1.25
        # Left hole
        holes.append(Polyline(
            pts=((0,    hole_y_bot),
                 (0,    hole_y_top),
                 (fh_w, hole_y_top),
                 (fh_w, hole_y_bot),
                 (0,    hole_y_bot)),
            closed=True, layer=LAYER_CUT))
        # Right hole
        holes.append(Polyline(
            pts=((x_mirror(s, fh_w), hole_y_bot),
                 (x_mirror(s, fh_w), hole_y_top),
                 (x_mirror(s, 0),        hole_y_top),
                 (x_mirror(s, 0),        hole_y_bot),
                 (x_mirror(s, fh_w), hole_y_bot)),
            closed=True, layer=LAYER_CUT))
    return holes


# ============================================================
# === Phase 6: missing fold scores + corner connectors =======
# ============================================================
#
# 14 additional fold scores per dieline (7 per side, mirrored). These run
# along the panel-to-dust-flap fold lines (where each panel's dust flap
# folds inward when the box is assembled). Plus small cut connectors at
# the panel-corner transitions.
#
# Per-panel fold-score x positions (LEFT side):
#   FRONT_TUCK  : x = 2*t       (= 3.0 at baseline)
#   BOTTOM      : x = 0         (split into n_holes+1 segments by finger holes)
#   BACK_WALL   : x = t         (= 1.5 at baseline)
#   LID         : x = 4.5*t     (= 6.75 at baseline)
#   LID_TUCK    : x = 1.5*t     (= 2.25 at baseline)


def dust_flap_fold_scores(s: MailerSpec) -> List[Line]:
    """Vertical fold scores on each panel where the dust flap folds inward."""
    L, W, H, t = s.L, s.W, s.H, s.t
    ents = []

    # FRONT_TUCK fold: x = 2t, y from y_bot (-H) to y_score_tuck_bottom (+t/2)
    x_ft = 2 * t
    ents.append(Line(x_ft, -H, x_ft, y_score_tuck_bottom(s), LAYER_SCORE))
    ents.append(Line(x_mirror(s, x_ft), -H, x_mirror(s, x_ft), y_score_tuck_bottom(s), LAYER_SCORE))

    # BACK_WALL fold: x = t, y from y_score_bottom_back to y_back_cut (W+2t+H)
    x_bw = t
    y_back_cut = W + 2*t + H
    ents.append(Line(x_bw, y_score_bottom_back(s), x_bw, y_back_cut, LAYER_SCORE))
    ents.append(Line(x_mirror(s, x_bw), y_score_bottom_back(s), x_mirror(s, x_bw), y_back_cut, LAYER_SCORE))

    # LID fold: x = 4.5t, y from y_back_cut + t to y_score_lid_tuck
    x_lid = 4.5 * t
    ents.append(Line(x_lid, y_back_cut + t, x_lid, y_score_lid_tuck(s), LAYER_SCORE))
    ents.append(Line(x_mirror(s, x_lid), y_back_cut + t, x_mirror(s, x_lid), y_score_lid_tuck(s), LAYER_SCORE))

    # LID_TUCK fold: x = 1.5t, y from cy_main_arc + r to y_top - t/2
    # Baseline: x=2.25, y from 488.57 to 567.5 (length 78.93). Main arc top at 567.5, score
    # at 486.75. So the fold runs almost the full LID_TUCK height.
    x_lt = 1.5 * t
    y_lt_bot = y_score_lid_tuck(s) + (s.t * 1.213)  # baseline 488.57; offset = 1.82 = 1.213*t
    y_lt_top = y_score_lid_tuck(s) + (H + 0.5*t)    # = main arc top = cy + r = 567.5
    ents.append(Line(x_lt, y_lt_bot, x_lt, y_lt_top, LAYER_SCORE))
    ents.append(Line(x_mirror(s, x_lt), y_lt_bot, x_mirror(s, x_lt), y_lt_top, LAYER_SCORE))

    # BOTTOM panel: x = 0 (left edge), split into n_holes+1 segments by finger holes.
    # The tooth pattern gives us y ranges of each long-inner ('I') segment which is
    # where the score continues; the finger hole regions get NO score line.
    # Baseline: x=0 segments at y=0-40.6, y=81.2-121.8, y=162.4-203 (3 segments
    # for n_holes=2). Plus matching x=L+0 mirrors.
    #
    # Trace from tooth pattern: 'I' segments get a score line, 'O' segments
    # (where finger holes are) do NOT.
    pattern = _tooth_pattern_y(s)
    for kind, ys, ye in pattern:
        if kind != 'I':
            continue
        # x=0 score from y=ys-t/2 to y=ye+t/2  -- extend slightly to merge with
        # adjacent transitions. Actually baseline shows: y=0-40.6 (first segment)
        # spans from y_score_tuck_bottom-area to the lower edge of first finger hole.
        # The first segment baseline: y=0-40.6, where 0 = y_lid_tuck_bottom - t/2 (= -t/2)?
        # Hmm actually y=0 to 40.6 starts at y=0 not y=-t/2. So segments at exact 'I'
        # boundaries.
        # Looking at baseline: I segments at y=1.5-40.6, 81.2-121.8, 162.4-201.5.
        # The score segments are at y=0-40.6, y=81.2-121.8, y=162.4-203.
        # So score extends slightly past the tooth: bottom edge goes down to y=0 (= ys - t = 1.5 - 1.5)
        # and top edge goes up to y=203 for the last segment (= ye + 1.5 = 201.5 + 1.5).
        # Middle segment stays exactly at I-segment bounds.
        # Simplification: snap to the SCORE lines bounding the BOTTOM panel
        # (y_score_tuck_bottom = 0.75 to y_score_bottom_back = 202.25) for first
        # and last segments.
        score_y0 = max(ys - t, y_score_tuck_bottom(s) - t/2)
        score_y1 = min(ye + t, y_score_bottom_back(s) + t/2)
        # Skip degenerate segments
        if score_y1 - score_y0 < t/2:
            continue
        # Left side
        ents.append(Line(0, score_y0, 0, score_y1, LAYER_SCORE))
        # Right side
        ents.append(Line(x_mirror(s, 0), score_y0, x_mirror(s, 0), score_y1, LAYER_SCORE))

    return ents


def corner_connectors(s: MailerSpec) -> List:
    """Small cut connectors at panel-corner transitions that bridge the
    dust-flap fold area to the BACK_WALL / FRONT_TUCK panel cuts.

    Baseline connector inventory (left half):
      (0, 0)      -> (3, 0.75)         FRONT_TUCK corner -> dust flap fold
      (0, 203)    -> (1.5, 202.25)     BOTTOM corner -> BACK_WALL fold
      (6.75, 283.75) -> (1.5, 283)     LID fold start -> BACK_WALL top cut

    Plus:
      y=285.25 horizontal from x=-33.625 to x=6.75: connects LID corner arc
      base to LID dust flap fold.
    """
    L, W, H, t = s.L, s.W, s.H, s.t
    y_back_cut = W + 2*t + H
    ents = []

    # FRONT_TUCK corner connector (left): (0, 0) -> (2t, t/2)
    ents.append(Line(0, 0, 2*t, t/2, LAYER_CUT))
    ents.append(Line(x_mirror(s, 0), 0, x_mirror(s, 2*t), t/2, LAYER_CUT))

    # BOTTOM-to-BACK_WALL corner connector (left): (0, W+t) -> (t, W+1.5t)
    ents.append(Line(0, W + t, t, y_score_bottom_back(s), LAYER_CUT))
    ents.append(Line(x_mirror(s, 0), W + t, x_mirror(s, t), y_score_bottom_back(s), LAYER_CUT))

    # LID-to-BACK_WALL corner connector (left): (4.5t, y_back_cut + t/2) -> (t, y_back_cut)
    ents.append(Line(4.5*t, y_back_cut + t/2, t, y_back_cut, LAYER_CUT))
    ents.append(Line(x_mirror(s, 4.5*t), y_back_cut + t/2, x_mirror(s, t), y_back_cut, LAYER_CUT))

    # LID-base horizontal connector: y = y_score_back_lid + t, from lower-LID-corner-arc base
    # to LID fold position. Baseline: y=285.25 from x=-33.625 to x=6.75.
    lid_arc_cx = -0.5*H + 4.25*t   # = -33.625 at baseline (lower-left corner arc x center)
    y_lid_base = y_score_back_lid(s) + t   # = 283.75 + 1.5 = 285.25
    ents.append(Line(lid_arc_cx, y_lid_base, 4.5*t, y_lid_base, LAYER_CUT))
    ents.append(Line(x_mirror(s, lid_arc_cx), y_lid_base, x_mirror(s, 4.5*t), y_lid_base, LAYER_CUT))

    return ents


def lid_tuck_top_cut(s: MailerSpec) -> List[Line]:
    """The flat top cut between the two LID_TUCK main arc tops.

    Baseline: y=568.25 from x=2.25 to x=312.75 (length 310.5). Plus two
    tiny verticals (at x=2.25 and x=312.75) connecting the arc tops at
    y=567.5 to the flat top at y=568.25. Length 0.75 = t/2 each.
    """
    L, t = s.L, s.t
    x_arc_left = 1.5 * t       # LID_TUCK main arc cx
    y_arc_top = y_score_lid_tuck(s) + (s.H + 0.5*s.t)   # = 567.5 at baseline
    y_flat_top = y_top(s)      # = 568.25 at baseline
    return [
        # Flat top horizontal cut
        Line(x_arc_left, y_flat_top, x_mirror(s, x_arc_left), y_flat_top, LAYER_CUT),
        # Small vertical connectors
        Line(x_arc_left, y_arc_top, x_arc_left, y_flat_top, LAYER_CUT),
        Line(x_mirror(s, x_arc_left), y_arc_top, x_mirror(s, x_arc_left), y_flat_top, LAYER_CUT),
    ]


# ============================================================
# === Generator main =========================================
# ============================================================

def generate(spec: MailerSpec, phase: int = 1) -> List:
    """Compose all parts into the full dieline entity list.

    Phase argument controls how much detail is included:
      1 = spine outline + scores only
      2 = + side walls
      3 = + dust flaps
      4 = + ear-lock arcs
      5 = + finger holes (full)
    """
    spec.validate()
    entities = []
    entities.extend(spine_scores(spec))
    if phase < 3:
        entities.extend(spine_outline(spec))
    if phase >= 2:
        left = left_side_wall(spec)
        entities.extend(left)
        entities.extend(mirror_x(left, mirror_axis_x(spec)))
    if phase >= 3:
        # Dust flaps replace the simple spine outline at y=-H and y=W+H+2.5t
        # because they emit those edges with the dust-flap chamfers.
        entities.extend(panel_outer_edges(spec))
        ft_left = front_tuck_dust_flap_left(spec)
        bw_left = back_wall_dust_flap_left(spec)
        entities.extend(ft_left)
        entities.extend(mirror_x(ft_left, mirror_axis_x(spec)))
        entities.extend(bw_left)
        entities.extend(mirror_x(bw_left, mirror_axis_x(spec)))
    if phase >= 4:
        entities.extend(lid_outer_edges(spec))
        entities.extend(lid_corner_arcs(spec))
        entities.extend(lid_tuck_main_arcs(spec))
        entities.extend(lid_tuck_horn_arcs(spec))
        entities.extend(lid_tuck_score_arcs(spec))
        entities.extend(lid_tuck_tangent_diagonals(spec))
        entities.extend(lid_tuck_top_cut(spec))
    if phase >= 5:
        entities.extend(finger_holes(spec))
    if phase >= 6:
        entities.extend(dust_flap_fold_scores(spec))
        entities.extend(corner_connectors(spec))
    elif phase == 3:
        # Phase 3 only: emit a simple top cut where Phase 4 will put arcs
        entities.append(Line(0, y_top(spec), spec.L, y_top(spec), LAYER_CUT))
    return entities


if __name__ == "__main__":
    spec = MailerSpec(L=300, W=200, H=80, t=1.5)
    ents = generate(spec, phase=1)
    print(f"Phase 1: {len(ents)} entities")
    for e in ents:
        print(f"  {type(e).__name__}: {e}")
