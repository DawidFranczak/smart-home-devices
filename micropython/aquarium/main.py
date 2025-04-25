import asyncio
import time
from communication_protocol.communication_module import CommunicationModule
from aquarium import Aquarium
import ujson  # type: ignore
import network  # type: ignore
import machine  # type: ignore

with open("communication_protocol/settings.json", "r") as f:
    connection_settings = ujson.load(f)


async def main():
    communication_module = CommunicationModule(
        connection_settings["server_ip"],
        connection_settings["server_port"],
        connection_settings["ssid"],
        connection_settings["password"],
        "aquarium",
    )
    aquarium = Aquarium(14, 12, 13, 0, communication_module)
    await asyncio.gather(communication_module.start())


if __name__ == "__main__":
    asyncio.run(main())
