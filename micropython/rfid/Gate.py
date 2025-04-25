from machine import Pin  # type: ignore
import time
import asyncio


class Gate:
    def __init__(self, buzzer, gate):
        self.gate = Pin(gate, Pin.OUT)  # D2
        self.buzzer = Pin(buzzer, Pin.OUT)  # D1
        self.start_time: int = 0
        self.is_in_process: bool = False

    def in_process(self) -> bool:
        return self.is_in_process

    async def access(self, open_gate_timeout):
        self.is_in_process = True
        self.gate.value(1)
        self.buzzer.value(1)
        start_time: int = time.ticks_ms()
        current_time: int = time.ticks_ms()

        while time.ticks_diff(current_time, start_time) <= open_gate_timeout:
            current_time: int = time.ticks_ms()
            await asyncio.sleep(0.01)

        self.buzzer.value(0)
        self.gate.value(0)
        self.is_in_process = False

    async def access_denied(self):
        self.is_in_process = True
        for _ in range(4):
            self.buzzer.value(1)
            await asyncio.sleep(0.2)
            self.buzzer.value(0)
            await asyncio.sleep(0.2)
        self.is_in_process = False
