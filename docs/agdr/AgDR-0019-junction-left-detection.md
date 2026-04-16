---
id: AgDR-0019
timestamp: 2026-03-19T20:00:00Z
agent: cursor
model: composer
trigger: user-prompt
status: executed
---

# Broader Junction Detection for Left-Hand Rule

> In the context of 5-sensor line-maze training in `working_code.ino`, facing missed left turns because junction handling only activated when outer sensors `s0` or `s4` saw the line, I decided to treat additional geometric patterns as junctions and to resolve choices with `canLeft = s0 || s1` before straight then right, because many real T-approaches light `s1` and `s2` before `s0`, accepting that we must exclude symmetric “wide straight” `(s1,s2,s3)` without outers and keep outer-only-without-center as soft steer rather than `DEC_SL`.

## Context
- `if (leftPath || rightPath)` with `leftPath = s0` missed branches where only inner-left and center see the line.
- Left-hand rule requires the same priority once any junction is recognized: left before straight before right.

## Options Considered
| Option | Pros | Cons |
|-----|---|---|
| **Expand `atJunction` with inner patterns; `canLeft = s0 \|\| s1`; outer-only → soft L/R** | Matches hardware geometry; fixes common missed L | Needs care to avoid turning on wide straight |
| **Lower threshold / longer debounce only** | Small diff | Does not fix structural blind spot when `s0` never fires |
| **Require hardware move of leftmost sensor** | Physical reliability | User asked for firmware; extra assembly |

## Decision
Chosen: **Expand `atJunction` with inner patterns; `canLeft = s0 \|\| s1`; outer-only without center → `DEC_L` / `DEC_R`**, because the failure mode is logical (gate never opens) and inner sensors already carry left evidence at many Ts.

## Consequences
- Junction when `(s0&&s2)`, `(s4&&s2)`, `(s2&&s3&&s4)`, or **narrow left stem** `(s1&&s2&&!s3&&!s4)` **only if** `s0` is on or was seen within `LEFT_OUTER_SENSOR_MEMORY_MS` (reduces false sharp-left on tight single-line curves).
- Wide straight `(s1,s2,s3)` with no `s0`/`s4` stays in normal follow (no false `DEC_SL`).
- `s0` alone without `s1`/`s2` remains correction `DEC_L`, not a sharp junction.
- If a real left branch never passes under `s0`, increase `LEFT_OUTER_SENSOR_MEMORY_MS` or adjust sensor placement.
