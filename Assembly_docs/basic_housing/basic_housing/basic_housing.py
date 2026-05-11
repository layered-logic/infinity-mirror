"""Basic Infinity Mirror PCB housing — Fusion 360 script.

Builds a tight 3D-printable enclosure around a 25.83 x 21.7 mm STM8 board:
- Reference envelope bodies for the PCB and every on-board component (kept
  visible after cuts so they show where things land).
- Tray body: floor + 4 walls + 2 compression snap pegs through PCB mounting
  holes + button pocket + USB-C cutout + lid-tab slots in the two short walls.
- Lid body: top plate + JST plug pull-through hole + 2 snap tabs that engage
  the slots in the tray.

World coords:
    +X = front-to-back of mirror (long PCB axis runs along this)
    +Y = right (USB face)
    +Z = up, into the mirror cavity
Z=0 is the interior FLOOR surface; PCB sits at Z = 1.5 to 3.1.

See `sketch_housing_layout.py` in this folder for the matplotlib layout
sign-off this came from.

BUILD_MODE:
- 'all'       : build envelopes + cut tools + tray + lid (default).
- 'envelopes' : just the reference bodies (PCB, components, button).
- 'tray'      : tray + the cut tools it needs (handy for tray-only re-runs).
- 'lid'       : lid + the cut tools it needs.
- any single key in COMPONENTS : just that one body.

Run in a FRESH Fusion design — re-running in a design that already has
script-generated bodies will collide.
"""

import adsk.core
import adsk.fusion
import math
import traceback

# ============================================================
# === Tunable parameters =====================================
# ============================================================

BUILD_MODE = 'all'

# --- PCB ---
PCB_X_MM = 25.83
PCB_Y_MM = 21.70
PCB_T_MM = 1.60

# Mounting holes (PCB-local, USB at bottom edge, BL = origin)
HOLE_1_PCB = (22.15, 17.50)
HOLE_2_PCB = (3.25,  9.50)
HOLE_DIA_MM = 3.15

# Components (PCB-local center positions, approximate from STEP)
SW1_PCB = (3.0,  2.0)      # 2-pin header for wired button
USB_PCB = (13.0, 2.0)      # USB-C receptacle
JST_PCB = (22.0, 2.0)      # JST_XH 3-pin vertical (LED wire)
U1_PCB  = (13.0, 17.0)     # STM8 TSSOP-20
J6_PCB  = (22.0, 17.0)     # expansion header (no cutout needed)

# Component bounding boxes (X, Y, Z above PCB), centered on positions above
USB_BOX = (9.0, 7.5, 3.3)
JST_BOX = (8.0, 6.0, 6.7)
U1_BOX  = (6.5, 4.4, 1.1)
SW1_BOX = (3.0, 2.5, 8.0)
J6_BOX  = (3.0, 2.5, 4.0)

# --- Housing geometry ---
PCB_EDGE_GAP = 0.5
STANDOFF_MM  = 1.5
TOP_CLEAR_MM = 0.2
WALL_T_MM    = 1.5
FLOOR_T_MM   = 1.5
LID_T_MM     = 1.5

# --- Button (5x6x5 in world: cap on +Y face) ---
BUTTON_BODY_X_MM = 5.0
BUTTON_BODY_Y_MM = 6.0
BUTTON_BODY_Z_MM = 5.0
BUTTON_CAP_DIA_MM = 4.0
BUTTON_GAP_FROM_PCB_X = 0.0

# Button print-clearance
POCKET_CLEAR_XY = 0.15
POCKET_CLEAR_Z  = 0.20
CAP_HOLE_CLEAR  = 0.20

# --- JST plug pull-out hole in lid ---
JST_PLUG_HOLE_DX = 9.0
JST_PLUG_HOLE_DY = 7.0

