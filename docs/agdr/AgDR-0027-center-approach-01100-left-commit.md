---
id: AgDR-0027
timestamp: 2026-03-19T22:00:16Z
agent: cursor
model: GPT-5.4
trigger: user-prompt
status: executed
---

# Use Centered Approach As 01100 Evidence

> In the context of `working_code.ino` left-junction classification, facing missed left Ts where the real sensor sequence can be `00100 -> 01100 -> 00100` and `s0` never fires, I decided to treat `01100` as a committed left junction when it immediately follows a clean centered approach sample, to achieve left-T visibility without globally forcing every isolated `01100` to turn left, accepting some residual risk of false left commits on certain center-to-left curve transitions.

## Context
- Current `narrowLeftStem` logic only accepts `01100` when `s0` is live, recently seen, or the post-left assist window is active.
- That creates a real classifier blind spot when hardware stays slightly right-of-center and never lights `s0` at a true left T.
- A blanket `01100 => DEC_SL` rule would fix the blind spot but would also reopen the broader "tight left curve looks like a junction" problem that earlier hardening changes were trying to avoid.

## Options Considered
| Option | Pros | Cons |
|-----|---|---|
| **Always classify `01100` as `DEC_SL`** | Fully removes the `s0` blind spot; very small code change | Highest false-left risk on tight curves and noisy left-leaning tracking |
| **Classify `01100` as `DEC_SL` only after an immediate centered `00100` approach** | Fixes the reported `00100 -> 01100 -> 00100` blind spot; keeps stale/free-floating `01100` stricter than blanket always-left | Still heuristic; can still over-commit if a curve presents the same transition |
| **Keep the existing `s0`/assist-only gate** | Lowest false-left risk; no new state | Leaves the reported junction literally unobservable when `s0` never fires |

## Decision
Chosen: **Classify `01100` as `DEC_SL` only after an immediate centered `00100` approach**, because it directly addresses the confirmed blind spot while preserving a meaningful context gate around `01100` instead of making every isolated `01100` a left turn.

## Consequences
- True left Ts can now commit left even when `s0` never appears.
- Existing tests that only prove stale, ungated `01100` should stay straight can remain meaningful if they do not include a centered approach immediately before the sample.
- Regression coverage must prove both sides: `00100 -> 01100` commits left, while stale standalone `01100` still stays straight.
