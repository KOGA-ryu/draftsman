# Draftsman Code Brief

Draftsman is a C++/Qt 6 project-binder shell. The app should stay reusable: project-specific structure, feature selection, theme choices, and repo facts belong in data/config files unless the reusable shell needs a new renderer or capability.

## Current Source Shape

- `src/main_window.*`: top-level app shell, state loading, settings save paths, and view refresh routing.
- `src/ledger_view.h`: center surface tab host and Settings tab host.
- `src/project_rail.*`: left project/workflow rail.
- `src/right_context_panel.*`: right inspector shell and routing.
- `src/shell_layout.*`: human/agent editable layout contract for rail sections, tabs, center panels, right inspector panels, and panel lines.
- `src/project_registry.*`: editable project/worker registry contract.
- `src/ui_theme.*`: theme mode, palette, UI/code font settings, and theme persistence.
- `src/ui_rules.*` and `src/ui_style_*`: derived visual tokens and Qt stylesheet chunks.
- `scripts/validate_shell_layout.py`: validates shell layout JSON.
- `scripts/validate_ui_theme.py`: validates theme JSON.

## Data Files

- `data/shell_layout.json`: blank shell tabs, panels, inspector panels, and review lines.
- `data/projects.json`: project and worker registry.
- `data/ui_theme.json`: theme mode, colors, UI font, code font, and font sizes.
- Future: `data/features.json` should hold feature catalog entries and project enablement.

## Builder Rules

- Do not hard-code project-specific feature content in C++.
- Add reusable renderers in C++ only when config cannot express the behavior.
- Keep all UI colors and fonts behind `ui_theme` and `ui_rules`.
- Keep feature salvage notes under `docs/features/`.
- Keep builder-ready implementation plans under `docs/work_orders/`.
- Every code-producing Builder Dex session must update `docs/BUILDER_SIGNOFF.md` before commit.
- Every sign-off entry must include session identity, files touched, verification, and remaining risk.

## Verification Baseline

Run these before sign-off when code or config changes:

```sh
python3 scripts/validate_shell_layout.py data/shell_layout.json
python3 scripts/validate_ui_theme.py data/ui_theme.json
cmake --build build
ctest --test-dir build --output-on-failure
```

For visible UI work, refresh proof screenshots under `docs/proof/blank_slate/`.
