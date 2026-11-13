# Repository work instructions

## Scope and starting point

This project is in investigation/planning. Read README.md, the current package in docs/project/roadmap.md, and only the source/documents needed for that package. Check repository status and preserve unrelated edits.

**Current focus: hardware selection and wiring.** No software, firmware or driver work until the hardware design is finished. Ask the owner for each hardware decision (part choice, circuit option, wiring choice) separately; present the options with a recommendation, then record the answer.

The roadmap is the single work queue. A listed package, proposed ADR or recommendation is not implementation approval. For a broad request to continue, use the next package and its stated boundaries.

---

## Evidence and engineering rules

- Keep evidence detail in docs/research/sources.md: revision, page and what was checked. Other pages cite a source ID (for example S10) where it matters, without repeating evidence labels or caveats.
- Base technical claims on a source; say plainly when something is an estimate or inference.
- Match radio identity and hardware/software revision before choosing wiring or serial parameters. Never substitute candidate defaults for installed settings.
- Do not silently expand an analysis task into firmware implementation, flashing, hardware changes or aircraft work.
- Reuse existing architecture/tools; review file-level licenses before copying code. Do not select a repository license without the owner's decision.

---

## Small increments and verification

One question or behavior change per increment. Before changing logic, identify the affected requirement, expected outcome and suitable regression check. For broad later packages, choose a bounded subtask rather than implementing the whole plan.

For documentation-only work, check consistency, relative links/anchors, source support and whitespace; do not build applications or run device tests. State what was not tested. Simulation and static analysis are not physical acceptance.

Update the document that owns the information: requirements for requirements/assumptions/risks; existing-installation for the installed inventory; open-items for missing evidence and owners; sources for evidence; architecture for decisions; wiring for signal pinning and bench bring-up; power for supply, grounding and current assumptions; modules for parts; housing for mechanical design, part sizes and connector mounting; roadmap for work status. Dated reviews in docs/research/reviews are frozen records. Keep documentation current. Remove superseded selections, guidance, references and assets once they no longer support the current project state. Use Git history for previous versions. Keep one concise verification summary per package, with detailed evidence only where useful.

## Writing style

Write for a person building the bridge, not for an auditor.

- Prefer bullet lists and tables over paragraphs. Short fragments are fine when they stay understandable. Say what to do, then why if it is not obvious.
- Do not append "unverified", "remains open" or "confirm…" to individual cells or sentences; the page's status box covers it.
- Open questions live only in docs/project/open-items.md; other pages link to it.
- Use warning boxes only for things that can damage hardware or affect flight safety, one box per hazard.
- Each fact has one owning page; elsewhere, link to it instead of restating it.
- No decision narratives or chronology of owner choices; record the current selection.
- README: goal, scope, status and where to start.

## Page layout

Pages must be easy to scan for a human reader. Every page in docs/ (except the frozen reviews) follows this pattern:

| Element | Form |
| --- | --- |
| Title | `# <icon> Title`, preceded by `<a id="top"></a>` |
| Breadcrumb | `🏠 [Overview](…) › <category icon> Category › **Page**` |
| Purpose | One sentence on what the page is for |
| Status | One `> [!NOTE]` box with **Status:** — the only status statement on the page |
| Contents | One line of key-style buttons for the H2 sections when a page has three or more: `📑 **On this page:** <kbd>[1 · <icon> Title](#slug)</kbd> …` |
| H2 sections | `## 1. <icon> Title`, each preceded by a stable `<a id="short-slug"></a>`; link to that id, never to the generated heading anchor |
| H3 sections | `### 1.1 Title`, no icon |
| Separators | `---` between H2 sections |
| Alerts | `[!TIP]` for current selections/decisions, `[!WARNING]` for hazards only (one per hazard) |
| Collapsibles | Only for large reference images. Summary: `<summary><b>🖼️ Show …</b> (click to expand)</summary>` |
| Footer | `<kbd>[⬆️ Back to top](#top)</kbd> <kbd>[🏠 Overview](…)</kbd>` |

Category icons: 🔧 Hardware · 🧩 Design · 📋 Project · 🔬 Research.

### Visual elements

Pick the element that makes the content easiest to scan, not plain paragraphs by default.

| Element | Use it for |
| --- | --- |
| Alert boxes | `[!NOTE]` page status, `[!TIP]` current selection, `[!IMPORTANT]` a step the builder must not miss, `[!WARNING]` hazards |
| Tables | Pin maps, parts, comparisons, option lists, status overviews |
| Task lists (`- [ ]`) | Checklists the builder works through, such as bench checks |
| Mermaid diagrams | Signal flow, power tree, sequences, state |
| Code blocks | Commands, configuration, serial frames; always with a language tag |
| Images and badges | Wiring diagrams, pin views, photos; local SVG badges such as the pin labels. Hardware diagrams (supply, ground loops, signal wiring) are generated: edit `tools/diagrams.py`, run `python tools/diagrams.py`, check the SVGs in a browser, commit script and SVGs together |
| Math (` ```math ` blocks, `$`…`$` inline) | Calculations such as dividers, currents and fuse sizing; use ` ```math ` blocks, since `$$` blocks lose backslashes on GitHub; write units as `\text{ V}`, not with `\,` |
| Blockquotes | Short verbatim quotes from a source, with its source ID |

---

## Preservation and delivery

> [!IMPORTANT]
> This repository is public. Do not commit credentials, private firmware/configuration backups, identifying device logs or other private datasets. Use sanitized findings and synthetic fixtures.

Commit and push relevant, verified changes to `main` on the project remote at the end of each task, following these steps:

1. **Review:** `git status --short`, `git diff --stat`, and read `git diff` for the task's text files. For binary files (CAD, images), check only that they belong to the task.
2. **Stop and report, without committing, if:** a changed file may not belong to the task (leave it unstaged), the diff contains private data (see the box above), or `git diff --check` reports whitespace errors.
3. **Stage** the task's paths only, never `git add -A` or `git add .`.
4. **Commit:** subject of at most 72 characters in the imperative, a blank line, then what changed and why, and what was verified. English. Add the co-author line your tool requires.
5. **Sync:** `git fetch origin`. If `origin/main` has new commits, check the files they change (`git diff --stat HEAD...origin/main`). No overlap with the task's files: `git rebase origin/main`. Overlap or a rebase conflict: abort and report.
6. **Push:** `git push origin HEAD:main`, as a fast-forward.
7. **Report** the commit hash, the files committed, the push result, and anything left unstaged or unsaved.

Never force-push, reset, check out over or clean away changes, amend pushed commits, skip hooks (`--no-verify`), or use a bare `git stash` / `git stash pop` (the stash is shared with other worktrees).

Finish with a concise outcome, meaningful verification, commit/publication status and unresolved items. Include a very brief emoji summary. Keep the existing English documentation language unless the user requests otherwise; communicate with the user in their chosen language.
