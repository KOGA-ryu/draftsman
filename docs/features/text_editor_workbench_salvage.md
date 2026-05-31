# Text Editor Workbench Salvage

Status: reviewed seed feature

Source repo: `/Users/kogaryu/dev/features`

Source branch: `codex/theme-foundation-cleanup`

Source files inspected:

- `tools/features_binder/src/text_editor_workspace_blank.cpp`
- `tools/features_binder/src/text_editor_workspace_blank.h`
- `tools/features_binder/src/text_editor_workbench_page.cpp`
- `tools/features_binder/src/text_editor_workbench_page.h`
- `tools/features_binder/src/text_editor_ui.cpp`
- `tools/features_binder/src/text_editor_ui.h`
- `tools/features_binder/src/text_editor_workspace_state.h`
- `tools/features_binder/src/text_action_proof_model.h`
- `tools/features_binder/src/text_editor_rust_action_client.h`

## What To Keep

- A center workbench with a document editor surface.
- Right inspector panels for options, workspace state, last result, and receipts.
- Command palette pattern with searchable text actions.
- Action model split between visible action records, action inputs, action results, receipts, and fixtures.
- A controller/state object that owns workspace facts and emits state changes.
- `uiPath` and `componentState` properties for proof/debuggable UI surfaces.
- Read-only output/receipt preview panels.
- Fixture runner concept for proofing text actions.

## What Not To Copy Directly

- Do not copy the old monolithic `text_editor_workspace_blank.cpp` shape into Draftsman.
- Do not mix text-action execution, proof fixture logic, command palette rendering, and document widget construction in one file.
- Do not bring in Rust runner coupling until the UI feature has a clean local state contract.
- Do not hard-code panel lists in widget construction once `data/features.json` exists.

## Draftsman Rebuild Shape

Suggested modules:

- `src/features/text_editor/text_editor_feature.h`
- `src/features/text_editor/text_editor_state.*`
- `src/features/text_editor/text_editor_page.*`
- `src/features/text_editor/text_editor_context.*`
- `src/features/text_editor/text_action_model.*`
- `src/features/text_editor/text_action_runner.*`

Suggested settings:

- `enabled`
- `target_tabs`
- `show_right_context`
- `default_document_name`
- `default_language`
- `enable_command_palette`
- `enable_fixture_runner`
- `enable_clipboard_write`
- `max_preview_chars`

Suggested renderer surfaces:

- center: document editor, command palette, action result shelf
- right context: action options, workspace state, last result, receipt
- future left rail: documents, clipboard, drafts, fixtures

## Data Contract Seed

```json
{
  "feature_id": "text_editor_workbench",
  "label": "Text Editor",
  "status": "planned",
  "renderer_type": "text_editor_workbench",
  "enabled": false,
  "settings": {
    "target_tabs": ["Artifacts", "Research", "Reviews"],
    "show_right_context": true,
    "default_document_name": "scratch.txt",
    "default_language": "text",
    "enable_command_palette": true,
    "enable_fixture_runner": false,
    "enable_clipboard_write": false,
    "max_preview_chars": 12000
  }
}
```

## Required Before Implementation

- Add a first `data/features.json` contract.
- Add a `Features` Settings tab.
- Add a generic feature registry parser before adding text editor-specific UI routing.
- Add tests for feature parsing and disabled-by-default behavior.

## Architect Notes For Workers

Researcher Dex should return only the action model, proof model, and runner boundaries needed for a specific work order. Builder Dex should not browse the old repo broadly. If more source context is needed, request one narrow file list.
