# Work Order: Rabbit-Hole Review Workspace Fixture Renderer

Status: planned

Depends on: `docs/work_orders/rabbit_hole_review_workspace_foundation.md`

Feature: `rabbit_hole_review_workspace`

## Goal

Render the first generic rabbit-hole review workspace from one local fixture JSON subject. This should prove the navigation and review model without implementing project persistence, SVG parsing, or exports.

## Scope

1. Add a generic review model:
   - subject
   - route node
   - component
   - comment
   - review status
2. Add a small fixture JSON under `data/fixtures/` or `docs/examples/`.
3. Add a parser that ignores unknown fields.
4. Add a center renderer for configured target tabs:
   - subject header
   - breadcrumb
   - back / forward / home controls
   - local tabs: Overview, Components, Layers, Validation, Comments
   - rabbit-hole card grid
   - route comment list
5. Add an optional right-context renderer:
   - selected route facts
   - selected component facts
   - status summary
6. Keep left subs out of this work order unless Architect Dex explicitly approves the scope expansion.

## Non-Goals

- Do not save comments to disk yet.
- Do not add export buttons yet.
- Do not parse or focus SVG internals yet.
- Do not hardcode flower taxonomy in C++.
- Do not build a production approval workflow.

## Fixture Rule

The fixture may use sample review content, but the model names and C++ identifiers must stay generic. If the fixture is based on the flower prototype, flower-specific words may appear only inside fixture data, never as C++ type names.

## Acceptance Criteria

- With the feature disabled, Draftsman still renders the blank shell.
- With the feature enabled on `Reviews` or `Artifacts`, the fixture review workspace appears.
- Card clicks drill into child routes.
- Back, forward, and home work in memory.
- Breadcrumb updates with the active route.
- Comments display with route/status metadata.
- No persistence or export writes occur.
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

- default disabled state
- enabled fixture workspace
- drilldown route with breadcrumb
- route comments visible
