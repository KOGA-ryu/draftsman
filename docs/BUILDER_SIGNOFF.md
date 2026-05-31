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

### 2026-05-31 - Rabbit-hole review workspace fixture renderer

Builder/session key: builder-dex-rabbit-hole-review-workspace-2026-05-31

Work order: Rabbit-hole review gate UI pattern, delegated from source thread `019e7d7e-69e9-72c3-95f1-7801532952ef`

Summary: Added the disabled `rabbit_hole_review_workspace` feature entry, generic review fixture/model parser, enabled-tab routing helper, center rabbit-hole review workspace, and right-context review summary. The renderer uses generic subject/route/component/comment/status records and does not port flower/SVG-specific concepts into C++ model names.

Files touched:
- `CMakeLists.txt`
- `data/features.json`
- `docs/BUILDER_SIGNOFF.md`
- `docs/examples/rabbit_hole_review_fixture.json`
- `docs/proof/blank_slate/04_features_settings_1280x800.png`
- `docs/proof/blank_slate/06_rabbit_hole_review_enabled_1280x800.png`
- `docs/proof/blank_slate/07_rabbit_hole_review_drilldown_1280x800.png`
- `src/rabbit_hole_review_context.h`
- `src/rabbit_hole_review_context.cpp`
- `src/rabbit_hole_review_model.h`
- `src/rabbit_hole_review_model.cpp`
- `src/rabbit_hole_review_page.h`
- `src/rabbit_hole_review_page.cpp`
- `src/rabbit_hole_review_state.h`
- `src/rabbit_hole_review_state.cpp`
- `src/repo_profile_pages.cpp`
- `src/right_context_panel.cpp`
- `tests/state_backend_smoke.cpp`

Settings/data changed: Added `rabbit_hole_review_workspace` to `data/features.json` with `enabled: false`; added `docs/examples/rabbit_hole_review_fixture.json`.

Verification run:
- `python3 scripts/validate_shell_layout.py data/shell_layout.json`
- `python3 scripts/validate_ui_theme.py data/ui_theme.json`
- `python3 scripts/validate_features.py data/features.json`
- `cmake --build build`
- `ctest --test-dir build --output-on-failure`
- Confirmed `text_editor_workbench enabled= False` and `rabbit_hole_review_workspace enabled= False`.
- Ran `rg` over `src`, `data/features.json`, and the fixture for prototype-specific terms; no matches.

Proof/screenshots:
- Refreshed default disabled state `docs/proof/blank_slate/01_blank_repo_binder_1280x800.png` with no content diff.
- Refreshed `docs/proof/blank_slate/04_features_settings_1280x800.png` showing both disabled feature entries.
- Added `docs/proof/blank_slate/06_rabbit_hole_review_enabled_1280x800.png` using temporary proof config with only the rabbit-hole feature enabled.
- Added `docs/proof/blank_slate/07_rabbit_hole_review_drilldown_1280x800.png` using a temporary proof fixture ordered to start on a deeper route for breadcrumb/comment proof.

Known gaps: No persistence writes, comment saving, exports, SVG parsing, visual asset internals, left sub navigation, or production approval workflow were implemented.

Follow-up owner: Architect Dex to issue focused work orders for persistence/export contracts, left-sub routing, and any future preview adapter.

### 2026-05-31 - Text editor workbench renderer

Builder/session key: builder-dex-text-editor-workbench-renderer-2026-05-31

Work order: Text Editor Workbench Renderer, delegated from source thread `019e7d7e-69e9-72c3-95f1-7801532952ef`

Summary: Added a small enabled-feature routing layer and a first reusable `text_editor_workbench` renderer. Enabled target tabs render a local `QPlainTextEdit` workbench with document metadata, status facts, output/receipt shelves, and text-editor right-context panels. The shipped `data/features.json` entry remains disabled by default.

Files touched:
- `CMakeLists.txt`
- `docs/BUILDER_SIGNOFF.md`
- `docs/proof/blank_slate/01_blank_repo_binder_1280x800.png`
- `docs/proof/blank_slate/04_features_settings_1280x800.png`
- `docs/proof/blank_slate/05_text_editor_workbench_enabled_1280x800.png`
- `src/feature_routing.h`
- `src/feature_routing.cpp`
- `src/ledger_view.h`
- `src/main.cpp`
- `src/main_window.h`
- `src/main_window.cpp`
- `src/repo_profile_pages.cpp`
- `src/right_context_panel.cpp`
- `src/text_editor_workbench_state.h`
- `src/text_editor_workbench_state.cpp`
- `src/text_editor_workbench_page.h`
- `src/text_editor_workbench_page.cpp`
- `src/text_editor_workbench_context.h`
- `src/text_editor_workbench_context.cpp`
- `tests/state_backend_smoke.cpp`

Settings/data changed: `data/features.json` remains disabled by default. Added a proof-only `--settings-tab` launch option so Settings tab screenshots can be captured without manual UI interaction.

Verification run:
- `python3 scripts/validate_shell_layout.py data/shell_layout.json`
- `python3 scripts/validate_ui_theme.py data/ui_theme.json`
- `python3 scripts/validate_features.py data/features.json`
- `cmake --build build`
- `ctest --test-dir build --output-on-failure`
- Confirmed `data/features.json` still reports `text_editor_workbench enabled= False`.

Proof/screenshots:
- Refreshed `docs/proof/blank_slate/01_blank_repo_binder_1280x800.png` for the default disabled feature state.
- Added `docs/proof/blank_slate/04_features_settings_1280x800.png` for the Settings `Features` tab.
- Added `docs/proof/blank_slate/05_text_editor_workbench_enabled_1280x800.png` using a temporary `/tmp/draftsman_text_editor_proof/data/features.json` with only `text_editor_workbench.enabled` set to true.

Known gaps: Rust text action runner, command palette, fixture runner, clipboard writing, document persistence, and project-specific document lists remain intentionally unimplemented.

Follow-up owner: Architect Dex to issue focused work orders for command actions, persistence, and runner integration.

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
