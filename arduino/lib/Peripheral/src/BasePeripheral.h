#pragma once
#include <Arduino.h>
#include "Message.h"
#include "Types.h"
#include "PeripheralManager.h"

class Message;

class SystemContext;

class BasePeripheral {
    protected:
        int  id;
        SystemContext& systemContext;
        String  mac = WiFi.macAddress();
        void notify(String eventType, String command, JsonDocument payload, String messageId = "", MessageType type=MessageType::ACTION,  MessageTarget target=MessageTarget::BOTH, float eventValue=0.0);
        void notify(const Message& originalMsg, ActionResult result);
        void notify(String eventType, JsonDocument payload = JsonDocument(), float eventValue = 0.0);
        void emitToEngine(Message& msg, String evType, MessageTarget target, float val = 0.0);
        bool getSettings(const String& extra, JsonDocument& doc, JsonObject& obj);
        String baseStatePath = "states." + String(id) + ".";

    public:
        BasePeripheral(int id, SystemContext& systemContext)
            : id(id), systemContext(systemContext) {}

        virtual void begin() = 0;
        virtual void loop() = 0;

        virtual void onMessage( Message& message) = 0;
        virtual void triggerAction(String targetAction, String extraSettings) = 0;
        int getId() const { return id; }
};
