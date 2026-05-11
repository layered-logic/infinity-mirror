"""Layout sketch for the Basic Infinity Mirror PCB housing.

Three views (top-down + right-side elevation + end cross-section through USB-C).
This is the matplotlib sign-off step from HOUSING_DESIGN_WORKFLOW.md, before
any Fusion work happens.

World axes:
    +X  : front-to-back of mirror (long PCB axis runs along this)
    +Y  : right (USB face)
    +Z  : up, into the mirror cavity

PCB-local convention (from the user):
    USB-C is on the y=0 edge of the PCB; (0,0) is the bottom-left when looking
    at the PCB with the USB port at the bottom. Long axis = 25.83 mm (x), short
    axis = 21.7 mm (y). The y=0 PCB edge maps to the world +Y face of the
    housing (so PCB y axis points world -Y).

Run:
    python sketch_housing_layout.py
"""

import os
import matplotlib.pyplot as plt
import matplotlib.patches as patches

# ============================================================
# === Tunable parameters =====================================
# ============================================================

# --- PCB ---
PCB_X_MM = 25.83
PCB_Y_MM = 21.70
PCB_T_MM = 1.60

# Mounting holes (PCB-local, USB at bottom edge, BL = origin)
HOLE_1_PCB = (22.15, 17.50)
HOLE_2_PCB = (3.25,  9.50)
HOLE_DIA_MM = 3.15         # confirmed by Bill

# Components (PCB-local center positions, approximate from STEP)
SW1_PCB = (3.0,  2.0)      # 2-pin header for the wired button
USB_PCB = (13.0, 2.0)      # USB-C receptacle
JST_PCB = (22.0, 2.0)      # JST_XH 3-pin vertical (LED wire)
U1_PCB  = (13.0, 17.0)     # STM8 TSSOP-20
J6_PCB  = (22.0, 17.0)     # expansion header (no cutout needed)

# Component bounding boxes (X, Y, Z above PCB) centered on positions above
# Z is the height ABOVE the PCB top surface
USB_BOX = (9.0, 7.5, 3.3)
JST_BOX = (8.0, 6.0, 6.7)
U1_BOX  = (6.5, 4.4, 1.1)
SW1_BOX = (3.0, 2.5, 8.0)  # header pins + mating crimp body
J6_BOX  = (3.0, 2.5, 4.0)

# --- Housing tolerances / wall thicknesses ---
PCB_EDGE_GAP   = 0.5       # XY gap between PCB outline and housing inner wall
STANDOFF_MM    = 1.5       # PCB bottom face to housing floor (solder clear)
TOP_CLEAR_MM   = 0.2       # above the tallest fixed component (tightened)
WALL_T_MM      = 1.5
FLOOR_T_MM     = 1.5
LID_T_MM       = 1.5

# --- Button (5x5x6 mm body, ~4 mm cap) ---
# Cap on world +Y face (same face as USB-C). With cap pointing +Y, the body's
# 6 mm "cap-to-leads" axis lies along Y; the 5x5 cross-section is X and Z.
# Body sits in the front extension (X<0), flush against the PCB front edge.
BUTTON_BODY_X_MM  = 5.0    # extent along world X
BUTTON_BODY_Y_MM  = 6.0    # extent along world Y (cap-to-leads)
BUTTON_BODY_Z_MM  = 5.0    # extent along world Z
BUTTON_CAP_DIA_MM = 4.0
BUTTON_GAP_FROM_PCB_X = 0.0  # flush against PCB front edge (was 1.0)

# --- JST plug pull-out hole in lid (so the plug protrudes through) ---
JST_PLUG_HOLE_DX = 9.0
JST_PLUG_HOLE_DY = 7.0

# --- Snap peg through PCB mounting holes (3.15 mm) ---
PEG_DIA_MM       = 2.95    # peg shaft — slip fit in 3.15 hole
PEG_SNAP_DIA_MM  = 3.80    # snap head expanded, sits on PCB top
PEG_TOTAL_Z_MM   = 5.0     # standoff (1.5) + PCB (1.6) + snap head (1.5) + margin

# ============================================================
# === Coord transforms =======================================
# ============================================================

