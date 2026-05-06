---
title: Task Format V2 — Design Specification
type: design-doc
status: draft
created: 2026-05-06
tags: [meta, task-tracking, design]
---

# Task Format V2 — Design Specification

Design-only document. No migration commits this session. Goal: specify a stable task identity scheme and structured completion log to replace the current slug-based format that causes false-close cascades in the downstream collector.

---

## 1. Current Format Audit

### 1.1 File inventory

| File | Role | Lines | Task density |
|------|------|-------|--------------|
| `sprint_plan.md` | Human roadmap; consumed by collector to derive planned tasks | 186 | ~35 task bullets across 11 weeks |
| `sprint_log.md` | Progress narrative; Done entries are the current completion signal | ~535 | ~55 Done `[x]` entries through Week 5 |
| `docs/mini-sprint-app-demo.md` | Micro-plan for Apr 28–May 5 app push (8 sessions, own DoD) | — | Treated by collector as a sub-plan |
| `docs/post-mini-sprint-bugs.md` | Bug backlog; uses `#N` numeric IDs, but they skip (`#1,#2,#6` open; `#3,#4,#5` closed) | — | 6 bugs total; 3 open, 3 closed |
| `sprint_plan.md` LLC Compliance Checklist | Separate `[x]`/`[ ]` checkbox list embedded in the plan | — | 8 compliance tasks |

### 1.2 Current ID scheme

The downstream collector currently derives task IDs as `github:sprint:<week>:<slug>` where `<week>` is the sprint week number (or a category tag like `llc`) and `<slug>` is the title slugified. Neither segment is stable:

- **Week segment**: a task that slips to the next sprint gets a new week prefix.
- **Slug segment**: renaming a task — even a minor wording change — produces a new ID downstream.

There are no explicit ID fields anywhere in the planning or log files. Identity is entirely narrative.

### 1.3 Task count

**sprint_plan.md (planned tasks, high-level):**

| Week | Tasks | Notes |
|------|-------|-------|
| 1 | 4 | Market analysis, Stakeholder mapping, LLC setup group, ESP32 module |
| 2 | 3 | BOM breakdown, Sustainability audit, Schematic/PCB layout |
| 3 | 2 | Operations mapping, PCB order (critical path) |
| 4 | 5 | Brand identity, Right-to-Repair docs, User research, Firmware kickoff, Compliance focus day |
| 5 | 4 | App Demo Mini-Sprint, Wireframes, User research cont., Portfolio design rationale |
| 6 | 2 | Supply chain map, PCBs arrive + validate |
| 7 | 3 | Service Blueprint, Custom order flow, Firmware stress-test |
| 8 | 3 | Repairability Manual, ToS/Warranty, Firmware app integration |
| 9 | 3 | Media assets, BOM finalization, Pre-production mirror |
| 10 | 1 | Portfolio case study |
| 11 | 3 | Venture readiness doc, Kickstarter readiness, Golden sample |
| **Total** | **33** | Plus ~8 compliance checklist items |

**sprint_log.md (completed Done entries, granular):**

| Week | Done entries |
|------|-------------|
| 1 | 8 |
| 2 | 3 |
| 3 | 1 |
| 4 | ~18 |
| 5 | ~25 (including mini-sprint sessions) |
| **Total** | **~55** |

The Done-entry count significantly exceeds the plan count because the log captures fine-grained sub-milestones (Session 2a, Session 2b, Session 2c) that have no corresponding entry in `sprint_plan.md`.

### 1.4 Rename traces and quirks

**Confirmed renames:**

1. `BOI Report filing` (Week 1 plan task) → renamed in-place after the Apr 22 exemption discovery. Plan text struck through and rewritten inline. Log entry records a different title.
2. `Sales tax permit` (compliance checklist) → completed as `WA BLS registration filed` in the log. Different enough that fuzzy-matching is unreliable.
3. `App Demo Mini-Sprint` (one Week 5 bullet) → expanded mid-sprint into Sessions 0–8, each logged separately. The plan item was never checked off.
4. `ESP32 firmware kickoff` (Week 4 plan) → completed via ~5 distinct Done entries. The plan item title doesn't appear verbatim anywhere in the log.