# --- Snap peg through 3.15 mm PCB mounting hole ---
PEG_SHAFT_DIA_MM = 2.95
PEG_HEAD_DIA_MM  = 3.80
PEG_HEAD_T_MM    = 1.20
PEG_SLIT_W_MM    = 0.50    # split column for compression

# --- Lid snap tabs (hang INSIDE the ±X walls, barbs poke OUTWARD into
#     through-slots cut in the walls). Inside cantilevers are stiffer because
#     they have to flex in the same direction as the wall they're trying to
#     pass, so we use a thinner barb and a longer tab vs the v0 outside design. ---
TAB_WIDTH_MM    = 5.0      # along Y
TAB_THICK_MM    = 1.2      # along X
TAB_LENGTH_MM   = 6.0      # along -Z, hanging from lid bottom
TAB_BARB_PROJ   = 0.5      # how far the barb projects outward into the slot
TAB_BARB_T_MM   = 1.2      # barb height along Z
TAB_WALL_GAP_MM = 0.1      # gap between tab outer face and inner wall surface
SLOT_WIDTH_MM   = TAB_WIDTH_MM + 0.4
SLOT_HEIGHT_MM  = TAB_BARB_T_MM + 0.4
SLOT_Z_FROM_TOP = 2.5      # slot center below interior top

# --- Button retainer block (provides cavity walls around the suspended
#     button body — without it, the body has nothing on top/bottom/sides
#     since it doesn't sit on the floor) ---
RETAINER_PAD_MM = 1.0      # block wall thickness around the button cavity

# ============================================================
# === Coord transforms / derived dims ========================
# ============================================================

def pcb_to_world(px, py):
    """PCB-local (x,y) -> world (X,Y). USB edge (y=0) faces world +Y."""
    return (px, PCB_Y_MM - py)

# Housing INTERIOR extents (Z=0 at interior floor)
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

# Button placement (world coords). Cap Z is anchored to the USB-C cutout
# center so the two features sit at the same height on the +Y face.
BTN_X_MIN  = INT_X_MIN
BTN_X_MAX  = INT_X_MIN + BUTTON_BODY_X_MM
BTN_Y_MAX  = INT_Y_MAX
BTN_Y_MIN  = INT_Y_MAX - BUTTON_BODY_Y_MM
BTN_CAP_CZ = PCB_TOP_Z + USB_BOX[2] / 2          # = USB cutout center Z (≈ 4.75)
BTN_Z_MIN  = BTN_CAP_CZ - BUTTON_BODY_Z_MM / 2
BTN_Z_MAX  = BTN_CAP_CZ + BUTTON_BODY_Z_MM / 2
BTN_CAP_CX = (BTN_X_MIN + BTN_X_MAX) / 2

# ============================================================
# === Helpers ================================================
# ============================================================

def cm(mm_val):
    return mm_val / 10.0

def pt3d(x_mm, y_mm, z_mm=0.0):
    return adsk.core.Point3D.create(cm(x_mm), cm(y_mm), cm(z_mm))

def val(x_mm):
    return adsk.core.ValueInput.createByReal(cm(x_mm))

def offset_xy_plane(root, z_mm):
    inp = root.constructionPlanes.createInput()
    inp.setByOffset(root.xYConstructionPlane, val(z_mm))
    return root.constructionPlanes.add(inp)

def offset_xz_plane(root, y_mm):
    inp = root.constructionPlanes.createInput()
    inp.setByOffset(root.xZConstructionPlane, val(y_mm))
    return root.constructionPlanes.add(inp)

def _pick_largest_profile(sketch):
    chosen, largest = None, -1.0
    for i in range(sketch.profiles.count):
        p = sketch.profiles.item(i)
        a = p.areaProperties().area
        if a > largest:
            largest, chosen = a, p
    return chosen

def _bool_op(root, target, tool, operation, keep_tool=False):
    coll = adsk.core.ObjectCollection.create()
    coll.add(tool)
    cf = root.features.combineFeatures
    inp = cf.createInput(target, coll)
    inp.operation = operation
    inp.isKeepToolBodies = keep_tool
    cf.add(inp)

