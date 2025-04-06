import asyncio
from communication_protocol.message import connect_message, health_check_message
import machine  # type: ignore
import network  # type: ignore
import usocket  # type: ignore
from ucollections import deque  # type: ignore
from communication_protocol.communication_protocol import DeviceMessage
from communication_protocol.message_event import MessageEvent
from communication_protocol.message_type import MessageType


class CommunicationModule:
    def __init__(
        self, host_name: str, host_port: int, ssid: str, password: str, fun: str
    ):
        self.ssid = ssid
        self.password = password
        self.fun = fun
        self.to_server_queue = deque([], 1000)
        self.from_server_queue = deque([], 1000)
        self.wlan = network.WLAN(network.STA_IF)
        self.host_name = host_name
        self.host_port = host_port
        self.send_data_event = asyncio.Event()
        self.mac = ":".join(["{:02x}".format(b) for b in self.wlan.config("mac")])
        self.socket = None

    def is_connected(self) -> bool:
        return self.wlan.isconnected()

    def get_message(self) -> DeviceMessage | None:
        if self.from_server_queue:
            return self.from_server_queue.popleft()
        return None

    def extract_method_name_from_message(self, message: DeviceMessage) -> str:
        name = message.message_event.lower()
        type = message.message_type.lower()
        return f"_{name}_{type}"

    def send_message(self, message: DeviceMessage) -> None:
        self.to_server_queue.append(message)
        self.send_data_event.set()

    def get_mac(self) -> str:
        return self.mac

    async def start(self) -> None:
        receive_from_router_task = None
        send_to_router_task = None
        send_ping_task = None
        send_health_check_task = None
        while True:
            await self._connect_to_network()
            try:
                addr_info = usocket.getaddrinfo(self.host_name, self.host_port)[0][-1]
                if self.socket:
                    self.socket.close()
                if receive_from_router_task:
                    receive_from_router_task.cancel()
                if send_to_router_task:
                    send_to_router_task.cancel()
                if send_ping_task:
                    send_ping_task.cancel()
                if send_health_check_task:
                    send_health_check_task.cancel()

                self.socket = usocket.socket(usocket.AF_INET, usocket.SOCK_STREAM)
                self.socket.connect(addr_info)
                message: DeviceMessage = self.get_connect_message()
                self.socket.send(message.to_json().encode())
                receive_from_router_task = asyncio.create_task(
                    self._receive_from_router()
                )
                send_to_router_task = asyncio.create_task(self._send_to_router())
                send_ping_task = asyncio.create_task(self._send_ping())
                send_health_check_task = asyncio.create_task(self._send_health_check())

                await asyncio.gather(
                    receive_from_router_task,
                    send_to_router_task,
                    send_ping_task,
                    send_health_check_task,
                )
            except Exception as e:
                print(e)
                print("Resetuje połączenie")

    async def _receive_from_router(self) -> None:
        self.socket.setblocking(False)
        while True:
            try:
                data = self.socket.recv(1024)
                if data == b"P":
                    continue
                data = data.decode("utf-8")
                data: DeviceMessage = DeviceMessage.from_json(data)
                self.from_server_queue.append(data)
            except OSError as e:
                if e.args[0] not in (11, 35):
                    print(e)
                    raise
            await asyncio.sleep(0.1)

    async def _send_to_router(self) -> None:
        while True:
            if not self.to_server_queue:
                await self.send_data_event.wait()
                self.send_data_event.clear()
                continue
            message: DeviceMessage = self.to_server_queue.popleft()
            message.device_id = self.mac
            message = message.to_json()
            self.socket.sendall(message.encode())

    async def _connect_to_network(self):
        if not self.wlan.isconnected():
            self.wlan.active(True)
            self.wlan.connect(self.ssid, self.password)
            print("Trwa łączenie z siecią wifi...")
            count = 0
            while not self.wlan.isconnected():
                print(".")
                count += 1
                if count == 5:
                    machine.reset()
                await asyncio.sleep(1)
            print("Adres IP:", self.wlan.ifconfig()[0])

    async def _send_health_check(self) -> None:
        while True:
            self.to_server_queue.append(
                health_check_message(self.mac, self.wlan.status("rssi"))
            )
            self.send_data_event.set()
            await asyncio.sleep(120)

    async def _send_ping(self) -> None:
        while True:
            self.socket.sendall(b"P")
            await asyncio.sleep(1)

    def get_connect_message(self) -> DeviceMessage:
        return connect_message(self.mac, self.fun, self.wlan.status("rssi"))
