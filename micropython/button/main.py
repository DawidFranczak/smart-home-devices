import json
import asyncio
from communication_protocol.communication_module import CommunicationModule
from button import Button

settings_path = "communication_protocol/settings.json"

with open(settings_path, "r") as f:
    settings = json.load(f)


async def main():
    communication_module = CommunicationModule(
        settings["server_ip"],
        settings["server_port"],
        settings["ssid"],
        settings["password"],
        "button",
    )
    button = Button(2, communication_module)
    await asyncio.gather(communication_module.start(), button.start())


if __name__ == "__main__":
    asyncio.run(main())