def _extrude_sketch(root, sketch, depth_mm,
                    operation=adsk.fusion.FeatureOperations.NewBodyFeatureOperation):
    prof = _pick_largest_profile(sketch)
    if prof is None:
        raise RuntimeError(f'no profile on sketch {sketch.name}')
    ext = root.features.extrudeFeatures
    inp = ext.createInput(prof, operation)
    inp.setDistanceExtent(False, val(depth_mm))
    feat = ext.add(inp)
    if operation == adsk.fusion.FeatureOperations.NewBodyFeatureOperation:
        return feat.bodies.item(0)
    return None

def _box_xy(root, x0_mm, y0_mm, z0_mm, dx_mm, dy_mm, dz_mm, name):
    """Axis-aligned box, sketched on XY plane at z0, extruded +Z by dz."""
    plane = offset_xy_plane(root, z0_mm)
    sk = root.sketches.add(plane)
    sk.sketchCurves.sketchLines.addTwoPointRectangle(
        pt3d(x0_mm, y0_mm), pt3d(x0_mm + dx_mm, y0_mm + dy_mm))
    body = _extrude_sketch(root, sk, dz_mm)
    body.name = name
    return body

def _cyl_xy(root, cx_mm, cy_mm, z0_mm, dia_mm, dz_mm, name):
    """Vertical cylinder, axis along Z."""
    plane = offset_xy_plane(root, z0_mm)
    sk = root.sketches.add(plane)
    sk.sketchCurves.sketchCircles.addByCenterRadius(
        pt3d(cx_mm, cy_mm), cm(dia_mm / 2))
    body = _extrude_sketch(root, sk, dz_mm)
    body.name = name
    return body

def _cyl_y(root, cx_mm, cz_mm, y0_mm, dia_mm, dy_mm, name):
    """Cylinder with axis along Y. Sketched on offset XZ plane at y0, extruded +Y.

    Note: Fusion's xZ plane has sketch local +Y axis pointing in world -Z
    (the basis is right-handed with normal = world +Y, so sketch +X cross
    sketch +Y = +Y → sketch +Y = world -Z). We negate cz_mm here so callers
    can pass world Z directly.
    """
    plane = offset_xz_plane(root, y0_mm)
    sk = root.sketches.add(plane)
    sk.sketchCurves.sketchCircles.addByCenterRadius(
        adsk.core.Point3D.create(cm(cx_mm), cm(-cz_mm), 0.0),
        cm(dia_mm / 2))
    body = _extrude_sketch(root, sk, dy_mm)
    body.name = name
    return body

def _find_body(root, name):
    for i in range(root.bRepBodies.count):
        b = root.bRepBodies.item(i)
        if b.name == name:
            return b
    return None

# ============================================================
# === Builders: envelopes ====================================
# ============================================================

def build_pcb_envelope(root):
    _box_xy(root, 0, 0, PCB_BOT_Z, PCB_X_MM, PCB_Y_MM, PCB_T_MM,
            'PCB_envelope')

def _component_envelope(root, name, pcb_pos, box):
    wx, wy = pcb_to_world(*pcb_pos)
    bx, by, bz = box
    _box_xy(root, wx - bx/2, wy - by/2, PCB_TOP_Z, bx, by, bz, name)

def build_usb_envelope(root):
    _component_envelope(root, 'USB_envelope', USB_PCB, USB_BOX)

def build_jst_envelope(root):
    _component_envelope(root, 'JST_envelope', JST_PCB, JST_BOX)

def build_sw1_envelope(root):
    _component_envelope(root, 'SW1_envelope', SW1_PCB, SW1_BOX)

def build_u1_envelope(root):
    _component_envelope(root, 'U1_envelope', U1_PCB, U1_BOX)

