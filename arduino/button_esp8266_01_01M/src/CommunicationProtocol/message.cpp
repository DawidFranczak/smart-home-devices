#include "message.h"

DeviceMessage on_click_request(String mac) {
  JsonDocument payload;
  return DeviceMessage(String(millis()), "on_click", "request", mac, payload);
}

DeviceMessage on_hold_request(String mac) {
  JsonDocument payload;
  return DeviceMessage(String(millis()), "on_hold", "request", mac, payload);
}