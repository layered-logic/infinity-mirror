"""Flat-pattern sketches for the Layered Logic infinity-mirror packaging.

Two templates, both hand-derived (Path B from the May 12 chat) — no CEFbox
dependency. Defaults match the BOM target: 8" x 8" x 3" outer (203 x 203 x
76 mm), E-flute corrugated (1.6 mm). Sketch is the sign-off step before any
SVG generator code lands.

  1. REFT mailer        — Roll End Front Tuck, single-piece self-locking.
                          What CEFbox calls "Ear-Lock Mailer."
  2. Tray-with-backer   — Three-panel U-fold insert. Back panel is the
     insert                raster surface for the right-to-repair QR code.

Color convention (matches LightBurn / Inkscape laser-cut conventions):
  red    = through-cut
  blue   = score / crease (laser scores one liner; die-cut tooling creases)
  grey   = raster region (engrave / etch) — insert only

Run: python sketch_dielines.py
Output: dielines_preview.png

Dimensions are mm throughout. Inner-vs-outer convention: dieline panels are
drawn at OUTER box dimensions; bend allowance for E-flute is ~0 (corrugation
crushes at the fold), so score lines sit at exact panel boundaries.
"""

import os
import matplotlib.pyplot as plt
import matplotlib.patches as patches

# ============================================================
# === Tunable parameters =====================================
# ============================================================

# --- Outer box dimensions (mm). Default = 8" x 8" x 3" per BOM. ---
L = 203.0   # length (longer dim, runs along the spine of the dieline)
W = 203.0   # width
H = 76.0    # depth / height

# --- Material ---
T_MAT = 1.6   # E-flute thickness. Score lines sit at panel boundaries
              # (bend allowance treated as 0 for corrugated; revisit if a
              # post-fold fit-test shows the box closing short).

# --- REFT mailer ---
T_FRONT_TUCK = H - 2 * T_MAT          # front tuck depth: reach the floor cleanly
EAR_WIDTH = 12.0                       # ear-lock tab width, mm
EAR_DEPTH = 8.0                        # ear-lock tab depth, mm
EAR_INSET = 6.0                        # how far from the L-end the ear starts

# --- Tray-with-backer insert ---
INSERT_BACKER_H = 0.75 * H             # backer height (tall — full QR surface)
INSERT_FRONT_LIP_H = 20.0              # front retaining lip
INSERT_L = L - 2 * T_MAT               # fits inside outer box walls
INSERT_W = W - 2 * T_MAT
QR_SIZE = 50.0                         # QR raster square edge, mm
QR_INSET_FROM_FOLD = 18.0              # below the upper edge of the backer

# --- Drawing styling ---
COLOR_CUT = '#d62728'    # red
COLOR_SCORE = '#1f4ec7'  # blue
COLOR_RASTER = '#b0b0b0' # grey
COLOR_LABEL = '#222'
LW_CUT = 1.6
LW_SCORE = 1.0
SCORE_DASH = (0, (6, 3))


def _rect_cut(ax, x, y, w, h):
    """Draw a rectangle outline in CUT color (red)."""
    ax.add_patch(patches.Rectangle((x, y), w, h, fill=False,
                                   edgecolor=COLOR_CUT, linewidth=LW_CUT))


def _line(ax, x0, y0, x1, y1, color, lw, dash=None):
    ax.plot([x0, x1], [y0, y1], color=color, linewidth=lw,
            linestyle=dash if dash else '-')


def _score(ax, x0, y0, x1, y1):
    _line(ax, x0, y0, x1, y1, COLOR_SCORE, LW_SCORE, SCORE_DASH)


def _label(ax, x, y, text, size=8, color=COLOR_LABEL, weight='normal'):
    ax.text(x, y, text, ha='center', va='center', fontsize=size,
            color=color, fontweight=weight)


# ============================================================
# === REFT mailer flat pattern ===============================
# ============================================================
#
# Spine layout (bottom of figure to top): TUCK -> FRONT -> BOTTOM -> BACK -> LID
# Roll-end side panels flank the BOTTOM panel on the left and right.
#
# Bounding box:
#   width  = L + 2H       (bottom L + two roll-ends each H wide)
#   height = T_FRONT_TUCK + H + W + H + W   (tuck + front + bottom + back + lid)