def pcb_to_world(pcb_x, pcb_y):
    """PCB-local (x,y) -> world (X,Y). USB edge (y=0) faces world +Y."""
    return (pcb_x, PCB_Y_MM - pcb_y)

# ============================================================
# === Derived dimensions =====================================
# ============================================================

# Housing INTERIOR extents (world coords)
INT_X_MIN = -(BUTTON_GAP_FROM_PCB_X + BUTTON_BODY_X_MM + PCB_EDGE_GAP)
INT_X_MAX = PCB_X_MM + PCB_EDGE_GAP
INT_Y_MIN = -PCB_EDGE_GAP
INT_Y_MAX = PCB_Y_MM + PCB_EDGE_GAP
INT_Z_MIN = 0.0
INT_Z_MAX = STANDOFF_MM + PCB_T_MM + JST_BOX[2] + TOP_CLEAR_MM

PCB_BOT_Z = STANDOFF_MM
PCB_TOP_Z = STANDOFF_MM + PCB_T_MM

# Housing EXTERIOR
EXT_X_MIN = INT_X_MIN - WALL_T_MM
EXT_X_MAX = INT_X_MAX + WALL_T_MM
EXT_Y_MIN = INT_Y_MIN - WALL_T_MM
EXT_Y_MAX = INT_Y_MAX + WALL_T_MM
EXT_Z_MIN = INT_Z_MIN - FLOOR_T_MM
EXT_Z_MAX = INT_Z_MAX + LID_T_MM

# Button placement in world coords (cap on +Y wall, body inside)
BUTTON_X_MIN = INT_X_MIN
BUTTON_X_MAX = INT_X_MIN + BUTTON_BODY_X_MM
BUTTON_Y_MAX = INT_Y_MAX
BUTTON_Y_MIN = INT_Y_MAX - BUTTON_BODY_Y_MM
BUTTON_CAP_CX = (BUTTON_X_MIN + BUTTON_X_MAX) / 2

# ============================================================
# === Drawing helpers ========================================
# ============================================================

def _rect(ax, x0, y0, w, h, **kw):
    ax.add_patch(patches.Rectangle((x0, y0), w, h, **kw))

def _circ(ax, x0, y0, r, **kw):
    ax.add_patch(patches.Circle((x0, y0), r, **kw))

def _box_about(ax, cx, cy, w, h, **kw):
    _rect(ax, cx - w/2, cy - h/2, w, h, **kw)

# ============================================================
# === Views ==================================================
# ============================================================