def build_j6_envelope(root):
    _component_envelope(root, 'J6_envelope', J6_PCB, J6_BOX)

def build_button_envelope(root):
    _box_xy(root, BTN_X_MIN, BTN_Y_MIN, BTN_Z_MIN,
            BUTTON_BODY_X_MM, BUTTON_BODY_Y_MM, BUTTON_BODY_Z_MM,
            'BTN_body_envelope')
    # Cap: thin disc protruding 1 mm past +Y wall
    cap_dia = BUTTON_CAP_DIA_MM
    _cyl_y(root, BTN_CAP_CX, BTN_CAP_CZ, BTN_Y_MAX,
           cap_dia, (EXT_Y_MAX + 1.0) - BTN_Y_MAX,
           'BTN_cap_envelope')

# ============================================================
# === Builders: cut tool bodies ==============================
# ============================================================

def build_usb_cutout(root):
    """Tool body sized to cut a window through the +Y wall for the USB-C."""
    bx, _, bz = USB_BOX
    wx, _ = pcb_to_world(*USB_PCB)
    y0 = INT_Y_MAX - 0.3      # start inside the void
    y1 = EXT_Y_MAX + 0.5      # end past outside surface
    x0 = wx - bx/2 - 0.2
    z0 = PCB_TOP_Z - 0.2
    _box_xy(root, x0, y0, z0,
            bx + 0.4, y1 - y0, bz + 0.4,
            'USBcutout_tool')

def build_button_cap_hole(root):
    """Cylinder cutting through the +Y wall for the button cap."""
    y0 = INT_Y_MAX - 0.3
    y1 = EXT_Y_MAX + 0.5
    _cyl_y(root, BTN_CAP_CX, BTN_CAP_CZ, y0,
           BUTTON_CAP_DIA_MM + CAP_HOLE_CLEAR * 2, y1 - y0,
           'BTNcap_cutout_tool')

def build_button_retainer(root):
    """Solid block joined to the tray that provides the pocket walls around
    the suspended button body. Spans top/bottom/±X of the body. Open on -Y
    (where the button is inserted from the housing interior); the +Y face
    of the cavity meets the housing +Y wall (separately cut by the cap hole).
    """
    pad = RETAINER_PAD_MM
    x0 = BTN_X_MIN - pad
    y0 = BTN_Y_MIN
    z0 = BTN_Z_MIN - pad
    _box_xy(root, x0, y0, z0,
            (BTN_X_MAX + pad) - x0,
            BTN_Y_MAX - y0,
            (BTN_Z_MAX + pad) - z0,
            'BTNretainer_block')

def build_button_pocket(root):
    """Interior pocket for the button body, carved out of the retainer block.

    Opens on -Y (extends past the retainer's -Y face into housing void so the
    button slides in from inside). Stops at the +Y inner wall surface — the
    cap hole is a separate cylindrical cut through the wall.
    """
    dx = BUTTON_BODY_X_MM + POCKET_CLEAR_XY * 2
    dy = BUTTON_BODY_Y_MM + POCKET_CLEAR_XY * 2 + RETAINER_PAD_MM  # past block
    dz = BUTTON_BODY_Z_MM + POCKET_CLEAR_Z
    y1 = INT_Y_MAX                                # flush with inner wall
    y0 = y1 - dy
    x0 = BTN_CAP_CX - dx/2
    z0 = BTN_Z_MIN - POCKET_CLEAR_Z / 2
    _box_xy(root, x0, y0, z0, dx, dy, dz, 'BTNpocket_tool')

def build_jst_lid_hole(root):
    """Rectangular cutout in the lid above the JST plug."""
    wx, wy = pcb_to_world(*JST_PCB)
    z0 = INT_Z_MAX - 0.2
    dz = LID_T_MM + 0.4
    _box_xy(root,
            wx - JST_PLUG_HOLE_DX/2, wy - JST_PLUG_HOLE_DY/2, z0,
            JST_PLUG_HOLE_DX, JST_PLUG_HOLE_DY, dz,
            'JSTlid_cutout_tool')

