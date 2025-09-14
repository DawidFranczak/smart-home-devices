#include "ButtonMessage.h"

Message onClickMessage(String mac) {
  JsonDocument payload;
  return Message(String(millis()), "on_click", "request", mac, payload);
}

Message onHoldMessage(String mac) {
  JsonDocument payload;
  return Message(String(millis()), "on_hold", "request", mac, payload);
}