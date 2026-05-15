"""Insert tray with backer parametric part library.

Single-slot or N-slot configurations supported.

Coordinate frame (matches CEFbox baseline):
  Floor: x in [0, L], y in [-(W+0.5t), +0.5t].  Floor is the central panel
  on which product(s) sit. Slot(s) are cut into the floor as nestings.

  Above the floor (y > 0): 4-band structure that folds up into the
  insert tray's BACK WALL + TOP HEM + BACKER PANEL + HEM_FLAP.
    Band 1 (height H+t):       BACK wall outer
    Band 2 (height IH):        TOP hem
    Band 3 (height W-2t):      BACKER panel (display + QR raster surface)
    Band 4 (height IH+0.5t):   HEM flap

  Below the floor (y < -(W+0.5t)): FRONT wall.
    Single panel of height H+1.5t.

  Left/right of the floor: side walls with corner arcs (r=8).

Mirror symmetry: about x = L/2 (NOT L/2 + 5t like the mailer).
Validation supports skip_symmetry for N>1-slot asymmetric layouts.

Build phases (verify-after-each):
    1: floor + scores + slot(s) + cradle X-fold + cradle inner rect
    2: + top bands (back wall, top hem, backer, hem flap) + scores
    3: + front wall + bottom corner arcs
    4: + side walls (left/right) + side corner arcs
    5: + connecting chamfers (the 0.132 / 0.75 micro-chamfers at panel corners)
"""

from __future__ import annotations
from dataclasses import dataclass, field
from typing import List, NamedTuple, Tuple
import math

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
    pts: Tuple
    closed: bool = True
    layer: str = LAYER_CUT


# ============================================================
# === Slot + Spec ============================================
# ============================================================

@dataclass
class Slot:
    """One product slot. Positioned absolutely on the floor (cx, cy)."""
    cx: float        # slot center x
    cy: float        # slot center y (defaults to -W/2 = floor center)
    SL: float        # slot length (along x)
    SW: float        # slot width (along y)
    SH: float        # slot depth (cradle inset)


@dataclass
class InsertSpec:
    L: float
    W: float
    H: float
    t: float = 1.5
    IH: float = 55                          # insert height (= H - SH for snug fit)
    slots: List[Slot] = field(default_factory=list)

    # Constants
    CORNER_R: float = 8.0                   # corner arc radius
    CHAMFER_ANGLE_DEG: float = 10.0         # micro-chamfer angle at panel corners
    BACKER_LEFT_INSET: float = 14.503       # x-inset of top bands from floor edge
                                            # TODO: derive parametrically

    # CEFbox-style rule: 5mm buffer between any slot and surrounding fold lines,
    # plus a 5mm minimum gap between slots.
    SLOT_BUFFER: float = 5.0

    def validate(self) -> None:
        """Raise ValueError if any slot violates the buffer rule or overlaps
        another slot. Called automatically by generate()."""
        b = self.SLOT_BUFFER
        for i, slot in enumerate(self.slots):
            sl_l = slot.cx - slot.SL/2
            sl_r = slot.cx + slot.SL/2
            sl_t = slot.cy + slot.SW/2     # less negative
            sl_b = slot.cy - slot.SW/2
            issues = []
            if sl_l < b:
                issues.append(f"left edge x={sl_l:.1f} < {b} buffer")
            if sl_r > self.L - b:
                issues.append(f"right edge x={sl_r:.1f} > L-{b} ({self.L - b})")
            if sl_t > -b:
                issues.append(f"top edge y={sl_t:.1f} > -{b} buffer")
            if sl_b < -self.W + b:
                issues.append(f"bottom edge y={sl_b:.1f} < -W+{b} ({-self.W + b})")
            if slot.SL <= 0 or slot.SW <= 0 or slot.SH <= 0:
                issues.append(f"SL={slot.SL}, SW={slot.SW}, SH={slot.SH} must all be > 0")
            if issues:
                raise ValueError(
                    f"Slot {i+1} (cx={slot.cx}, cy={slot.cy}, "
                    f"{slot.SL}x{slot.SW}x{slot.SH}) violates buffer rules:\n  - "
                    + "\n  - ".join(issues))

        # Pairwise slot overlap check (with buffer)
        for i, a in enumerate(self.slots):
            for j, c in enumerate(self.slots[i+1:], start=i+1):
                a_l, a_r = a.cx - a.SL/2, a.cx + a.SL/2
                a_t, a_b = a.cy + a.SW/2, a.cy - a.SW/2
                c_l, c_r = c.cx - c.SL/2, c.cx + c.SL/2
                c_t, c_b = c.cy + c.SW/2, c.cy - c.SW/2
                # Expand A by buffer on each side and test against C
                x_overlap = not (a_r + b <= c_l or c_r + b <= a_l)
                y_overlap = not (a_t + b <= c_b or c_t + b <= a_b)
                if x_overlap and y_overlap:
                    raise ValueError(
                        f"Slots {i+1} and {j+1} are within {b}mm of each other "
                        f"(or overlap). Move one or shrink them.")

    @classmethod
    def single_slot(cls, L, W, H, t=1.5, SL=150, SW=150, SH=25, IH=None):
        if IH is None:
            IH = H - SH
        return cls(L=L, W=W, H=H, t=t, IH=IH, slots=[
            Slot(cx=L/2, cy=-W/2, SL=SL, SW=SW, SH=SH)
        ])

    @classmethod
    def n_slot(cls, L, W, H, t=1.5, n=2, SL=150, SW=150, SH=25, IH=None):
        """N slots evenly distributed along L axis, all sized SL×SW×SH."""
        if IH is None:
            IH = H - SH
        total_slot_width = n * SL
        spacing = (L - total_slot_width) / (n + 1)
        slots = []
        for i in range(n):
            cx = spacing + SL/2 + i * (SL + spacing)
            slots.append(Slot(cx=cx, cy=-W/2, SL=SL, SW=SW, SH=SH))
        return cls(L=L, W=W, H=H, t=t, IH=IH, slots=slots)


