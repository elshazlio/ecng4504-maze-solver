from __future__ import annotations

import sys
from typing import Any

from bleak.backends.device import BLEDevice

try:
    from bleak.backends.corebluetooth.CentralManagerDelegate import CentralManagerDelegate
except ImportError:
    CentralManagerDelegate = None  # type: ignore[assignment]


def _looks_like_mac_address(address: str) -> bool:
    parts = address.strip().replace("-", ":").split(":")
    if len(parts) != 6:
        return False
    return all(len(part) == 2 for part in parts)


async def resolve_ble_device_for_address(
    address: str,
    *,
    platform: str | None = None,
    manager_cls: Any | None = None,
) -> BLEDevice | None:
    """
    Resolve a BLEDevice directly from a public MAC address on macOS.

    CoreBluetooth identifies peripherals by UUID, but BT05/HM-10 style modules are
    often easier to discover from `system_profiler` or module stickers via MAC.
    """
    if not address:
        return None

    platform = platform or sys.platform
    if platform != "darwin":
        return None

    normalized = address.strip().upper().replace("-", ":")
    if not _looks_like_mac_address(normalized):
        return None

    manager_cls = manager_cls or CentralManagerDelegate
    if manager_cls is None:
        return None

    manager = manager_cls()
    await manager.wait_until_ready()
    peripheral = manager.central_manager.retrievePeripheralWithAddress_(normalized)
    if peripheral is None:
        return None

    return BLEDevice(
        peripheral.identifier().UUIDString(),
        peripheral.name(),
        (peripheral, manager),
    )
