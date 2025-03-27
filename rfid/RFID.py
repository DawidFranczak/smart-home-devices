import time
import Gate
from communication_protocol.communication_module import CommunicationModule
from communication_protocol.communication_protocol import DeviceMessage
from communication_protocol.message import accept_message
from message import add_tag_response, check_uid_request
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
            self._check_button()
            self._check_sensor()
            self._check_message()
            await asyncio.sleep(0.1)

    def _check_message(self):
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
        response = method(message)
        self.communication_module.send_message(response)

    def _check_button(self):
        if not self.opto_pin.value():
            time.sleep(0.03)
            if not self.opto_pin.value():
                self.gate.access(self.settings["open_gate_timeout"])

    def _check_sensor(self) -> None:
        success, uid = self.sensor.read_uid()
        if not success:
            return
        self.communication_module.send_message(
            check_uid_request(self.communication_module.get_mac(), uid)
        )

    ############################# REQUEST #############################
    def _add_tag_request(self, message: DeviceMessage) -> DeviceMessage:
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
            time.sleep(0.01)
        return add_tag_response(
            self.communication_module.get_mac(),
            uid,
            message.payload["name"],
            message.message_id,
        )

    ############################# RESPONSE #############################

    def _set_settings_response(self, message: DeviceMessage) -> DeviceMessage:
        return accept_message(message)
