# Work Order: Text Editor Workbench Foundation

Status: planned

Owner: Builder Dex, after Architect Dex approves `data/features.json`.

## Goal

Add the foundation needed to enable a text editor workbench as a configurable Draftsman feature without copying the old workbench wholesale.

## Scope

1. Add `data/features.json` with a disabled `text_editor_workbench` feature entry.
2. Add a feature registry model/parser/store.
3. Add tests proving feature entries parse and unknown fields are ignored.
4. Add a Settings `Features` tab that can list feature records and toggle `enabled`.
5. Do not add the text editor UI yet.

## Non-Goals

- Do not port `text_editor_workspace_blank.cpp`.
- Do not wire the Rust action runner.
- Do not add clipboard-writing behavior.
- Do not add project-specific text editor content.

## Acceptance Criteria

- `data/features.json` validates or parses as loaded.
- `text_editor_workbench` is present and disabled by default.
- Settings exposes a feature list with enabled state.
- Saving Settings persists the feature enabled flag.
- Existing shell/theme/project settings continue to work.
- Builder adds a sign-off entry to `docs/BUILDER_SIGNOFF.md`.

## Verification

```sh
python3 scripts/validate_shell_layout.py data/shell_layout.json
python3 scripts/validate_ui_theme.py data/ui_theme.json
cmake --build build
ctest --test-dir build --output-on-failure
```

Refresh proof screenshots if the Settings UI changes visibly.
