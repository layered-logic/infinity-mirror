"""Convert Gemini JSON exports in this folder to markdown with Obsidian frontmatter.

Run from repo root or from external_chats/.  Writes {basename}.md next to each .json.
"""
import json
import re
from pathlib import Path

HERE = Path(__file__).parent


def slugify(s: str) -> str:
    s = re.sub(r"[^\w\s-]", "", s).strip().lower()
    return re.sub(r"[\s_]+", "-", s)


def render(msg: dict) -> str:
    role = "User" if msg["role"] == "Prompt" else "Gemini"
    body = msg["say"].strip()
    return f"### {role}\n\n{body}\n"


def convert(json_path: Path, frontmatter: dict, summary: str) -> Path:
    data = json.loads(json_path.read_text(encoding="utf-8"))
    meta = data["metadata"]
    messages = data["messages"]

    fm_lines = ["---"]
    for k, v in frontmatter.items():
        if isinstance(v, list):
            fm_lines.append(f"{k}: [{', '.join(v)}]")
        else:
            fm_lines.append(f"{k}: {v}")
    fm_lines.append("---")

    out = [
        "\n".join(fm_lines),
        "",
        f"# {meta['title']}",
        "",
        "## Summary",
        "",
        summary.strip(),
        "",
        f"- **Source:** Gemini ([chat link]({meta['link']}))",
        f"- **Exported:** {meta['dates']['exported']}",
        f"- **Message count:** {len(messages)}",
        "",
        "---",
        "",
        "## Conversation",
        "",
    ]
    for m in messages:
        out.append(render(m))

    md_path = json_path.with_suffix(".md")
    md_path.write_text("\n".join(out), encoding="utf-8")
    return md_path


if __name__ == "__main__":
    # Configure each conversion here — summaries written by hand after reading JSONs.
    configs = [
        {
            "json": "Gemini-Market_Research.json",
            "frontmatter": {
                "title": "Gemini chat — GitHub repo sharing + market research",
                "type": "external-chat-log",
                "source": "gemini",
                "exported": "2026-04-06",
                "status": "archived",
                "tags": "[external-chat, gemini, market-research, repo-context, week-1]",
            },
            "summary": (
                "Starter Gemini conversation (Apr 2 → Apr 6) used as the scaffolding "
                "thread for pointing an external model at the infinity-mirror GitHub "
                "repo and then extending into a full market-analysis pass. Covers "
                "workflows for sharing public/private repos with Gemini, early "
                "product-context briefing, and a multi-round competitive scan across "
                "Etsy / Amazon / Pinterest / commercial signage vendors. The final "
                "portion of this thread is what seeded "
                "[docs/Market Analysis Report.md](../docs/Market%20Analysis%20Report.md). "
                "This export (32 messages) supersedes the earlier Apr 2 export of the "
                "same thread."
            ),
        },
        {
            "json": "Gemini-Sharing GitHub Repos with Gemini.json",
            "frontmatter": {
                "title": "Gemini chat — GitHub repo sharing (early partial export)",
                "type": "external-chat-log",
                "source": "gemini",
                "exported": "2026-04-02",
                "status": "superseded",
                "supersededBy": "Gemini-Market_Research.md",
                "tags": "[external-chat, gemini, repo-context, week-1, superseded]",
            },
            "summary": (
                "Earlier partial export (18 messages, Apr 2) of the same Gemini thread "
                "captured in full in [Gemini-Market_Research.md](Gemini-Market_Research.md). "
                "Retained only for timestamp provenance of the initial repo-sharing "
                "workflow discussion. Prefer the superseding export for all content."
            ),
        },
        {
            "json": "Gemini-Small Business Bank Account Recommendation.json",
            "frontmatter": {
                "title": "Gemini chat — Small business bank account recommendation",
                "type": "external-chat-log",
                "source": "gemini",
                "exported": "2026-04-09",
                "status": "archived",
                "tags": "[external-chat, gemini, banking, business-foundation, week-2]",
            },
            "summary": (
                "Fiduciary-framed Gemini session (Apr 9) evaluating small-business "
                "bank account options for Layered Logic LLC (WA, solo-founder, low "
                "expected revenue). Informed the BlueVine / Bluevine vs. regional "
                "bank decision that closed out as business bank account application "
                "submitted Apr 9 and approved Apr 14. Referenced from "
                "[sprint_log.md](../sprint_log.md) Week 1 banking entry."
            ),
        },
    ]

    for cfg in configs:
        src = HERE / cfg["json"]
        if not src.exists():
            print(f"SKIP (missing): {src}")
            continue
        out = convert(src, cfg["frontmatter"], cfg["summary"])
        print(f"WROTE: {out.name}")