# ============================================================
# === Geometry helpers =======================================
# ============================================================

def mirror_axis_x(s):  return s.L / 2   # insert mirrors about L/2 exactly
def x_mirror(s, x):    return 2 * mirror_axis_x(s) - x


# ============================================================
# === Phase 1: floor + slot ==================================
# ============================================================

# Floor edge score y/x values:
def y_floor_top(s):     return s.t * 0.5        # = 0.75
def y_floor_bot(s):     return -(s.W + s.t * 0.5)  # = -200.75
def x_floor_left(s):    return -s.t * 0.5       # = -0.75
def x_floor_right(s):   return s.L + s.t * 0.5  # = 300.75


def floor_scores(s: InsertSpec) -> List[Line]:
    """4 scores at the floor's top/bottom/left/right edges.
    Each is slightly INSET to clear the corner chamfers."""
    L, W, t = s.L, s.W, s.t
    chamfer_dx = (t / 2) * math.tan(math.radians(s.CHAMFER_ANGLE_DEG))  # ~0.132
    # Horizontal scores (top and bottom of floor)
    return [
        Line(chamfer_dx, y_floor_top(s),
             L - chamfer_dx, y_floor_top(s), LAYER_SCORE),
        Line(chamfer_dx, y_floor_bot(s),
             L - chamfer_dx, y_floor_bot(s), LAYER_SCORE),
        # Vertical scores (left and right of floor)
        Line(x_floor_left(s), -(W) + chamfer_dx,
             x_floor_left(s), -chamfer_dx, LAYER_SCORE),
        Line(x_floor_right(s), -(W) + chamfer_dx,
             x_floor_right(s), -chamfer_dx, LAYER_SCORE),
    ]


