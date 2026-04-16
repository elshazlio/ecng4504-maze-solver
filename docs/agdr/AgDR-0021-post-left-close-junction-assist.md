---
id: AgDR-0021
timestamp: 2026-03-19T20:19:12Z
agent: cursor
model: GPT-5.4
trigger: user-prompt
status: executed
---

# Post-Left Close Junction Assist

> In the context of `working_code.ino` maze navigation, facing a repeatable miss where the robot completes one left turn and then drives straight through the next very short-distance preferred-left junction, I decided to add a short post-left close-junction assist window so inner-left plus center evidence can still commit left immediately after a left turn, to achieve reliable left-hand-rule behavior on closely spaced nodes, accepting a small amount of extra post-turn state and bounded risk of post-left overcommit.

## Context
- Hardware now behaves consistently and correctly on most of the maze, isolating one remaining repeatable failure.
- The missed branch occurs specifically after a successful left turn when the next left-preferred junction arrives very quickly.
- Current `narrowLeftStem` logic requires `s0` or recent `s0` evidence before treating `s1+s2` as a committed left junction.
- On a very short post-left segment, the next branch can plausibly appear as `01100` before `s0` ever lights.

## Options Considered
| Option | Pros | Cons |
|-----|---|---|
| **Short post-left close-junction assist window** | Targets the exact geometry; preserves current stricter global left detection; smallest logic change that matches the observed failure | Adds more post-turn state and timing to tune |
| Always treat `01100` as a left junction | Very simple; fixes any branch that never reaches `s0` | Higher risk of phantom lefts on tight curves or noisy left-leaning tracking |
| Motion-only retune (`JUNCTION_CREEP_MS`, speed, settle) | No new decision-state logic | Does not fix the logical blind spot when `01100` is explicitly rejected |

## Decision
Chosen: **Short post-left close-junction assist window**, because the failure is context-specific and the most robust fix is to preserve strict global junction rules while selectively allowing immediate post-left `01100` evidence to commit the next left.

## Consequences
- Closely spaced left-after-left nodes can commit even if `s0` is not seen first.
- Global false-left resistance on ordinary curves remains stricter than with a blanket `01100 => DEC_SL` rule.
- The added assist window must expire or clear on stable straight reacquisition so it does not leak into unrelated segments.
