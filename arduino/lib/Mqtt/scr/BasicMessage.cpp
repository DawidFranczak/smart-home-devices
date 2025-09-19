#include "BasicMessage.h"

Message connectRequest(String mac, String fun, int wifiStrength){
  JsonDocument payload;
  payload["wifi_strength"] = wifiStrength;
  payload["fun"] = fun;
  return Message(String(millis()), "device_connect", "request", mac, payload, 1, true);
}

Message healthCheckRequest(String mac, int wifiStrength){
  JsonDocument payload;
  payload["wifi_strength"] = wifiStrength;
  return Message(String(millis()), "health_check", "request", mac, payload);
}

Message disconnectRequest(String mac){
  JsonDocument payload;
  return Message(String(millis()), "device_disconnect", "request", mac, payload);
}
Message getSettings(String mac){
  JsonDocument payload;
  return Message(String(millis()), "get_settings", "request", mac, payload);
}

Message basicResonse(Message& message, bool accept) {
  JsonDocument payload;
  if (accept) payload["status"] = "accepted";
  else payload["status"] = "rejected";
  return Message(message.message_id, message.message_event, "response", message.device_id, payload);
}