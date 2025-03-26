import asyncio
from machine import Pin, PWM  # type: ignore
from communication_protocol.communication_module import CommunicationModule
from communication_protocol.communication_protocol import DeviceMessage
from communication_protocol.message import accept_message


class Aquarium:
    def __init__(
        self,
        pin_r: int,  # D5
        pin_g: int,  # D6
        pin_b: int,  # D7
        pin_fluo: int,  # D3
        communication_protocol: CommunicationModule,
    ):
        self.communication_protocol = communication_protocol
        self.pin_r = PWM(Pin(pin_r))
        self.pin_g = PWM(Pin(pin_g))
        self.pin_b = PWM(Pin(pin_b))
        self.pin_fluo = Pin(pin_fluo, Pin.OUT)
        self.r_value = 0
        self.g_value = 0
        self.b_value = 0
        self.led_value = False
        self.fluo_value = False

    async def start(self):
        while True:
            message: DeviceMessage | None = self.communication_protocol.get_message()
            if not message:
                await asyncio.sleep(1)
                continue
            method_name = self.communication_protocol.extract_method_name_from_message(
                message
            )
            method = getattr(self, method_name)
            response = method(message)
            self.communication_protocol.send_message(response)

    def _update_led(self):
        self.led_value = True
        self.pin_r.duty(self.r_value)
        self.pin_g.duty(self.g_value)
        self.pin_b.duty(self.b_value)

    def _turn_off_led(self):
        self.led_value = False
        self.pin_r.duty(0)
        self.pin_g.duty(0)
        self.pin_b.duty(0)

    def _update_fluo(self, value):
        self.pin_fluo.value(value)

    ############################# REQUEST #############################

    def _set_rgb_request(self, message: DeviceMessage):
        rgb = message.payload
        self.r_value = rgb["r"]
        self.g_value = rgb["g"]
        self.b_value = rgb["b"]
        if self.led_value:
            self._update_led()
        return accept_message(message)

    def _set_fluo_request(self, message: DeviceMessage):
        self._update_fluo(message.payload["value"])
        return accept_message(message)

    def _set_led_request(self, message: DeviceMessage):
        led = message.payload["value"]
        if led:
            self._update_led()
        else:
            self._turn_off_led()
        return accept_message(message)

    def _set_settings_request(self, message: DeviceMessage):
        self.r_value = message.payload["color_r"]
        self.g_value = message.payload["color_g"]
        self.b_value = message.payload["color_b"]
        self.led_value = message.payload["led_mode"]
        self.fluo_value = message.payload["fluo_mode"]
        if self.led_value:
            self._update_led()
        self._update_fluo(self.fluo_value)
        return accept_message(message)
