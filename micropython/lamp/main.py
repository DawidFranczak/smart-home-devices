import asyncio
from lamp import Lamp
from communication_protocol.communication_module import CommunicationModule
import ujson  # type: ignore

with open("communication_protocol/settings.json", "r") as f:
    connection_settings = ujson.load(f)


async def main():
    communication_module = CommunicationModule(
        connection_settings["server_ip"],
        connection_settings["server_port"],
        connection_settings["ssid"],
        connection_settings["password"],
        "lamp",
    )
    lamp = Lamp(8, communication_module)
    await asyncio.gather(communication_module.start(), lamp.start())


if __name__ == "__main__":
    asyncio.run(main())
