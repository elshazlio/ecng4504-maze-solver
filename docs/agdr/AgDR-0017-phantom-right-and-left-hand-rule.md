---
id: AgDR-0017
timestamp: 2026-03-19T12:00:00Z
agent: cursor
model: composer
trigger: user-prompt
status: executed
---

# Stricter Junction Detection and Left-Hand Rule Enforcement

> In the context of maze navigation in `working_code.ino`, facing phantom right turns, poor straight-line centering, and incorrect left-hand rule behavior at junctions, I decided to tighten junction detection and remove early right commits to achieve reliable left-hand rule following and fewer false turns, accepting that right-only junctions are now handled via curve-following rather than discrete node scan.

## Context
- Robot takes phantom right turns on curves and straights.
- Robot does not center itself well on straight paths.
- Robot detects hard lefts but sometimes turns right anyway.
- Robot gets lost in white space after wrong turns and spins until it finds the line.
- Left-hand rule is not followed correctly at junctions.

## Options Considered
| Option | Pros | Cons |
|-----|---|---|
| **Stricter junction triggers + remove early right commit** | Eliminates phantom rights from curves, enforces left-hand rule, improves centering | Right-only junctions no longer start node scan; robot follows curve instead of recording discrete R |
| **Add temporal debounce only** | Smaller change, might reduce glitches | Does not fix root cause: right+center triggers junction on curves |
| **Require 4+ sensors for junction** | Strong filter | May miss real junctions, too aggressive |

## Decision
Chosen: **Stricter junction triggers + remove early right commit**, because rightEdgeSeen+centerSeen alone is indistinguishable from a curve and was the primary cause of phantom rights. Requiring wideCenterSeen+edge or leftEdgeSeen+centerSeen ensures we only start node scan at real junctions.

## Consequences
- `shouldStartNodeScan`: no longer triggers on rightEdgeSeen+centerSeen alone; wideCenterSeen alone (centered-on-straight) no longer triggers.
- `initialNodeDecision`: wideCenterSeen with edge returns left if left seen, else straight.
- Removed early return in `handleNodeScan` when right+center seen without foundLeft.
- `chooseNodeDecisionFromEvidence`: right requires 3+ evidence and !foundLeft.
- `FOLLOW_SOFT_THRESHOLD` lowered from 700 to 500 for better centering.
- Right-only junctions: robot follows the curve via chooseFollowDecision; no discrete R recorded in path.
