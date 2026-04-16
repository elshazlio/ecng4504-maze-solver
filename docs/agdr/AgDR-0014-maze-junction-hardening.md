---
id: AgDR-0014
timestamp: 2026-03-19T00:00:00Z
agent: cursor
model: GPT-5.4
trigger: user-prompt
status: executed
---

# Use Targeted Maze Junction Hardening

> In the context of the maze-training controller in `working_code.ino`, facing missed left turns at junctions and false finish detection near all-black overlaps, I decided to use targeted maze junction hardening to achieve more reliable traversal without destabilizing the rest of the controller, accepting a moderate logic change instead of a tiny tuning pass or a larger control rewrite.

## Context
- The current controller already performs left-hand following reasonably well on much of the maze.
- The run log shows two concrete failures: a left branch is detected but missed, and a later all-black condition is treated as finish even though the car is not at the real terminal marker.
- The project needs a reliability-focused improvement quickly, without replacing the whole controller architecture.

## Options Considered
| Option | Pros | Cons |
|-----|---|---|
| **Targeted hardening of follow, node-scan, and finish logic** | Directly addresses the observed failures, preserves the current controller shape, fits the existing host-side test harness | Requires touching several interacting heuristics and retuning them together |
| **Tuning only** | Fastest change, smallest code delta, easy to trial on hardware | Likely to leave the false-finish bug intact, may only mask the missed-left symptom |
| **Larger controller rewrite** | Strongest long-term structure, cleaner state handling, more room for future improvements | Highest regression risk, longer implementation time, too disruptive for the current need |

## Decision
Chosen: **Targeted hardening of follow, node-scan, and finish logic**, because it fixes both observed failures at the correct layer of behavior while keeping the existing self-contained sketch and test harness intact.

## Consequences
- `working_code.ino` will gain stricter finish qualification, softer normal-follow corrections, and safer node-scan commitment rules.
- The host-side regression tests will expand to cover the exact missed-left and false-finish patterns seen in the run log.
- The controller may become slightly slower or more conservative at junction entry in exchange for higher reliability.