**Silent disappearances:**

- `Order PCBs` (Week 3 critical path) — appears in the plan and Week 3 milestone table but has no corresponding Done entry. Not cancelled, not explicitly deferred. Silently shifted to "expected Week 6."

**Other quirks:**

- The bug file uses `#N` IDs that skip, implying items were renumbered. A sequential counter without a registry causes this.
- `### In Progress` sections in `sprint_log.md` are always empty in practice.
- Blocked items don't carry forward across weeks.

---

## 2. Design Goals (from the request)

1. **Stable task identity** — ID is immutable across renames, scope edits, sprint reshuffles.
2. **Structured, machine-parseable** — minimum fields: id, title, sprint, status, priority, deadline, dependencies, artifact links.
3. **First-class completion logging** — closing a task is an explicit append, not "task vanished from plan."
4. **Append-only history** — when each task was added, edited, or completed is readable without `git log`.
5. **Human-friendly** — markdown + frontmatter; hand-editable daily.
6. **Downstream compatibility** — IDs round-trip into `github:<scheme>:<id>` in ASCII with no spaces, dots, or quotes.

**Decisions confirmed:**

- Completion signal: `[x]`/`[ ]` checkbox. Resolved-N/A gets `[x]`; Notes explains why.
- Task names are stable; description, notes, and metadata are freely editable.
- Hierarchy uses **dash separators** in IDs (`LL-042-2-1`, not dots) so anchors render reliably across GitHub, Obsidian, and any URL-handling system. Unlimited depth.
- IDs are permanent and immutable. They are never overwritten, reformatted, or promoted. A child task that grows in scope spawns grandchildren beneath it — it does not become a top-level task.
- Sprint field is tied to **the sprint a task was started in**. It does not change when work spans into later sprints; the active sprint goes in Notes.
- Each task carries four date fields: `added`, `first_engaged`, `last_engaged`, `resolved`.
- Q3 (rename threshold): Claude flags ambiguous cases in real time during documentation passes; no fixed rule.

---

## 3. Proposed ID Scheme

### 3.1 Top-level tasks: `LL-NNN`

Each top-level task gets a sequential identifier of the form `LL-NNN` where `NNN` is a zero-padded decimal integer starting at `001`. Examples: `LL-001`, `LL-042`, `LL-100`.

Properties:
- **Deterministic** — assigned once at task creation from the registry's `next_id` counter.
- **Short-string-safe** — 6 characters, pure ASCII, no spaces, dots, or quotes.
- **Human-greppable** — `grep LL-007` finds every mention across the vault.
- **Immutable** — the ID never changes; title, sprint, and metadata can all change.
- **Downstream composite** — `github:task:LL-001`. The `task` scheme segment is fixed; `LL-NNN` is the stable leaf. This replaces the old `github:sprint:5:session-1-foundation` which embedded both week and slug.

The 3-digit zero-pad is load-bearing for greppability: `grep LL-042` finds task 042 and its descendants without also matching `LL-421` or `LL-4200`. Don't drop the padding.

### 3.2 Sub-milestones: `LL-NNN-M` and deeper

When a task has discrete deliverable sub-tasks, they get child IDs by appending `-M` to the parent, where `M` is a sequential integer starting at `1`. Examples: `LL-042-1`, `LL-042-2`. Grandchildren extend the same way: `LL-042-2-1`, `LL-042-2-2`.

Properties:
- **Greppable by parent** — `grep LL-042` finds the parent and all descendants at every depth.
- **Independently closeable** — each node has its own `[x]`/`[ ]` state. A parent does not auto-close when children close (see §4.1 cleanup workflow).
- **IDs are permanent and never reformatted.** A child task whose scope grows does not get promoted to a top-level task — it spawns grandchildren beneath it. This is what keeps the downstream collector seeing continuous evolution rather than disappearance + new-task-arrival.
- **Downstream composite** — `github:task:LL-042-2-1`. All-dash format is URL-safe and survives every renderer.
- **Unlimited depth in principle, shallow in practice.** Most tasks are flat (`LL-NNN`) or one level deep (`LL-NNN-M`). A second level (`LL-NNN-M-K`) arises when a session within a mini-sprint has its own named intra-session deliverables. Go deeper only when there is a real named deliverable.

