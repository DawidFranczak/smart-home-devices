from mfrc522 import MFRC522
import uasyncio as asyncio  # type: ignore


class Sensor:
    def __init__(self, sensor: MFRC522, reset_time: int):
        self.sensor: MFRC522 = sensor
        self.reset_time: int = reset_time

    async def start(self):
        self._reset()
        await asyncio.sleep(self.reset_time)

    def read_uid(self) -> tuple[bool, str]:
        try:
            (stat, tag_type) = self.sensor.request(self.sensor.REQIDL)
            if stat != self.sensor.OK:
                return False, ""
            (stat, raw_uid) = self.sensor.anticoll()
            if stat != self.sensor.OK:
                return False, ""
            self.sensor.stop_crypto1()
            if len(raw_uid) != 5:
                return False, ""
            uid = "".join(str(part) for part in raw_uid)
            return True, uid
        except Exception as e:
            pass

    def _reset(self):
        self.sensor.reset()
        self.sensor.init()
