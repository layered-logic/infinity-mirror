---
name: user-flow
description: Author a Layered Logic user-flow diagram (Graphviz DOT inside a self-contained HTML preview). Use when the user types `/user-flow [name]` to draft a service-blueprint stage flow, journey map, or recovery flow. Follows the locked methodology in docs/user-flow-authoring.md — user-voice diamonds, pain→recovery→missing pattern, port-pinned orthogonal edges. Supports both interactive Q&A and batch-blob input.
---

# /user-flow — Layered Logic user-flow authoring

## Step 1 — Read the methodology + existing flows (always)

Before drafting anything, read these two files so the new flow stays consistent in tone, voice, and rendering:

1. [docs/user-flow-authoring.md](../../../docs/user-flow-authoring.md) — methodology, DOT template, HTML preview template, port conventions, the `rank=same` gotcha.
2. [docs/service-blueprint-flows.md](../../../docs/service-blueprint-flows.md) — Stage 1 example to match node-label tone (sentence length, casing, line-break placement in labels).

Also check [.preview/stage1.html](../../../.preview/stage1.html) to see what a rendered output looks like end-to-end.

## Step 2 — Determine input mode

The user invoked with one of these shapes:

- `/user-flow` → no args. Start interactive Q&A with "What's the flow name (kebab-case)?"
- `/user-flow stage-2-purchase` → arg is the flow name. Start Q&A with the next prompt (start-state).
- `/user-flow stage-2-purchase` followed by a multi-line blob → **batch mode**. Parse the blob; skip whatever's filled, fall back to Q&A only for missing pieces.

### Batch blob format

The blob is forgiving but the canonical shape is:

```
start: <Purple-oval label, typically "I encounter ..." or "I open ...">
end-happy: <Green-oval label, typically "I [act]" or "[Outcome]">
end-bounces: <semicolon-separated red-oval labels, optional>

gates:
- <user-voice question 1?> | no->bounce(<red-oval label>)
- <user-voice question 2?> | no->recovery(<recovery user-voice question?>) | missing=<Missing: build-this item>
- <user-voice question 3?> | no->recovery(<probe?>) | missing=<Missing: item>
- <user-voice question 4?> | no->bounce(<red-oval label>)
```

Parsing rules:
- One gate per line, prefixed with `-`.
- After `|`, segments are: `no->bounce(label)` OR `no->recovery(probe text)`; if `recovery`, expect a `| missing=` segment.
- If `end-bounces:` is omitted, the bounce labels referenced inside gates become the red ovals (deduplicated).
- If a gate omits `| no->...` entirely, ask interactively for that piece.

If parsing fails on any line, surface a concise error pointing at the line, then ask the user for the corrected line. Don't proceed silently with a partial parse.

## Step 3 — Q&A loop (only for missing fields)

Ask each in turn, one prompt per turn. Don't batch into a single message.

1. **Flow name** (if not in arg): kebab-case, used for the file name. Validate `/^[a-z][a-z0-9-]*$/`.
2. **Start state**: purple oval label, user-voice. Example: `I encounter a Layered Logic mirror`.
3. **Happy end**: green oval label. Example: `I click through or engage`.
4. **Bounces**: explicit red-oval labels the user already has in mind (semicolon-separated). Skip if none — bounces named inside gates work too.
5. **Gates loop** — for each gate, in one go ask:
   > "Gate N: what's the user-voice question? If the user says **No**, do they (a) bounce to a red oval, (b) hit a recovery probe?"
   - If (a): "What's the red-oval label?"
   - If (b): "What's the recovery probe (user-voice question)? And what's the `Missing:` build-this item if recovery also fails?"
   - Then: "Another gate, or are we done?"

**Push back if a gate forks on channel modality** (Etsy vs Instagram, mobile vs desktop, etc.) — per the methodology, those are the same encounter. Ask the user to compress to the core that channels share.

**Don't manufacture recovery probes for unrecoverable pains** — if the user says "wrong vibe" or "wrong time," it's fine for that gate's `No` to go straight to a red bounce without a recovery layer.

## Step 4 — Reorder pause (REQUIRED, never skip)

