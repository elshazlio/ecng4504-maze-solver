---
id: AgDR-0038
timestamp: 2026-04-06T12:00:00Z
agent: cursor
model: unknown
trigger: user-prompt
status: executed
---

# Training resume after power loss via EEPROM checkpoint

> In the context of intermittent power loss during maze training, facing loss of in-RAM path state, I decided to persist partial training state in EEPROM (versioned struct with `trainingInProgress` and per-node metadata) updated after each stored node, to achieve resumable training after reboot, accepting extra EEPROM writes per junction and a one-time layout bump from v1 to v2.

## Context
- Training progress lives in `rawPath`, `exitsRecorded`, `segmentTimeMs`, and `rawPathLen`; previously only the completed run was written at `onTrainingComplete()`.
- Power loss clears RAM; PAUSE/RESUME only helps for intentional pause, not blackout.

## Options Considered
| Option | Pros | Cons |
|--------|------|------|
| **EEPROM v2 extended struct + save after each `recordNode`** | Survives power loss; single storage area; matches existing `savePathsToEEPROM` pattern | More EEPROM wear (bounded by junction count); struct grows (~1kB); must not treat checkpoint as solved path for START |
| **External SD / SPI flash module** | Huge space, fewer wear concerns | New hardware, wiring, code complexity |
| **Only periodic save (e.g. every N nodes)** | Fewer writes | Can lose last N junctions on outage |

## Decision
Chosen: **EEPROM v2 extended struct with checkpoint after each recorded node**, because it matches the Mega setup, needs no new parts, and maximizes recovery fidelity for competition-style runs.

## Consequences
- `EEPROM_VERSION` becomes 2; v1 blobs are migrated on read when checksum matches legacy layout.
- `LOAD` / `START` must ignore incomplete-training blobs (`trainingInProgress`).
- New Bluetooth command `RESUME_TRAIN` arms training without clearing RAM when a checkpoint exists.
