from communication_protocol.communication_protocol import DeviceMessage
from communication_protocol.message_event import MessageEvent
from communication_protocol.message_type import MessageType


def on_click_request(mac: str) -> DeviceMessage:
    return DeviceMessage(
        MessageEvent.ON_CLICK,
        MessageType.REQUEST,
        mac,
        {},
    )


def on_hold_request(mac: str) -> DeviceMessage:
    return DeviceMessage(
        MessageEvent.ON_HOLD,
        MessageType.REQUEST,
        mac,
        {},
    )