def build_lidtab_slots(root):
    """Two rectangular through-slots in the ±X walls for the lid tabs."""
    slot_z0 = INT_Z_MAX - SLOT_Z_FROM_TOP - SLOT_HEIGHT_MM/2
    slot_y0 = (INT_Y_MIN + INT_Y_MAX) / 2 - SLOT_WIDTH_MM / 2

    # -X wall slot
    _box_xy(root,
            EXT_X_MIN - 0.2, slot_y0, slot_z0,
            WALL_T_MM + 0.4, SLOT_WIDTH_MM, SLOT_HEIGHT_MM,
            'lidtab_slot_front_tool')
    # +X wall slot
    _box_xy(root,
            INT_X_MAX - 0.2, slot_y0, slot_z0,
            WALL_T_MM + 0.4, SLOT_WIDTH_MM, SLOT_HEIGHT_MM,
            'lidtab_slot_back_tool')

# ============================================================
# === Builders: tray =========================================
# ============================================================

def build_tray(root):
    """Tray = floor + walls - interior void - cutouts - tab slots + snap pegs.

    Requires the relevant cut tool bodies to exist (call build_*_cutout
    builders first, or use BUILD_MODE='all').
    """
    # 1. Outer block
    tray = _box_xy(root,
                   EXT_X_MIN, EXT_Y_MIN, EXT_Z_MIN,
                   EXT_X_MAX - EXT_X_MIN,
                   EXT_Y_MAX - EXT_Y_MIN,
                   INT_Z_MAX - EXT_Z_MIN,
                   'tray')

    # 2. Hollow out the interior (open top)
    void = _box_xy(root,
                   INT_X_MIN, INT_Y_MIN, 0.0,
                   INT_X_MAX - INT_X_MIN,
                   INT_Y_MAX - INT_Y_MIN,
                   INT_Z_MAX,
                   '_tray_void')
    _bool_op(root, tray, void,
             adsk.fusion.FeatureOperations.CutFeatureOperation,
             keep_tool=False)

    # 3. Add the button retainer block back in (it lives inside the void
    #    but provides the pocket walls around the suspended button).
    retainer = _find_body(root, 'BTNretainer_block')
    if retainer is not None:
        _bool_op(root, tray, retainer,
                 adsk.fusion.FeatureOperations.JoinFeatureOperation,
                 keep_tool=False)

    # 4. Cutouts (USB, button cap, button pocket, lid-tab slots).
    #    Keep tool bodies for visual reference.
    for tool_name in (
        'USBcutout_tool',
        'BTNcap_cutout_tool',
        'BTNpocket_tool',
        'lidtab_slot_front_tool',
        'lidtab_slot_back_tool',
    ):
        tool = _find_body(root, tool_name)
        if tool is None:
            continue
        _bool_op(root, tray, tool,
                 adsk.fusion.FeatureOperations.CutFeatureOperation,
                 keep_tool=True)

    # 5. Snap pegs through PCB mounting holes
    _build_snap_pegs(root, tray)

