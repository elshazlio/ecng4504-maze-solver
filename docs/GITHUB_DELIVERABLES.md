# GitHub contents vs. course charter deliverables

This file ties the **[project charter](project_description.md)** (“Github repository link that has the following content…”) to **folders and files in this repository**. Use it when preparing Phase 0 / 1 / 2 zip submissions so the link matches what instructors expect.

## Phase 0 — Initial demonstration

Charter asks for a **GitHub repository link** containing:

| Deliverable | In this repo |
|-------------|----------------|
| **(a) MCU code** | Arduino sketches at repository root (e.g. `working_code.ino`, `mina_final_code.ino`, `probably_final_code.ino`). Supplementary C++ tests under `tests/` where applicable. |
| **(b) Initial software architecture and hardware schematics (KiCAD)** | `Architecture Diagram Only.docx`, `PCB Schematic.png`, `Maze.png`, `component_list(1).xlsx`, materials under `3d design/`, and any further diagrams under `docs/`. |

Also required in the **zip** (not necessarily all in Git, but often mirrored): Excel components list, demo video, contribution write-up, `team.txt`, etc.—see charter.

**Note:** Phase 0 states wireless communication is **not** required; BLE/Python tooling may still be present for team use.

## Phase 1 — Final demonstration

| Deliverable | In this repo |
|-------------|----------------|
| **(a) MCU code** | Same as Phase 0; keep the branch/tag used for the demo identifiable (commit message or release). |
| **(b) Python script code** | `Bluetooth stuff/maze_dashboard/` (main FastAPI application), plus `Bluetooth stuff/log-viewer/` if used as part of the laptop workflow. |
| **(c) Updated software architecture and hardware schematics (KiCAD)** | Updated diagrams and schematics in `docs/`, `Architecture Diagram Only.docx`, `PCB Schematic.png`, `3d design/`, etc., as revised for Phase 1. |

Document **architecture or wiring changes** in `docs/` (and/or AgDRs in `docs/agdr/`) so “any change… shall be documented along with the reason” is satisfied.

## Phase 2 — Product / PCB

| Deliverable | In this repo |
|-------------|----------------|
| **KiCAD project files, Gerbers, BOM, FSM, 3D screenshots, manufacturer screenshot** | Typically under `3d design/`, `docs/`, and any `*.kicad_*` / Gerber folders you add. Expand this section as files are committed. |

## Maintenance

When you add KiCAD projects, Gerbers, or new Python modules, update this file and the root **README** table so reviewers can find them in one pass.