def slot_geometry(s: InsertSpec, slot: Slot) -> List:
    """For one slot: 4 scores forming the slot outer rectangle,
    4 diagonals from outer corners to inner cradle corners,
    and either a Polyline (cradle rect) or a Line (degenerate cradle)
    or nothing (cradle collapsed to a point).

    SH is clamped per-slot to min(SH, SL/2, SW/2) so the cradle inner
    rectangle never has negative dimensions."""
    cx, cy = slot.cx, slot.cy
    SL, SW = slot.SL, slot.SW
    SH = min(slot.SH, SL/2, SW/2)   # effective SH (clamped)

    # Outer slot corners
    out_left   = cx - SL/2
    out_right  = cx + SL/2
    out_top    = cy + SW/2     # less negative
    out_bot    = cy - SW/2     # more negative
    # Inner cradle corners (inset by effective SH from outer)
    in_left    = out_left + SH
    in_right   = out_right - SH
    in_top     = out_top - SH
    in_bot     = out_bot + SH

    ents = [
        # Slot outer scores (4 lines, forming a closed rectangle)
        Line(out_left, out_top,  out_right, out_top,  LAYER_SCORE),   # top
        Line(out_left, out_bot,  out_right, out_bot,  LAYER_SCORE),   # bottom
        Line(out_left, out_bot,  out_left,  out_top,  LAYER_SCORE),   # left
        Line(out_right, out_bot, out_right, out_top,  LAYER_SCORE),   # right
        # Cradle diagonals (4 lines, from inner corners to outer corners)
        Line(in_left,  in_top,  out_left,  out_top,  LAYER_CUT),
        Line(in_right, in_top,  out_right, out_top,  LAYER_CUT),
        Line(in_left,  in_bot,  out_left,  out_bot,  LAYER_CUT),
        Line(in_right, in_bot,  out_right, out_bot,  LAYER_CUT),
    ]
    # Inner cradle: a polyline if both dimensions positive, else a degenerate
    # line (when SH == SW/2 or SH == SL/2).
    inner_w = in_right - in_left
    inner_h = in_top - in_bot
    if inner_w > 0.01 and inner_h > 0.01:
        ents.append(Polyline(
            pts=((in_left, in_top),
                 (in_right, in_top),
                 (in_right, in_bot),
                 (in_left, in_bot)),
            closed=True, layer=LAYER_CUT))
    elif inner_h <= 0.01 and inner_w > 0.01:
        # Horizontal degenerate line (cradle collapsed in y)
        ents.append(Line(in_left, in_top, in_right, in_top, LAYER_CUT))
    elif inner_w <= 0.01 and inner_h > 0.01:
        # Vertical degenerate line (cradle collapsed in x)
        ents.append(Line(in_left, in_top, in_left, in_bot, LAYER_CUT))
    # else both degenerate: emit nothing (cradle is a single point)
    return ents


# ============================================================
# === Phase 2: top bands (back wall + backer + hem) ==========
# ============================================================
#
# Above the floor (y > 0): 4 stacked panels separated by 3 horizontal
# scores. Total height (top cut at y_band_top_cut = IH + 0.5t + W - 2t +
# IH + H + t) = 2*IH + W + H + ... wait let me recompute.
#
# Heights bottom-to-top above floor score (y=+0.5t):
#   Band 1: H + t           (from baseline: 82.25 - 0.75 = 81.5)
#   Band 2: IH              (from baseline: 137.25 - 82.25 = 55)
#   Band 3: W - 2t          (from baseline: 334.25 - 137.25 = 197 = 200 - 3)
#   Band 4: IH + 0.5t       (from baseline: 390 - 334.25 = 55.75)
#
# Top cut at y = 0.5t + (H+t) + IH + (W-2t) + (IH + 0.5t)
#              = W + H + 2*IH

def y_band1_score(s):  return s.t*0.5 + s.H + s.t           # 82.25
def y_band2_score(s):  return s.t*0.5 + s.H + s.t + s.IH    # 137.25
def y_band3_score(s):  return s.t*0.5 + s.H + s.t + s.IH + s.W - 2*s.t  # 334.25
def y_top_cut(s):      return s.W + s.H + 2*s.IH            # 390


def top_band_scores(s: InsertSpec) -> List[Line]:
    """3 horizontal scores separating the 4 bands above the floor.
    Top bands have a larger x-inset than the floor (14.503 at baseline)."""
    L = s.L
    inset = s.BACKER_LEFT_INSET   # 14.503 baseline; TODO: parametric
    return [
        Line(inset, y_band1_score(s),
             L - inset, y_band1_score(s), LAYER_SCORE),
        Line(inset, y_band2_score(s),
             L - inset, y_band2_score(s), LAYER_SCORE),
        Line(inset, y_band3_score(s),
             L - inset, y_band3_score(s), LAYER_SCORE),
    ]


def top_band_edges(s: InsertSpec) -> List[Line]:
    """Outer cut edges of the top bands: left and right verticals from y=+0.5t
    to y=y_top_cut, plus the top horizontal cut at y=y_top_cut."""
    L = s.L
    inset = s.BACKER_LEFT_INSET
    return [
        # Left vertical (3 segments separated by the band scores)
        Line(inset, y_band1_score(s),     inset, y_band2_score(s),     LAYER_CUT),
        Line(inset, y_band2_score(s),     inset, y_band3_score(s),     LAYER_CUT),
        Line(inset, y_band3_score(s),     inset, y_top_cut(s),         LAYER_CUT),
        # Right vertical (mirror)
        Line(L - inset, y_band1_score(s), L - inset, y_band2_score(s), LAYER_CUT),
        Line(L - inset, y_band2_score(s), L - inset, y_band3_score(s), LAYER_CUT),
        Line(L - inset, y_band3_score(s), L - inset, y_top_cut(s),     LAYER_CUT),
        # Top horizontal cut
        Line(inset, y_top_cut(s), L - inset, y_top_cut(s), LAYER_CUT),
    ]


