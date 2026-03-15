#ifndef MESSAGE_H
#define MESSAGE_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include "Types.h"

struct Message {
    MessageDirection direction;
    String command;
    MessageType type;
    Scope scope;
    String device_id;
    int peripheral_id;
    String message_id;
    JsonDocument payload;

    Message() = default;

    Message(
        MessageDirection dir,
        const String& cmd,
        MessageType msgType,
        Scope sc,
        const String& devId,
        int periphId,
        const String& msgId,
        JsonDocument pl = JsonDocument()
    ) : direction(dir), command(cmd), type(msgType), scope(sc),
        device_id(devId), peripheral_id(periphId), message_id(msgId) {
              payload.set(pl);
        }

   String toJson() const {
        JsonDocument doc;
        JsonObject root = doc.to<JsonObject>();

        root["direction"]     = static_cast<uint8_t>(direction);
        root["command"]       = command;
        root["type"]          = static_cast<uint8_t>(type);
        root["scope"]         = static_cast<uint8_t>(scope);
        root["device_id"]     = device_id;
        root["peripheral_id"] = peripheral_id;
        root["message_id"]    = message_id;
        root["payload"] = JsonObject();

        if (!payload.isNull()) {
            root["payload"].set(payload.as<JsonVariantConst>());
        }

        String output;
        serializeJson(doc, output);
        return output;
    }

    static Message fromJson(const String& jsonStr) {
        JsonDocument doc;
        DeserializationError err = deserializeJson(doc, jsonStr);
        if (err) {
            Serial.println("Payload length:");
            Serial.println(jsonStr.length());

            Serial.println("Payload content:");
            Serial.println(jsonStr);
            Serial.println(err.c_str());
            return Message();  
        }

        Message msg;

        msg.direction = static_cast<MessageDirection>(
            doc["direction"].as<int>()  
        );

        msg.command       = doc["command"].as<String>();        
        msg.type          = static_cast<MessageType>(doc["type"].as<int>());
        msg.scope         = static_cast<Scope>(doc["scope"].as<int>());

        msg.device_id     = doc["device_id"].as<String>();
        msg.peripheral_id = doc["peripheral_id"].as<int>();
        msg.message_id    = doc["message_id"].as<String>();

        if (!doc["payload"].isNull()) {
            msg.payload.set(doc["payload"]);
        }

        return msg;
    }
};

#endif
