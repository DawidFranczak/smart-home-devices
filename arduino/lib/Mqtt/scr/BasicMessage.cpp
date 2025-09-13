#include "BasicMessage.h"

Message connectMessage(String mac, String fun, int wifiStrength){
  JsonDocument payload;
  payload["wifi_strength"] = wifiStrength;
  payload["fun"] = fun;
  return Message(String(millis()), "device_connect", "request", mac, payload);
}

Message healthCheck(String mac, int wifiStrength){
  JsonDocument payload;
  payload["wifi_strength"] = wifiStrength;
  return Message(String(millis()), "health_check", "request", mac, payload);
}

Message basicResonse(Message& message) {
  JsonDocument payload;
  payload["status"] = "accept";
  return Message(message.message_id, message.message_event, "response", message.device_id, payload);
}