### 3.3 Why not alternatives

| Alternative | Problem |
|-------------|---------|
| UUID | Not human-greppable; hard to type |
| GitHub issue numbers | Requires creating a GH issue per task; high friction; two systems to sync |
| Date-based (`2026-W01-003`) | Week prefix misleads when tasks slip; encodes mutable position |
| Semantic slugs (`eng-ota-v1`) | Same fragility as the current system |
| Dotted hierarchy (`LL-042.2.1`) | Dot can be rewritten or dropped by Markdown anchor renderers; URL/JSON-pointer ambiguity |
| PR numbers | PRs close; parked/planned tasks have no PR; wrong granularity |

### 3.4 Next-ID discipline

The `tasks.md` file-level frontmatter carries a `next_id` counter. Adding a task = read `next_id`, write the block, increment the counter. No lookup table needed; the file IS the lookup table.

Children are numbered locally within their parent: `next_child_id` is implicit (one greater than the highest `-M` already written under the parent). No separate counter needed.

---

## 4. File Structure

Three files carry the new format. `sprint_plan.md` and `sprint_log.md` remain narrative; they gain `[LL-NNN]` inline links into the registry.

### 4.1 `tasks.md` — Task Registry (new file)

The machine-parseable source of truth. Tasks are level-3 markdown headings with `[x]`/`[ ]` inline completion state, followed by structured key-value lines and a free-text Notes block. Children are level-4 (or deeper) headings.

**File-level frontmatter:**

```yaml
---
title: Task Registry
type: task-registry
next_id: LL-056
updated: 2026-05-06
---
```

**Top-level task block:**

```markdown
<a id="LL-001"></a>
### [x] LL-001 — Market analysis and competitive landscape

sprint: 1 | priority: high | deadline: 2026-04-06
added: 2026-03-30 | first_engaged: 2026-03-30 | last_engaged: 2026-04-06 | resolved: 2026-04-06
artifacts: [Market Analysis Report](docs/Market%20Analysis%20Report.md) · commit `93f686c`
dependencies: —

**Notes:** Cross-platform analysis of 30+ listings across Etsy, Amazon, Pinterest. Bifurcated market: 4 segments (Bespoke Luxury, B2B Branding, Sensory/Therapy, Lifestyle/Gaming). $50–$120 bracket has 1,900+ Etsy listings — saturation risk if industrial design doesn't signal $200+ value.
```

**Child task block (nested under its parent section):**

```markdown
<a id="LL-042-1"></a>
#### [x] LL-042-1 — Session 0: First flash on real silicon

parent: LL-042 | sprint: 5
added: 2026-04-28 | first_engaged: 2026-04-28 | last_engaged: 2026-04-28 | resolved: 2026-04-28
artifacts: sprint_log.md · commit `50035df`

**Notes:** Booted V1 firmware on XIAO ESP32-C3 after fixing two pre-flash-review-missed bugs (Unicore core-ID assert, state_bus defaults hardcoding). Boot log clean end-to-end.
```

**Date field semantics:**

| Field | Meaning |
|-------|---------|
| `added` | Date the task was created in the registry |
| `first_engaged` | Date work first began on the task (often = added if no planning lag) |
| `last_engaged` | Date the task was last touched (bump during session cleanup if work landed against it) |
| `resolved` | Date the task was marked `[x]` (omit if open; equals `last_engaged` for tasks closed in one session) |

`last_engaged` is the field most likely to rot. Discipline: at the end of each session's documentation pass, bump `last_engaged` on every task that received work that session. The collector can use this field to detect stale-but-not-closed tasks.

**Sprint field semantics:**

`sprint:` is the sprint a task was **started in**. It does not update as work spans into later sprints. If the active sprint differs from the started-in sprint, note the active sprint in the Notes field (e.g., `(active sprint: 7)`). Slipping is captured as a `slipped` event in `task_log.md`, not as a sprint-field rewrite. This keeps `sprint:` as a stable bucketing dimension and avoids losing the original sprint context.

**Field formatting rules:**

