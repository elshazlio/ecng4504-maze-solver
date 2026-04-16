---
id: AgDR-0028
timestamp: 2026-03-24T12:00:00Z
agent: cursor
model: claude
trigger: user-prompt
status: executed
---

# Maze control dashboard: Python stack and BLE transport

> In the context of restoring a PC-side maze robot UI with BLE, auto-refresh, and path visualization, I decided to use **FastAPI + WebSockets + asyncio Bleak** in one self-contained module under `Bluetooth stuff/maze_dashboard/`, to achieve **live duplex control and parsing aligned with `working_code.ino`**, accepting **tighter coupling to asyncio** than a threaded Flask app would need.

## Context
- Course charter (`docs/project_description.md`) requires a Python app that commands TRAIN/SOLVE, shows real-time phase status, and visualizes the returned path.
- Firmware exposes HM-10 UART at 9600 baud with lines: `STATUS:…`, `PATH_RAW:` / `PATH_OPT:` / `RESULT:`, `NODE:n:turn`, and text banners.
- Prior `ble_log_receiver.py` was stubbed offline; the dashboard must **scan, connect, reconnect**, and stream logs to the browser.

## Options Considered

| Option | Pros | Cons |
|--------|------|------|
| **FastAPI + WebSocket + Bleak (async)** | One event loop for BLE and HTTP; efficient push to browser; fits Bleak’s async API | Slightly more setup than Flask; dev must use `uvicorn` |
| **Flask + threading + pyserial (USB dongle)** | Simple mental model if BLE appears as `/dev/tty.*` | Not true BLE on many setups; extra hardware; two stacks (serial vs BLE) |
| **Separate Bun server + Python BLE subprocess** | Matches older plan docs | User asked for a **Python** website; more moving parts |

## Decision
Chosen: **FastAPI + WebSocket + Bleak**, because it keeps BLE I/O and fan-out to clients on one asyncio loop, matches the firmware’s line-oriented protocol, and avoids adding a second language runtime for this deliverable.

## Consequences
- Run with: `uvicorn app:app --reload` from `maze_dashboard/` (after `pip install -r requirements.txt`).
- Browser connects via WebSocket for logs and SVG updates; commands go as JSON over the same socket.
- Tradeoff: requires `bleak` and Bluetooth permissions on the host OS; HM-10 must expose Nordic UART UUIDs (default for HM-10 firmware).