def draw_reft_mailer(ax, origin=(0, 0)):
    ox, oy = origin

    bbox_w = L + 2 * H
    bbox_h = T_FRONT_TUCK + 2 * H + 2 * W

    # The bottom panel spans the FULL inner column; roll-ends sit to its left/right.
    # We position the spine column starting H to the right of origin.
    col_x = ox + H

    # Spine y-positions (cumulative, bottom upward)
    y_tuck_bot = oy
    y_tuck_top = y_tuck_bot + T_FRONT_TUCK   # = y_front_bot
    y_front_top = y_tuck_top + H              # = y_bottom_bot
    y_bottom_top = y_front_top + W            # = y_back_bot
    y_back_top = y_bottom_top + H             # = y_lid_bot
    y_lid_top = y_back_top + W                # top of bbox

    # ---- CUT lines: outer outline of the whole pattern ----

    # Tuck flap (bottom-most), with ear cutouts on left and right ends.
    # Ear = a small rectangular notch cut INTO the tuck near each L-end,
    # producing a flex tab that catches into a matching slit in the front
    # panel when the tuck is folded down inside the box.
    tx0 = col_x
    tx1 = col_x + L
    # Tuck outline: counterclockwise from bottom-left
    ax.plot([tx0, tx1, tx1, tx0, tx0],
            [y_tuck_bot, y_tuck_bot, y_tuck_top, y_tuck_top, y_tuck_bot],
            color=COLOR_CUT, linewidth=LW_CUT)

    # Ear slits cut INTO the tuck near each end. Two parallel slits per ear
    # define a flex tab between them; the tab pops out when assembled.
    for end_x, sign in [(tx0, +1), (tx1, -1)]:
        # Slit 1 (closer to fold edge)
        sx0 = end_x + sign * EAR_INSET
        sx1 = sx0 + sign * EAR_WIDTH
        sy = y_tuck_bot + (T_FRONT_TUCK - EAR_DEPTH)
        _line(ax, sx0, sy, sx1, sy, COLOR_CUT, LW_CUT)
        # Slits perpendicular to the long edge, defining the tab sides
        _line(ax, sx0, sy, sx0, y_tuck_top, COLOR_CUT, LW_CUT)
        _line(ax, sx1, sy, sx1, y_tuck_top, COLOR_CUT, LW_CUT)

    # Front panel (between tuck and bottom)
    _rect_cut(ax, col_x, y_tuck_top, L, H)

    # Bottom panel + roll-end side panels: drawn as one combined cut outline
    # spanning L + 2H wide. The score lines between bottom and side panels are
    # drawn separately (blue dashed) — they're folds, not cuts.
    _rect_cut(ax, ox, y_front_top, bbox_w, W)

    # Back panel
    _rect_cut(ax, col_x, y_bottom_top, L, H)

    # Lid panel
    _rect_cut(ax, col_x, y_back_top, L, W)

    # ---- SCORE lines (blue dashed) — fold creases ----

    # Spine horizontal scores (fold up/down)
    _score(ax, col_x, y_tuck_top, col_x + L, y_tuck_top)   # tuck <-> front
    _score(ax, col_x, y_front_top, col_x + L, y_front_top) # front <-> bottom
    _score(ax, col_x, y_bottom_top, col_x + L, y_bottom_top) # bottom <-> back
    _score(ax, col_x, y_back_top, col_x + L, y_back_top)   # back <-> lid

    # Two vertical scores between bottom and roll-ends (sides fold up)
    _score(ax, col_x, y_front_top, col_x, y_bottom_top)
    _score(ax, col_x + L, y_front_top, col_x + L, y_bottom_top)

    # ---- LABELS ----
    _label(ax, col_x + L / 2, y_tuck_bot + T_FRONT_TUCK / 2,
           f'FRONT TUCK\n{L:.0f} x {T_FRONT_TUCK:.1f}', size=8)
    _label(ax, col_x + L / 2, y_tuck_top + H / 2,
           f'FRONT\n{L:.0f} x {H:.0f}', size=8)
    _label(ax, col_x + L / 2, y_front_top + W / 2,
           f'BOTTOM\n{L:.0f} x {W:.0f}', size=9, weight='bold')
    _label(ax, col_x + L / 2, y_bottom_top + H / 2,
           f'BACK\n{L:.0f} x {H:.0f}', size=8)
    _label(ax, col_x + L / 2, y_back_top + W / 2,
           f'LID\n{L:.0f} x {W:.0f}', size=8)
    _label(ax, ox + H / 2, y_front_top + W / 2,
           f'LEFT\nROLL-END\n{H:.0f} x {W:.0f}', size=7)
    _label(ax, ox + bbox_w - H / 2, y_front_top + W / 2,
           f'RIGHT\nROLL-END\n{H:.0f} x {W:.0f}', size=7)
    _label(ax, col_x + L / 2, y_lid_top + 10,
           f'REFT mailer — bbox {bbox_w:.0f} x {bbox_h:.0f} mm',
           size=10, weight='bold')


