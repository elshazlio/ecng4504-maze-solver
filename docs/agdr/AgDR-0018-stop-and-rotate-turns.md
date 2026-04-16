---
id: AgDR-0018
timestamp: 2026-03-19T18:00:00Z
agent: cursor
model: composer
trigger: user-prompt
status: executed
---

# Stop-and-Rotate for Turns and Junctions

> In the context of line-maze navigation in `working_code.ino`, facing sluggish asymmetric turns (one side stopped, one side at base speed) and unclear junction motion, I decided to use in-place spin with explicit motor stops before junction rotations and to replace soft L/R corrections with bounded spin-until-center, because that matches the requested behavior and avoids dragging a stalled wheel on low-friction surfaces, accepting slightly higher peak current draw and the need for timeout bounds to avoid overshoot loops.

## Context
- User observes slow “pivot” turns where one track runs and the other is stopped or very slow.
- Junction handling already used `spinLeft`/`spinRight` but mixed with forward motion and did not clearly separate “stop” from “rotate.”
- Left-hand rule priority at branches must remain correct.

## Options Considered
| Option | Pros | Cons |
|-----|---|---|
| **Stop motors, then in-place spin until center sensor hits line (with timeouts)** | Predictable heading change, no single-wheel drag, matches request | Higher instantaneous torque; may need tuning per surface |
| **Keep differential slow turns (one motor off)** | Smooth, continuous path | User-reported poor behavior; wheel stall/slip |
| **PID / proportional steering only** | Smooth on curves | Does not address discrete junction geometry; larger change |

## Decision
Chosen: **Stop motors, then in-place spin until center sensor hits line (with timeouts)**, because it directly fixes the reported motion and aligns with left-hand junction turns; differential L/R is replaced by the same spin-to-center pattern with a shorter timeout for small errors.

## Consequences
- `DEC_L` / `DEC_R` execute bounded `spinLeft` / `spinRight` until `SENS_PINS[2]` sees the line (or timeout).
- `DEC_SL` / `DEC_SR` / `DEC_U` use an explicit stop/settle phase before the spin phase.
- `decideDirectionFull` junction priority unchanged: left branch before straight before right.
