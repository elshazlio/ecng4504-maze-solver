---
id: AgDR-0041
timestamp: 2026-04-06T18:30:00Z
agent: cursor
model: composer
trigger: user-prompt
status: executed
---

# Training: dead-end grace after junction + no raw node on zero-exit probe

> In the context of training runs where logs showed spurious `DEAD_END_RECOVERY` after forced turns and a bogus `NODE … exits=DEAD` after phantom intersections, facing misclassified `persistentNoLine` and `turnTaken == 'B'` when no probe exit was seen, I decided to gate training dead-end recovery on a short post-junction window and to stop defer-recording (and path append) when the probe reports zero exits, accepting that an all-failed probe still performs a U-turn for recovery but no longer creates a graph node, and that true dead ends are still handled by the lost-line recovery path.

## Context
- Operator trace: physical sequence used forced turns and real LSR/LS/SR nodes; firmware inserted extra `DEAD_END` and a `NODE` with `exits=DEAD` between expected steps.
- `choices == 0` implies `selectTurn` returns `'B'`, but the condition `choices >= 2 || turnTaken == 'B'` routed that into the same deferred junction path as multi-exit nodes, producing `RECORD_*` and `RAW_PATH` pollution.
- Brief no-line intervals after any `inspectAndHandleNodeTraining` exit can exceed `LOST_LINE_TIMEOUT_MS` and trigger `handleTrainingDeadEndRecovery` even when the line returns moments later.

## Options Considered
| Option | Pros | Cons |
|--------|------|------|
| **A. Grace timer + fix defer condition** | Targets both failure modes; small localized change | Tuning constant needed on hardware |
| **B. Only raise `LOST_LINE_TIMEOUT_MS`** | One constant | Slows every dead-end classification; does not fix zero-exit node bug |
| **C. Retry probe when choices==0** | Might recover missed exits | Longer stop time; more code; still need a fallback |

## Decision
Chosen: **A**, because the zero-exit bug is a logic error independent of timeouts, and post-junction line loss is a known transition effect already called out in AgDR-0020-style guarding.

## Consequences
- `RAW_PATH` no longer gains a probe-only `B` when no exit was sensed; motion still calls `doTurn('B')` as a recovery attempt.
- Training dead-end recovery does not run until `DEAD_END_GRACE_AFTER_NODE_MS` after the last junction handler exit; `lastNodeHandledMs` is initialized when the train run acquires the line so the first segment is covered.
