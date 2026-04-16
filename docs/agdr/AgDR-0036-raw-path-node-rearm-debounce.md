---
id: AgDR-0036
timestamp: 2026-04-03T12:00:00Z
agent: cursor
model: gpt-5.2
trigger: user-prompt
status: executed
---

# Raw path duplicate junction recording (node zone re-arm)

> In the context of maze training recording, facing duplicate `RAW_PATH` characters when the robot’s sensors still report a junction after `NODE_LOCKOUT_MS`, I decided to re-arm recording only after `nodeCandidateNow()` has been false (left the junction sensor pattern), to achieve one stored decision per physical junction, accepting a small amount of extra state in the training/solving loops.

## Context
- `RAW_PATH` showed `LLLBSLBLLL` where the correct sequence was `LLBSLBLL` (extra leading and trailing `L`).
- Movement followed the left-hand rule; the bug is in when `inspectAndHandleNode*` runs, not in turn selection.
- `millis() - lastNodeHandledMs >= NODE_LOCKOUT_MS` allows a second trigger while the robot is still on the same wide/black pattern.

## Options Considered
| Option | Pros | Cons |
|--------|------|------|
| **A — Increase `NODE_LOCKOUT_MS` only** | Trivial one-line change | Depends on maze geometry and speed; can still fail on long intersections or be too slow for tight mazes. |
| **B — Re-arm after `!nodeCandidateNow()`** | One decision per junction regardless of how long the pattern stays true; does not change `doTurn` / follow logic | Adds one boolean and two small branches; rare sensor flicker could still re-arm early (mitigated by keeping lockout). |

## Decision
Chosen: **B**, because it matches the physical definition of “next junction” (sensors must show a normal line segment between stored nodes) and is independent of junction size.

## Consequences
- `RAW_PATH` / `NODE n` logs should align with actual junction count.
- Solving uses the same gate so `solveStepIndex` is not advanced twice at one junction.
- Movement code paths are unchanged; only the condition that calls existing inspection functions is stricter.
