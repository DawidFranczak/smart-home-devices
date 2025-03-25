import machine
from communication_protocol.communication_protocol import DeviceMessage
from communication_protocol.message_event import MessageEvent
from communication_protocol.message_type import MessageType
from communication_protocol.status_type import StatusType


def accept_message(message: DeviceMessage):
    return DeviceMessage(
        message_id=message.message_id,
        message_event=message.message_event,
        message_type=MessageType.RESPONSE,
        device_id=message.device_id,
        payload={"status": StatusType.ACCEPT},
    )


def connect_message(mac, fun, wifi_strength):
    return DeviceMessage(
        message_id=machine.unique_id().hex(),
        message_event=MessageEvent.DEVICE_CONNECT,
        message_type=MessageType.REQUEST,
        device_id=mac,
        payload={
            "fun": fun,
            "wifi_strength": wifi_strength,
        },
    )


def health_check_message(message_id, mac, wifi_strength):
    return DeviceMessage(
        message_id=message_id,
        message_event=MessageEvent.HEALTH_CHECK,
        message_type=MessageType.RESPONSE,
        device_id=mac,
        payload={
            "wifi_strength": wifi_strength,
        },
    )
