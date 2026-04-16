---
id: AgDR-0032
timestamp: 2026-04-02T12:00:00Z
agent: cursor
model: unknown
trigger: user-prompt
status: executed
---

# Firmware PATH + PAUSE/RESUME to match maze dashboard

> In the context of the Python BLE dashboard sending `PATH`, `PAUSE`, and `RESUME` while `working_code.ino` only handled a smaller command set, I decided to **extend the sketch with additive commands and four paused substates plus a resume stage snapshot**, to achieve **protocol parity without changing existing training/solving algorithms or timing constants**, accepting **more enum values and switch arms** in the main loop.

## Context
- Dashboard maps GETPATH → `PATH`, and sends PAUSE/RESUME per AgDR-0030 UX.
- Firmware must not alter maze logic, speeds, or timeouts—only new branches and reporting.

## Options Considered

| Option | Pros | Cons |
|--------|------|------|
| **A — New `PATH` handler + paused `RobotState` values + `resumeStage[]` snapshot** | Matches dashboard `STATE=PAUSED_*`; RESUME restores `STAGE=` exactly; no changes to `followLine` / node logic | Larger enum and `loop()` switch |
| **B — Global `paused` flag only** | Fewer enum values | Harder to align `stateName()` with dashboard; easy to desync STATE vs motion |
| **C — Change dashboard only** | No robot flash | Does not satisfy “close mismatches” on the robot |

## Decision
Chosen: **Option A**, because it mirrors AgDR-0030 and the Python parser’s `PAUSED_*` / `STATE=` expectations while keeping all motion code paths identical when not paused.

## Consequences
- `PATH` prints path lengths and `RAW_PATH=` / `OPTIMAL_PATH=` when non-empty (subset of STATUS-style telemetry).
- Paused states hold motors off until `RESUME`; training/solving internals are unchanged.
- Tradeoff: must maintain `resumeStage` and paused enum cases when adding future motion states.
