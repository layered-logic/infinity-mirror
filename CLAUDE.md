# Independent Study

## Project Overview
Long-term independent study project. See memory files for evolving context.

## Conventions
- Keep documentation up to date as the project evolves.
- Use git commits to track meaningful milestones.
- Store persistent context in the Claude memory system rather than ad-hoc notes.

## Task Tracking
Tasks have stable `LL-NNN` IDs defined in `tasks.md`. `sprint_plan.md` and `sprint_log.md` reference them inline as `[LL-NNN](tasks.md#LL-NNN)`. Status transitions are appended to `task_log.md`. IDs are immutable — a child whose scope grows spawns grandchildren (`LL-NNN-M-K`), never gets promoted to a top-level ID. Bump the `last_engaged` field during session cleanup whenever a task receives work. Full schema in [docs/task-format-v2.md](docs/task-format-v2.md).
