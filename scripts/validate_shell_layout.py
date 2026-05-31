#!/usr/bin/env python3
import argparse
import json
import sys


def expect(condition, errors, message):
    if not condition:
        errors.append(message)


def validate_item(item, path, errors):
    expect(isinstance(item, dict), errors, f"{path} must be an object")
    if not isinstance(item, dict):
        return
    expect(isinstance(item.get("id"), str) and item["id"].strip(), errors, f"{path}.id is required")
    expect(isinstance(item.get("label"), str) and item["label"].strip(), errors, f"{path}.label is required")
    if "enabled" in item:
        expect(isinstance(item["enabled"], bool), errors, f"{path}.enabled must be boolean")


def validate_panel(panel, path, visible_tabs, errors):
    expect(isinstance(panel, dict), errors, f"{path} must be an object")
    if not isinstance(panel, dict):
        return
    expect(isinstance(panel.get("id"), str) and panel["id"].strip(), errors, f"{path}.id is required")
    expect(isinstance(panel.get("label"), str) and panel["label"].strip(), errors, f"{path}.label is required")
    tab = panel.get("tab")
    expect(isinstance(tab, str) and tab.strip(), errors, f"{path}.tab is required")
    panel_enabled = panel.get("enabled", True)
    if isinstance(tab, str) and tab != "*" and panel_enabled:
        expect(tab in visible_tabs, errors, f"{path}.tab '{tab}' does not match an enabled tab label")
    if "lines" in panel:
        lines = panel["lines"]
        expect(isinstance(lines, (list, str)), errors, f"{path}.lines must be an array or string")
        if isinstance(lines, list):
            for index, line in enumerate(lines):
                expect(isinstance(line, str), errors, f"{path}.lines[{index}] must be a string")
    if "text" in panel:
        expect(isinstance(panel["text"], str), errors, f"{path}.text must be a string")
    if "min_height" in panel:
        expect(isinstance(panel["min_height"], int) and panel["min_height"] >= 32, errors, f"{path}.min_height must be >= 32")
    if "subtle" in panel:
        expect(isinstance(panel["subtle"], bool), errors, f"{path}.subtle must be boolean")
    if "enabled" in panel:
        expect(isinstance(panel["enabled"], bool), errors, f"{path}.enabled must be boolean")


def validate_shell_layout(data):
    errors = []
    expect(isinstance(data, dict), errors, "root must be an object")
    if not isinstance(data, dict):
        return errors

    expect(isinstance(data.get("app_title"), str) and data["app_title"].strip(), errors, "app_title is required")

    left_rail = data.get("left_rail")
    expect(isinstance(left_rail, dict), errors, "left_rail must be an object")
    sections = left_rail.get("sections") if isinstance(left_rail, dict) else None
    expect(isinstance(sections, list), errors, "left_rail.sections must be an array")
    if isinstance(sections, list):
        for section_index, section in enumerate(sections):
            path = f"left_rail.sections[{section_index}]"
            expect(isinstance(section, dict), errors, f"{path} must be an object")
            if not isinstance(section, dict):
                continue
            expect(isinstance(section.get("id"), str) and section["id"].strip(), errors, f"{path}.id is required")
            expect(isinstance(section.get("title"), str) and section["title"].strip(), errors, f"{path}.title is required")
            if "enabled" in section:
                expect(isinstance(section["enabled"], bool), errors, f"{path}.enabled must be boolean")
            items = section.get("items", [])
            expect(isinstance(items, list), errors, f"{path}.items must be an array")
            if isinstance(items, list):
                for item_index, item in enumerate(items):
                    validate_item(item, f"{path}.items[{item_index}]", errors)

    tabs = data.get("tabs")
    expect(isinstance(tabs, list), errors, "tabs must be an array")
    visible_tabs = set()
    if isinstance(tabs, list):
        for index, tab in enumerate(tabs):
            validate_item(tab, f"tabs[{index}]", errors)
            if isinstance(tab, dict) and tab.get("enabled", True) and isinstance(tab.get("label"), str):
                visible_tabs.add(tab["label"])
    expect(bool(visible_tabs), errors, "at least one enabled tab label is required")

    panels = data.get("panels")
    expect(isinstance(panels, list), errors, "panels must be an array")
    if isinstance(panels, list):
        for index, panel in enumerate(panels):
            validate_panel(panel, f"panels[{index}]", visible_tabs, errors)

    inspector = data.get("inspector")
    expect(isinstance(inspector, dict), errors, "inspector must be an object")
    inspector_panels = inspector.get("panels") if isinstance(inspector, dict) else None
    expect(isinstance(inspector_panels, list), errors, "inspector.panels must be an array")
    if isinstance(inspector_panels, list):
        for index, panel in enumerate(inspector_panels):
            validate_panel(panel, f"inspector.panels[{index}]", visible_tabs, errors)

    return errors


def main():
    parser = argparse.ArgumentParser(description="Validate a Draftsman shell layout JSON file.")
    parser.add_argument("path", help="Path to shell_layout.json")
    args = parser.parse_args()

    try:
        with open(args.path, "r", encoding="utf-8") as handle:
            data = json.load(handle)
    except OSError as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 2
    except json.JSONDecodeError as exc:
        print(f"error: invalid JSON: {exc}", file=sys.stderr)
        return 2

    errors = validate_shell_layout(data)
    if errors:
        for error in errors:
            print(f"error: {error}", file=sys.stderr)
        return 1

    print(f"ok: {args.path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
