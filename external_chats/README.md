---
title: External Chats — Archive
type: asset-folder
status: active
tags: [external-chat, archive]
---

# External Chats — Archive

Exports of AI conversations (Gemini, Claude, etc.) that produced artifacts now living in `docs/` or `Assembly_docs/`. Kept for provenance and for future sessions that need to replay the reasoning.

## Convention

Each chat is stored as both the raw export (`.json` for Gemini exporter, `.md` for Claude sessions) **and** a companion `.md` file with:
- YAML frontmatter (`type: external-chat-log`, `source:`, `exported:`, `status:`, `tags:`)
- A `## Summary` section linking to the artifacts the conversation produced
- The full conversation body rendered as `### User` / `### Gemini` (or `### Claude`) headers

`status: superseded` on a chat means a later export of the same thread exists — prefer the superseding file.

## Files

| Chat | Date | Source | Produced |
|---|---|---|---|
| [Gemini-Market_Research.md](Gemini-Market_Research.md) | Apr 6 | Gemini | Stakeholder map + Market Analysis Report |
| [Gemini-Sharing GitHub Repos with Gemini.md](Gemini-Sharing%20GitHub%20Repos%20with%20Gemini.md) *(superseded)* | Apr 2 | Gemini | — earlier partial export of the same thread |
| [Gemini-Small Business Bank Account Recommendation.md](Gemini-Small%20Business%20Bank%20Account%20Recommendation.md) | Apr 9 | Gemini | Mercury/Chase/Relay evaluation → bank application |
| [LED_Mirror_Project_Brief.md](LED_Mirror_Project_Brief.md) | Apr 14 | Claude | Factory operations plan + Assembly_docs/ artifacts |

## Regenerating markdown from JSON

`convert_chats.py` in this folder converts Gemini JSON exports to markdown. Summaries are configured by hand inside the script (they require reading the thread) — add a new entry to the `configs` list before running it on a new export.

```bash
cd external_chats && python convert_chats.py
```

## Related

- [Sprint Log](../sprint_log.md) · [Sprint Plan](../sprint_plan.md)
- [docs/stakeholder-map.md](../docs/stakeholder-map.md) · [docs/Market Analysis Report.md](../docs/Market%20Analysis%20Report.md)
