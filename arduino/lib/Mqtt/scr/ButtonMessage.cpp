#include "ButtonMessage.h"

Message onClickRequest(String mac) {
  JsonDocument payload;
  return Message(String(millis()), "on_click", "request", mac, payload);
}

Message onHoldRequest(String mac) {
  JsonDocument payload;
  return Message(String(millis()), "on_hold", "request", mac, payload);
}