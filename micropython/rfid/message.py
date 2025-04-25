from communication_protocol.communication_protocol import DeviceMessage
from communication_protocol.message_event import MessageEvent
from communication_protocol.message_type import MessageType


def on_read_request(mac: str, uid: str) -> DeviceMessage:
    return DeviceMessage(
        MessageEvent.ON_READ,
        MessageType.REQUEST,
        mac,
        {"uid": uid},
    )


def on_click_request(mac: str) -> DeviceMessage:
    return DeviceMessage(
        MessageEvent.ON_CLICK,
        MessageType.REQUEST,
        mac,
        {},
    )


def add_tag_response(mac: str, uid: str, name: str, message_id) -> DeviceMessage:
    return DeviceMessage(
        MessageEvent.ADD_TAG,
        MessageType.RESPONSE,
        mac,
        {"uid": uid, "name": name},
        message_id=message_id,
    )
