# Phased Implementation Plan (Arduino Mega + HM-10 + 5-Eye IR + L298N + 4 DC Motors)

## 1) Project Context and Constraints

- **MCU:** Arduino Mega 2560
- **Wireless:** HM-10 BLE 4.0 module (UART to Mega) - temporarily offline after suspected 5V overvoltage damage
- **Sensing:** 5-eye IR line sensor array
- **Actuation:** L298N motor driver + 4 yellow DC motors
- **PC side:** Python 3 already installed
- **Course constraints:** MCU code in C/C++, BLE link to PC app, autonomous training + solving behavior
- **Current temporary status:** BLE is disabled in the sketch and helper tools until the replacement HM-10 arrives; use USB serial telemetry for tuning in the meantime

---

## 2) Extra Software Prerequisites (Beyond Python 3)

## Required

1. **Arduino IDE 2.x** (or `arduino-cli`)
  - Install AVR core ("Arduino AVR Boards")
  - Needed to build/upload firmware to Arduino Mega
2. **USB-to-Serial driver** (only if your Mega clone needs it)
  - Typical chipsets: CH340 or CP210x
  - Verify serial port appears on macOS after connection
3. **Python packages**
  - `pyserial` (serial debugging and optional UART tooling)
  - `bleak` (BLE communication with HM-10 from laptop, temporarily unused until replacement hardware arrives)
  - `matplotlib` (path/maze visualization)
  - `networkx` (graph and shortest-path operations)
  - `pytest` (automated tests for planner/protocol)
4. **Git + GitHub account**
  - Required for collaboration and deliverables
5. **KiCad**
  - Required for schematics/PCB deliverables in later phase

## Recommended

1. **BLE debugging app** (LightBlue or nRF Connect)
  - Useful again after the replacement HM-10 is installed; currently blocked by the temporary BLE outage
2. **Serial monitor tool** (Arduino Serial Monitor or CoolTerm)
  - Faster telemetry debugging during motor/sensor tuning
3. **Diagram tool** (draw.io/Figma/Luidchart)
  - For architecture, FSM, and reporting diagrams

## Python package install command

```bash
python3 -m pip install pyserial bleak matplotlib networkx pytest
```

---

## 3) Firmware and PC Module Breakdown

## Firmware (Arduino Mega)

- `board_init`: pin mapping, timers, UART init, startup checks
- `motor_driver`: PWM + direction control for left/right motor groups
- `sensor_reader`: raw IR sampling and calibration normalization
- `line_controller`: line position error and steering correction logic
- `intersection_detector`: detect node/intersection events
- `maze_mapper`: store discovered nodes/edges during training run
- `path_planner`: shortest path generation from mapped graph
- `solver_executor`: execute computed path physically on track
- `ble_link`: UART protocol handling to/from HM-10 (temporarily disabled in firmware during the outage)
- `robot_fsm`: high-level states (Idle, Train, Plan, Solve, Done, Error)
- `telemetry_logger`: compact status/event output for debug and PC app

## PC (Python)

- `ble_client`: discovery/connect/read/write/reconnect logic (temporarily unavailable until replacement hardware arrives)
- `command_console`: send commands (`train`, `solve`, `stop`, `reset`)
- `status_monitor`: live stage/status/error output
- `maze_visualizer`: display mapped maze and final chosen route
- `session_logger`: save run metadata and final path per trial
- `protocol_codec`: encode/decode command and telemetry packets

---

## Known Device Addresses

| Device | Address (macOS UUID) | Notes |
|--------|----------------------|-------|
| HM-10 (HMSoft) | `07B13CAC-B95B-5DB8-18DF-C4C18A5AECE1` | Temporarily offline after suspected 5V overvoltage damage; keep address for the replacement bring-up |