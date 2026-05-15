---
title: Packaging Dielines — Implementation Notes
type: design-doc
status: shipped — both generators passing 10/10 reference-DXF validation tests
created: 2026-05-12
updated: 2026-05-12
tags: [packaging, dieline, hardware, hcde]
---

# Packaging Dielines

Parametric Python generators for two cardboard packaging templates: a **Roll End Front Tuck (REFT) mailer** (the outer box) and a **tray-with-backer insert** (the inner cradle, with N-slot support). Generators reverse-engineered from 10 reference DXFs purchased from CEFbox at controlled dimension variations, then validated against those references via automated entity-count + Y-symmetry tests.

Lives behind [LL-051](../tasks.md#LL-051) (packaging concept), unblocks [LL-044](../tasks.md#LL-044) (right-to-repair QR raster), and feeds the in-house laser-cut packaging path flagged as primary-supplier in [supply-chain-map.md](supply-chain-map.md).

## 1. Files

In [Assembly_docs/packaging/](../Assembly_docs/packaging/):

| File | Role |
|---|---|
| `mailer_reft.py` | REFT mailer parametric model — `MailerSpec(L, W, H, t)` → `generate(spec)` returns entity list |
| `insert_tray.py` | Insert tray parametric model — `InsertSpec(L, W, H, t, IH, slots=[...])` with `single_slot()` and `n_slot()` constructors |
| `cefbox_render.py` | Solid-line PNG renderer (red=cut, blue=score, color-only differentiation) |
| `validation.py` | Entity-count + Y-symmetry test suite, runs against the 5 reference DXFs per template |
| `dxf_inspect.py` | Print structured entity inventory of a reference DXF (for debugging) |
| `dxf_to_png.py` | Pure passthrough renderer for reference DXFs (verifies what's *in* the DXF before generator work) |
| `dxf_analyze.py` | Per-template summary + score-line position diffs across multiple reference DXFs |
| `dxf_recover.py` | Per-coordinate parametric formula recovery (legacy from the per-coord approach; superseded by the named-parts architecture) |

The 10 reference DXFs live in `Assembly_docs/Packaging_Templates/` (gitignored — paid CEFbox content).

## 2. The journey (how this got built)

### Path A → B → A pivots

- **Initial sketch (wrong)**: hand-derived a v1 sketch from packaging-engineering names ("REFT mailer", "stepped-tray insert"). Bill compared to the actual CEFbox UI and the geometry was structurally off in significant ways (missing dust flaps, wrong ear-lock topology, missing finger holes, wrong cradle structure).
- **Path A — per-coord parametric recovery**: bought 10 paid CEFbox DXFs at controlled dimension variations (5 mailer + 5 insert, varying L/W/H/t/SL/SW/SH/IH one parameter at a time). Built `dxf_analyze.py` and `dxf_recover.py` to parse with `ezdxf` and extract per-coordinate parametric formulas of the form `a·L + b·W + c·H + d·t + const`. Recovered ~60% of unique coords cleanly; the remaining ~40% were arc-endpoint coordinates needing separate `(center, radius, start_angle, end_angle)` recovery.
- **Path A → named-parts pivot**: the per-coord approach was producing fragmented geometry that didn't match references in important structural ways. Pivoted to a NAMED-PARTS architecture: model the dieline as a graph of structural pieces (panels, walls, arcs, cradles) with rules and Y-symmetry as a hard constraint. Built incrementally with verify-each-phase discipline.

### Critical bug found mid-build

The mailer dieline is **mirror-symmetric about `x = L/2 + 5t`** (= 157.5 at baseline t=1.5), NOT about `x = L/2` (= 150 at baseline). Every right-side coord was off by 10t until traced via a manual symmetry inspection of the baseline DXF (computing the bbox center directly from raw extrema). The fix: a single helper `mirror_axis_x(s)` and `x_mirror(s, x)` used everywhere right-side positions are computed from left-side ones.

The insert is symmetric about `x = L/2` exactly (no offset).

## 3. Mailer (`mailer_reft.py`)

### Inputs

```python
MailerSpec(L, W, H, t=1.5)
```

| Param | Default | Validated range |
|---|---|---|
| `L` | — | ≥ 60 |
| `W` | — | ≥ 180 (tooth pattern lower bound) |
| `H` | — | [20, 200] |
| `t` | 1.5 | [0.5, 5.0] |
| Also enforced | | `H ≥ 2t` (case-A non-degenerate) |

### Spine layout (5 panels stacked along y)

```
                   [ LID_TUCK ]              y_top   = 2W + 2H + 5.5t
                   ├─score────┤              y_score_lid_tuck = 2W + H + 4.5t
                   [   LID    ]              
                   ├─score────┤              y_score_back_lid = W + H + 2.5t
                   [ BACK_WALL ]
        ┌──────────┼──────────┼──────────┐
        │ LEFT     │          │ RIGHT    │
        │ ROLL-END │ BOTTOM   │ ROLL-END │
        │   2H wide│          │   2H wide│
        └──────────┼──────────┼──────────┘   y_score_bottom_back = W + 1.5t
                   ├─score────┤              y_score_tuck_bottom = +0.5t
                   [ FRONT_TUCK ]            y_bot   = -H
```

Panel heights, all derived (verified pixel-perfect against baseline at t=1.5):

| Panel | Height |
|---|---|
| FRONT_TUCK | H + 0.5t |
| BOTTOM | W + t |
| BACK_WALL | H + t |
| LID | W + 2t |
| LID_TUCK | H + t |

### 6-phase build (in mailer_reft.py)

| Phase | Adds |
|---|---|
| 1 | spine + 4 horizontal scores (verified pixel-perfect against baseline) |
| 2 | roll-end side walls with W-conditional tooth pattern (`n_finger_holes`: 2 at W≤220, 3 at W≤290, else `round(W/90)`) + 2 fold scores per side |
| 3 | dust flaps with case-A (shoulder chamfer present, baseline) and case-B (no shoulder, H ≥ ~96 at t=1.5) logic at threshold `H ≈ 100 − 2.5t` |
| 4 | ear-lock arcs at LID_TUCK top (4 main + 2 horns r=3 + 2 score-tangents) + 4 LID corner arcs (r ≈ H/2) + 4 tangent diagonals + LID outer vertical edges |
| 5 | finger holes (1 per outer tooth, x ∈ [0, 5.25] on left, mirrored to right; W-scaled count) |
| 6 | 14 panel-fold scores (FRONT_TUCK, BACK_WALL, LID, LID_TUCK each get an inset fold; BOTTOM split into N+1 segments by finger holes) + small corner connectors |

### Key parametric formulas (all confirmed via t=1.5 + t=3.0 samples)

| Feature | Formula |
|---|---|
| Side wall outer x | `−(2H + 4.833t + 2)` |
| Side wall inner x | `−(2H + 4.5t)` |
| Side-wall fold inner x | `−(H + 4t)` |
| Inner panel score x | `−(H + t)` |
| Dust flap outer x | `−(100 − 1.5t)` |
| Mirror axis | `L/2 + 5t` |
| Shoulder chamfer angle | constant 9° (rise/run = 0.158) |

## 4. Insert (`insert_tray.py`)

### Inputs

```python
InsertSpec(L, W, H, t=1.5, IH=55, slots=[Slot(cx, cy, SL, SW, SH), ...])
# or:
InsertSpec.single_slot(L, W, H, t, SL, SW, SH, IH=None)        # IH defaults to H − SH
InsertSpec.n_slot(L, W, H, t, n=2, SL, SW, SH, IH=None)        # N slots evenly distributed
```

| Param | Notes |
|---|---|
| `L`, `W`, `H` | outer box dimensions (the floor approximates L × W) |
| `t` | material thickness |
| `IH` | insert height (depth side walls fold down); defaults to `H − SH` for snug fit |
| `slots` | list of `Slot(cx, cy, SL, SW, SH)` records, positioned absolutely on the floor |

### 5-phase build

| Phase | Adds |
|---|---|
| 1 | floor + 4 edge scores + slot(s) with cradle X-fold (Polyline for the inner cradle rect) |
| 2 | top-band stack above floor (back wall + hem + backer + hem-flap) + 3 horizontal scores + outer cuts |
| 3 | front wall below floor + 2 bottom corner arcs (r=8) + bottom horizontal cut |
| 4 | left/right side wall flaps with corner arcs (4 total, r=8) |
| 5 | tangent diagonals (4 to side wall corners + 2 to top backer + 2 to front wall) + 8 corner micro-chamfers |

### Geometric rules (`InsertSpec.validate()`)

- **5mm slot-edge buffer** from each floor edge (left/right/top/bottom)
- **5mm minimum gap** between slots (pairwise)
- **All slot dimensions positive** (SL, SW, SH > 0)
- Effective SH per slot is clamped to `min(SH, SL/2, SW/2)` — cradle inner can't go negative
- Bad inputs raise `ValueError` with specific actionable messages

### Multi-slot

- `n_slot(n=2)` distributes N slots evenly along the L axis, all sized identically
- Custom asymmetric layouts: pass a custom `slots=[...]` list with arbitrary `(cx, cy, SL, SW, SH)` per slot
- Y-symmetry validation supports `skip_symmetry: True` for asymmetric configs

## 5. Validation (`validation.py`)

Two automated tests per generator-vs-reference-DXF comparison:

1. **Entity count match per layer** — counts of LINE/ARC/LWPOLYLINE on each layer (cuttingLine, foldingLine) match the reference within tolerance ±2
2. **Y-symmetry** — every entity has a mirror partner about the dieline's mirror axis. Arc canonicalization via `(midpoint, sweep)` to handle 0/360 wrap; polyline canonicalization dedupes explicit closing points; 0.01mm rounding precision to defeat FP drift between left/right mirrored values

### Run

```bash
python validation.py
```

### Result (current)

```
MAILER VALIDATION SUITE                INSERT VALIDATION SUITE
--- baseline 300x200x80 t1.5    PASS   --- baseline 300x200x80 ...    PASS
--- vary L: 400x200x80 t1.5     PASS   --- vary L: 400x200x80 ...     PASS
--- vary W: 300x250x80 t1.5     PASS   --- vary W: 300x250x80 ...     PASS
--- vary H: 300x200x120 t1.5    PASS   --- vary H: 300x200x120 IH95   PASS
--- vary t: 300x200x80 t3.0     PASS   --- vary slot: ...slot200x100  PASS

5/5 passed                              5/5 passed
```

## 6. Output convention

Both generators emit a list of `Line`, `Arc`, and `Polyline` namedtuples. Each entity has a `layer` field — `cuttingLine` (red, through-cut) or `foldingLine` (blue, score). Render to PNG via `cefbox_render.render_entities(ents, out_path, title)`.

The renderer uses solid lines for both cut and score, with color-only differentiation (red vs blue). This matches Bill's preference for laser-cutter / die-cutter input where the differentiation is by stroke color, not stroke style.

## 7. Practical notes

- **225×225×60 mailer at E-flute (t=1.5)**: dieline bbox = **498.5 × 638.25 mm**. Fits a 520×660 mm sheet with 10mm hold-down margin; **600×800 mm recommended** for 1-up production with registration room.
- **Reference DXFs are paid content** — do not commit `Assembly_docs/Packaging_Templates/` to public git. The `.gitignore` rule is in place.
- **Score representation**: a single line in the foldingLine layer is the convention most laser drivers (LightBurn, Inkscape laser plugins) expect for "score one liner of the corrugated".

## 8. Known follow-ons (separate workstreams, now unblocked)

- **`nest.py`** — sheet-packing optimizer via `rectpack` for N-up production layouts
- **First physical test cut + fold + fit** — laser-cut one of each at 225×225×60 / matching insert, fold and verify
- **[LL-044](../tasks.md#LL-044) wiring** — populate the QR raster region on the insert backer with a real URL (right-to-repair landing page)
- **Multi-slot validation reference** — generate a real CEFbox 2-slot insert DXF (when needed for a real PSU-included SKU) so multi-slot can be validated with `skip_symmetry: True` against an actual reference

## Related

- [LL-051 — Packaging concept](../tasks.md#LL-051) (this workstream) · [LL-044 — Packaging QR code](../tasks.md#LL-044) (downstream)
- [BOM Breakdown — Basic 6×6](bom-breakdown-basic-6x6.md) — 8×8×3" packaging line item, $2 estimate
- [Supply Chain Map](supply-chain-map.md) — die-cut vendor sourcing; in-house laser is now the proven path
- [Right-to-Repair Philosophy §6](right-to-repair-philosophy.md#6-what-this-asks-of-the-buyer) — QR-on-cardboard intent
- [Sustainability Audit §Packaging](sustainability-audit.md) — cardboard recyclability, no-foam rationale
- [Assembly_docs/basic_housing/](../Assembly_docs/basic_housing/) — parallel matplotlib-first / CAD-after pattern
- External reference templates: [CEFbox REFT mailer](https://www.cefbox.com/dielines/mailerBox/earLockMailers) · [CEFbox insert with backing](https://www.cefbox.com/dielines/insert/withBacking)