def draw_top_down(ax):
    """Plan view, looking down -Z (toward floor of housing)."""
    # Housing exterior
    _rect(ax, EXT_X_MIN, EXT_Y_MIN, EXT_X_MAX - EXT_X_MIN, EXT_Y_MAX - EXT_Y_MIN,
          linewidth=2.0, edgecolor='black', facecolor='#f4f4f4')
    # Housing interior
    _rect(ax, INT_X_MIN, INT_Y_MIN, INT_X_MAX - INT_X_MIN, INT_Y_MAX - INT_Y_MIN,
          linewidth=0.8, edgecolor='gray', facecolor='white', linestyle='--')

    # PCB outline
    _rect(ax, 0, 0, PCB_X_MM, PCB_Y_MM,
          linewidth=1.0, edgecolor='green', facecolor='#e2f2e2', alpha=0.8)

    # Mounting holes + snap-peg footprint (red dotted = snap head OD)
    for (hx, hy) in (HOLE_1_PCB, HOLE_2_PCB):
        wx, wy = pcb_to_world(hx, hy)
        _circ(ax, wx, wy, HOLE_DIA_MM / 2,
              linewidth=0.8, edgecolor='black', facecolor='white')
        _circ(ax, wx, wy, PEG_SNAP_DIA_MM / 2,
              linewidth=0.6, edgecolor='red', facecolor='none', linestyle=':')

    # Top-side components
    comps = [
        (USB_PCB, USB_BOX, 'USB-C', '#b3d4ff'),
        (JST_PCB, JST_BOX, 'JST',   '#ffd49a'),
        (SW1_PCB, SW1_BOX, 'SW1',   '#dddddd'),
        (U1_PCB,  U1_BOX,  'U1',    '#cfcfcf'),
        (J6_PCB,  J6_BOX,  'J6',    '#eeeeee'),
    ]
    for (cx, cy), (bx, by, _), label, color in comps:
        wx, wy = pcb_to_world(cx, cy)
        _box_about(ax, wx, wy, bx, by,
                   linewidth=0.6, edgecolor='black', facecolor=color, alpha=0.85)
        ax.text(wx, wy, label, ha='center', va='center', fontsize=7)

    # Button body (top-down: looks like a 5x5 mm pad in the corner)
    _rect(ax, BUTTON_X_MIN, BUTTON_Y_MIN,
          BUTTON_BODY_X_MM, BUTTON_BODY_Y_MM,
          linewidth=0.6, edgecolor='black', facecolor='#444444', alpha=0.7)
    ax.text(BUTTON_X_MIN + BUTTON_BODY_X_MM/2,
            BUTTON_Y_MIN + BUTTON_BODY_Y_MM/2,
            'BTN', ha='center', va='center', fontsize=6, color='white')
    # Cap circle on +Y wall
    _circ(ax, BUTTON_CAP_CX, EXT_Y_MAX - WALL_T_MM/2,
          BUTTON_CAP_DIA_MM/2,
          linewidth=0.7, edgecolor='red', facecolor='none')

    # USB-C cutout in +Y wall
    usb_wx, _ = pcb_to_world(USB_PCB[0], USB_PCB[1])
    _rect(ax, usb_wx - USB_BOX[0]/2, INT_Y_MAX,
          USB_BOX[0], EXT_Y_MAX - INT_Y_MAX,
          linewidth=0.7, edgecolor='red', facecolor='none')
    ax.annotate('USB-C cutout', xy=(usb_wx, EXT_Y_MAX),
                xytext=(usb_wx, EXT_Y_MAX + 2.2),
                ha='center', fontsize=6, color='red',
                arrowprops=dict(arrowstyle='-', color='red', linewidth=0.4))

    # JST plug pull-out hole (in lid, dashed since it's above)
    jst_wx, jst_wy = pcb_to_world(JST_PCB[0], JST_PCB[1])
    _rect(ax, jst_wx - JST_PLUG_HOLE_DX/2, jst_wy - JST_PLUG_HOLE_DY/2,
          JST_PLUG_HOLE_DX, JST_PLUG_HOLE_DY,
          linewidth=0.7, edgecolor='blue', facecolor='none', linestyle='--')
    ax.text(jst_wx, jst_wy - JST_PLUG_HOLE_DY/2 - 1.2,
            'lid hole (JST plug)', ha='center', va='top',
            fontsize=5.5, color='blue')

    # Direction labels
    ax.annotate('FRONT of mirror', xy=(EXT_X_MIN - 1, (EXT_Y_MIN + EXT_Y_MAX)/2),
                ha='right', va='center', fontsize=8, color='#444', rotation=90)
    ax.annotate('BACK of mirror', xy=(EXT_X_MAX + 1, (EXT_Y_MIN + EXT_Y_MAX)/2),
                ha='left', va='center', fontsize=8, color='#444', rotation=90)
    ax.annotate('USB face →', xy=((EXT_X_MIN + EXT_X_MAX)/2, EXT_Y_MAX + 4),
                ha='center', va='center', fontsize=8, color='#444')

    # Axes
    ax.set_aspect('equal')
    ax.set_xlim(EXT_X_MIN - 6, EXT_X_MAX + 6)
    ax.set_ylim(EXT_Y_MIN - 4, EXT_Y_MAX + 6)
    ax.set_xlabel('world X (mm)')
    ax.set_ylabel('world Y (mm)')
    ax.set_title(f'Top-down (looking −Z) — '
                 f'housing {EXT_X_MAX-EXT_X_MIN:.1f} × '
                 f'{EXT_Y_MAX-EXT_Y_MIN:.1f} mm')
    ax.grid(True, linewidth=0.3, alpha=0.4)


