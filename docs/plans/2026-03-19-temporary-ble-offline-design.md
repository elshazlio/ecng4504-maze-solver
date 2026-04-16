# Temporary BLE Offline Mode Design

## Goal

Keep the car firmware, Bluetooth helper scripts, and local log viewer aligned with the current hardware reality while the HM-10 module is unavailable.

## Proposed Approach

Use a compile-time BLE availability flag in `working_code.ino` so the sketch keeps USB serial telemetry but does not initialize or write to `Serial1` during the outage. Keep the BLE-related code structure intact so the replacement module can be re-enabled by changing one flag instead of reconstructing earlier integration work.

Update the Bluetooth scripts and log viewer to present a clear temporary-outage state instead of trying to scan or connect to HM-10. This keeps the tooling honest and prevents wasted debugging effort on a module that is currently dead.

## Scope

- Firmware: disable HM-10 initialization and mirrored BLE telemetry
- Docs: mark the HM-10 entry and BLE workflow as temporarily unavailable
- Tools: make BLE scripts/viewer report outage status explicitly

## Non-Goals

- Permanent removal of BLE support
- Replacing Bluetooth with a different wireless transport
- Solving the current junction/phantom-turn regression in this same change

## Validation

- Host-side firmware tests still compile and pass
- BLE scripts show a temporary outage message without trying to connect
- Viewer status text reflects the offline state
