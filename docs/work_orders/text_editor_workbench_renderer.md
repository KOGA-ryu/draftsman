# Work Order: Text Editor Workbench Renderer

Status: planned

Depends on: `docs/work_orders/text_editor_workbench_foundation.md`

Feature: `text_editor_workbench`

## Goal

Add a first reusable text editor workbench renderer that can be enabled from `data/features.json` and displayed in Draftsman without porting the old monolithic editor source.

## Required Reading

- `docs/DRAFTSMAN_CODE_BRIEF.md`
- `docs/BUILDER_SIGNOFF.md`
- `docs/workflows/DEX_WORKER_PROTOCOL.md`
- `docs/features/text_editor_workbench_salvage.md`
- `data/features.json`

## Scope

1. Add a small feature routing layer that can detect enabled feature records by `renderer_type`.
2. When `text_editor_workbench` is enabled, show a Text Editor page on configured target tabs.
3. Implement a minimal center renderer:
   - document tab/title
   - `QPlainTextEdit` editor surface
   - read-only output/receipt shelf
   - status line with line/character/cursor facts
4. Implement minimal right-context panels:
   - action options placeholder
   - workspace state
   - last result
   - receipt
5. Use Draftsman theme/font tokens. Do not introduce feature-local colors.
6. Keep the feature disabled by default.
7. Add tests for enabled/disabled routing where practical.

## Non-Goals

- Do not wire the Rust text action runner yet.
- Do not add clipboard-writing behavior.
- Do not add the command palette yet unless Architect Dex issues a separate work order.
- Do not add fixture runner behavior yet.
- Do not copy `text_editor_workspace_blank.cpp` wholesale.
- Do not add per-project C++ pages.

## Suggested Modules

- `src/text_editor_workbench_state.*`
- `src/text_editor_workbench_page.*`
- `src/text_editor_workbench_context.*`

If the implementation needs a generic feature renderer hook, keep it small and separate from text editor-specific code.

## Settings Contract

Use the existing feature settings object:

```json
{
  "target_tabs": ["Artifacts", "Research", "Reviews"],
  "show_right_context": true,
  "default_document_name": "scratch.txt",
  "default_language": "text",
  "enable_command_palette": true,
  "enable_fixture_runner": false,
  "enable_clipboard_write": false,
  "max_preview_chars": 12000
}
```

For this work order, only `target_tabs`, `show_right_context`, `default_document_name`, `default_language`, and `max_preview_chars` need to affect behavior. Leave the other flags present but inactive.

## Acceptance Criteria

- With `text_editor_workbench.enabled = false`, Draftsman behaves exactly like the current blank shell.
- With `text_editor_workbench.enabled = true`, the configured tabs render the text editor workbench instead of blank panels.
- Right context shows text-editor context panels only when the feature is active and `show_right_context` is true.
- The editor uses the configured UI theme, UI font, code font, and code font size.
- No clipboard writes occur.
- No external runner process is required.
- Builder Dex adds a sign-off entry to `docs/BUILDER_SIGNOFF.md`.

## Verification

```sh
python3 scripts/validate_shell_layout.py data/shell_layout.json
python3 scripts/validate_ui_theme.py data/ui_theme.json
python3 scripts/validate_features.py data/features.json
cmake --build build
ctest --test-dir build --output-on-failure
```

Refresh proof screenshots for:

- default disabled feature state
- Settings `Features` tab
- enabled text editor workbench state, using a temporary proof config if needed