After all gates are collected, present them as a numbered list and ask:

> "Here's the order I have:
> 1. <question 1>
> 2. <question 2>
> ...
>
> For consumer-purchase flows the typical natural-cognitive cascade is **vibe → visualize → value → trust → timing**. Is this order right, or should I rearrange? (Reply with the new order as a list of numbers, e.g. `2 1 4 3 5`, or say 'keep' to ship as-is.)"

If the user gives a reorder, apply it before rendering. If they say keep, proceed.

This pause is mandated by [feedback_flow_natural_progression](~/.claude/projects/C--Users-bowhi-Desktop-Independent-Study/memory/feedback_flow_natural_progression.md) — never skip it, even in batch mode.

## Step 5 — Render

Generate the DOT block using the template in [docs/user-flow-authoring.md § Reusable DOT template](../../../docs/user-flow-authoring.md#reusable-dot-template).

Key invariants:
- `rankdir=LR; splines=ortho;`
- Start = purple oval, happy end = green oval, gates = orange diamonds, bounces = red ovals, recoveries = orange diamonds (same fill as gates), missings = yellow rounded rectangles.
- Edges:
  - Happy path (Yes): `Source:e -> Target:w [label="  Yes", weight=10]` — weight=10 on every one.
  - No drops: `Source:s -> Target:n [label="No"]`.
  - Recovery rejoin (Yes from a recovery probe back to next gate): `Recovery:e -> NextGate:w [label="  Yes"]` — splines=ortho handles the inverted-L automatically.
- **Per-column `rank=same` groups only** — one group per gate column containing the gate + its bounce/recovery/missing children. NEVER one big `rank=same` group for the whole happy path (that gotcha kills the LR layout).

Wrap the DOT in the HTML template from the methodology doc § "Reusable HTML preview template" — paste the DOT into the `dotSource` template literal. Set `<title>` and the `<h1>` to a human-readable stage title (e.g. "Stage 2 — Purchase").

Write the file to `.preview/<flow-name>.html`. Don't write a separate `.dot` file — the DOT lives inside the HTML.

## Step 6 — Hand off for review

Tell the user:

> "Drafted. Open `.preview/<flow-name>.html` in a browser to review. When you're happy, say **ship it** and I'll commit + push. If you want changes, describe them and I'll re-render."

Loop on changes until the user says ship it or equivalent.

## Step 7 — Commit + push

Once approved, commit with:

```
flow: <flow-name> — <one-line summary of what the flow models>

<2-3 sentence description of the stage and the key gates>
```

Then push to whatever remote/branch is current. If the user has uncommitted unrelated changes, only stage `.preview/<flow-name>.html` (don't `git add -A`).

## Edge cases + reminders

- **Channel-fork pushback** (Step 3) — if the user describes gates that differ only by channel, push back before drafting.
- **No recovery probe is fine** — not every pain needs one. Don't pad.
- **Stage-1 was Discover** — if the user is authoring Stage 2+, the start-state label probably begins where Stage 1's happy-end label ended. Suggest this if not provided.
- **Yellow `Missing:` items** are concrete build-this backlog items, not vague handwaving. Push back on labels like "Better UX" — ask for the specific asset/copy/signal that would have saved the user.
- **The HTML template has a CDN script tag** pointing at `@viz-js/viz@3`. Don't change this. The template is locked.
- **Don't add a Mermaid mirror block** to `service-blueprint-flows.md` — that step was deliberately deferred for now (per the session that authored this skill). If the user later asks for it as a portability artifact, that's a separate task.

## Self-check before rendering

Before writing the HTML file, scan the DOT mentally:

- [ ] Every diamond label uses `I` / `me` / `my`.
- [ ] No `rank=same` group contains more than one gate from the happy path.
- [ ] Every happy-path edge has `weight=10`.
- [ ] Every `Yes` edge uses `:e -> :w`. Every `No` edge uses `:s -> :n`.
- [ ] Bounces are red ovals; recoveries are orange diamonds; missings are yellow rounded rectangles.
- [ ] No gate forks on channel modality.
- [ ] The reorder pause happened.

If any box is unchecked, fix before writing.
