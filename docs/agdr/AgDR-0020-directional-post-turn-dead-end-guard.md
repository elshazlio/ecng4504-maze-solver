---
id: AgDR-0020
timestamp: 2026-03-19T19:42:17Z
agent: cursor
model: gpt-5.3-codex-high
trigger: user-prompt
status: executed
---

# Directional Post-Turn Dead-End Guard

> In the context of `working_code.ino` maze navigation, facing a first-junction right-turn regression where brief post-turn line-loss triggers an immediate U-turn back to start, I decided to add a directional post-turn dead-end guard to preserve turn reacquire behavior before classifying dead-end, to achieve robust first-turn completion and avoid false reversal, accepting a short delay before true dead-end classification immediately after a turn.

## Context
- Hardware behavior now shows: on the first right turn, the robot starts turning correctly, then triggers `DEC_U` and returns to the start bubble.
- Prior decisions already documented that removing post-turn guarding can reintroduce false post-turn U-turn behavior.
- Global dead-end timing changes alone risk hurting true dead-end recovery across the whole maze.
- A fix should be targeted to post-turn transition windows, not all line-loss events.

## Options Considered
| Option | Pros | Cons |
|-----|---|---|
| **Directional post-turn dead-end guard (chosen)** | Targets the exact failure mode; keeps normal dead-end behavior elsewhere; preserves intended turn direction while reacquiring line | Adds controller state and a short intentional dead-end delay after turns |
| Increase `DEAD_END_CONFIRM_MS` globally | Very small code change; easy to reason about | Slows true dead-end recovery everywhere and does not directly encode turn context |
| Increase right-turn align timeout only | May improve some right-turn reacquire cases | Asymmetric tuning; can overspin; still allows false dead-end classification immediately after timeout |

## Decision
Chosen: **Directional post-turn dead-end guard**, because this failure is a context-specific transition bug (turn exit reacquire) and the most solid fix is to preserve directionally consistent recovery through brief no-line windows before allowing dead-end classification.

## Consequences
- Brief no-line intervals immediately after committed turns continue directional reacquire instead of escalating to `DEC_U`.
- True dead ends remain available after the guard window expires.
- A new guard window constant becomes part of turn tuning and should be revalidated on hardware if maze surface contrast changes.
