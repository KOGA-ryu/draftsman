# Draftsman

Draftsman is a C++/Qt blank project-binder shell. It starts as a polished
layout skeleton for organizing projects, artifacts, reviews, notes, and future
project-specific workbenches.

The current app intentionally ships with an empty registry:

```json
{
  "projects": [],
  "workers": []
}
```

The first screen keeps only the reusable UI structure:

- top chrome controls
- left project rail
- center binder tabs and panels
- detail lens rail
- right inspector panels
- Settings entry point

## Build

```sh
cmake -S . -B build
cmake --build build
```

## Test

```sh
ctest --test-dir build --output-on-failure
```

## Run

```sh
./build/draftsman
```

## Proof

Reference blank-shell screenshots live in:

```text
docs/proof/blank_slate/
```
