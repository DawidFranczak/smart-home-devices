#include "message.h"

DeviceMessage on_read_request(String mac, String uid) {
  JsonDocument payload;
  payload["uid"] = uid;
  return DeviceMessage(String(millis()), "on_read", "request", mac, payload);
}

DeviceMessage on_click_request(String mac) {
  JsonDocument payload;
  return DeviceMessage(String(millis()), "on_click", "request", mac, payload);
}

DeviceMessage on_hold_request(String mac) {
  JsonDocument payload;
  return DeviceMessage(String(millis()), "on_hold", "request", mac, payload);
}

DeviceMessage add_tag_response(DeviceMessage message, String uid) {
  JsonDocument payload;
  payload["uid"] = uid;
  payload["name"] = message.payload["name"].as<String>();
  return DeviceMessage(message.message_id, "add_tag", "response", message.device_id, payload);
}
DeviceMessage accept_response(DeviceMessage message) {
  JsonDocument payload;
  payload["status"] = "accept";
  return DeviceMessage(message.message_id, message.message_event, "response", message.device_id, payload);
}