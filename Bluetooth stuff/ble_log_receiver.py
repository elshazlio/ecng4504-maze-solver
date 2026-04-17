"""
Stream telemetry from the robot over Bluetooth Classic SPP (ZS-040 / HC-05 on macOS).

Same serial device as maze_dashboard — pair the module in System Settings → Bluetooth,
then:

  export MAZE_BT_SERIAL=/dev/cu.HC-05-DevB
  export MAZE_BT_BAUD=9600
  python ble_log_receiver.py

Reuse last successful dashboard port:

  export MAZE_BT_SERIAL=last

(Run the dashboard once with a real path so maze_dashboard/bt_serial_last.json exists.)
"""
from __future__ import annotations

import json
import os
import sys
from pathlib import Path

SCRIPT_DIR = Path(__file__).resolve().parent
BT_SERIAL_LAST_PATH = SCRIPT_DIR / "maze_dashboard" / "bt_serial_last.json"


def emit(event_type: str, **payload: object) -> None:
    event: dict[str, object] = {"type": event_type, **payload}
    print(json.dumps(event), flush=True)


def emit_status(state: str, message: str | None = None, error: str | None = None) -> None:
    payload: dict[str, object] = {"state": state, "message": message or status_label(state)}
    if error:
        payload["error"] = error
    emit("status", **payload)


def status_label(state: str) -> str:
    if state == "connected":
        return "Serial connected"
    if state == "offline":
        return "Serial not configured or unavailable"
    return state


def _load_last_port() -> str | None:
    if not BT_SERIAL_LAST_PATH.is_file():
        return None
    try:
        data = json.loads(BT_SERIAL_LAST_PATH.read_text(encoding="utf-8"))
        p = str(data.get("port", "")).strip()
        if p.startswith("/dev/"):
            return p
    except (OSError, json.JSONDecodeError, TypeError, ValueError):
        pass
    return None


def resolve_port() -> str | None:
    raw = (os.environ.get("MAZE_BT_SERIAL") or os.environ.get("MAZE_SERIAL_PORT") or "").strip()
    if not raw:
        return None
    if raw.lower() == "last":
        return _load_last_port()
    return raw


def main() -> int:
    try:
        import serial
    except ImportError:
        emit_status(
            "offline",
            message="Install pyserial",
            error='pip install pyserial   (also in maze_dashboard/requirements.txt)',
        )
        return 1

    port = resolve_port()
    if not port:
        emit_status(
            "offline",
            message="Set MAZE_BT_SERIAL for ZS-040 / HC-05",
            error="Example: export MAZE_BT_SERIAL=/dev/cu.HC-05-DevB  (use ls /dev/cu.* after pairing)",
        )
        return 0

    try:
        baud = int(os.environ.get("MAZE_BT_BAUD", "9600") or 9600)
    except ValueError:
        baud = 9600

    if not Path(port).exists():
        emit_status("offline", message=f"Missing {port}", error="Pair the module and check /dev/cu.*")
        return 1

    try:
        ser = serial.Serial(port, baud, timeout=0.3)
    except Exception as e:
        emit_status("offline", message=f"Could not open {port}", error=str(e))
        return 1

    emit_status("connected", message=f"Reading {port} @ {baud} baud")
    try:
        while True:
            raw = ser.readline()
            if not raw:
                continue
            line = raw.decode("utf-8", errors="replace").rstrip("\r\n")
            emit("log", line=line)
    except KeyboardInterrupt:
        pass
    finally:
        try:
            ser.close()
        except Exception:
            pass
    emit_status("offline", message="Stopped")
    return 0


if __name__ == "__main__":
    sys.exit(main())
