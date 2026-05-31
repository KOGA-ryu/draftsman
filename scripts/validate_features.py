#!/usr/bin/env python3
import argparse
import json
import sys


def expect(condition, errors, message):
    if not condition:
        errors.append(message)


def validate_feature(feature, path, errors):
    expect(isinstance(feature, dict), errors, f"{path} must be an object")
    if not isinstance(feature, dict):
        return
    for key in ("feature_id", "label", "status", "renderer_type"):
        expect(isinstance(feature.get(key), str) and feature[key].strip(), errors, f"{path}.{key} is required")
    expect(isinstance(feature.get("enabled"), bool), errors, f"{path}.enabled must be boolean")
    if "target_surfaces" in feature:
        expect(isinstance(feature["target_surfaces"], list), errors, f"{path}.target_surfaces must be an array")
        if isinstance(feature["target_surfaces"], list):
            for index, value in enumerate(feature["target_surfaces"]):
                expect(isinstance(value, str) and value.strip(), errors, f"{path}.target_surfaces[{index}] must be a string")
    if "settings" in feature:
        expect(isinstance(feature["settings"], dict), errors, f"{path}.settings must be an object")


def validate_features(data):
    errors = []
    expect(isinstance(data, dict), errors, "root must be an object")
    if not isinstance(data, dict):
        return errors
    expect(data.get("schema_version") == 1, errors, "schema_version must be 1")
    features = data.get("features")
    expect(isinstance(features, list), errors, "features must be an array")
    seen = set()
    if isinstance(features, list):
        for index, feature in enumerate(features):
            validate_feature(feature, f"features[{index}]", errors)
            if isinstance(feature, dict) and isinstance(feature.get("feature_id"), str):
                feature_id = feature["feature_id"]
                expect(feature_id not in seen, errors, f"duplicate feature_id: {feature_id}")
                seen.add(feature_id)
    return errors


def main():
    parser = argparse.ArgumentParser(description="Validate a Draftsman feature registry JSON file.")
    parser.add_argument("path", help="Path to features.json")
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

    errors = validate_features(data)
    if errors:
        for error in errors:
            print(f"error: {error}", file=sys.stderr)
        return 1

    print(f"ok: {args.path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
