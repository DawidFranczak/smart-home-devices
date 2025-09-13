#include <Arduino.h>
#include <ArduinoJson.h>

struct Message {
  String message_id;
  String message_event;
  String message_type;
  String device_id;
  JsonDocument payload;
  
  Message(String message_id, String message_event, String message_type, String device_id, JsonDocument payload)
  :  message_id(message_id), message_event(message_event), message_type(message_type), device_id(device_id), payload(payload) {}

  String toJson() const {
    JsonDocument doc;
    doc["message_id"] = message_id;
    doc["message_event"] = message_event;
    doc["message_type"] = message_type;
    doc["device_id"] = device_id;
    doc["payload"] = payload;

    String output;
    serializeJson(doc, output);
    return output;
  }

  static Message fromJson(String json_str) {
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, json_str);
    if (error) {
      JsonDocument empty_payload;
      return Message("", "", "", "", empty_payload);
    }
    JsonDocument payload;
    payload.set(doc["payload"]);
    return Message(
      doc["message_id"].as<String>(),
      doc["message_event"].as<String>(),
      doc["message_type"].as<String>(),
      doc["device_id"].as<String>(),
      payload
    );
  }
};
