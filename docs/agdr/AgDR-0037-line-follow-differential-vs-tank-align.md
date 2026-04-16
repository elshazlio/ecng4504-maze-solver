---
id: AgDR-0037
timestamp: 2026-04-03T12:00:00Z
agent: cursor
model: composer
trigger: user-prompt
status: executed
---

# Differential forward line follow vs in-place tank correction

> In the context of maze line following in `working_code.ino`, facing unwanted in-place spinning while cruising between junctions, I decided to steer with differential forward drive during `followLineNoPID` and keep tank-style rotation only for `alignToCenterStrict` (after stops / turns) and for lost-line search, because the user wants alignment in place only when stopped after turns, accepting that gentle arcs use more runway than spot spins and tuning of soft/hard deltas may be surface-specific.

## Context
- `followLineNoPID` used `spinLeft` / `spinRight` for routine left/right error recovery, which rotates the robot in place while it should be following the segment.
- `doTurn` already calls `stopMotors()` then `alignToCenterStrict()` for post-turn centering.

## Options Considered
| Option | Pros | Cons |
|-----|---|---|
| **Differential forward (both wheels forward, asymmetric PWM)** | Matches “no tank while moving”; smooth segment tracking | Needs delta tuning; wide errors may need longer path |
| **Keep tank spins in followLineNoPID** | Strong re-centering in tight geometry | Conflicts with requested behavior; feels like constant micro-turns |
| **Reduce spin speed only** | Minimal code change | Still in-place rotation while moving; does not meet the ask |

## Decision
Chosen: **differential forward** for all non-search corrections inside `followLineNoPID`, because it separates cruise steering from explicit post-stop alignment in `alignToCenterStrict`.

## Consequences
- New `driveSteerLeft` / `driveSteerRight` helpers clamp PWM and bias left vs right while staying in forward quadrants.
- Lost-line recovery after `LOST_LINE_TIMEOUT_MS` still uses `spinLeft`/`spinRight` at `SEARCH_SPEED` to hunt for the line.