def _build_snap_pegs(root, target):
    """Two compression snap pegs at the PCB mounting hole positions.

    Geometry per peg:
      - cylindrical shaft (PEG_SHAFT_DIA), height = PCB_TOP_Z (joins floor up
        to PCB top surface)
      - head cylinder (PEG_HEAD_DIA), sitting on top of shaft by PEG_HEAD_T
      - axial slit (PEG_SLIT_W) through both, oriented along world X so the
        peg's two halves can flex in ±Y to compress through the hole.
    """
    for label, (hx_pcb, hy_pcb) in (('peg1', HOLE_1_PCB), ('peg2', HOLE_2_PCB)):
        wx, wy = pcb_to_world(hx_pcb, hy_pcb)

        # Shaft
        shaft = _cyl_xy(root, wx, wy, 0.0,
                        PEG_SHAFT_DIA_MM, PCB_TOP_Z,
                        f'_{label}_shaft')
        _bool_op(root, target, shaft,
                 adsk.fusion.FeatureOperations.JoinFeatureOperation)

        # Head (slightly oversized — will get the slit cut so each half acts
        # as a flexing finger)
        head = _cyl_xy(root, wx, wy, PCB_TOP_Z,
                       PEG_HEAD_DIA_MM, PEG_HEAD_T_MM,
                       f'_{label}_head')
        _bool_op(root, target, head,
                 adsk.fusion.FeatureOperations.JoinFeatureOperation)

        # Slit: thin slab cut along world X axis, through full peg height
        slit_x = PEG_HEAD_DIA_MM + 0.6
        slit = _box_xy(root,
                       wx - slit_x/2, wy - PEG_SLIT_W_MM/2, 0.0,
                       slit_x, PEG_SLIT_W_MM,
                       PCB_TOP_Z + PEG_HEAD_T_MM + 0.4,
                       f'_{label}_slit')
        _bool_op(root, target, slit,
                 adsk.fusion.FeatureOperations.CutFeatureOperation)

# ============================================================
# === Builders: lid ==========================================
# ============================================================

def build_lid(root):
    """Lid = top plate - JST plug hole + 2 snap tabs."""
    # 1. Plate
    lid = _box_xy(root,
                  EXT_X_MIN, EXT_Y_MIN, INT_Z_MAX,
                  EXT_X_MAX - EXT_X_MIN,
                  EXT_Y_MAX - EXT_Y_MIN,
                  LID_T_MM,
                  'lid')
    # 2. JST plug hole
    tool = _find_body(root, 'JSTlid_cutout_tool')
    if tool is not None:
        _bool_op(root, lid, tool,
                 adsk.fusion.FeatureOperations.CutFeatureOperation,
                 keep_tool=True)
    # 3. Snap tabs
    _build_lid_tabs(root, lid)

def _build_lid_tabs(root, lid):
    """Two tabs hanging from lid bottom, INSIDE the ±X walls.

    Each tab has a barb that projects OUTWARD into a through-slot in the wall.
    Insertion: tab flexes inward as it slides past the wall above the slot,
    then springs outward when the barb reaches the slot Z. Release: push the
    barb back inward from outside through the slot opening, then lift.
    """
    tab_z_top = INT_Z_MAX
    tab_z_bot = tab_z_top - TAB_LENGTH_MM
    y_center  = (INT_Y_MIN + INT_Y_MAX) / 2
    tab_y0    = y_center - TAB_WIDTH_MM / 2
    barb_z0   = INT_Z_MAX - SLOT_Z_FROM_TOP - TAB_BARB_T_MM / 2

    # --- Front (-X) tab, inside the -X wall ---
    front_tab_x0 = INT_X_MIN + TAB_WALL_GAP_MM
    front_tab = _box_xy(root,
                        front_tab_x0, tab_y0, tab_z_bot,
                        TAB_THICK_MM, TAB_WIDTH_MM, TAB_LENGTH_MM,
                        '_lid_tab_front_shaft')
    _bool_op(root, lid, front_tab,
             adsk.fusion.FeatureOperations.JoinFeatureOperation)
    # Barb projects -X (toward wall) from tab outer face into the slot
    front_barb = _box_xy(root,
                         front_tab_x0 - TAB_BARB_PROJ,
                         tab_y0 + 0.5, barb_z0,
                         TAB_BARB_PROJ, TAB_WIDTH_MM - 1.0, TAB_BARB_T_MM,
                         '_lid_tab_front_barb')
    _bool_op(root, lid, front_barb,
             adsk.fusion.FeatureOperations.JoinFeatureOperation)

    # --- Back (+X) tab, inside the +X wall ---
    back_tab_x_max = INT_X_MAX - TAB_WALL_GAP_MM
    back_tab_x0   = back_tab_x_max - TAB_THICK_MM
    back_tab = _box_xy(root,
                       back_tab_x0, tab_y0, tab_z_bot,
                       TAB_THICK_MM, TAB_WIDTH_MM, TAB_LENGTH_MM,
                       '_lid_tab_back_shaft')
    _bool_op(root, lid, back_tab,
             adsk.fusion.FeatureOperations.JoinFeatureOperation)
    # Barb projects +X (toward wall) from tab outer face into the slot
    back_barb = _box_xy(root,
                       back_tab_x_max,
                       tab_y0 + 0.5, barb_z0,
                       TAB_BARB_PROJ, TAB_WIDTH_MM - 1.0, TAB_BARB_T_MM,
                       '_lid_tab_back_barb')
    _bool_op(root, lid, back_barb,
             adsk.fusion.FeatureOperations.JoinFeatureOperation)

