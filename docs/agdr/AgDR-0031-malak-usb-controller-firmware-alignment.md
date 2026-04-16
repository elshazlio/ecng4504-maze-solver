---
id: AgDR-0031
timestamp: 2026-04-01T12:00:00Z
agent: cursor
model: GPT-5.4
trigger: user-prompt
status: executed
---

# Malak USB web controller: align with working_code.ino

> In the context of a standalone Flask + pyserial page under `Malak's Stuff/`, facing wrong baud, broken command casing, and log parsers that matched an older protocol, I decided to **standardize on USB 115200, full-word commands (TRAIN, START, STOP, …), and parsers keyed to current `logLine()` output**, to achieve **reliable control without Cursor**, accepting **Flask + threaded serial** instead of merging into the Bun BLE viewer stack.

## Context
- `Website_Embedded.py` used `cmd.lower()`, which broke STOP vs START and single-letter shortcuts that do not match `processCommand()` after `normalizeCommand()`.
- Default baud was 9600 while `working_code.ino` uses `USB_BAUD = 115200` for `Serial`.
- Status/path regexes expected strings like `Robot Started:` that the current sketch does not emit.

## Options Considered

| Option | Pros | Cons |
|--------|------|------|
| **Align buttons + baud + parsers with `working_code.ino`** | Works with course firmware as-is; minimal dependencies (`flask`, `pyserial`) | UI state is heuristic (keyword scan), not a formal state machine |
| **Keep old single-letter UI and patch firmware** | Small PC change | Diverges from repo `working_code.ino` and course expectations |
| **Replace with FastAPI/Bun stack** | Matches other dashboards | Overkill for Malak’s one-file tool; user asked for “easy run” parity, not rewrite |

## Decision
Chosen: **Align the Flask app with current firmware (115200, TRAIN/START/STOP/…, log keywords like `SOLVING_DONE`, `OPTIMAL_PATH=`)**, because the bugs were protocol mismatches, not missing frameworks.

## Consequences
- Users must set `COM_PORT` / `SERIAL_PORT` to their machine’s USB serial device.
- Dashboard fields (`Running`, `Solve finished`, path) derive from telemetry keywords; edge cases may need parser tweaks if log text changes.