- The `<a id="LL-NNN"></a>` HTML anchor (with all-dash form for hierarchy) is what makes the heading linkable as `tasks.md#LL-NNN`. Keep it on its own line above the heading.
- The `[x]`/`[ ]` in the heading IS the completion flag. `[x]` = done (including resolved-N/A), `[ ]` = open.
- Key-value lines below the heading are mutable — update in place as facts change.
- Task names (after `LL-NNN —`) should be stable noun phrases. No dates, no verbs, no narrative tense. If a name does change substantively, note the old name in Notes and log a `renamed` event in `task_log.md`.
- `**Notes:**` is append-friendly — prepend new notes with a date tag when multiple sessions have touched a task. Not hard append-only; clarity beats strictness.

**Completion semantics:**

| Heading | Meaning |
|---------|---------|
| `### [x] LL-NNN — …` | Done — including resolved-N/A, confirmed-exempt, delivered, or any definitive close |
| `### [ ] LL-NNN — …` | Open — including planned, in-progress, and blocked. Current state lives in Notes |

Blocked/in-progress distinction lives in Notes and `task_log.md` events, not in the checkbox. The checkbox answers only: "is this task definitively over?"

**Parent-close workflow (session cleanup):** At the end of a session's documentation pass, scan `tasks.md` for any `[ ]` parent task whose children are all `[x]`. If all children are resolved, mark the parent `[x]`, set `resolved:` to today, and append a `done` row to `task_log.md` with a note like "all children resolved." This is a deliberate human step — parents do not auto-close, because a parent task can also have scope of its own beyond its children.

### 4.2 `task_log.md` — Event Log (new file)

Append-only. Every meaningful status transition gets one row. Answers "when did things happen" without `git log`.

**File-level frontmatter:**

```yaml
---
title: Task Event Log
type: task-log
---
```

**Format: a Markdown table.**

```markdown
| date | id | event | note | artifact |
|------|----|-------|------|----------|
```

**Event vocabulary:**

| Event | When to append |
|-------|---------------|
| `added` | Task created (omit during initial migration — registry `added` field covers it) |
| `started` | Work begun on a previously-planned task (set `first_engaged` here) |
| `blocked` | Task hit a blocker |
| `unblocked` | Blocker cleared |
| `done` | Task closed (including resolved-N/A) |
| `abandoned` | Task dropped; won't be done |
| `slipped` | Task active in a sprint later than its started-in sprint |
| `renamed` | Task name changed substantively (note old name) |

**Rules:**
- One row per event. Multiple events in one session = multiple rows.
- Never edit a row. If a row was wrong, append a correcting row with an explanatory note.
- `note` is ~1 sentence — not the full narrative.
- `artifact` is one link or `—`.

### 4.3 `sprint_plan.md` — Gains inline links, stays narrative

No restructuring. Each task bullet gains a `[LL-NNN](tasks.md#LL-NNN)` link.

```markdown
- [LL-001](tasks.md#LL-001) **Market analysis:** Who is selling LED art? What are the price points...
```

The plan remains the human-readable narrative roadmap.

### 4.4 `sprint_log.md` — Gains inline links, stays narrative

Done entries gain the registry link immediately after the `[x]` checkbox:

```markdown
- [x] [LL-001](tasks.md#LL-001) **Market analysis — competitive landscape research completed (Apr 6).** Cross-platform analysis...
```

Blocked entries:

```markdown
- [LL-019](tasks.md#LL-019) Packaging QR code — **Blocked Apr 28** — needs final packaging design...
```

In-Progress entries:

```markdown
- [LL-022](tasks.md#LL-022) User interview script — contact list still pending
```

---

## 5. Example Tasks

### Example 1 — Completed task, straightforward

**In `tasks.md`:**

```markdown
<a id="LL-002"></a>
### [x] LL-002 — Sustainability audit

sprint: 2 | priority: high | deadline: 2026-04-11
added: 2026-03-30 | first_engaged: 2026-04-07 | last_engaged: 2026-04-09 | resolved: 2026-04-09
artifacts: [sustainability-audit.md](docs/sustainability-audit.md) · commit `93f686c`
dependencies: —

**Notes:** Full 7-stage cradle-to-grave lifecycle analysis. 6,368 smart-plug samples: idle 0.1W, rainbow 1.25W avg, full-white 2.8W (firmware-capped at ~29% WS2812B max). LED lifespan projected 14+ yr. Key hotspot: coated acrylic panels non-recyclable at EOL, PSU oversizing (25W brick at 11% load). Cardboard + PETG separability are strengths.
```

