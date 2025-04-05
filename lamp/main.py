import asyncio
from lamp import Lamp
from communication_protocol.communication_module import CommunicationModule


async def main():
    communication_module = CommunicationModule(
        "192.168.1.143", 8080, "Tenda", "1RKKHAPIEJ", "lamp"
    )
    lamp = Lamp(8, communication_module)
    await asyncio.gather(communication_module.start(), lamp.start())


if __name__ == "__main__":
    asyncio.run(main())
