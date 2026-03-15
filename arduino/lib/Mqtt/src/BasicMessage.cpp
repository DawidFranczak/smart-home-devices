#include "BasicMessage.h"

Message connectEvent(String mac, String chipType, int wifiStrength, float firmware_version){
  JsonDocument payload;
  payload["wifi_strength"] = wifiStrength;
  payload["chip_type"] = chipType;
  payload["firmware_version"] = firmware_version;
  return Message(
      MessageDirection::INTENT,
      "device_connect",
      MessageType::EVENT,
      Scope::CPU,
      mac,
      0,
      String(millis()),
      payload
  );
}

Message healthCheckEvent(String mac, int wifiStrength){
  JsonDocument payload;
  payload["wifi_strength"] = wifiStrength;
  return Message(
      MessageDirection::INTENT,
      "health_check",
      MessageType::EVENT,
      Scope::CPU,
      mac,
      0,
      String(millis()),
      payload
  );
} 

Message disconnectEvent(String mac){
  JsonDocument payload;
  return Message(
      MessageDirection::INTENT,
      "device_disconnect",
      MessageType::EVENT,
      Scope::CPU,
      mac,
      0,
      String(millis()),
      payload
  );
} 
 

Message firmwareUpdateErrorEvent(String mac, String error){
  JsonDocument payload;
  payload["message"] = error;
  return Message(
      MessageDirection::INTENT,
      "update_firmware_error",
      MessageType::ACTION,
      Scope::CPU,
      mac,
      0,
      String(millis()),
      payload
  );
} 

Message basicPeripheralResult(Message& message, int peripheralId, bool accept) {
  JsonDocument payload;
  if (accept) payload["status"] = static_cast<uint8_t>(ActionResult::ACCEPTED);
  else payload["status"] = static_cast<uint8_t>(ActionResult::REJECTED);
  return  Message(
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

Message basicCPUResult(Message& message, bool accept) {
  JsonDocument payload;
  if (accept) payload["status"] = static_cast<uint8_t>(ActionResult::ACCEPTED);
  else payload["status"] = static_cast<uint8_t>(ActionResult::REJECTED);
  return Message(
      MessageDirection::RESULT,
      message.command,
      MessageType::ACTION,
      Scope::CPU,
      message.device_id,
      0,
      message.message_id,
      payload
  );
} 
