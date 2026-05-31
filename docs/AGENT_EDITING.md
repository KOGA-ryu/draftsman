# Agent Editing Contract

Draftsman is meant to be filled by agents without changing C++.

Use `data/shell_layout.json` for the visible blank-shell structure and light repo knowledge. Use `data/projects.json` for project registry facts. Keep generated or experimental source material outside the final blank unless the human asks to promote it.

Use `data/ui_theme.json` for the global four-color UI palette:

```json
{
  "base": "#141719",
  "surface": "#24282c",
  "accent": "#7fa8b8",
  "text": "#e9eee9"
}
```

Do not hard-code project colors into C++ or stylesheet files. Change the four theme colors and let the stylesheet derive the rest.

## Shell Layout

Agents may edit these fields in `data/shell_layout.json`:

- `app_title`: window/app label.
- `left_rail.sections`: project groups and workflow buckets.
- `tabs`: top binder tabs.
- `panels`: center content panels.
- `inspector.panels`: right-context panels.

Panel records have this shape:

```json
{
  "id": "repo_purpose",
  "label": "Repo Purpose",
  "tab": "Overview",
  "lines": [
    "Human-readable repo fact.",
    "Another short review line."
  ],
  "min_height": 96,
  "subtle": true,
  "enabled": true
}
```

`tab` must match a visible tab label, or `"*"` for inspector panels that should show on every tab. `lines` should stay short and reviewable. A string value is accepted as a convenience and split on newlines, but agents should write arrays.

## Agent Workflow

1. Inspect the source repo and produce a repo knowledge packet.
2. Write selected facts into a copy of `data/shell_layout.json`.
3. Keep the UI blank where facts are uncertain instead of inventing content.
4. Run `python3 scripts/validate_shell_layout.py data/shell_layout.json`.
5. Run `python3 scripts/validate_ui_theme.py data/ui_theme.json` if the palette changed.
6. Run `cmake --build build` and `ctest --test-dir build --output-on-failure`.

## Boundaries

Do not edit Qt source for project-specific content. Add, remove, rename, disable, or reorder tabs and panels through JSON. Only change C++ when the reusable shell needs a new capability.

Examples live in `docs/examples/repo_knowledge_shell_layout.json`.
