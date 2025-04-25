import time
import Gate
from communication_protocol.communication_module import CommunicationModule
from communication_protocol.communication_protocol import DeviceMessage
from communication_protocol.message import accept_message
from message import add_tag_response, on_click_request, on_read_request
from sensor import Sensor
import uasyncio as asyncio  # type: ignore
from machine import Pin  # type: ignore


class RFID:
    def __init__(
        self,
        communication_module: CommunicationModule,
        sensor: Sensor,
        gate: Gate,
        settings: dict,
    ):
        self.communication_module: CommunicationModule = communication_module
        self.sensor: Sensor = sensor
        self.gate: Gate = gate
        self.settings: dict = settings
        self.opto_pin = Pin(0, Pin.IN, Pin.PULL_UP)  # D3

    async def start(self):
        while True:
            await self._check_button()
            if not self.gate.in_process():
                await self._check_sensor()
            await self._check_message()
            await asyncio.sleep(0.1)

    async def _check_message(self):
        message: DeviceMessage | None = self.communication_module.get_message()
        if not message:
            return
        method_name: str = self.communication_module.extract_method_name_from_message(
            message
        )
        method = getattr(self, method_name, None)
        if not method:
            print(f"Method {method_name} not found")
            return
        response = await method(message)
        self.communication_module.send_message(response)

    async def _check_button(self):
        if not self.opto_pin.value():
            await asyncio.sleep(0.03)
            if not self.opto_pin.value():
                self.communication_module.send_message(
                    on_click_request(self.communication_module.get_mac())
                )
                await self.gate.access(self.settings["open_gate_timeout"])

    async def _check_sensor(self) -> None:
        success, uid = self.sensor.read_uid()
        if not success:
            return
        self.communication_module.send_message(
            on_read_request(self.communication_module.get_mac(), uid)
        )

    ############################# REQUEST #############################
    async def _add_tag_request(self, message: DeviceMessage) -> DeviceMessage:
        start_time = time.ticks_ms()
        while True:
            try:
                success, uid = self.sensor.read_uid()
                if success:
                    break
            except Exception as e:
                pass
            current_time = time.ticks_ms()
            if (
                time.ticks_diff(current_time, start_time)
                >= self.settings["add_tag_timeout"]
            ):
                break
            await asyncio.sleep(0.01)
        return add_tag_response(
            self.communication_module.get_mac(),
            uid,
            message.payload["name"],
            message.message_id,
        )

    async def _access_granted_request(self, message: DeviceMessage) -> DeviceMessage:
        time = (
            message.payload["open_gate_timeout"]
            if "open_gate_timeout" in message.payload
            else self.settings["open_gate_timeout"]
        )
        asyncio.create_task(self.gate.access(time))
        return accept_message(message)

    async def _access_denied_request(self, message: DeviceMessage) -> DeviceMessage:
        asyncio.create_task(self.gate.access_denied())
        return accept_message(message)

    ############################# RESPONSE #############################

    async def _set_settings_response(self, message: DeviceMessage) -> DeviceMessage:
        return accept_message(message)
