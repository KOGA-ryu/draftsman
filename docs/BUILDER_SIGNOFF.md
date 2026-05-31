# Builder Sign-Off Log

Every Builder Dex that changes Draftsman must add one entry here before commit. This is a repo receipt, not a chat summary.

## Entry Template

```text
Date:
Builder/session key:
Work order:
Summary:
Files touched:
Settings/data changed:
Verification run:
Proof/screenshots:
Known gaps:
Follow-up owner:
```

## Sign-Offs

### 2026-05-31 - Text editor workbench feature foundation

Builder/session key: builder-dex-text-editor-workbench-foundation-2026-05-31

Work order: `docs/work_orders/text_editor_workbench_foundation.md`

Summary: Added the generic feature registry foundation, shipped a disabled `text_editor_workbench` feature entry, wired feature enablement into Settings persistence, and added parser/store coverage without porting the text editor UI renderer.

Files touched:
- `CMakeLists.txt`
- `README.md`
- `data/features.json`
- `docs/DRAFTSMAN_CODE_BRIEF.md`
- `docs/BUILDER_SIGNOFF.md`
- `docs/proof/blank_slate/03_shell_layout_settings_1280x800.png`
- `docs/schemas/features.schema.json`
- `scripts/validate_features.py`
- `src/app_state.h`
- `src/feature_registry.h`
- `src/feature_registry_parse.cpp`
- `src/feature_registry_store.cpp`
- `src/feature_settings_page.h`
- `src/feature_settings_page.cpp`
- `src/ledger_view.h`
- `src/main_window.cpp`
- `src/main_window.h`
- `src/main_window_loaders.cpp`
- `src/main_window_paths.cpp`
- `src/main_window_registry_editor.cpp`
- `src/main_window_state.cpp`
- `src/right_context_panel.cpp`
- `tests/state_backend_smoke.cpp`

Settings/data changed: Added `data/features.json`; added a Settings `Features` tab that toggles and persists each feature `enabled` flag.

Verification run:
- `python3 scripts/validate_shell_layout.py data/shell_layout.json`
- `python3 scripts/validate_ui_theme.py data/ui_theme.json`
- `python3 scripts/validate_features.py data/features.json`
- `cmake --build build`
- `ctest --test-dir build --output-on-failure`

Proof/screenshots: Refreshed `docs/proof/blank_slate/03_shell_layout_settings_1280x800.png` after adding the Settings `Features` tab.

Known gaps: Text editor workbench renderer, right-context editor panels, Rust action runner, fixture runner, and clipboard behavior remain intentionally unimplemented for the next work order.

Follow-up owner: Architect Dex to issue the renderer work order; Builder Dex to implement only the approved text editor UI renderer scope.

### 2026-05-31 - Codex architecture setup

Builder/session key: codex-local-draftsman-architecture-2026-05-31

Work order: create worker protocol, repo code brief, and text editor salvage seed.

Summary: Added the first process documents for Dex researcher/builder handoff and recorded the text editor workbench as the first salvage feature candidate.

Files touched:
- `docs/DRAFTSMAN_CODE_BRIEF.md`
- `docs/BUILDER_SIGNOFF.md`
- `docs/workflows/DEX_WORKER_PROTOCOL.md`
- `docs/features/text_editor_workbench_salvage.md`
- `docs/work_orders/text_editor_workbench_foundation.md`
- `README.md`

Settings/data changed: none.

Verification run: documentation-only change; no build required.

Proof/screenshots: not applicable.

Known gaps: `data/features.json` and a Settings `Features` tab are still planned, not implemented.

Follow-up owner: Architect Dex to turn the text editor salvage note into a feature catalog implementation plan; Builder Dex to implement only from approved work orders.
