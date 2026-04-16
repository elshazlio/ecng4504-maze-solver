# ECNG 4504 — Autonomous maze-learning & maze-solving robot

**American University in Cairo · School of Sciences and Engineering**  
**ECNG 4504 — Embedded Systems for Wireless Communications · Spring 2026**

Course project: an **autonomous robotic vehicle** that **learns** an unknown maze during training (internal map), then **computes and executes an optimal path** to solve it. The PC talks to the robot over **BLE**; a **Python** application issues training/solve commands, receives status and paths, and supports visualization and logging—per the official **[project charter](docs/project_description.md)**.

## Repository layout (submission-oriented)

| Charter expectation | Where it lives in this repo |
|---------------------|-----------------------------|
| **MCU code** (C/C++) | Arduino sketches at repo root (e.g. `working_code.ino`, `mina_final_code.ino`, `probably_final_code.ino`, `reference_code_dont_touch.ino`) and related material under `tests/` |
| **Python host application** | `Bluetooth stuff/maze_dashboard/` (FastAPI + WebSockets + BLE), helpers such as `ble_discover.py` |
| **HM-10 / UART log viewer (optional tooling)** | `Bluetooth stuff/log-viewer/` (Bun) |
| **Architecture & hardware documentation** | `docs/`, diagrams such as `Architecture Diagram Only.docx`, `PCB Schematic.png`, assets under `3d design/`, `Maze.png`, `component_list(1).xlsx` |
| **Team / course metadata** | `team.txt` (names, IDs, emails, team number per charter) |
| **Decision history (engineering notes)** | `docs/agdr/` |

**How this maps to Phase 0 / 1 / 2 GitHub deliverables** (MCU, Python, KiCAD updates, etc.) is summarized in **[docs/GITHUB_DELIVERABLES.md](docs/GITHUB_DELIVERABLES.md)**.

## Run the PC-side apps locally

See **[DEV-SERVERS.md](DEV-SERVERS.md)** for copy-paste commands (log viewer + maze dashboard).

## Clone & setup

```bash
git clone https://github.com/elshazlio/ecng4504-maze-solver.git
cd ecng4504-maze-solver
```

Create the Python venv for the dashboard (once):

```bash
cd "Bluetooth stuff/maze_dashboard"
python3 -m venv .venv && source .venv/bin/activate && pip install -r requirements.txt
```

---

*Repository name on GitHub: **ecng4504-maze-solver** — ECNG 4504 maze **learning** + **solving** as defined in the course charter.*
