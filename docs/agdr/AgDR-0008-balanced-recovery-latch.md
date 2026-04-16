---
id: AgDR-0008
timestamp: 2026-03-18T00:00:00Z
agent: cursor
model: GPT-5.4
trigger: user-prompt
status: executed
---

# Balanced Recovery Latch For Junction Turns

> In the context of a partially working `working_code.ino` Phase 0 maze follower, facing missed or late turns caused by staggered sensor triggering, crooked approach angles, and forward momentum through intersections, I decided to use a short guarded recovery latch that preserves recent committed junction intent only while the post-junction reading is uncertain and releases on a clearly centered follow reading, to improve turn reliability while accepting heuristic thresholds and possible hardware retuning.

## Context
- The sketch already records a committed intersection intent, but currently reuses it only during full all-black crossings.
- Real hardware can produce staggered or partial sensor patterns after a junction because the five sensors do not trigger simultaneously.
- The robot must continue respecting left-hand following, so a confidently seen left should not be discarded too quickly.
- A full follower/node/turn state machine would be more structured, but is larger and riskier than needed for this pass.

## Options Considered
| Option | Pros | Cons |
|--------|------|------|
| Keep the current all-black-only latch and only retune timing constants | Smallest code change, easiest to compare with current behavior | Does not address messy partial post-junction readings, still likely to forget left too early |
| Use a balanced recovery latch during uncertain post-junction readings | Targets staggered sensors and momentum, preserves left-hand priority without overcommitting too long | Adds heuristic ambiguity rules and another timing window to tune |
| Use an aggressive sticky latch until a strong competing pattern appears | Most likely to catch late turns after overshoot | Higher risk of false turns and suppressing valid live corrections on noisy tracks |

## Decision
Chosen: **Use a balanced recovery latch during uncertain post-junction readings**, because it directly addresses the observed goldfish-memory behavior after junctions while still allowing clear live sensor patterns to retake control before the robot commits to a wrong branch.

## Consequences
- The robot should keep honoring a recent left or straight node intent through brief partial/noisy recovery readings after a junction.
- A clearly centered follow reading should release the latch so normal line following resumes promptly.
- Left-hand following remains preserved because new clear left evidence still outranks older non-left intent.
- The sketch stays heuristic; if hardware testing still shows inconsistent behavior, the next step should be a fuller state-machine rewrite rather than more ad-hoc latch growth.
