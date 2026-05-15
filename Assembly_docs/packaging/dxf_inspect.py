"""Print structured entity inventory for a baseline DXF.

Groups by entity type and sorts by position so a human can step through
and identify the named parts (spine panels, roll-end side walls,
finger holes, ear-lock notches, dust flap chamfers, slot cradle, etc.)
that the part library will encode.

Run:  python dxf_inspect.py <path.dxf>
      python dxf_inspect.py mailer       (uses baseline mailer)
      python dxf_inspect.py insert       (uses baseline insert)
"""

import sys
import math
from pathlib import Path
from collections import defaultdict

import ezdxf


REFS_DIR = Path(r"C:\Users\bowhi\Desktop\Independent_Study\Assembly_docs\Packaging_Templates")
BASELINES = {
    "mailer": "mailer_300x200x80_t1.5.dxf",
    "insert": "insert_300x200x80_t1.5_slot150x150x25_IH55.dxf",
}


def inspect(path):
    doc = ezdxf.readfile(path)
    msp = doc.modelspace()

    by_type_layer = defaultdict(list)
    for e in msp:
        by_type_layer[(e.dxftype(), e.dxf.layer)].append(e)

    print(f"\n=== {path.name} ===")
    for (et, layer), ents in sorted(by_type_layer.items()):
        print(f"\n  {et} on layer {layer!r}: {len(ents)} entities")
        if et == "LINE":
            # Group by orientation
            horiz = []
            vert = []
            diag = []
            for e in ents:
                x0, y0 = e.dxf.start.x, e.dxf.start.y
                x1, y1 = e.dxf.end.x, e.dxf.end.y
                if abs(y0 - y1) < 0.01:
                    horiz.append((round(y0, 3), round(min(x0, x1), 3),
                                  round(max(x0, x1), 3)))
                elif abs(x0 - x1) < 0.01:
                    vert.append((round(x0, 3), round(min(y0, y1), 3),
                                 round(max(y0, y1), 3)))
                else:
                    diag.append((round(x0, 3), round(y0, 3),
                                 round(x1, 3), round(y1, 3)))
            print(f"    horizontal ({len(horiz)}): sorted by y, then x_min")
            for y, xmin, xmax in sorted(horiz):
                print(f"      y={y:>9.3f}  x: {xmin:>9.3f} -> {xmax:>9.3f}  "
                      f"(len {xmax-xmin:>7.3f})")
            print(f"    vertical ({len(vert)}): sorted by x, then y_min")
            for x, ymin, ymax in sorted(vert):
                print(f"      x={x:>9.3f}  y: {ymin:>9.3f} -> {ymax:>9.3f}  "
                      f"(len {ymax-ymin:>7.3f})")
            print(f"    diagonal ({len(diag)}): sorted by start")
            for x0, y0, x1, y1 in sorted(diag):
                dx, dy = x1 - x0, y1 - y0
                ang = math.degrees(math.atan2(dy, dx))
                length = math.hypot(dx, dy)
                print(f"      ({x0:>9.3f}, {y0:>9.3f}) -> ({x1:>9.3f}, "
                      f"{y1:>9.3f})  ang={ang:>+6.1f} deg  len={length:>6.3f}")
        elif et == "ARC":
            for e in ents:
                cx, cy = e.dxf.center.x, e.dxf.center.y
                print(f"    center=({cx:>9.3f}, {cy:>9.3f})  r={e.dxf.radius:.3f}"
                      f"  sa={e.dxf.start_angle:>7.2f}  ea={e.dxf.end_angle:>7.2f}")
        elif et == "LWPOLYLINE":
            for e in ents:
                pts = [(round(p[0], 3), round(p[1], 3)) for p in e.get_points()]
                print(f"    closed={e.closed}  {len(pts)} pts:")
                for px, py in pts:
                    print(f"      ({px:>9.3f}, {py:>9.3f})")
        elif et == "POINT":
            for e in ents:
                print(f"    ({e.dxf.location.x:>9.3f}, {e.dxf.location.y:>9.3f})")


def main():
    if len(sys.argv) < 2:
        print("Usage: python dxf_inspect.py <path|mailer|insert>")
        sys.exit(1)
    arg = sys.argv[1]
    if arg in BASELINES:
        path = REFS_DIR / BASELINES[arg]
    else:
        path = Path(arg)
    inspect(path)


if __name__ == "__main__":
    main()
