# Independent Study

## Project Overview
Long-term independent study project. See memory files for evolving context.

## Business Documents
Business material lives in the separate `..\Business` workspace (has its own CLAUDE.md), not in this repo. This repo is public. Never write legal, financial, or customer-account material here. The former `Business/` folder was migrated out on 2026-08-31 and holds only a pointer.

## Conventions
- Keep documentation up to date as the project evolves.
- Use git commits to track meaningful milestones.
- Store persistent context in the Claude memory system rather than ad-hoc notes.

## Task Tracking
Tasks have stable `LL-NNN` IDs defined in `tasks.md`. `sprint_plan.md` and `sprint_log.md` reference them inline as `[LL-NNN](tasks.md#LL-NNN)`. Status transitions are appended to `task_log.md`. IDs are immutable — a child whose scope grows spawns grandchildren (`LL-NNN-M-K`), never gets promoted to a top-level ID. Bump the `last_engaged` field during session cleanup whenever a task receives work. Full schema in [docs/task-format-v2.md](docs/task-format-v2.md).

## Firmware & Hardware
- The development mirror is at **`192.168.5.229`**. Use that IP directly for OTA, debugging, and remote control — do **not** subnet-scan, ARP, or mDNS to "find" it.
- The mirror sits on a different subnet from the dev PC by design. This is expected and fine: cross-subnet routing/firewall rules allow HTTP both ways across the UniFi VLANs (Ethernet and Wi-Fi alike). The subnet mismatch is never the problem — don't investigate it.
- Firmware work is PowerShell-only (ESP-IDF). OTA flashing procedure: [docs/ota-dev-runbook.md](docs/ota-dev-runbook.md).
