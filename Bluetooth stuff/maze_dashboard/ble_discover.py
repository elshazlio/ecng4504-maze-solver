#!/usr/bin/env python3
"""
Print GATT services and characteristics, then optionally save UUIDs for the dashboard.

  MAZE_BLE_ADDR=AA:BB:CC:DD:EE:FF python ble_discover.py
  MAZE_BLE_NAME=HMSoft python ble_discover.py
  python ble_discover.py --save          # writes ble_uart_cache.json next to this script

The dashboard loads ble_uart_cache.json first and probes notify on those UUIDs.
"""
from __future__ import annotations

import argparse
import asyncio
import json
import os
import sys
from pathlib import Path
from typing import Any

from bleak import BleakClient, BleakScanner

DEFAULT_NAME_SUBSTRINGS: tuple[str, ...] = ("HMSOFT", "BT05")


def _adv_substrings() -> list[str]:
    raw = os.environ.get("MAZE_BLE_NAME", "").strip()
    if raw:
        return [s.strip().upper() for s in raw.split(",") if s.strip()]
    return list(DEFAULT_NAME_SUBSTRINGS)
# Max wait; stops as soon as a matching advertisement is seen (not a fixed sleep).
SCAN_TIMEOUT = float(os.environ.get("MAZE_BLE_SCAN_TIMEOUT", "12.0") or 12.0)
SCRIPT_DIR = Path(__file__).resolve().parent
CACHE_FILE = SCRIPT_DIR / "ble_uart_cache.json"


def adv_matches_name(d: Any, ad: Any) -> bool:
    subs = _adv_substrings()
    for n in (getattr(ad, "local_name", None), getattr(d, "name", None)):
        if not n:
            continue
        u = str(n).upper()
        if any(s in u for s in subs):
            return True
    return False


def _can_write(props: list[str]) -> bool:
    p = set(props)
    return "write" in p or "write-without-response" in p


def _can_notify(props: list[str]) -> bool:
    p = set(props)
    return "notify" in p or "indicate" in p


def pick_uart_recommendation(client: BleakClient) -> tuple[str, str] | None:
    """Prefer FFE1 single-pipe, else any char with write+notify, else Nordic split."""
    singles: list[tuple[str, list[str]]] = []
    nord_w = nord_n = None
    for svc in client.services:
        for c in svc.characteristics:
            u = c.uuid.lower()
            props = list(c.properties)
            if _can_write(c.properties) and _can_notify(c.properties):
                singles.append((c.uuid, props))
            if u == "6e400002-b5a3-f393-e0a9-e50e24dcca9e" and _can_write(c.properties):
                nord_w = c.uuid
            if u == "6e400003-b5a3-f393-e0a9-e50e24dcca9e" and _can_notify(c.properties):
                nord_n = c.uuid
    singles.sort(key=lambda x: (0 if "ffe1" in x[0].lower() else 1, x[0]))
    if singles:
        u = singles[0][0]
        return (u, u)
    if nord_w and nord_n:
        return (nord_w, nord_n)
    return None


async def run(addr: str | None, save: bool) -> None:
    if not addr or len(addr.replace(":", "")) < 10:
        print("Scanning for names matching", ",".join(_adv_substrings()), "...")
        chosen = await BleakScanner.find_device_by_filter(adv_matches_name, timeout=SCAN_TIMEOUT)
        if not chosen:
            print("No matching device before timeout. Advertised names (brief scan):")
            devices = await BleakScanner.discover(timeout=5.0)
            for d in devices:
                print(" ", d.address, repr(d.name))
            sys.exit(1)
        addr = chosen.address
        print("Using", addr, repr(chosen.name))
    else:
        addr = addr.replace("-", ":")
        print("Using address", addr)

    print("Connecting...")
    async with BleakClient(addr) as client:
        print("Connected.\n")
        for svc in client.services:
            print("Service", svc.uuid, "-", svc.description)
            for c in svc.characteristics:
                props = ", ".join(c.properties)
                print(f"  Char {c.uuid}  [{props}]  handle={c.handle}")
            print()

        rec = pick_uart_recommendation(client)
        if rec:
            w, n = rec
            print("Recommended for dashboard (write + notify):")
            print("  write_uuid :", w)
            print("  notify_uuid:", n)
            print()
            print("Or one-liner:")
            if w == n:
                print(f'  export MAZE_BLE_UART_UUID="{w}"')
            else:
                print(f'  export MAZE_BLE_WRITE_UUID="{w}"')
                print(f'  export MAZE_BLE_NOTIFY_UUID="{n}"')
            print()
            if save:
                CACHE_FILE.write_text(
                    json.dumps({"write_uuid": w, "notify_uuid": n}, indent=2) + "\n",
                    encoding="utf-8",
                )
                print("Saved", CACHE_FILE)
                print("The maze dashboard loads this file automatically on startup.")
        else:
            print("No UART-like characteristics found (need write + notify on same or split chars).")
            if save:
                sys.exit(1)


def main() -> None:
    p = argparse.ArgumentParser(description="Discover BLE GATT UART UUIDs for maze_dashboard.")
    p.add_argument(
        "--address",
        "-a",
        help="BLE address (else MAZE_BLE_ADDR or scan by MAZE_BLE_NAME)",
    )
    p.add_argument(
        "--save",
        action="store_true",
        help=f"Write {CACHE_FILE.name} for automatic use by app.py",
    )
    args = p.parse_args()
    addr = (args.address or os.environ.get("MAZE_BLE_ADDR", "")).strip()
    asyncio.run(run(addr or None, args.save))


if __name__ == "__main__":
    main()
