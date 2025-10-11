#include "Message.h"

Message onReadRequest(String mac, String uid) {
  JsonDocument payload;
  payload["uid"] = uid;
  return Message(String(millis()), "on_read", "request", mac, payload);
}

Message addTagResponse(Message message, String uid) {
  JsonDocument payload;
  payload["uid"] = uid;
  payload["name"] = message.payload["name"].as<String>();
  return Message(message.message_id, "add_tag", "response", message.device_id, payload);
}