def draw_right_elevation(ax):
    """View from +Y looking -Y. Shows the I/O face (USB cutout + button cap)."""
    # Housing exterior silhouette (XZ)
    _rect(ax, EXT_X_MIN, EXT_Z_MIN, EXT_X_MAX - EXT_X_MIN, EXT_Z_MAX - EXT_Z_MIN,
          linewidth=2.0, edgecolor='black', facecolor='#f4f4f4')

    # Lid + floor seams
    ax.axhline(INT_Z_MAX, color='black', linewidth=0.6, linestyle='--')
    ax.text(EXT_X_MAX + 0.5, INT_Z_MAX, 'lid seam',
            va='center', ha='left', fontsize=6, color='gray')
    ax.axhline(INT_Z_MIN, color='black', linewidth=0.6, linestyle='--')
    ax.text(EXT_X_MAX + 0.5, INT_Z_MIN, 'floor',
            va='center', ha='left', fontsize=6, color='gray')

    # PCB shown as hidden line (dashed, behind +Y wall)
    ax.plot([0, PCB_X_MM], [PCB_TOP_Z, PCB_TOP_Z],
            color='green', linewidth=0.7, linestyle=':')
    ax.plot([0, PCB_X_MM], [PCB_BOT_Z, PCB_BOT_Z],
            color='green', linewidth=0.7, linestyle=':')
    ax.text(PCB_X_MM/2, PCB_TOP_Z - 0.3, 'PCB (hidden)',
            ha='center', va='top', fontsize=5.5, color='green')

    # USB-C cutout in face
    usb_x = USB_PCB[0]
    _rect(ax, usb_x - USB_BOX[0]/2, PCB_TOP_Z,
          USB_BOX[0], USB_BOX[2],
          linewidth=0.8, edgecolor='red', facecolor='white')
    ax.text(usb_x, PCB_TOP_Z + USB_BOX[2] + 0.4, 'USB-C cutout',
            ha='center', va='bottom', fontsize=6, color='red')

    # Button cap window
    _circ(ax, BUTTON_CAP_CX, PCB_BOT_Z + BUTTON_BODY_Z_MM/2,
          BUTTON_CAP_DIA_MM/2,
          linewidth=0.8, edgecolor='red', facecolor='white')
    ax.text(BUTTON_CAP_CX, PCB_BOT_Z + BUTTON_BODY_Z_MM + 0.6, 'button cap',
            ha='center', va='bottom', fontsize=6, color='red')

    # JST plug hole in lid (visible against the +Y wall as a notch in top)
    jst_x = JST_PCB[0]
    _rect(ax, jst_x - JST_PLUG_HOLE_DX/2, INT_Z_MAX,
          JST_PLUG_HOLE_DX, EXT_Z_MAX - INT_Z_MAX,
          linewidth=0.7, edgecolor='blue', facecolor='none', linestyle='--')
    ax.text(jst_x, EXT_Z_MAX + 0.6, 'lid hole (JST)',
            ha='center', va='bottom', fontsize=5.5, color='blue')

    ax.set_aspect('equal')
    ax.set_xlim(EXT_X_MIN - 4, EXT_X_MAX + 8)
    ax.set_ylim(EXT_Z_MIN - 2, EXT_Z_MAX + 4)
    ax.set_xlabel('world X (mm)')
    ax.set_ylabel('world Z (mm)')
    ax.set_title('Right-side elevation (looking −Y, at the I/O face)')
    ax.grid(True, linewidth=0.3, alpha=0.4)