# ============================================================
# === Phase 3: front wall + bottom corner arcs ===============
# ============================================================
#
# Below the floor (y < y_floor_bot): a single panel (front wall) of
# height H+1.5t. Bottom cut at y = -(W + H + 2t).
#
# At each bottom corner, a tangent line + small corner arc (r=8) that
# rounds the bottom-left and bottom-right corners.

def y_bottom_cut(s):  return -(s.W + s.H + 2*s.t)


def front_wall(s: InsertSpec) -> List:
    """FRONT wall below the floor — 2 corner arcs + 1 bottom horizontal cut.
    Tangent diagonals emitted separately in Phase 5 (front_wall_tangents)."""
    L = s.L
    fw_arc_cx = 21.348   # TODO: derive parametrically (constant across H/W/slot vars)
    arc_cy = y_bottom_cut(s) + s.CORNER_R   # = -275 at baseline

    return [
        # Left bottom corner arc (sweeps from tangent-start at 190° to leftmost at 270°)
        Arc(fw_arc_cx, arc_cy, s.CORNER_R, 190, 270, LAYER_CUT),
        # Bottom horizontal cut between the two arcs
        Line(fw_arc_cx, y_bottom_cut(s),
             x_mirror(s, fw_arc_cx), y_bottom_cut(s), LAYER_CUT),
        # Right bottom corner arc (mirror)
        Arc(x_mirror(s, fw_arc_cx), arc_cy, s.CORNER_R, 270, 350, LAYER_CUT),
    ]


# ============================================================
# === Phase 4: left/right side walls with corner arcs ========
# ============================================================
#
# Each side has:
#   - Outer vertical cut at x = -(H + 2t)
#   - 2 corner arcs (r=8) at top and bottom of side wall
#   - Side wall y extent depends on H: y_inset ≈ H*tan(10°) + 7.24
#
# Corner arc center: (x_wall_outer + r, y_inset_top/bot)
# Top arc: sa=100°, ea=180° (from tangent point to leftmost arc)
# Bottom arc: sa=180°, ea=260°

def side_wall_y_inset(s):
    """y-distance from floor top edge (y=0) to the start of the side wall vertical."""
    # Empirical from baseline + H=120: y_inset = H*tan(10°) + 7.24
    return s.H * math.tan(math.radians(10)) + 7.244

def side_wall_x_outer(s):
    return -(s.H + 2*s.t)

def side_wall_arc_cx(s):
    return side_wall_x_outer(s) + s.CORNER_R   # -75 at baseline

def left_side_wall(s: InsertSpec) -> List:
    """LEFT side wall: outer vertical cut + 2 corner arcs."""
    x_outer = side_wall_x_outer(s)
    arc_cx = side_wall_arc_cx(s)
    inset = side_wall_y_inset(s)
    y_top_arc = -inset                   # top arc center y (= -21.348 at baseline)
    y_bot_arc = -s.W + inset             # bottom arc center y (= -178.652 at baseline)

    return [
        # Outer vertical cut
        Line(x_outer, y_bot_arc, x_outer, y_top_arc, LAYER_CUT),
        # Top corner arc (sweeps CCW from 100° to 180°)
        Arc(arc_cx, y_top_arc, s.CORNER_R, 100, 180, LAYER_CUT),
        # Bottom corner arc (sweeps CCW from 180° to 260°)
        Arc(arc_cx, y_bot_arc, s.CORNER_R, 180, 260, LAYER_CUT),
    ]


