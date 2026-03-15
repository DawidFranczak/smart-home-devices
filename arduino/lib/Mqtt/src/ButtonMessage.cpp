#include "ButtonMessage.h"

// Message onClickRequest(String mac) {
//   JsonDocument payload;
//   return Message(String(millis()), "on_click", "request", mac, payload);
// }

// Message onHoldRequest(String mac) {
//   JsonDocument payload;
//   return Message(String(millis()), "on_hold", "request", mac, payload);
// }

// Message onToggleRequest(String mac) {
//   JsonDocument payload;
//   return Message(String(millis()), "on_toggle", "request", mac, payload);
// }

Message toggleResult(Message& message, int peripheralId, bool isOn, bool accept) {
  JsonDocument payload;
  payload["is_on"] = isOn;
  if (accept) payload["status"] = static_cast<uint8_t>(ActionResult::ACCEPTED);
  else payload["status"] = static_cast<uint8_t>(ActionResult::REJECTED);
 return Message(
      MessageDirection::RESULT,
      message.command,
      MessageType::ACTION,
      Scope::PERIPHERAL,
      message.device_id,
      peripheralId,
      message.message_id,
      payload
  );
} 

Message toggleEvent(int peripheralId, bool isOn) {
  JsonDocument payload;
  payload["is_on"] = isOn;
 return Message(
      MessageDirection::INTENT,
      "on_toggle",
      MessageType::EVENT,
      Scope::PERIPHERAL,
      "",
      peripheralId,
      String(millis()),
      payload
  );
} 