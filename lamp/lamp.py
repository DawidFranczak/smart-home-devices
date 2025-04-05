from communication_protocol.communication_module import CommunicationModule
from communication_protocol.communication_protocol import DeviceMessage
from communication_protocol.message import accept_message
from pca9685 import PCA9685
import machine  # type: ignore
from machine import SoftI2C, Pin  # type: ignore
import asyncio


class Lamp:
    def __init__(self, lamp_amount: int, communication_module: CommunicationModule):
        i2c = SoftI2C(scl=Pin(5), sda=Pin(4), freq=400000, timeout=50000)
        self.lamps: list[int] = [i for i in range(lamp_amount)]
        self.brightness: int = 4095
        self.step: int = 10
        self.lighting_time: int = 10
        self.lamp_on = False
        self.pca9685 = PCA9685(i2c)
        self.pca9685.freq(50)
        self.communication_module = communication_module
        self.is_pending = False

    async def start(self):
        for i in self.lamps:
            self.pca9685.duty(i, 0)
        while True:
            message: DeviceMessage | None = self.communication_module.get_message()
            if not message:
                await asyncio.sleep(1)
                continue
            method_name = self.communication_module.extract_method_name_from_message(
                message
            )
            method = getattr(self, method_name)
            response = await method(message)
            self.communication_module.send_message(response)

    async def blink_lamp(self, reverse=False):
        self.is_pending = True
        await self._turn_on(reverse)
        await asyncio.sleep(self.lighting_time)
        await self._turn_off(reverse)
        self.is_pending = False

    async def _turn_on(self, reverse=False):
        if self.lamp_on:
            return
        self.lamp_on = True
        for i in self.lamps[::-1] if reverse else self.lamps:
            for temp_brightness in range(0, self.brightness, self.step):
                if temp_brightness > self.brightness:
                    temp_brightness = self.brightness
                self.pca9685.duty(i, temp_brightness)
            self.pca9685.duty(i, self.brightness)

    async def _turn_off(self, reverse=False):
        if not self.lamp_on:
            return
        self.lamp_on = False
        for i in self.lamps[::-1] if reverse else self.lamps:
            for temp_brightness in range(self.brightness, 0, -self.step):
                if temp_brightness < 0:
                    temp_brightness = 0
                self.pca9685.duty(i, temp_brightness)
            self.pca9685.duty(i, 0)

    ############################# REQUEST #####################################

    async def _set_settings_request(self, message: DeviceMessage) -> DeviceMessage:
        return await self._set_settings_response(message)

    async def _off_request(self, message: DeviceMessage) -> DeviceMessage:
        if self.is_pending:
            return accept_message(message)
        reverse = message.payload.get("reverse", False)
        asyncio.create_task(self._turn_off(reverse))
        return accept_message(message)

    async def _on_request(self, message: DeviceMessage) -> DeviceMessage:
        if self.is_pending:
            return accept_message(message)
        reverse = message.payload.get("reverse", False)
        asyncio.create_task(self._turn_on(reverse))
        return accept_message(message)

    async def _blink_request(self, message: DeviceMessage) -> DeviceMessage:
        if self.is_pending:
            return accept_message(message)
        reverse = message.payload.get("reverse", False)
        asyncio.create_task(self.blink_lamp(reverse))
        return accept_message(message)

    ############################# RESPONSE ####################################

    async def _set_settings_response(self, message: DeviceMessage):
        self.brightness = int(message.payload["brightness"] * 40.95)
        self.step = message.payload["step"]
        self.lighting_time = message.payload["lighting_time"]
        return accept_message(message)
