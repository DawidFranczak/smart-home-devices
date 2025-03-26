from communication_protocol.communication_protocol import DeviceMessage
from communication_protocol.message_event import MessageEvent
from communication_protocol.message_type import MessageType


def check_uid_request(mac: str, uid: str) -> DeviceMessage:
    return DeviceMessage(
        MessageEvent.CHECK_UID,
        MessageType.REQUEST,
        mac,
        {"uid": uid},
    )


def add_tag_response(mac: str, uid: str, name: str, message_id) -> DeviceMessage:
    return DeviceMessage(
        MessageEvent.ADD_TAG,
        MessageType.RESPONSE,
        mac,
        {"uid": uid, "name": name},
        message_id=message_id,
    )
