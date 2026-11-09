# Repository work instructions

## Scope and starting point

This project is in investigation/planning. Read README.md, the current package in docs/roadmap.md, and only the source/documents needed for that package. Check repository status and preserve unrelated edits. Do not repeat whole-repository audits for a narrow task.

The roadmap is the single work queue. A listed package, proposed ADR or recommendation is not implementation approval. Complete the user's authorized scope without repeated confirmation; ask only when missing information materially affects scope, cost, privacy, compatibility or an irreversible action. For a broad request to continue, use the next package and its stated boundaries.

---

## Evidence and engineering rules

- Use docs/sources.md labels: rechecked source, prior review, reported, assumption/proposed, measured.
- Pin technical source claims to revision and file/function or manual issue/page. Report inference separately from observation. Earlier chat statements are context, not proof.
- Match radio identity and hardware/software revision before choosing wiring or serial parameters. Never substitute candidate defaults for installed settings.
- Do not infer radio acceptance from a TCP/serial write, nor infer actual acknowledgment bytes from constants in a driver.
- Preserve active channel, navigation feeds and local controls. Treat unknown/stale state explicitly. Keep binary radio traffic and diagnostics separate.
- Do not silently expand an analysis task into firmware implementation, flashing, hardware changes or aircraft work.
- Reuse existing architecture/tools; review file-level licenses before copying code. Do not select a repository license without the owner's decision.
- Do not incur paid API, CI or service charges or introduce infrastructure without authorization covering it.

---

## Small increments and verification

One question or behavior change per increment. Before changing logic, identify the affected requirement, expected outcome and suitable regression check. For broad later packages, choose a bounded subtask rather than implementing the whole plan.

For documentation-only work, check consistency, relative links/anchors, source support and whitespace; do not build applications or run device tests. For logic changes, run relevant checks and record versions/results. State what was not tested. Simulation and static analysis are not physical acceptance. Do not add tests that only mirror the implementation's assumptions.

Update the document that owns the information: baseline for requirements/inventory/risks; sources for evidence; architecture for decisions; roadmap for work status. Preserve useful prior findings, label superseded guidance and link replacements. Keep one concise verification summary per package, with detailed evidence only where useful.

Use minimal technical wording: short fragments and tables. Remove repetition, decision narratives, personal justifications and chronology of owner choices. Record current selections/status, constraints, open items and necessary evidence only. Keep source/test dates and revisions where needed for traceability. README: goal, scope, architecture and next step.

---

## Preservation and delivery

> [!IMPORTANT]
> This repository is public. Do not commit credentials, private firmware/configuration backups, identifying device logs or other private datasets. Use sanitized findings and synthetic fixtures.

Review the diff and stage only task files. Commit and push relevant, verified changes to the project remote at the end of each task. Never reset/overwrite unrelated changes or force-push to simplify publication. Confirm the remote base before writing and handle concurrent changes explicitly. Report failed commits or pushes and any unsaved work.

Finish with a concise outcome, meaningful verification, commit/publication status and unresolved items. Include a very brief emoji summary. Keep the existing English documentation language unless the user requests otherwise; communicate with the user in their chosen language.
