"""
Connect to HM-10 and receive logs from the Arduino (working_code.ino).
The Arduino sends sensor readings and status over Serial1 -> HM-10 UART -> BLE.

Run: python ble_log_receiver.py
"""
import json
import sys


def emit(event_type, **payload):
    event = {"type": event_type, **payload}
    print(json.dumps(event), flush=True)


def emit_status(state, message=None, error=None):
    payload = {"state": state, "message": message or status_label(state)}
    if error:
        payload["error"] = error
    emit("status", **payload)


def status_label(state):
    if state == "offline":
        return "BLE offline - HM-10 unavailable"
    return "BLE offline - HM-10 unavailable"


if __name__ == "__main__":
    emit_status(
        "offline",
        message="BLE offline - HM-10 unavailable",
        error="Temporary outage: suspected 5V overvoltage damage. Use USB serial telemetry.",
    )
    sys.exit(0)
