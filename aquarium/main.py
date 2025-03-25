import asyncio
from communication_protocol.communication_module import CommunicationModule
from aquarium import Aquarium


async def main():
    communication_module = CommunicationModule(
        "192.168.1.143", 8080, "Tenda", "1RKKHAPIEJ", "aquarium"
    )
    aquarium = Aquarium(14, 12, 13, 0, communication_module)
    await asyncio.gather(communication_module.start(), aquarium.start())


if __name__ == "__main__":
    asyncio.run(main())