**In `task_log.md`:**

```
| 2026-04-09 | LL-002 | done | 7-stage lifecycle analysis complete; energy data from 6,368 samples | docs/sustainability-audit.md |
```

**In `sprint_log.md`:**

```markdown
- [x] [LL-002](tasks.md#LL-002) **Sustainability audit completed (Apr 9).** Full 7-stage cradle-to-grave...
```

---

### Example 2 — Resolved-N/A task with explanation in Notes

**In `tasks.md`:**

```markdown
<a id="LL-008"></a>
### [x] LL-008 — BOI Report filing with FinCEN

sprint: 1 | priority: medium | deadline: 2026-06-21
added: 2026-03-30 | first_engaged: 2026-04-22 | last_engaged: 2026-04-22 | resolved: 2026-04-22
artifacts: https://fincen.gov/boi
dependencies: —

**Notes:** Confirmed N/A, Apr 22 — FinCEN interim final rule exempts all US-formed domestic entities (formerly "domestic reporting companies") and their beneficial owners. Layered Logic LLC (WA, Mar 23 2026) qualifies. No filing required under the current rule. **Re-check before 2026-06-21** in case the rule is reversed. Marked `[x]` because the task is definitively resolved; no further action unless FinCEN reinstates the requirement.
```

**In `task_log.md`:**

```
| 2026-04-22 | LL-008 | done | Confirmed N/A — FinCEN interim final rule exempts all US-domestic entities | https://fincen.gov/boi |
```

**In `sprint_plan.md`:**

```markdown
- [LL-008](tasks.md#LL-008) ~~**BOI Report** filing with FinCEN~~ — **N/A / exempt** (confirmed Apr 22)...
```

---

### Example 3 — Parent task with children and grandchildren (App Demo Mini-Sprint)

**In `tasks.md`:**

```markdown
<a id="LL-042"></a>
### [x] LL-042 — App Demo Mini-Sprint

sprint: 5 | priority: high | deadline: 2026-05-05
added: 2026-04-28 | first_engaged: 2026-04-28 | last_engaged: 2026-05-01 | resolved: 2026-05-01
artifacts: [mini-sprint-app-demo.md](docs/mini-sprint-app-demo.md) · commit `5216633`
dependencies: LL-040 (firmware V1 architecture block)

**Notes:** 8-session push Apr 28–May 1 targeting faculty advisor demo on May 5. All 5 demo capabilities shipped: setup, on/off, color, pattern, factory reset. Demo hit 5 days early. Sessions 0–8 tracked as children LL-042-0 through LL-042-8. Closed during session cleanup once all children resolved.

---

<a id="LL-042-0"></a>
#### [x] LL-042-0 — Session 0: First flash on real silicon

parent: LL-042 | sprint: 5
added: 2026-04-28 | first_engaged: 2026-04-28 | last_engaged: 2026-04-28 | resolved: 2026-04-28
artifacts: sprint_log.md · commit `50035df`

**Notes:** Booted V1 firmware on XIAO ESP32-C3. Fixed two pre-flash-review-missed bugs (Unicore core-ID assert, state_bus led_count hardcoding). Boot log clean; button gestures and LED response confirmed. Toolchain end-to-end verified.

---

<a id="LL-042-2"></a>
#### [x] LL-042-2 — Session 2: Transport ↔ state-bus control surface

parent: LL-042 | sprint: 5
added: 2026-04-29 | first_engaged: 2026-04-29 | last_engaged: 2026-04-29 | resolved: 2026-04-29
artifacts: sprint_log.md · commit `37be211`

**Notes:** Three intra-session sub-milestones (2a/2b/2c) tracked as grandchildren. All closed same session. Final result: any WS client can read or write state, every other client receives the change within ~65ms.

---

<a id="LL-042-2-1"></a>
##### [x] LL-042-2-1 — Session 2a: get_state op live

parent: LL-042-2 | sprint: 5
added: 2026-04-29 | first_engaged: 2026-04-29 | last_engaged: 2026-04-29 | resolved: 2026-04-29
artifacts: sprint_log.md · commit `37be211`

**Notes:** Read-only op. `state_to_json()` helper serializes all 7 fields including `#RRGGBB` hex conversion. Verified end-to-end with NVS-persisted state.

