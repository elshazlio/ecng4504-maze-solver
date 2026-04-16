---
id: AgDR-0040
timestamp: 2026-04-06T14:00:00Z
agent: cursor
model: gpt-5.2
trigger: user-prompt
status: executed
---

# Training: do not append raw path on lost-line dead-end recovery

> In the context of maze training where `rawPath` should reflect junction topology and probe-classified decisions, facing spurious `B` steps when the robot only lost the line briefly and ran U-turn recovery, I decided to omit `recordNode` from `handleTrainingDeadEndRecovery` while keeping motion identical, accepting that cul-de-sacs reached only via lost-line (never probed as a junction) will no longer appear in the graph.

## Context
- `persistentNoLine()` triggers the same probe + U-turn sequence as before.
- Previously every such event appended `B` with mask `DEAD`, inflating `RAW_PATH` vs operator intent.

## Options Considered
| Option | Pros | Cons |
|--------|------|------|
| **A. Unrecord lost-line recovery only** | Matches goal; one code path change | Rare real dead ends seen only after line loss are not graphed |
| **B. Longer `LOST_LINE_TIMEOUT_MS`** | Fewer false triggers | Changes when recovery runs (behavior change) |
| **C. Record B only after extra confirmation** | More accurate `B` | More state, slower recovery path |

## Decision
Chosen: **A**, because the plan required unchanged physical behavior and junction logic; lost-line recovery remains the same, only the maze-memory append is skipped, with `segmentStartMs` reset like `recordNode` would.

## Consequences
- `RAW_PATH` / EEPROM training data omit U-turn recoveries that were not probe-classified dead ends.
- Logs include `DEAD_END_RECOVERY_NO_PATH` (with `segMs` for diagnostics) alongside `DEAD_END_DETECTED`.
- Probe-based `B` recording in `inspectAndHandleNodeTraining` is unchanged.
