import json
import machine  # type: ignore
from .message_event import MessageEvent
from .message_type import MessageType


class DeviceMessage:
    def __init__(
        self,
        message_event: MessageEvent,
        message_type: MessageType,
        device_id: str,
        payload: dict,
        message_id=None,
    ) -> None:
        self.message_event: MessageEvent = message_event
        self.message_type: MessageType = message_type
        self.device_id: str = device_id
        self.payload = payload
        self.message_id = message_id if message_id else machine.unique_id().hex()

    def to_json(self) -> str:
        message_dict = {
            "message_id": self.message_id,
            "message_event": self.message_event,
            "message_type": self.message_type,
            "device_id": self.device_id,
            "payload": self.payload,
        }
        return json.dumps(message_dict)

    @classmethod
    def from_json(cls, json_str: str) -> "DeviceMessage":
        data = json.loads(json_str)
        data = json.loads(data)
        return cls(
            message_id=data["message_id"],
            message_event=data["message_event"],
            message_type=data["message_type"],
            device_id=data["device_id"],
            payload=data["payload"],
        )

    def __str__(self) -> str:
        return f"{self.message_id} - {self.message_event} - {self.message_type} - {self.device_id} - {self.payload}"
