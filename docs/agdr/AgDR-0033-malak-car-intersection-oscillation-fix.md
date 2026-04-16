---
id: AgDR-0033
timestamp: 2026-04-02T12:00:00Z
agent: cursor
model: composer
trigger: user-prompt
status: executed
---

# Malak car_code: fix intersection metastable oscillation

> In the context of line-following maze code in `Malak's Stuff/car_code.ino`, facing repeated node handling and forward/stop hunting at turns, I decided to relax `isCentered`, add a post-node `nodeDetected` cooldown, and skip all-white dead-end probing while `inIntersection`, to achieve stable exit from junctions without re-triggering the same node, accepting a short blind window where a second junction very close in time could be missed.

## Context
- `isCentered` required far sensors white while inner sensors straddled the line; at real junctions far sensors often still see branch lines, so exit from `inIntersection` was unreliable or flickery.
- After a brief `isCentered` true, `nodeDetected` could fire again immediately because far sensors still saw the same junction.
- `handleDeadEndOrGap` ran before the intersection exit path and could add forward probes during white gaps, fighting `followLine` stop behavior.

## Options Considered
| Option | Pros | Cons |
|--------|------|------|
| **A — Relax `isCentered` only** | Simple, fixes stuck `inIntersection` | May still re-detect same node if far sensors see black |
| **B — Cooldown on `nodeDetected` only** | Blocks duplicate node cycles | Does not fix never clearing `inIntersection` if `isCentered` too strict |
| **C — A + B + skip dead-end probe while `inIntersection`** | Addresses all three mechanisms observed | Tunable `postNodeCooldownMs`; very tight adjacent nodes in time could be skipped once |

## Decision
Chosen: **Option C**, because the failure mode was a combination of strict exit criteria, immediate re-detection, and dead-end probing order; fixing one layer alone leaves the others able to oscillate.

## Consequences
- `isCentered` uses center + near sensors only for alignment exit.
- `ignoreNodeDetectionUntil` gates `nodeDetected` for `postNodeCooldownMs` after each completed node action (including dead-end 180).
- Dead-end forward probe does not run while `inIntersection` is true.
