#pragma once
#include "Types.h"
#include <ESP8266WiFi.h>
#include "Message.h"

class Message;

class EventEngine;

class BasePeripheral {
    protected:
        int  id;
        EventEngine& engine;
        String  mac = WiFi.macAddress();
        void notify(String eventType, String command, JsonDocument payload, String messageId = "", MessageType type=MessageType::ACTION);


    public:
        BasePeripheral(int id, EventEngine& engine)
            : id(id), engine(engine) {
                
            }

        virtual void begin() = 0;
        virtual void loop() = 0;

        virtual void onMessage( Message& message) = 0;
        virtual void triggerAction(String targetAction, String extraSettings) = 0;
        int getId() const { return id; }
};