def draw_end_section(ax):
    """End cross-section in YZ plane, slicing at X = USB center.

    Shows the Z stack-up: floor / standoff / PCB / connector / lid.
    """
    # Housing exterior YZ silhouette
    _rect(ax, EXT_Y_MIN, EXT_Z_MIN, EXT_Y_MAX - EXT_Y_MIN, EXT_Z_MAX - EXT_Z_MIN,
          linewidth=2.0, edgecolor='black', facecolor='#f4f4f4')
    # Interior
    _rect(ax, INT_Y_MIN, INT_Z_MIN, INT_Y_MAX - INT_Y_MIN, INT_Z_MAX - INT_Z_MIN,
          linewidth=0.8, edgecolor='gray', facecolor='white', linestyle='--')

    # PCB cross-section (full width in Y at the slice X)
    _rect(ax, 0, PCB_BOT_Z, PCB_Y_MM, PCB_T_MM,
          linewidth=1.0, edgecolor='green', facecolor='#e2f2e2')
    ax.text(PCB_Y_MM/2, PCB_BOT_Z - 0.6, 'PCB',
            ha='center', va='top', fontsize=7, color='green')

    # USB-C body in YZ (it sits on PCB, body extends out +Y face)
    usb_y_center = pcb_to_world(USB_PCB[0], USB_PCB[1])[1]  # world Y of USB
    _rect(ax, usb_y_center - USB_BOX[1]/2, PCB_TOP_Z,
          USB_BOX[1], USB_BOX[2],
          linewidth=0.6, edgecolor='black', facecolor='#b3d4ff')
    # USB receptacle face protrudes through +Y wall
    ax.annotate('USB-C\n(exits +Y)',
                xy=(usb_y_center + USB_BOX[1]/2, PCB_TOP_Z + USB_BOX[2]/2),
                xytext=(EXT_Y_MAX + 3, PCB_TOP_Z + USB_BOX[2]/2),
                fontsize=6, ha='left', va='center',
                arrowprops=dict(arrowstyle='->', color='black', linewidth=0.4))

    # Standoff label
    ax.annotate('', xy=(0, INT_Z_MIN), xytext=(0, PCB_BOT_Z),
                arrowprops=dict(arrowstyle='<->', color='gray', linewidth=0.4))
    ax.text(-0.4, STANDOFF_MM/2, f'{STANDOFF_MM} mm\nstandoff',
            ha='right', va='center', fontsize=5.5, color='gray')

    # Top clearance label
    ax.annotate('', xy=(PCB_Y_MM, PCB_TOP_Z + JST_BOX[2]),
                xytext=(PCB_Y_MM, INT_Z_MAX),
                arrowprops=dict(arrowstyle='<->', color='gray', linewidth=0.4))
    ax.text(PCB_Y_MM + 0.4, PCB_TOP_Z + JST_BOX[2] + TOP_CLEAR_MM/2,
            f'{TOP_CLEAR_MM} mm\ntop clear', ha='left', va='center',
            fontsize=5.5, color='gray')

    # Wall labels
    ax.text((EXT_Y_MIN + INT_Y_MIN)/2, (EXT_Z_MIN + EXT_Z_MAX)/2,
            f'{WALL_T_MM}\nmm', ha='center', va='center', fontsize=5.5, color='#666')
    ax.text((EXT_Y_MAX + INT_Y_MAX)/2, (EXT_Z_MIN + EXT_Z_MAX)/2,
            f'{WALL_T_MM}\nmm', ha='center', va='center', fontsize=5.5, color='#666')

    ax.set_aspect('equal')
    ax.set_xlim(EXT_Y_MIN - 5, EXT_Y_MAX + 12)
    ax.set_ylim(EXT_Z_MIN - 3, EXT_Z_MAX + 3)
    ax.set_xlabel('world Y (mm)')
    ax.set_ylabel('world Z (mm)')
    ax.set_title(f'End section at X ≈ {USB_PCB[0]} mm (through USB-C) — '
                 f'total Z = {EXT_Z_MAX - EXT_Z_MIN:.1f} mm')
    ax.grid(True, linewidth=0.3, alpha=0.4)


def main():
    fig = plt.figure(figsize=(11, 14))
    ax1 = fig.add_subplot(3, 1, 1)
    ax2 = fig.add_subplot(3, 1, 2)
    ax3 = fig.add_subplot(3, 1, 3)
    draw_top_down(ax1)
    draw_right_elevation(ax2)
    draw_end_section(ax3)

    fig.suptitle(
        'Basic Infinity Mirror Housing — Layout v1\n'
        f'External: {EXT_X_MAX-EXT_X_MIN:.1f} (X) × '
        f'{EXT_Y_MAX-EXT_Y_MIN:.1f} (Y) × '
        f'{EXT_Z_MAX-EXT_Z_MIN:.1f} (Z) mm     |     '
        f'PCB: {PCB_X_MM} × {PCB_Y_MM} × {PCB_T_MM} mm',
        fontsize=12, fontweight='bold')
    fig.tight_layout(rect=[0, 0, 1, 0.97])

    out = os.path.join(os.path.dirname(os.path.abspath(__file__)),
                       'housing_layout_v1.png')
    fig.savefig(out, dpi=150, bbox_inches='tight')
    print(f'wrote {out}')
    plt.show()


if __name__ == '__main__':
    main()