---

<a id="LL-042-2-2"></a>
##### [x] LL-042-2-2 — Session 2b: set_state op live

parent: LL-042-2 | sprint: 5
added: 2026-04-29 | first_engaged: 2026-04-29 | last_engaged: 2026-04-29 | resolved: 2026-04-29
artifacts: sprint_log.md · commit `37be211`

**Notes:** Pivotal milestone — full write surface via WS. `parse_hex_color()` helper, 4 mutable fields, sensitive fields explicitly rejected. Async response design documented (set_state result is best-effort; broadcasts are authoritative). Visual confirmation: LEDs went bright green, then breathing/blue/50%.

---

<a id="LL-042-2-3"></a>
##### [x] LL-042-2-3 — Session 2c: Cross-client state-change broadcasts

parent: LL-042-2 | sprint: 5
added: 2026-04-29 | first_engaged: 2026-04-29 | last_engaged: 2026-04-29 | resolved: 2026-04-29
artifacts: sprint_log.md · commit `37be211`

**Notes:** `broadcast_state()` + `LL_EV_STATE_CHANGED` subscription. Two-watcher triangle verified at ~65ms. Multi-field set_state sends N broadcasts (all carry final state — coalescing is a future optimization). Closes Session 2 DoD.
```

*(Sessions 1, 3–8 follow the same pattern; intra-session sub-milestones get grandchild IDs as needed.)*

**In `sprint_plan.md`:**

```markdown
- [LL-042](tasks.md#LL-042) **App Demo Mini-Sprint (Apr 28 → May 5).** Engineering push for working end-to-end demos...
```

**In `sprint_log.md`:**

```markdown
- [x] [LL-042](tasks.md#LL-042) **App demo mini-sprint locked (Apr 28).** ...
- [x] [LL-042-0](tasks.md#LL-042-0) **Session 0 — V1 firmware first flash on real silicon (Apr 28).** ...
- [x] [LL-042-2-2](tasks.md#LL-042-2-2) **Session 2b mini-milestone: `set_state` op live (Apr 29).** ...
```

---

## 6. Migration Plan

Manual migration; no code or commits this session. Estimated: ~2–3 focused hours to backfill, including name cleanup.

### Phase 1: Create the two new files

1. Create `tasks.md` with file-level frontmatter and `next_id: LL-001`.
2. Create `task_log.md` with file-level frontmatter and the column header row.

### Phase 2: Backfill completed tasks (with name cleanup)

For each Done `[x]` entry in `sprint_log.md` (Week 1 → Week 5 in order):

1. Assign the next `LL-NNN`, increment `next_id`.
2. **Clean up the task name** before writing it. Convert action-narrative titles into stable noun phrases:
   - "Session 0 — V1 firmware first flash on real silicon (Apr 28)" → "Session 0: First flash on real silicon"
   - "Sustainability audit completed (Apr 9)" → "Sustainability audit"
   - "WA BLS registration filed (Apr 22)" → "WA Business Licensing Service registration"
   - Drop date suffixes (the date moves to `added`/`resolved`).
   - Drop verbs of completion ("completed", "finalized", "locked", "filed").
   - Prefer noun phrases that age well; the name should still make sense in 6 months when the task is being grepped from a memory or a portfolio doc.
3. Write the task block. Set `[x]`, fill in all four date fields from the log narrative.
4. For grouped entries that are sub-milestones of one parent (e.g., the 8 mini-sprint sessions), write the parent block first, then the children, then any grandchildren.
5. Append a `done` row to `task_log.md`.

### Phase 3: Backfill open and blocked tasks

For each task in `sprint_plan.md` without a Done entry:

1. Assign an `LL-NNN`, write a `[ ]` block in `tasks.md`.
2. Set `added` to when the task was first written down. `first_engaged`, `last_engaged`, `resolved` are blank or omitted as appropriate.
3. If the task is blocked, append a `blocked` row to `task_log.md`.
4. If the task slipped (e.g., PCB order), append a `slipped` event with a Notes line indicating the active sprint. The `sprint:` field stays as the started-in sprint.

### Phase 4: Add inline links to plan and log

- `sprint_plan.md`: prefix each task bullet with `[LL-NNN](tasks.md#LL-NNN)`.
- `sprint_log.md`: insert `[LL-NNN](tasks.md#LL-NNN)` after each `[x]`/`[ ]` checkbox on Done/Blocked/In-Progress lines.

Mostly grep-and-replace work once IDs are assigned.

### Phase 5: Update the downstream collector (separate session)

- Source of planned tasks: `[ ]` headings in `tasks.md`.
- Source of completions: `[x]` headings in `tasks.md` + `done` rows in `task_log.md`.
- Downstream composite: `github:task:LL-NNN`, `github:task:LL-NNN-M`, `github:task:LL-NNN-M-K`.
- Validation: collector warns if a `sprint_log` Done entry has an inline tag with no matching row in `task_log.md`.

### Edge cases during migration

| Case | Resolution |
|------|-----------|
| PCB order (Week 3) — no Done entry, no cancel | Write `[ ]` block. Append `slipped` event to task_log. Notes describes the deferral. |
| BOI Report — resolved N/A | `[x]` with explanation in Notes. Append `done` event. |
| Bug file `#1–#6` | Leave as-is in `post-mini-sprint-bugs.md`. If a bug becomes scheduled fix work, create an `LL-NNN` for the fix and link **bidirectionally**: the task's `artifacts:` references `post-mini-sprint-bugs.md#bug-N`; the bug's entry adds an `LL-NNN` reference in its body. Closing one does not auto-close the other — close each side explicitly. |
| Compliance checklist `[x]`/`[ ]` items in plan | Each gets an `LL-NNN`. Plan items gain inline links. |
| Tasks with "parked" status | Write `[ ]` block, append `abandoned` event. **Never** reopen the same ID later — if the track resumes, create a new `LL-NNN` and reference the old one in Notes. |
| A child task that grew into its own structure | Add grandchildren beneath it — never promote it to a top-level ID. |
| Same conceptual task appears in plan AND mini-sprint AND log entries | One `LL-NNN`. Parent if it has 3+ named deliverables, otherwise single task with detail in Notes. |

---

## 7. Remaining Open Questions

**Q1 — Parent-close workflow. (Resolved)**

Close parents `[x]` explicitly during session-cleanup when all children are `[x]`. Set `resolved:` to today, append `done` event noting "all children resolved." See §4.1.

**Q2 — Granularity of child IDs. (Resolved)**

Name everything — intra-session sub-milestones included. The hierarchy preserves history of how work actually unfolded.

**Q3 — When does a task deserve a new ID vs. an in-place evolution? (Resolved as workflow)**

No fixed rule. Claude flags ambiguous cases during documentation passes and asks. Guiding question: would a person familiar with the original task be confused to see the old name on the new work? If yes → new task. If the original name still fits and scope just grew → extend in place (children/grandchildren) without touching the parent ID.

---

## 8. Downstream Composite ID Format

| Scope | Composite |
|-------|-----------|
| Top-level task | `github:task:LL-001` |
| Child task | `github:task:LL-042-1` |
| Grandchild task | `github:task:LL-042-2-1` |

The old format (`github:sprint:5:session-1-foundation`) embedded both the week (unstable) and a slug (unstable). The new format drops both. The `task` scheme segment is fixed. The `LL-NNN[-M[-K]]` leaf is immutable.

If the collector needs to express where in the sprint plan a task lives, that comes from the registry's `sprint` field — structured data, not baked into the ID.

---

## Related

- [Sprint Plan](../sprint_plan.md) — the plan this spec augments
- [Sprint Log](../sprint_log.md) — the log this spec augments
- [Post-Mini-Sprint Bugs](post-mini-sprint-bugs.md) — existing local `#N` bug IDs, separate from task IDs
