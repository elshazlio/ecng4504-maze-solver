---
id: AgDR-0039
timestamp: 2026-04-06T12:00:00Z
agent: cursor
model: gpt-5.2
trigger: user-prompt
status: executed
---

# Dynamic training record resolution (candidates + movement witness)

> In the context of maze training runs where junction exit sensing is noisy but driving is already tuned and must not change, facing ambiguous recorded exit topology, I decided to defer committing each training node to RAM until after the same physical maneuver completes, using burst-sampled exit hypotheses plus a short post-turn sensor witness and vote-weighted scoring to pick one stored mask, accepting extra RAM and a short delay before the path string grows.

## Context
- Recorded data (`exitsRecorded[]`, `rawPath[]`) should reflect the true junction geometry more reliably than a single probe snapshot.
- Line following, probe timings, speeds, and left-hand turn selection for **motion** must remain identical to the current sketch’s behavior.

## Options Considered
| Option | Pros | Cons |
|--------|------|------|
| **A. Multi-sample burst at probe stops only** | Simple; no motion change; captures noise distribution | Does not use “movement” to disambiguate |
| **B. Deferred record + witness window + votes** | Uses post-turn observations; keeps control on first samples = old behavior | More state; record appears slightly after the turn |
| **C. Change probe distances / extra stop for resample** | Could reduce ambiguity | Violates “physical performance unchanged” |

## Decision
Chosen: **B**, because it keeps probe timing and motor commands bit-identical to the prior logic (first read in each burst matches the old single read), adds candidate hypotheses from paired burst samples, and resolves the stored mask after a bounded witness period using votes and outer-sensor hints—forced flush before the next junction so ordering stays coherent.

## Consequences
- `rawPath` / `exitsRecorded` update shortly after the turn, not atomically with `doTurn`, but before the next node is handled.
- RAM use increases slightly for candidate lists and witness flags.
- If witness is weak, resolution falls back to the primary mask from the first burst pair (same as the old single-snapshot mask).
