---
id: AgDR-0015
timestamp: 2026-03-19T00:00:00Z
agent: cursor
model: GPT-5.4
trigger: user-prompt
status: executed
---

# Use Geometry-Aware Junction And Finish Qualification

> In the context of the maze-training controller in `working_code.ino`, facing missed left turns on plain 10 cm junctions and false finish detection away from the unique 20 cm finish bubble, I decided to keep the current FSM shape but make its node-release and finish-qualification logic geometry-aware to achieve more reliable traversal, accepting a moderate heuristic retune instead of a finish-only patch or a larger rewrite.

## Context
- The maze uses plain 10 cm lines with no widened junction markers.
- The finish bubble is the only 20 cm circular marker on the course.
- Current hardware runs show three linked failures: missed left turns, false finish calls at non-finish nodes, and phantom left/U-turn behavior on straights.
- The project already has a self-contained sketch and host-side regression harness that should be preserved.

## Options Considered
| Option | Pros | Cons |
|-----|---|---|
| **Retune the current FSM for narrow junctions and unique-finish geometry** | Smallest meaningful change, directly matches the real maze, preserves the current sketch/test harness | Still heuristic, needs careful regression coverage |
| **Harden finish detection only** | Lowest-risk code delta, directly addresses false stops | Leaves missed-left and phantom-turn behavior largely untouched |
| **Rewrite the controller more aggressively** | Cleaner long-term structure, more explicit state ownership | Highest change risk, slower to validate on hardware |

## Decision
Chosen: **Retune the current FSM for narrow junctions and unique-finish geometry**, because it addresses the root-cause mismatch between the controller assumptions and the real maze layout while keeping the current self-contained architecture intact.

## Consequences
- `working_code.ino` will use stricter finish qualification than a single all-black sample.
- Node-scan release and live-steering fallback will be adjusted so narrow real left branches are less likely to be dropped while stale left intent is less likely to survive into unrelated recovery patterns.
- Regression tests will be extended to cover ordinary all-black crossings, narrow left-branch commitment, and stale-latch release.
