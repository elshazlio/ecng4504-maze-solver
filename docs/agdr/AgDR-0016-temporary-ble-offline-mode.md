---
id: AgDR-0016
timestamp: 2026-03-19T00:00:00Z
agent: cursor
model: GPT-5.4
trigger: user-prompt
status: executed
---

# Use A Temporary BLE Offline Mode

> In the context of the maze car firmware and support tooling, facing a burned-out HM-10 module and a temporary loss of Bluetooth hardware, I decided to use a compile-time BLE offline mode to achieve safe firmware behavior and accurate tooling/docs during the outage, accepting a small restoration step later instead of hard-removing BLE support or leaving stale HM-10 behavior in place.

## Context
- The current HM-10 module is unavailable after suspected 5V overvoltage damage.
- `working_code.ino` still initializes `Serial1` and mirrors telemetry to the BLE path.
- The repository also contains BLE scripts and a local log viewer that currently assume HM-10 availability.
- The outage is temporary; the codebase should remain easy to restore once replacement hardware arrives.

## Options Considered
| Option | Pros | Cons |
|-----|---|---|
| **Compile-time BLE offline mode** | Safe temporary behavior, preserves USB serial telemetry, easy to reverse later, keeps BLE structure documented | Requires touching firmware plus support tools |
| **Hard-remove BLE code and tooling for now** | Smallest runtime surface, simplest immediate code | Loses restoration path clarity and risks rework later |
| **Docs-only update** | Fastest documentation pass | Leaves the sketch and tools behaving as if BLE still exists |

## Decision
Chosen: **Compile-time BLE offline mode**, because it reflects the real hardware outage accurately while preserving a clean path to re-enable HM-10 support later.

## Consequences
- `working_code.ino` will stop initializing or writing to `Serial1` while the outage flag is off.
- USB serial remains the active telemetry path for tuning and debugging.
- BLE helper scripts and the local viewer will report the temporary outage instead of attempting real connections.
