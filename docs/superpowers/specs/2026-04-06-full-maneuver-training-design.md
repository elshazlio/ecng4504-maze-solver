# Full Maneuver Training Design

## Goal
Make training, logs, EEPROM data, and solve playback all describe the same thing: the exact maneuver stream the robot physically executed, including forced turns, decision-node turns, straight-through traversals, and dead-end U-turns.

## Problem Summary
The current firmware stores a reduced path model that intentionally records only real decision nodes and dead ends. That design no longer matches the required behavior. It also makes debugging harder because the operator sees one sequence while the firmware stores another. On top of that, the current node-selection flow can misclassify some non-left nodes as `L` because sensing and turn selection are coupled through a left-first policy.

## Recommended Architecture
Replace the current junction-only raw path model with a maneuver-stream model.

Each stored step represents one executed maneuver, not one inferred junction. A step must contain:
- maneuver turn: `L`, `S`, `R`, or `B`
- event kind: forced, decision, or dead-end
- sensed exits mask for the triggering location when applicable
- segment time for the travel leading into that step

This creates one canonical sequence that can be:
- logged directly during training
- saved to EEPROM without losing forced maneuvers
- inspected later without reconstructing hidden steps
- replayed during solve with event-aware logic

## Component Changes
### Training recorder
Training should append a step whenever the robot commits to an executed maneuver.

That includes:
- forced single-exit turns
- decision-node turns after probe resolution
- zero-exit recovery only when the firmware has positively classified a dead end
- straight-through traversals when the robot intentionally continues straight

The node probe must stop using "first available left/straight/right" as the stored-truth generator. Instead, sensing should determine what exits exist, and a separate decision step should determine which maneuver is executed and recorded.

### Data model
The current arrays store only turn char, exits mask, and segment time. They need one additional event-kind field so the firmware can distinguish:
- a forced `L` from a decision-node `L`
- a straight forced corridor continuation from a decision-node `S`
- a true dead-end `B` from a temporary zero-exit probe failure

The event kind must be persisted alongside the turn sequence in RAM and EEPROM.

### Solve playback
Solve should no longer assume that only multi-choice nodes consume stored path entries. Instead, it should replay the same maneuver stream model that training recorded.

This means solve must:
- inspect the current sensed exits
- verify the next stored maneuver is still plausible
- execute the stored maneuver with the same motor routine used in training
- surface a clear mismatch log if the maze geometry no longer matches the stored step

### Path reduction
The current optimizer reduces `xBx` patterns in a junction-only path. That logic is too weak once forced maneuvers are included.

Optimization should therefore become event-aware:
- only consider simplification across dead-end events
- never collapse across forced moves that are part of the physically required route
- preserve a raw full-stream path exactly, even if an additional derived optimized path is built later

The raw full-stream sequence is the source of truth. Any optimized sequence is secondary and must be derivable without destroying the original record.

## Data Flow
1. Training follows the line.
2. When the robot reaches a location that requires action, sensing classifies the situation.
3. The firmware decides the executed maneuver.
4. The firmware performs that maneuver.
5. The firmware records one maneuver-stream step with event kind, turn, exits, and segment time.
6. EEPROM persistence saves that full stream.
7. Solve loads the same stream and replays it step by step.

## Error Handling
- If sensing sees no exits during a normal probe, the firmware must distinguish between a confirmed dead end and a transient probe miss before recording `B`.
- If solve reaches a step whose stored maneuver is impossible for the currently sensed exits, the firmware should log a path mismatch with both the stored step and live exits.
- If EEPROM data is from the older junction-only format, the firmware should reject it or version it clearly rather than silently interpreting it as full-stream data.

## Testing Strategy
Because this change alters the storage model, behavior must be locked down with regression coverage before production edits:
- add tests for maneuver recording so forced, decision, straight, and dead-end events all append correctly
- add tests proving false-left contamination does not rewrite intended straight decisions
- add tests for EEPROM round-trip of the extended step model
- add tests for solve playback consuming forced and decision events from the same stored stream
- add tests for any optimizer so it preserves the raw stream and only reduces allowed dead-end patterns

## Success Criteria
The design is successful when:
- the training log can represent the exact physical maneuver sequence the operator observed
- the saved raw path preserves forced and decision steps without collapsing them into a junction-only abstraction
- solve playback consumes that same path model consistently
- a run that physically contains straight-through `SR` decisions can no longer be recorded as all-left unless the sensors actually reported that geometry

## Scope Boundaries
This change is focused on path semantics and recording/playback correctness. It does not require a new UI, a new communications protocol, or a large unrelated refactor of line-following motor control unless tests prove one of those layers is the root cause of a remaining misclassification.
