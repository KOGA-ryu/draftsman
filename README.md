# Draftsman

Draftsman is a C++/Qt blank project-binder shell. It starts as a polished
layout skeleton for organizing projects, artifacts, reviews, notes, and future
project-specific workbenches.

The current app intentionally ships with an empty registry:

```json
{
  "projects": [],
  "workers": []
}
```

The first screen keeps only the reusable UI structure:

- top chrome controls
- left project rail
- center binder tabs and panels
- detail lens rail
- right inspector panels
- Settings entry point

The shell is intentionally easy for agents to customize without touching C++.
Repo knowledge can be written into `data/shell_layout.json` as tabs, panels,
and short review lines, then validated before the app is run.

The low-light palette and font controls live in `data/ui_theme.json`:
`theme_mode`, `base`, `surface`, `accent`, `text`, `ui_font`, `code_font`,
`ui_font_size`, and `code_font_size`. The app derives the rest of the UI
network from those values.

```sh
python3 scripts/validate_shell_layout.py data/shell_layout.json
python3 scripts/validate_ui_theme.py data/ui_theme.json
```

Agent-facing editing notes live in:

```text
docs/AGENT_EDITING.md
docs/examples/repo_knowledge_shell_layout.json
docs/schemas/shell_layout.schema.json
docs/schemas/ui_theme.schema.json
```

## Build

```sh
cmake -S . -B build
cmake --build build
```

## Test

```sh
ctest --test-dir build --output-on-failure
```

## Run

```sh
./build/draftsman
```

## Proof

Reference blank-shell screenshots live in:

```text
docs/proof/blank_slate/
```
