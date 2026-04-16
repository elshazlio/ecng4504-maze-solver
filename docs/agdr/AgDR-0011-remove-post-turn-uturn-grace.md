---
id: AgDR-0011
timestamp: 2026-03-18T00:00:00Z
agent: cursor
model: GPT-5.4
trigger: user-prompt
status: executed
---

# Remove Post-Turn U-Turn Grace

> In the context of the current `working_code.ino` Phase 0 maze follower, facing a robot that now handles many junctions better but fails to recover when it misses a short dead-end branch, I decided to remove the global post-turn grace window that blocks dead-end U-turn classification and return to pure dead-end confirmation timing to restore prompt recovery, accepting some risk that the earlier false post-turn U-turn symptom could reappear.

## Context
- `AgDR-0010` added a global post-turn grace window to suppress false `DEC_U` decisions shortly after completed tank turns.
- Hardware testing now shows a different failure mode: after missing a short left dead-end, the robot continues straight into line loss and does not recover promptly.
- The reference sketch does not include a post-turn grace window and reportedly does not miss recovery at this dead end.
- A fuller turn/follow state machine would separate post-turn settle logic from genuine dead-end handling more cleanly, but that is larger than needed for this pass.

## Options Considered
| Option | Pros | Cons |
|--------|------|------|
| Remove the global post-turn grace and rely on `DEAD_END_CONFIRM_MS` alone | Smallest code change, restores prompt dead-end recovery, matches the simpler reference behavior more closely | May reintroduce some false post-turn `DEC_U` calls on noisy exits |
| Keep the grace window and only broaden left-branch detection | Preserves the recent false-U-turn protection | Does not address the reported failure where the robot already missed the branch and then cannot recover cleanly |
| Replace the grace heuristic with a fuller movement state machine | Most structured long-term solution | Much larger change and higher regression risk for the current sketch |

## Decision
Chosen: **Remove the global post-turn grace and rely on `DEAD_END_CONFIRM_MS` alone**, because the currently observed hardware failure is not just a missed branch but a failure to recover once the line is lost, and restoring immediate confirmed dead-end handling is the smallest change that directly targets that regression.

## Consequences
- Genuine dead ends should trigger `DEC_U` again as soon as sustained no-line loss passes `DEAD_END_CONFIRM_MS`.
- Missed short dead-end branches should be less likely to devolve into a long blind drive and uncontrolled spin.
- The earlier false-U-turn symptom after sharp turns may need to be re-tested on hardware and could require a more specific future fix if it returns.
