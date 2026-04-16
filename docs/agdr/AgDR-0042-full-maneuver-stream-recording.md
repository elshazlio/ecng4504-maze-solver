---
id: AgDR-0042
timestamp: 2026-04-06T19:00:00Z
agent: cursor
model: gpt-5.4
trigger: user-prompt
status: executed
---

# Training and solve: record the full maneuver stream

> In the context of maze training where the car physically executes forced turns, real decision nodes, and dead-end U-turns, facing logs and EEPROM paths that only stored a reduced junction-only sequence and also misclassified some straight decisions as left turns, I decided to store and replay the full maneuver stream with explicit event typing to achieve one consistent source of truth across motion, logs, persistence, and solve playback, accepting a broader refactor of the training, optimization, and solve pipeline.

## Context
- The operator expects the learned path to match the car's real maneuver sequence, including forced left, forced right, straight-through, node decisions, and dead-end U-turns.
- The current firmware explicitly treats `rawPath` as "real junction decisions only", so forced turns are omitted by design.
- The current training probe also selects turns with left-hand priority, which can lock a node into `L` when left is spuriously seen and can suppress intended `S` decisions from the stored path.
- A path model that omits physically executed maneuvers makes logs, EEPROM data, and solve behavior diverge from what the robot actually did.

## Options Considered
| Option | Pros | Cons |
|--------|------|------|
| **A. Patch the current junction-only recorder** | Smallest code change; least disruption to EEPROM and solver shape | Preserves the wrong abstraction for the requested behavior; forced maneuvers still have to live outside the stored path; debugging remains ambiguous |
| **B. Store the full maneuver stream with explicit event types** | Matches physical behavior exactly; gives logs, storage, and solving one shared model; makes forced vs decision events explicit; easier to debug and verify | Requires coordinated changes across training, persistence, optimization, and solve playback |
| **C. Keep junction-only storage and add richer logs beside it** | Improves observability with lower risk than a full model change | Still leaves two incompatible truths in the firmware and does not satisfy the requirement that the learned path record everything |

## Decision
Chosen: **B**, because the requirement is to learn what the car actually did, not a reduced abstraction of selected junctions, and a single full-stream model is more reliable than trying to keep a junction-only path and side-channel maneuver logs in sync.

## Consequences
- Training will append every executed maneuver to the raw learned sequence and will tag each step with its event kind.
- The code will stop depending on left-hand-priority selection as both the decision policy and the stored-path abstraction.
- Solve playback will be updated to consume the same maneuver stream model rather than assuming only decision nodes are persisted.
- Path reduction logic will need stricter rules so dead-end simplification does not collapse unrelated forced maneuvers.
