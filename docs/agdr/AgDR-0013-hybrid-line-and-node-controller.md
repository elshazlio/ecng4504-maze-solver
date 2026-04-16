---
id: AgDR-0013
timestamp: 2026-03-19T03:59:41Z
agent: cursor
model: GPT-5.4
trigger: user-prompt
status: executed
---

# Use A Hybrid Line And Node Controller

> In the context of the maze-solving car firmware, facing phantom left turns on straight segments, missed left turns at intersections, and unreliable dead-end recovery, I decided to use a hybrid controller that separates segment following from node inspection and committed turn execution to achieve more reliable navigation, accepting a moderate firmware refactor instead of a small averaging-only tweak.

## Context
- The current `working_code.ino` mixes normal line following, intersection classification, and turn commitment in one pattern-driven path.
- The robot shows a left bias on straights, which can be amplified when outer-sensor blips are treated as immediate left turns.
- The team considered buffering the last 20 sensor readings and deciding from an average or dominant value.
- Research on similar line-maze robots consistently favored fast segment tracking plus a short stateful node-inspection phase over long averaging windows.

## Options Considered
| Option | Pros | Cons |
|--------|------|------|
| Hybrid segment-following plus node FSM | Separates straight tracking from node decisions, reduces phantom-turn sensitivity, preserves left-hand-rule logic, enables explicit turn commitment and stable reacquisition | Requires refactoring `working_code.ino` and retuning multiple constants |
| Long-window averaging on current logic | Smaller code change, easy to explain, can smooth some noisy samples | Adds lag at intersections, can miss brief left branches, does not address fragile turn execution or stale latched decisions |
| Hardware-first correction with wheel encoders or tighter motor feedback | Best long-term fix for motor bias, improves straight tracking fundamentally | Larger hardware and software scope, slower to deliver, does not by itself solve brittle node classification |

## Decision
Chosen: **Hybrid segment-following plus node FSM**, because it addresses both classes of failures at the same abstraction level: continuous steering handles straight-line reliability, while explicit node inspection and committed turns handle left-hand maze decisions without relying on stale latch state or long averaging windows.

## Consequences
- `working_code.ino` will gain internal state for sensor snapshots, node scanning, turn commitment, and recovery while remaining self-contained in one file.
- Host-side regression tests will need to cover phantom-left detection, missed-left node handling, and configurable U-turn direction.
- Long averaging over 20 samples may still be used as short debounce inspiration, but not as the main control method.
- If straight drift remains after the logic redesign, a static motor trim or later hardware feedback step may still be needed.
