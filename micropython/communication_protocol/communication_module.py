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
        self.to_server_queue = deque([], 100)
        self.from_server_queue = deque([], 100)
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
        send_health_check_task = None
        while True:
            if not self.is_connected():
                print("Nie można połączyć się z routerem")
                self.connect()
                await asyncio.sleep(1)

            else:
                print("Połączono z routerem")
                await asyncio.sleep(1)
            # try:
            #     addr_info = usocket.getaddrinfo(self.host_name, self.host_port)[0][-1]
            #     if self.socket:
            #         self.socket.close()
            #     if receive_from_router_task:
            #         receive_from_router_task.cancel()
            #     if send_to_router_task:
            #         send_to_router_task.cancel()
            #     if send_health_check_task:
            #         send_health_check_task.cancel()

            #     self.socket = usocket.socket(usocket.AF_INET, usocket.SOCK_STREAM)
            #     self.socket.connect(addr_info)
            #     message: DeviceMessage = self.get_connect_message()
            #     self.socket.send(message.to_json().encode())
            #     receive_from_router_task = asyncio.create_task(
            #         self._receive_from_router()
            #     )
            #     send_to_router_task = asyncio.create_task(self._send_to_router())
            #     send_health_check_task = asyncio.create_task(self._send_health_check())

            #     await asyncio.gather(
            #         receive_from_router_task,
            #         send_to_router_task,
            #         send_health_check_task,
            #     )
            # except Exception as e:
            #     print(e)
            #     print("Resetuje połączenie")

    async def _receive_from_router(self) -> None:
        self.socket.setblocking(False)
        while True:
            try:
                data = self.socket.recv(1024)
                if not data:
                    print("Router zamknął połączenie")
                    raise ConnectionError("Rozłączono z routerem")
                data = data.decode("utf-8")
                data: DeviceMessage = DeviceMessage.from_json(data)
                self.from_server_queue.append(data)
            except OSError as e:
                if e.args[0] not in (11, 35):  # EAGAIN, EWOULDBLOCK
                    print(f"Błąd sieciowy: {e}")
                    raise
            except Exception as e:
                print(f"Błąd w odbiorze: {e}")
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
            try:
                self.socket.sendall(message.encode())
            except Exception as e:
                print("Błąd podczas wysyłania:", e)

    async def _send_health_check(self) -> None:
        while True:
            self.to_server_queue.append(
                health_check_message(self.mac, self.wlan.status("rssi"))
            )
            self.send_data_event.set()
            await asyncio.sleep(60)

    async def connect(self):
        self.wlan.active(True)
        if not self.is_connected():
            print("Próba połączenia z Wi-Fi...")
            self.wlan.connect(self.ssid, self.password)
            await asyncio.sleep(1)  # Czekaj asynchronicznie
            if not self.is_connected():
                self.wlan.disconnect()
                self.wlan.active(False)
            print("Połączono z Wi-Fi!")

    def get_connect_message(self) -> DeviceMessage:
        return connect_message(self.mac, self.fun, self.wlan.status("rssi"))
