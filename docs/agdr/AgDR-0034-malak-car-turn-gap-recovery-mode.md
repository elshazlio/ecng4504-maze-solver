---
id: AgDR-0034
timestamp: 2026-04-02T12:15:00Z
agent: cursor
model: gpt-5.4
trigger: user-prompt
status: executed
---

# Malak car_code: turn-gap recovery mode

> In the context of `Malak's Stuff/car_code.ino`, facing the robot stopping when all three alignment sensors briefly see white while leaving a turn, I decided to remember the chosen node-exit direction and continue that recovery motion across white gaps to achieve stable turn completion, accepting a small amount of extra state and the need to tune recovery aggressiveness if the track is unusually tight.

## Context
- After a node decision, the code stays in `inIntersection` until the robot is centered on the new line.
- During that exit, `followLine()` stops the motors whenever `center`, `leftNear`, and `rightNear` are all white.
- Real turns often contain a brief white gap before the center sensor fully reacquires the new line.

## Options Considered
| Option | Pros | Cons |
|-----|---|---|
| **Coast forward on all-white while `inIntersection`** | Very simple, no extra state | Can overshoot left/right turns and ignore intended rotation direction |
| **Remember turn direction and keep recovering that way** | Matches the active maneuver, handles left/right differently, directly targets turn-gap loss | Adds state and needs a chosen recovery behavior |
| **Pure time-based grace window** | Easy to implement and tune initially | Fragile across battery level, floor friction, and different track geometries |

## Decision
Chosen: **Remember turn direction and keep recovering that way**, because the failure only happens while exiting a known maneuver and using that maneuver as context is more robust than blindly going straight or relying only on timing.

## Consequences
- A small `intersectionRecoveryMode` state is tracked across node exits.
- All-white during `inIntersection` no longer means "stop"; it means "continue the selected recovery motion."
- If recovery is too weak or too strong on hardware, the fine-turn behavior may need small tuning.