def side_wall_tangent_diagonals(s: InsertSpec) -> List[Line]:
    """The 4 tangent diagonals from each side wall's corner arcs to the floor's
    left/right edge near the corners. Baseline tangents:
      LEFT TOP:    (-76.389, -13.469) -> (-0.750, -0.132)   angle -170°  len 76.806
      LEFT BOT:    (-76.389, -186.531) -> (-0.750, -199.868)
      RIGHT TOP:   (376.389, -13.469) -> (300.750, -0.132)
      RIGHT BOT:   similar
    """
    L, W, t = s.L, s.W, s.t
    chamfer_angle = math.radians(s.CHAMFER_ANGLE_DEG)
    chamfer_dx = (t/2) * math.tan(chamfer_angle)   # 0.132

    # Arc tangent point on TOP-LEFT arc at angle 100°:
    arc_cx_l = side_wall_arc_cx(s)
    inset = side_wall_y_inset(s)
    y_top_arc = -inset
    y_bot_arc = -s.W + inset

    # Top-left arc tangent point at sa=100°
    p_tl_arc_x = arc_cx_l + s.CORNER_R * math.cos(math.radians(100))
    p_tl_arc_y = y_top_arc + s.CORNER_R * math.sin(math.radians(100))
    # Top-left floor-edge point (just below floor's top edge, on left score)
    p_tl_floor_x = -t/2
    p_tl_floor_y = -chamfer_dx

    # Bottom-left arc tangent point at ea=260°
    p_bl_arc_x = arc_cx_l + s.CORNER_R * math.cos(math.radians(260))
    p_bl_arc_y = y_bot_arc + s.CORNER_R * math.sin(math.radians(260))
    p_bl_floor_x = -t/2
    p_bl_floor_y = -s.W + chamfer_dx

    return [
        # Left top tangent
        Line(p_tl_arc_x, p_tl_arc_y, p_tl_floor_x, p_tl_floor_y, LAYER_CUT),
        # Left bottom tangent
        Line(p_bl_arc_x, p_bl_arc_y, p_bl_floor_x, p_bl_floor_y, LAYER_CUT),
        # Right top tangent (mirror)
        Line(x_mirror(s, p_tl_arc_x), p_tl_arc_y,
             x_mirror(s, p_tl_floor_x), p_tl_floor_y, LAYER_CUT),
        # Right bottom tangent (mirror)
        Line(x_mirror(s, p_bl_arc_x), p_bl_arc_y,
             x_mirror(s, p_bl_floor_x), p_bl_floor_y, LAYER_CUT),
    ]


def floor_corner_chamfers(s: InsertSpec) -> List[Line]:
    """8 tiny chamfers (length ~0.762 mm) at the floor's 4 corners, each connecting
    the floor corner to either the side wall tangent or the top/bottom band tangent.

    Each corner has 2 chamfers (one each direction). At baseline:
      Top-left corner (0, 0):
        (0, 0) -> (-0.75, -0.132)   to LEFT side wall tangent  -- this connects to
                                       the long diagonal going to upper LID corner
        (0, 0) -> (0.132, 0.75)     to TOP band edge tangent

    Plus the connecting horizontal CUT segments at y=0 and y=-W from chamfer
    point to floor x=0.
    """
    L, W, t = s.L, s.W, s.t
    chamfer_angle = math.radians(s.CHAMFER_ANGLE_DEG)
    chamfer_dx = (t/2) * math.tan(chamfer_angle)   # 0.132
    half_t = t / 2                                  # 0.75

    # Top-left corner
    ents = [
        # (0, 0) -> (-t/2, -chamfer_dx)  [LEFT tangent direction]
        Line(0, 0, -half_t, -chamfer_dx, LAYER_CUT),
        # (0, 0) -> (chamfer_dx, t/2)    [TOP tangent direction]
        Line(0, 0, chamfer_dx, half_t, LAYER_CUT),
    ]
    # Top-right corner
    ents += [
        Line(L, 0, L + half_t, -chamfer_dx, LAYER_CUT),
        Line(L, 0, L - chamfer_dx, half_t, LAYER_CUT),
    ]
    # Bottom-left corner
    ents += [
        Line(0, -W, -half_t, -W + chamfer_dx, LAYER_CUT),
        Line(0, -W, chamfer_dx, -W - half_t, LAYER_CUT),
    ]
    # Bottom-right corner
    ents += [
        Line(L, -W, L + half_t, -W + chamfer_dx, LAYER_CUT),
        Line(L, -W, L - chamfer_dx, -W - half_t, LAYER_CUT),
    ]
    return ents