# ============================================================
# === Mode groups ============================================
# ============================================================

ENVELOPE_KEYS = [
    'pcb_env', 'usb_env', 'jst_env', 'sw1_env', 'u1_env', 'j6_env',
    'button_env',
]
TRAY_CUT_KEYS = [
    'button_retainer',  # must come before tray so the join finds it
    'usb_cutout', 'button_cap_hole', 'button_pocket', 'lidtab_slots',
]
LID_CUT_KEYS = ['jst_lid_hole']

# ============================================================
# === Registry ===============================================
# ============================================================

COMPONENTS = {
    # Envelopes (reference, kept after cuts)
    'pcb_env':     build_pcb_envelope,
    'usb_env':     build_usb_envelope,
    'jst_env':     build_jst_envelope,
    'sw1_env':     build_sw1_envelope,
    'u1_env':      build_u1_envelope,
    'j6_env':      build_j6_envelope,
    'button_env':  build_button_envelope,

    # Cut tools (consumed by tray/lid via boolean ops, but kept visible
    # so the user can see where each cut landed). The button_retainer is
    # actually JOINED to the tray rather than cut — it provides the cavity
    # walls for the suspended button.
    'button_retainer': build_button_retainer,
    'usb_cutout':      build_usb_cutout,
    'button_cap_hole': build_button_cap_hole,
    'button_pocket':   build_button_pocket,
    'jst_lid_hole':    build_jst_lid_hole,
    'lidtab_slots':    build_lidtab_slots,

    # Printable bodies
    'tray':  build_tray,
    'lid':   build_lid,
}

# Group aliases for BUILD_MODE
GROUPS = {
    'envelopes': ENVELOPE_KEYS,
    'tray':      TRAY_CUT_KEYS + ['tray'],
    'lid':       LID_CUT_KEYS + ['lid'],
    'all':       (ENVELOPE_KEYS + TRAY_CUT_KEYS + LID_CUT_KEYS
                  + ['tray', 'lid']),
}

# ============================================================
# === Entry point ============================================
# ============================================================

def run(context):
    ui = None
    try:
        app = adsk.core.Application.get()
        ui = app.userInterface
        design = adsk.fusion.Design.cast(app.activeProduct)
        if not design:
            ui.messageBox('Open or create a Design document and re-run.')
            return
        root = design.rootComponent
        mode = BUILD_MODE.lower()

        if mode in GROUPS:
            keys = GROUPS[mode]
        elif mode in COMPONENTS:
            keys = [mode]
        else:
            ui.messageBox(
                f'Unknown BUILD_MODE: {BUILD_MODE!r}\n'
                f'Valid: {list(GROUPS.keys()) + list(COMPONENTS.keys())}')
            return

        for k in keys:
            COMPONENTS[k](root)

        ui.messageBox(f'Done. Built: {", ".join(keys)}')
    except Exception:
        if ui:
            ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))
