# Work Order Seed: Rabbit-Hole Review Workspace

Status: planning seed

Source prototype: `/Users/kogaryu/game/mosaic_dungeon_floor_v0/pattern_lab_2d`

Prototype files:

- `/Users/kogaryu/game/mosaic_dungeon_floor_v0/pattern_lab_2d/scripts/build_flower_color_playground_v0.py`
- `/Users/kogaryu/game/mosaic_dungeon_floor_v0/pattern_lab_2d/UI_RABBIT_HOLE_WORKSPACE_CONTRACT.md`
- latest proof batch: `/Users/kogaryu/game/mosaic_dungeon_floor_v0/pattern_lab_2d/outputs/batches/20260531_012123_flower_color_playground_v0`

## Feature Name

`rabbit_hole_review_workspace`

## Purpose

Provide a compact Draftsman review workspace for large AI-generated creative/project artifacts. The UI compresses many review targets into navigable rabbit holes: section -> subject -> component -> feature -> comment / approve / reject.

The key reusable idea is route-attached review, not flower-specific art.

## Quick UI Summary

Prototype made:

- standalone HTML flower review playground generated from Python
- compact rail with sections: Source, Palette, Layers, Taxonomy, Look, Preview, Review, Export, Diagnostics
- candidate preview window with SVG recoloring and focused component dimming
- taxonomy rabbit-hole drilldown with back, forward, home, and show-all
- component review statuses: pending, accepted, needs_rework, rejected
- route-attached comments stored with exact route/focus keys
- export buttons for recolored SVG, review comments JSON, taxonomy review JSON, and full state JSON

Working now:

- loads approved flower SVG candidates from a batch manifest
- reads SVG `data-layer` and `data-asset` groups
- builds taxonomy components from those groups
- maps components into anatomy roles such as `tepal`, `backing_leaf`, `receptacle_center`, `ornament_filler`, and `arc_linework`
- focuses/dims preview by selected route/component
- stores comments with route metadata
- preserves palette/recolor controls

Intentionally not implemented:

- no C++/Qt implementation
- no real feature-anchor highlighting below whole SVG component group
- no generation of flower linework
- no production approval workflow
- no generic Draftsman data model yet

Proof paths:

- HTML: `/Users/kogaryu/game/mosaic_dungeon_floor_v0/pattern_lab_2d/outputs/batches/20260531_012123_flower_color_playground_v0/flower_color_playground_v0.html`
- manifest: `/Users/kogaryu/game/mosaic_dungeon_floor_v0/pattern_lab_2d/outputs/batches/20260531_012123_flower_color_playground_v0/manifest.json`
- screenshot: `/Users/kogaryu/game/mosaic_dungeon_floor_v0/pattern_lab_2d/outputs/batches/20260531_012123_flower_color_playground_v0/safari_initial_check.png`

## Reusable Parts To Keep

- left navigation rail with stable sections
- route stack: current route, back stack, forward stack, home
- breadcrumb rendering
- route-attached comments
- component focus/dimming preview behavior
- local subject tabs: Overview, Components, Layers, Linework, Palette, Validation, Comments, Exports
- review statuses and failure reasons
- exportable review state JSON
- data-driven taxonomy graph built from metadata, not hardcoded widget trees

## Project-Specific Parts To Keep Out Of C++

- flower-specific taxonomy names
- lotus/tepal anatomy labels
- exact palette colors from the prototype
- SVG-specific parsing details
- generated batch paths from Pattern Lab 2D
- Python-only output policies

Draftsman should receive generic records such as `review_subject`, `review_component`, `review_route`, `review_comment`, and `review_status`.

## Expected Settings/Data Contract

Seed feature entry:

```json
{
  "feature_id": "rabbit_hole_review_workspace",
  "label": "Rabbit-Hole Review",
  "status": "planned",
  "renderer_type": "rabbit_hole_review_workspace",
  "enabled": false,
  "settings": {
    "target_tabs": ["Reviews", "Artifacts"],
    "show_left_subs": true,
    "show_right_context": true,
    "default_subject_id": null,
    "persist_review_state": true,
    "max_comment_chars": 4000,
    "enable_component_focus": true,
    "enable_exports": true
  }
}
```

