import asyncio
from message import on_click_request, on_hold_request
from communication_protocol.communication_protocol import DeviceMessage
from communication_protocol.message import accept_message
from machine import Pin  # type: ignore
from communication_protocol.communication_module import CommunicationModule


class Button:
    def __init__(self, input_pin: int, communication_module: CommunicationModule):
        self.button = Pin(input_pin, Pin.IN, Pin.PULL_UP)
        self.communication_module: CommunicationModule = communication_module

    async def start(self):
        while True:
            await self._check_message()
            await self.check_button()
            await asyncio.sleep(0.1)

    async def check_button(self):
        if not self.button.value():
            await asyncio.sleep(0.03)
            if self.button.value():
                return
            await asyncio.sleep(1)
            message = on_click_request(self.communication_module.mac)
            if not self.button.value():
                message = on_hold_request(self.communication_module.mac)
            self.communication_module.send_message(message)
            while not self.button.value():
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
            return
        response = await method(message)
        self.communication_module.send_message(response)

    async def _set_settings_response(self, message: DeviceMessage) -> DeviceMessage:
        return accept_message(message)
