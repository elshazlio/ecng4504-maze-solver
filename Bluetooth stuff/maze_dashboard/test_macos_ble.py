import asyncio
import unittest


class ResolveBleDeviceForAddressTests(unittest.TestCase):
    def test_builds_ble_device_from_corebluetooth_peripheral(self) -> None:
        from macos_ble import resolve_ble_device_for_address

        class FakeIdentifier:
            def UUIDString(self) -> str:
                return "2D142714-E7A9-D5E8-58AD-668100AEFC00"

        class FakePeripheral:
            def name(self) -> str:
                return "BT05"

            def identifier(self) -> FakeIdentifier:
                return FakeIdentifier()

        class FakeCentralManager:
            def retrievePeripheralWithAddress_(self, address: str):
                if address == "78:04:73:16:19:C7":
                    return FakePeripheral()
                return None

        class FakeManagerDelegate:
            def __init__(self) -> None:
                self.central_manager = FakeCentralManager()

            async def wait_until_ready(self) -> None:
                return None

        device = asyncio.run(
            resolve_ble_device_for_address(
                "78:04:73:16:19:C7",
                platform="darwin",
                manager_cls=FakeManagerDelegate,
            )
        )

        self.assertIsNotNone(device)
        assert device is not None
        self.assertEqual(device.address, "2D142714-E7A9-D5E8-58AD-668100AEFC00")
        self.assertEqual(device.name, "BT05")


if __name__ == "__main__":
    unittest.main()