Subject data shape:

```json
{
  "subject_id": "flower_candidate_001",
  "label": "Flower Candidate 001",
  "status": "candidate",
  "preview_surface": {
    "kind": "svg_or_image",
    "path": "relative/or/resolved/path"
  },
  "components": [
    {
      "component_id": "inner_petals::lotus_inner_petal_v0",
      "label": "Inner Upright Tepal",
      "role": "component",
      "parent_route": ["flower_anatomy", "petal_tepal_rings"],
      "focus_selector": "optional renderer-specific selector"
    }
  ],
  "routes": [
    {
      "route_id": "flower_anatomy/petal_tepal_rings/inner_upright_tepals",
      "label": "Inner Upright Tepals",
      "children": []
    }
  ],
  "comments": []
}
```

Comment data shape:

```json
{
  "comment_id": "comment_001",
  "subject_id": "flower_candidate_001",
  "route": "flower_anatomy/petal_tepal_rings/inner_upright_tepals/inner_tepal_03/apex_tip",
  "selected_tab": "Linework",
  "target_component": "inner_petals::lotus_inner_petal_v0",
  "target_feature": "apex_tip",
  "status": "needs_rework",
  "comment": "Tip is too blunt. Needs a softer taper.",
  "created_at": "ISO-8601"
}
```

## Renderer Surfaces Needed

Center surface:

- subject header
- breadcrumb
- back / forward / home controls
- tab bar
- scrollable subject body
- focused preview surface
- rabbit-hole card grid
- route comment thread

Left surface:

- permanent collapsible subs
- source/subject picker
- settings shortcuts
- export/diagnostic shortcuts

Right context surface:

- selected route facts
- selected component facts
- validation/failure reasons
- compact review state summary
- export buttons

## Dependencies

Use existing Draftsman Qt/C++ infrastructure:

- `data/features.json`
- feature registry / feature routing
- theme tokens from `data/ui_theme.json`
- existing left rail, center page, and right context patterns

No new external dependencies are expected for the first C++ pass.

## Implementation Phases

1. Feature contract only
   - Add disabled `rabbit_hole_review_workspace` feature entry.
   - Add schema fields if needed.
   - No UI yet.

2. Generic review model
   - Add records for subject, route node, component, comment, status.
   - Use local fixture JSON for one proof subject.

3. Center renderer
   - Render subject header, breadcrumb, tabs, cards, comment list.
   - Implement back, forward, home in memory.

4. Left subs
   - Render collapsible subs.
   - Clicking a sub changes active route/subject in center.

5. Preview/focus surface
   - Add generic preview placeholder first.
   - Later support SVG/image focus selectors if needed.

6. Export and persistence
   - Save/load review state JSON.
   - Export comments and statuses.

## Acceptance Criteria

- Feature disabled by default.
- Enabling the feature renders a rabbit-hole review workspace on configured tabs.
- Left subs collapse/expand.
- Clicking a sub updates the main subject route.
- Rabbit-hole cards drill deeper.
- Back, forward, and home work.
- Tabs are subject-local, not global junk drawers.
- Comments store exact route, selected tab, target component/feature, status, and timestamp.
- Theme/font tokens are used; no feature-local hardcoded styling.
- No flower-specific names appear in generic C++ model names.
- Builder adds sign-off entry to `docs/BUILDER_SIGNOFF.md`.

## Verification

```sh
python3 scripts/validate_shell_layout.py data/shell_layout.json
python3 scripts/validate_ui_theme.py data/ui_theme.json
python3 scripts/validate_features.py data/features.json
cmake --build build
ctest --test-dir build --output-on-failure
```

Refresh proof screenshots for:

- feature disabled state
- enabled rabbit-hole workspace
- collapsed and expanded left sub
- drilldown route with breadcrumb
- route comment stored/displayed

## Known Risks / Gaps

- The prototype is HTML/SVG; Draftsman needs a generic review data model rather than copying DOM behavior.
- Component focus may begin as whole-preview highlighting only.
- Persistence must not become project-specific C++.
- The feature can become another cluttered inspector if Architect Dex does not keep the rabbit-hole compression rule strict.
- Generic route/comment model should be proven with one fixture before adding renderer-specific adapters.
