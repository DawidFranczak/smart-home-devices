#ifndef MESSAGE_H
#define MESSAGE_H

#include <Arduino.h>
#include <ArduinoJson.h>

struct Message {
    String message_id;
    String message_event;
    String message_type;
    String device_id;
    bool retain;
    JsonDocument payload;

    Message(String id, String event, String type, String dev_id, JsonDocument pl, bool r = false)
        : message_id(id), message_event(event), message_type(type), device_id(dev_id), retain(r), payload(pl) {}

    String toJson() {
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

#endif
