# Work Order: Rabbit-Hole Review Workspace Foundation

Status: planned

Depends on: `docs/work_orders/rabbit_hole_review_workspace_handoff.md`

Feature: `rabbit_hole_review_workspace`

## Goal

Add the first Draftsman foundation for a generic rabbit-hole review workspace without porting the flower prototype UI or embedding flower-specific taxonomy in C++.

The reusable idea is route-attached review: subject -> route -> component -> selected tab -> comment/status/receipt.

## Required Reading

- `docs/DRAFTSMAN_CODE_BRIEF.md`
- `docs/BUILDER_SIGNOFF.md`
- `docs/workflows/DEX_WORKER_PROTOCOL.md`
- `docs/work_orders/rabbit_hole_review_workspace_handoff.md`
- `data/features.json`
- `src/feature_routing.*`
- `src/text_editor_workbench_state.*`

## Scope

1. Add a disabled `rabbit_hole_review_workspace` feature entry to `data/features.json`.
2. Preserve the existing disabled-by-default behavior of all features.
3. Add a small state/settings helper for the new feature, similar to the text editor workbench state helper.
4. Add tests that prove:
   - the feature is present and disabled by default
   - routing requires `enabled = true`
   - routing requires a configured target tab
   - `show_left_subs`, `show_right_context`, `persist_review_state`, `max_comment_chars`, `enable_component_focus`, and `enable_exports` parse with safe defaults
5. Do not add the center renderer yet.
6. Do not add review persistence yet.

## Non-Goals

- Do not port the standalone HTML flower playground.
- Do not parse SVG.
- Do not add flower, lotus, tepal, petal, or Pattern Lab names to C++ model types.
- Do not add project-specific hardcoded sample data to source files.
- Do not add export buttons or file writes yet.
- Do not change the text editor workbench.

## Settings Contract

Add the feature entry in this shape:

```json
{
  "feature_id": "rabbit_hole_review_workspace",
  "label": "Rabbit-Hole Review",
  "status": "planned",
  "renderer_type": "rabbit_hole_review_workspace",
  "enabled": false,
  "target_surfaces": ["left", "center", "right_context"],
  "settings": {
    "target_tabs": ["Reviews", "Artifacts"],
    "show_left_subs": true,
    "show_right_context": true,
    "default_subject_id": "",
    "persist_review_state": true,
    "max_comment_chars": 4000,
    "enable_component_focus": true,
    "enable_exports": true
  }
}
```

Use an empty string for `default_subject_id` unless the existing feature parser is extended to handle JSON nulls intentionally.

## Suggested Modules

- `src/rabbit_hole_review_state.h`
- `src/rabbit_hole_review_state.cpp`

The helper should expose:

- `QString rendererType()`
- `RabbitHoleReviewState rabbitHoleReviewState(const DexFeatures::FeatureRecord &feature)`
- `const DexFeatures::FeatureRecord *activeRabbitHoleReviewFeature(const DexFeatures::FeatureRegistry &registry, const QString &tab)`
- `bool rabbitHoleReviewActive(const DexFeatures::FeatureRegistry &registry, const QString &tab)`
- `bool rabbitHoleReviewLeftSubsActive(const DexFeatures::FeatureRegistry &registry, const QString &tab)`
- `bool rabbitHoleReviewContextActive(const DexFeatures::FeatureRegistry &registry, const QString &tab)`

## Acceptance Criteria

- `data/features.json` validates.
- `rabbit_hole_review_workspace` is present and disabled by default.
- No visible UI changes occur while disabled.
- Tests cover enabled/disabled routing and parsed settings defaults.
- No flower-specific names are introduced into reusable C++ identifiers.
- Builder Dex adds a sign-off entry to `docs/BUILDER_SIGNOFF.md`.

## Verification

```sh
python3 scripts/validate_shell_layout.py data/shell_layout.json
python3 scripts/validate_ui_theme.py data/ui_theme.json
python3 scripts/validate_features.py data/features.json
cmake --build build
ctest --test-dir build --output-on-failure
```

Refresh the default disabled proof screenshot only if the Settings feature list or shell visibly changes.
