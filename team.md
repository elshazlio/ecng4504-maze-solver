# Team 5: ECNG 4504

**Course:** Embedded Systems for Wireless Communications · Spring 2026  
**Institution:** American University in Cairo · School of Sciences and Engineering

## Members


| Name                    | ID        | Email                                                       |
| ----------------------- | --------- | ----------------------------------------------------------- |
| Abanoub Emad Said Kamel | 900201630 | [abanoubemad@aucegypt.edu](mailto:abanoubemad@aucegypt.edu) |
| John Kamal              | 900212208 | [johnk.kamal@aucegypt.edu](mailto:johnk.kamal@aucegypt.edu) |
| Mina Yasser Gerges      | 900214039 | [Minayasser@aucegypt.edu](mailto:Minayasser@aucegypt.edu)   |
| Omar ElShazli           | 900201477 | [Elshazlio@aucegypt.edu](mailto:Elshazlio@aucegypt.edu)     |


---

## Contributions (aligned with this repository)

### Mina Yasser Gerges

Led the **on-robot firmware** in `working_code.ino`: maze **training** and **solving** behavior (map build, optimal path execution, left-hand-rule navigation), sensor and motion tuning, and calibration for reliable runs. Integrated **BLE (HM-10)** with the main control flow and supported **robot-side** wireless testing so the PC tools could drive and monitor the vehicle end-to-end. Also led **hardware assembly** and **reassembly** after numerous failures during bring-up, keeping the mechanical and electrical stack usable for continued software iteration.

### Abanoub Emad Said Kamel

Led **hardware**: component selection and the **bill of materials** (`component_list(1).xlsx`), wiring, and motor/sensor interfacing. Prepared the **schematic** and carried the design through **PCB layout and routing**—trace planning, **footprint placement**, **ground and power planes**, clearance and **design-rule** checks for a board that could be fabricated and assembled reliably—with the schematic exported as `**PCB Schematic.png`** in this repo.

Led **3D mechanical design in Autodesk Fusion** (**Fusion 360**): chassis and related parts, **parametric modeling**, and **assembly** so motors, sensors, and the PCB stack fit the platform; sources are in `**3d design/`** (Fusion archive per `[README.md](README.md)`).

### John Kamal & Omar ElShazli

Led **host-side software** that talks to the robot over **wireless UART** (same framing as USB), with a **dual transport** so the laptop stack could follow the hardware path from **HM-10 (BLE)** to **ZS-040 (Bluetooth Classic serial / SPP)** — often on an **HC-05**-class breakout — without rewriting the application. **Transport selection** is by environment: leave `**MAZE_BT_SERIAL` unset** for **Bleak + HM-10**, or set it to a `**/dev/cu.*`** device for **pyserial + ZS-040**.

**Maze dashboard (`Bluetooth stuff/maze_dashboard/`):**

- **FastAPI** backend with **WebSockets** so the browser gets **live telemetry** (logs, phase, paths, pause state) as the robot prints lines.
- **HM-10 path:** **Bleak** scans and connects to the advertised module, discovers **Nordic UART Service** or **HM-10-style FFE1** characteristics, and bridges notify/write to the same command and log pipeline the firmware already uses on `Serial1`.
- **ZS-040 / HC-05 path:** when the vehicle moved to a **paired Bluetooth Classic** module, the host opens a **serial port** (`pyserial`) on macOS `**/dev/cu.*`** instead of BLE — driven by `**MAZE_BT_SERIAL**` and `**MAZE_BT_BAUD**`, with optional reuse of the last good port via `**bt_serial_last.json**` so development sessions did not require rediscovering the device name every time.
- **Browser UI:** command buttons (`TRAIN`, `START`, etc.), scrollable log with tagged lines (paths, nodes, turns), and an **SVG path preview** built from firmware path strings and solve/training turn lines (turtle geometry, including visualization fixes for forced vs intersection legs).
- **Run archive** (saved sessions) and supporting helpers such as `**ble_discover.py`** to capture UUIDs and a stable BLE address for faster reconnects.

**Log viewer (`Bluetooth stuff/log-viewer/`) and streaming:**

- Bun-based **web** UI served alongside small Node/Bun glue, with `**ble_log_receiver.py`** sharing the same `**MAZE_BT_SERIAL**` / `**MAZE_BT_BAUD**` convention as the dashboard so logs could be tailed and debugged outside the full stack when needed.
- Supporting scripts `**ble_discover.py**`, `**ble_scan.py**`, and `**ble_log_receiver.py**`, plus **end-to-end testing** of dropouts, partial lines, and reconnect behavior against the real link.

**HM-10 → ZS-040 shift and challenges:**

- **Different stack on the PC:** BLE used **GATT notify/write** and scan/pair semantics; classic Bluetooth used **macOS pairing in System Settings** and **serial port enumeration**, which changed how we diagnosed “no data” (wrong `/dev/cu.*` outlet vs wrong baud vs module not connected as SPP).
- **Operational friction:** finding the correct **callout serial device**, matching **baud** to firmware (`MAZE_BT_BAUD`), and avoiding stale **BLE** assumptions when the robot no longer exposed a GATT UART service.
- **Reliability:** reconnection after sleep or unplug, **line-boundary** issues in streaming text, and keeping **one mental model** (command + log stream) across two transports so `**working_code.ino`** did not need parallel host protocols.

---

*Repository deliverables (firmware, PC apps, BOM, schematic image, mechanical CAD) are listed in `[README.md](README.md)`.*