---
id: AgDR-0025
timestamp: 2026-03-19T21:28:00Z
agent: cursor
model: GPT-5.4
trigger: user-prompt
status: executed
---

# Left-Biased Center Reacquire

> In the context of `working_code.ino` committed-left execution, facing a regression where ordinary left junctions were missed after the new non-blocking turn FSM because the turn stayed active until a strict `00100` lock appeared, I decided to treat a left-biased center reacquire as valid committed-left completion to achieve pre-refactor left-junction timing on tight exits, accepting a bounded risk of ending the turn slightly earlier on some left-heavy samples.

## Context
- `AgDR-0022` replaced the blocking left-turn execution with a non-blocking committed-left state machine.
- The previous blocking align path stopped as soon as the center sensor saw line, even if the pattern was left-biased like `01100`.
- The new FSM only completes on two consecutive strict center-only samples, so `01100` can keep the robot spinning through a real outgoing left junction until timeout.
- `AgDR-0023` post-left assist cannot help if the committed-left maneuver never completes in time to arm it.

## Options Considered
| Option | Pros | Cons |
|-----|---|---|
| **Accept left-biased center reacquire for committed-left completion** | Closest behavioral match to the pre-FSM turn exit; directly addresses the observed missed-left regression; small self-contained change | Can complete a little earlier than strict `00100`; requires careful regression coverage |
| Keep strict `00100` completion and increase timeouts/assist windows | Minimal structural change; preserves strict lock definition | Does not address the actual blind spot while `01100` is present; likely maze-specific retuning |
| Rework the committed-left FSM into a larger inspection/exit model | Most explicit long-term model of junction exit state | Higher risk and larger scope than needed for the current regression |

## Decision
Chosen: **Accept left-biased center reacquire for committed-left completion**, because the regression comes from over-constraining turn completion relative to the earlier behavior, and relaxing completion to allow `01100` is the smallest fix that restores timely post-left decision-making.

## Consequences
- `serviceCommittedTurn()` completion will allow a center lock with inner-left support, not only pure center-only.
- Regression tests must prove that `01100` during committed-left spin can complete the maneuver and expose the next close left.
- Future tuning may still revisit the full exit-state model if hardware data shows this relaxed reacquire is too permissive.
