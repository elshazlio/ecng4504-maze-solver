---
id: AgDR-0007
timestamp: 2026-03-18T00:00:00Z
agent: cursor
model: GPT-5.4
trigger: user-prompt
status: executed
---

# Minimal Left-Node Reliability Tuning

> In the context of an already partially working `working_code.ino` sketch for Phase 0, facing missed or late left turns at `T` and `+` intersections on the team maze, I decided to keep the existing follower structure and apply a minimal behavior refinement that broadens intersection classification for wide nodes, preserves pending node intent through brief all-black crossings, and trims turn-entry timing, to improve reliability without destabilizing the current 70%-effective behavior.

## Context
- Phase 0 in `project_description.md` requires one successful training traversal and one successful solving traversal, with wireless deferred.
- The current sketch already follows the line reasonably well, so a large rewrite would risk losing working behavior.
- The observed issue is specific: some left turns at `T` and `+` intersections are missed or entered too late.
- In the current sketch, `onCount == 5` becomes `DEC_E`, which is temporarily downgraded to straight travel during training, and committed turns include a forward creep before spinning.

## Options Considered
| Option | Pros | Cons |
|--------|------|------|
| Replace the follower with a new multi-state node handler immediately | Most structured long-term approach, closer to a full FSM | Too much change for a sketch that already works acceptably, higher regression risk |
| Keep the existing structure and minimally tune classification plus turn timing | Smallest risk, directly targets the reported failure, easy to tune on hardware | Less architecturally clean, still relies on heuristic thresholds |

## Decision
Chosen: **Keep the current structure and apply a minimal reliability tuning pass**, by extracting a small intersection-classification helper, remembering pending node intent across brief all-black spans, and reducing the pre-spin forward creep and intersection lockout, because this directly addresses the missed-left symptom while preserving the rest of the sketch's working behavior.

## Consequences
- Wide `+` intersections should stop being mistaken for straight-only travel when all sensors briefly see black.
- Left turns should begin earlier, reducing overshoot into the white area beside the branch.
- The sketch remains easy to compare against the previous working baseline because the overall control flow is unchanged.
- If this tuning is still insufficient on hardware, the next step should be a true follow/node/turn FSM rather than more ad-hoc condition growth.
