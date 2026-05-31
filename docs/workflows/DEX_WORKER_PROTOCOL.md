# Dex Worker Protocol

Draftsman workers should keep messy repo exploration out of the main implementation context. Researcher Dex gathers compact facts. Architect Dex turns those facts into feature specs and work orders. Builder Dex implements only approved work orders and signs the repo log.

## Roles

### Researcher Dex

Researcher Dex inspects messy source repos and returns a compact packet. It should not ask Builder Dex to copy old code directly.

Required packet:

```json
{
  "source_repo": "/absolute/source/repo",
  "source_branch": "branch-or-commit",
  "feature_name": "Human label",
  "feature_summary": "What the feature does.",
  "useful_files": [
    "path/to/file.cpp"
  ],
  "ui_surfaces": [
    "center workbench",
    "right inspector"
  ],
  "data_contracts": [
    "state records, JSON shapes, action request/response shapes"
  ],
  "dependencies": [
    "Qt widgets, Rust runner, local scripts"
  ],
  "good_ideas": [
    "Reusable behavior worth rebuilding"
  ],
  "bad_code_or_risk": [
    "Coupling, hard-coded state, unclear ownership"
  ],
  "recommended_rebuild": "rewrite | adapt | ignore",
  "proof_refs": [
    "screenshots or tests if available"
  ]
}
```

### Architect Dex

Architect Dex reads researcher packets and writes:

- `docs/features/<feature_id>_salvage.md`
- `docs/work_orders/<feature_id>_foundation.md`
- updates to code brief or schemas when needed

Architect Dex decides:

- what is reusable
- what must be rebuilt
- which settings the human controls
- which data contract the feature consumes
- which renderer type should display it
- what tests/proof Builder Dex must produce

### Builder Dex

Builder Dex implements from work orders only.

Builder Dex must:

- read `docs/DRAFTSMAN_CODE_BRIEF.md`
- read the relevant work order
- keep changes scoped
- update validators/schema when config changes
- refresh proof screenshots for visible UI changes
- add an entry to `docs/BUILDER_SIGNOFF.md`
- never omit verification details

## Promotion Flow

1. `candidate`: Researcher packet exists.
2. `reviewed`: Architect Dex has critiqued it.
3. `planned`: Work order exists.
4. `implemented`: Builder Dex landed the reusable shell capability.
5. `enabled`: Human or project settings turn it on.
6. `retired`: Feature is intentionally no longer used.

## Feature Storage Direction

Draftsman should eventually use `data/features.json` to store:

- feature catalog entries
- feature status
- project enablement
- target surfaces
- renderer type
- feature-specific settings
- source/review notes

Until `data/features.json` exists, feature specs live in `docs/features/` and builder tasks live in `docs/work_orders/`.
