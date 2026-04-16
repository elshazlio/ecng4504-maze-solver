---
id: AgDR-0026
timestamp: 2026-03-19T21:35:26Z
agent: cursor
model: gpt-5.3-codex-high
trigger: user-prompt
status: executed
---

# Extend Left Outer Sensor Memory Window

> In the context of `working_code.ino` junction classification, facing continued missed left turns where `01100` appears after `s0` drops, I decided to extend `LEFT_OUTER_SENSOR_MEMORY_MS` to preserve recent outer-left evidence longer and keep the narrow-left branch gate open, to achieve better left-junction capture without globally forcing `01100 => DEC_SL`, accepting some extra risk of left over-commit on tight left-leaning curves.

## Context
- Current logic only treats `01100` as a left junction when `s0` is live, recent, or a post-left assist is active.
- Hardware still reports missed lefts, indicating the existing `LEFT_OUTER_SENSOR_MEMORY_MS` (120 ms) can expire before `01100` is sampled.
- The user selected a balanced strategy over a global always-left `01100` rule.

## Options Considered
| Option | Pros | Cons |
|-----|---|---|
| **Always classify `01100` as `DEC_SL`** | Maximizes left capture when `s0` never fires | Higher phantom-left risk on tight curves/noise |
| **Increase `LEFT_OUTER_SENSOR_MEMORY_MS`** | Preserves existing guardrails while widening valid junction timing; minimal code change | Still depends on seeing `s0` at least briefly; needs tuning |
| **Telemetry only (no behavior change)** | Data-driven tuning with less immediate behavior risk | Does not resolve current missed-left behavior now |

## Decision
Chosen: **Increase `LEFT_OUTER_SENSOR_MEMORY_MS`**, because it directly targets the observed timing miss while keeping the stricter junction model that avoids broad `01100` over-classification.

## Consequences
- `01100` can still commit left for a longer window after a real `s0` glimpse.
- Existing protections against broad false-left commits remain in place.
- If misses continue, next step is either larger window tuning or temporary telemetry capture to measure real sensor timing.
