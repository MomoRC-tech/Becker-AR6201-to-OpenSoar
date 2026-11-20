# Domain Docs

How the engineering skills should consume this repo's domain documentation when exploring the codebase. Layout: **single-context**.

## Before exploring, read these

- **`GLOSSARY.md`** at the repo root, if it exists.
- **`docs/design/architecture.md`**: this repo's decision record. Read the decisions that touch the area you're about to work in.
- **`docs/design/requirements.md`**: requirements, assumptions and risks.
- **`docs/project/roadmap.md`**: the current work package and its boundaries.

If `GLOSSARY.md` doesn't exist, **proceed silently**. Don't flag its absence; don't suggest creating it upfront. The `/domain-modeling` skill (reached via `/grill-with-docs` and `/improve-codebase-architecture`) creates it lazily when terms actually get resolved.

## Where decisions go

Each fact has one owning page (see the owning-page list in `AGENTS.md`). Record a resolved architecture decision in `docs/design/architecture.md`, not in a separate `docs/adr/` folder. A proposed decision is not approval; the owner decides.

## File structure

```
/
├── GLOSSARY.md                  ← created lazily by /domain-modeling
└── docs/
    ├── design/architecture.md   ← decisions
    ├── design/requirements.md   ← requirements, assumptions, risks
    └── project/roadmap.md       ← work queue
```

## Use the glossary's vocabulary

When your output names a domain concept (in an issue title, a refactor proposal, a hypothesis, a test name), use the term as defined in `GLOSSARY.md`. Don't drift to synonyms the glossary explicitly avoids.

If the concept you need isn't in the glossary yet, that's a signal: either you're inventing language the project doesn't use (reconsider) or there's a real gap (note it for `/domain-modeling`).

## Flag decision conflicts

If your output contradicts a decision in `docs/design/architecture.md`, surface it explicitly rather than silently overriding:

> _Contradicts the architecture decision on <topic>, but worth reopening because…_