# ============================================================
# === Tray-with-backer insert ================================
# ============================================================
#
# Three-panel flat pattern: BACKER + FLOOR + FRONT LIP, stacked vertically.
# Two score lines, no internal cuts (other than the QR raster region).
#
# QR lives on the BACKER panel — the inside face seen when the box is opened.
# Raster is added by the laser cutter in the same job as the cuts/scores.
#
# Bounding box:
#   width  = INSERT_L
#   height = INSERT_FRONT_LIP_H + INSERT_W + INSERT_BACKER_H

def draw_insert(ax, origin=(0, 0)):
    ox, oy = origin

    bbox_w = INSERT_L
    bbox_h = INSERT_FRONT_LIP_H + INSERT_W + INSERT_BACKER_H

    y_lip_bot = oy
    y_lip_top = y_lip_bot + INSERT_FRONT_LIP_H   # = y_floor_bot
    y_floor_top = y_lip_top + INSERT_W            # = y_backer_bot
    y_backer_top = y_floor_top + INSERT_BACKER_H

    # ---- CUT line: outer outline only ----
    _rect_cut(ax, ox, y_lip_bot, bbox_w, bbox_h)

    # ---- SCORE lines ----
    _score(ax, ox, y_lip_top, ox + bbox_w, y_lip_top)       # lip <-> floor
    _score(ax, ox, y_floor_top, ox + bbox_w, y_floor_top)   # floor <-> backer

    # ---- RASTER region: QR on the backer panel ----
    qr_x = ox + (bbox_w - QR_SIZE) / 2
    qr_y = y_backer_top - QR_INSET_FROM_FOLD - QR_SIZE
    ax.add_patch(patches.Rectangle((qr_x, qr_y), QR_SIZE, QR_SIZE,
                                   facecolor=COLOR_RASTER, edgecolor='none',
                                   alpha=0.55))
    _label(ax, qr_x + QR_SIZE / 2, qr_y + QR_SIZE / 2,
           f'QR raster\n{QR_SIZE:.0f}x{QR_SIZE:.0f}', size=7)

    # ---- LABELS ----
    _label(ax, ox + bbox_w / 2, y_lip_bot + INSERT_FRONT_LIP_H / 2,
           f'FRONT LIP\n{bbox_w:.0f} x {INSERT_FRONT_LIP_H:.0f}', size=8)
    _label(ax, ox + bbox_w / 2, y_lip_top + INSERT_W / 2,
           f'FLOOR\n{bbox_w:.0f} x {INSERT_W:.0f}', size=9, weight='bold')
    _label(ax, ox + bbox_w / 2, y_floor_top + INSERT_BACKER_H / 2 - 40,
           f'BACKER\n{bbox_w:.0f} x {INSERT_BACKER_H:.0f}', size=8)
    _label(ax, ox + bbox_w / 2, y_backer_top + 10,
           f'Insert tray with backer — bbox {bbox_w:.0f} x {bbox_h:.0f} mm',
           size=10, weight='bold')


# ============================================================
# === Render =================================================
# ============================================================

def render(out_path='dielines_preview.png'):
    fig, axes = plt.subplots(1, 2, figsize=(16, 11))
    fig.suptitle(
        f'Layered Logic packaging dielines — sketch v1 (Path B, hand-derived)\n'
        f'Outer box: {L:.0f} x {W:.0f} x {H:.0f} mm   '
        f'Material: E-flute {T_MAT} mm',
        fontsize=12)

    for ax in axes:
        ax.set_aspect('equal')
        ax.grid(True, linestyle=':', alpha=0.3)
        ax.set_xlabel('mm')
        ax.set_ylabel('mm')

    draw_reft_mailer(axes[0])
    axes[0].autoscale_view()
    axes[0].margins(0.05)

    draw_insert(axes[1])
    axes[1].autoscale_view()
    axes[1].margins(0.05)

    # Legend on the right subplot
    legend_elems = [
        patches.Patch(facecolor='none', edgecolor=COLOR_CUT, label='cut (through)'),
        patches.Patch(facecolor='none', edgecolor=COLOR_SCORE,
                      linestyle='--', label='score (crease)'),
        patches.Patch(facecolor=COLOR_RASTER, edgecolor='none',
                      alpha=0.55, label='raster (etch)'),
    ]
    axes[1].legend(handles=legend_elems, loc='upper right', fontsize=8)

    plt.tight_layout(rect=(0, 0, 1, 0.96))
    out_full = os.path.join(os.path.dirname(__file__) or '.', out_path)
    plt.savefig(out_full, dpi=150, bbox_inches='tight')
    print(f'Wrote {out_full}')


if __name__ == '__main__':
    render()
