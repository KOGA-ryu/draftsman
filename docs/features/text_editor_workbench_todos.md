# Text Editor Workbench TODOs

Status: TODO list only. Do not treat these as final implementation decisions.

The current `text_editor_workbench` renderer is an easy first demo package: an enabled feature can replace a blank binder tab with a reusable tool surface. The details below should be solved only when a project needs them.

## TODO Buckets

### Document Persistence

- Decide where scratch documents live.
- Decide whether autosave is allowed.
- Decide whether project configs can define document lists.
- Decide whether imported repo snippets are copied into Draftsman storage or referenced read-only.

### Command Runner

- Decide which commands can consume active document text or selected text.
- Decide whether commands are project-defined, feature-defined, or both.
- Decide where command results appear: output shelf, new document, receipt, or right context.

### Command Palette

- Decide whether actions begin as right-context buttons before adding a searchable palette.
- Decide how project-specific commands are discovered.
- Decide whether keyboard shortcuts are allowed.

### Fixture Runner

- Decide whether fixture execution belongs inside the text editor or a broader tool-test feature.
- Decide how fixture input/output is stored.
- Decide how fixture runs produce reviewable receipts.

### Clipboard Write

- Decide whether clipboard write is permitted.
- Require an explicit setting before any OS clipboard write.
- Decide whether each clipboard write needs human confirmation.

### Agent Receipts

- Decide the receipt schema for AI/worker actions touching editor text.
- Include session key, worker identity, input document, output document, action, timestamp, and status.
- Make receipts reviewable from the binder.

## Current Rule

Until these are deliberately designed, the text editor remains local UI only: no source file editing, no backend command execution, no clipboard write, and no persistence promises.
