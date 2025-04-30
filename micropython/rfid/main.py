import asyncio
import ujson  # type: ignore
from mfrc522 import MFRC522
from machine import Pin  # type: ignore
from communication_protocol.communication_module import CommunicationModule
from RFID import RFID
from Gate import Gate
from sensor import Sensor

# define BUZZER D1 5// Sygnalizacja otwarcia furki
# gate D2 4
# define ADDBUTTON D5 // Dodanie urządenia do serwera
# define WIFILED D4 // Sygnalizcja podłączenia do wifi

with open("settings.json", "r") as f:
    settings = ujson.load(f)

with open("communication_protocol/settings.json", "r") as f:
    connection_settings = ujson.load(f)


async def main():
    communication_module = CommunicationModule(
        connection_settings["server_ip"],
        connection_settings["server_port"],
        connection_settings["ssid"],
        connection_settings["password"],
        "rfid",
    )
    sensor = Sensor(MFRC522(14, 13, 12, 16, 15), settings["rfid_reset"])
    gate: Gate = Gate(5, 4)
    rfid: RFID = RFID(
        communication_module=communication_module,
        sensor=sensor,
        gate=gate,
        settings=settings,
    )
    await asyncio.gather(communication_module.start(), rfid.start(), sensor.start())


if __name__ == "__main__":
    asyncio.run(main())
