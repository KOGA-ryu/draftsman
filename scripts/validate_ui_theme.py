#!/usr/bin/env python3
import argparse
import json
import re
import sys


COLOR = re.compile(r"^#[0-9a-fA-F]{6}$")


def main():
    parser = argparse.ArgumentParser(description="Validate a Draftsman four-color UI theme.")
    parser.add_argument("path", help="Path to ui_theme.json")
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

    errors = []
    if not isinstance(data, dict):
        errors.append("root must be an object")
    else:
        for key in ("base", "surface", "accent", "text"):
            value = data.get(key)
            if not isinstance(value, str) or not COLOR.match(value):
                errors.append(f"{key} must be a #RRGGBB color")
        if data.get("theme_mode") not in ("light", "dark", "system"):
            errors.append("theme_mode must be light, dark, or system")
        for key in ("ui_font", "code_font"):
            if not isinstance(data.get(key), str):
                errors.append(f"{key} must be a string")
        for key in ("ui_font_size", "code_font_size"):
            value = data.get(key)
            if not isinstance(value, int) or value < 9 or value > 28:
                errors.append(f"{key} must be an integer from 9 to 28")
        for key in data:
            if key not in ("theme_mode", "base", "surface", "accent", "text", "ui_font", "code_font", "ui_font_size", "code_font_size"):
                errors.append(f"{key} is not a supported theme field")

    if errors:
        for error in errors:
            print(f"error: {error}", file=sys.stderr)
        return 1

    print(f"ok: {args.path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