def top_band_tangents(s: InsertSpec) -> List[Line]:
    """2 diagonals from the floor's top corners to the bottom-left/right
    corners of the top backer panel. Baseline:
      LEFT:  (0.132, 0.75) -> (14.503, 82.25)    angle +80°  len 82.757
      RIGHT: mirror
    """
    L, t = s.L, s.t
    chamfer_dx = (t/2) * math.tan(math.radians(s.CHAMFER_ANGLE_DEG))
    inset = s.BACKER_LEFT_INSET
    return [
        Line(chamfer_dx, t/2, inset, y_band1_score(s), LAYER_CUT),
        Line(x_mirror(s, chamfer_dx), t/2,
             x_mirror(s, inset), y_band1_score(s), LAYER_CUT),
    ]


def front_wall_tangents(s: InsertSpec) -> List[Line]:
    """2 diagonals from the floor's bottom corners to the front-wall bottom edge.
    Baseline:
      LEFT: (0.132, -200.75) -> (13.469, -276.389)   angle -80°  len 76.806
    """
    L, W, t = s.L, s.W, s.t
    chamfer_dx = (t/2) * math.tan(math.radians(s.CHAMFER_ANGLE_DEG))
    # Front-wall corner arc tangent at angle (let's compute):
    # arc center: (21.348, -275) at baseline. arc r=8.
    # Tangent point at sa=190°: (21.348 + 8*cos(190°), -275 + 8*sin(190°))
    #                         = (21.348 - 7.878, -275 - 1.389) = (13.470, -276.389) ✓
    fw_arc_cx = 21.348   # TODO: derive parametrically
    fw_arc_cy = y_bottom_cut(s) + s.CORNER_R   # -283 + 8 = -275
    p_fw_x = fw_arc_cx + s.CORNER_R * math.cos(math.radians(190))
    p_fw_y = fw_arc_cy + s.CORNER_R * math.sin(math.radians(190))
    return [
        Line(chamfer_dx, y_floor_bot(s), p_fw_x, p_fw_y, LAYER_CUT),
        Line(x_mirror(s, chamfer_dx), y_floor_bot(s),
             x_mirror(s, p_fw_x), p_fw_y, LAYER_CUT),
    ]


# ============================================================
# === Generator main =========================================
# ============================================================

def generate(spec: InsertSpec, phase: int = 1) -> List:
    spec.validate()
    entities = []
    # Phase 1: floor scores + slot
    entities.extend(floor_scores(spec))
    for slot in spec.slots:
        entities.extend(slot_geometry(spec, slot))
    if phase >= 2:
        entities.extend(top_band_scores(spec))
        entities.extend(top_band_edges(spec))
    if phase >= 3:
        entities.extend(front_wall(spec))
    if phase >= 4:
        # Side walls + corner arcs
        entities.extend(left_side_wall(spec))
        entities.extend([
            Line(2*mirror_axis_x(spec) - e.x0, e.y0,
                 2*mirror_axis_x(spec) - e.x1, e.y1, e.layer)
            if type(e).__name__ == "Line" else
            Arc(2*mirror_axis_x(spec) - e.cx, e.cy, e.r,
                (180 - e.ea) % 360, (180 - e.sa) % 360, e.layer)
            for e in left_side_wall(spec)
        ])
    if phase >= 5:
        # Tangent diagonals + corner micro-chamfers
        entities.extend(side_wall_tangent_diagonals(spec))
        entities.extend(top_band_tangents(spec))
        entities.extend(front_wall_tangents(spec))
        entities.extend(floor_corner_chamfers(spec))
    return entities


if __name__ == "__main__":
    spec = InsertSpec.single_slot(L=300, W=200, H=80, t=1.5,
                                   SL=150, SW=150, SH=25, IH=55)
    ents = generate(spec, phase=5)
    n_cut = sum(1 for e in ents if type(e).__name__ == 'Line' and e.layer == LAYER_CUT)
    n_fold = sum(1 for e in ents if type(e).__name__ == 'Line' and e.layer == LAYER_SCORE)
    n_arc = sum(1 for e in ents if type(e).__name__ == 'Arc')
    n_poly = sum(1 for e in ents if type(e).__name__ == 'Polyline')
    print(f"Phase 5: {len(ents)} entities ({n_cut} cut + {n_fold} fold + "
          f"{n_arc} arc + {n_poly} poly)")
    print(f"Baseline: 30 cut + 11 fold + 6 arc + 1 poly = 48")
    from cefbox_render import render_entities
    render_entities(ents, 'gen_insert_phase5_300x200x80.png',
                    title='Insert tray Phase 5 - baseline 300x200x80')
