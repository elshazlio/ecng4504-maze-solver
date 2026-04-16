---
id: AgDR-0030
timestamp: 2026-03-31T12:00:00Z
agent: cursor
model: unknown
trigger: user-prompt
status: executed
---

# Pause/resume over BLE and dashboard BLE-first UX

> In the context of maze training/solving controlled from the web dashboard, facing the need to halt mid-run without losing place and to avoid queued commands before BLE links, I decided to add explicit `PAUSE`/`RESUME` commands on the robot with stage snapshotting plus UI that disables controls until BLE is connected, accepting extra firmware complexity and one more protocol surface to maintain.

## Context
- Users need to pause training or solving mid-run and continue from the same logical stage.
- Clicking Train/Solve while the backend is still scanning/connecting queued multiple commands that all executed at connect, which felt broken.
- The Phase pill stayed `UNKNOWN` because several `STAGE=` values from firmware did not map to a dashboard phase.

## Options Considered
| Option | Pros | Cons |
|--------|------|------|
| **A — PAUSE/RESUME commands + paused substates** | Clear semantics; works from any controller; robot holds motors off while preserving stage snapshot | More enum values and tests in firmware |
| **B — STOP only, restart run** | Simple | Loses position/context; poor UX for long runs |
| **C — Pause only in dashboard (no firmware)** | No Arduino changes | Cannot stop motors; unsafe and ineffective |

## Decision
Chosen: **Option A**, because motors must stop on the robot and the maze state (stage string, path buffers) must remain intact until the user explicitly resumes.

## Consequences
- Firmware implements `PAUSE`/`RESUME` with snapshot of `currentStage` before showing `PAUSED_*` stages.
- Dashboard sends PAUSE/RESUME; command buttons stay disabled until `ble_connected`.
- Phase pill removed from the header; internal `phase` remains for map logic. Stage mapping extended so telemetry-derived phase updates when possible.
