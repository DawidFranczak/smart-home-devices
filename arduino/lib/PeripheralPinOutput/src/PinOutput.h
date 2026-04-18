#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>
#include "BasePeripheral.h"
#include "Message.h"
#include "ConfigManager.h"
#include "EventEngine.h"

class PinOutput : public BasePeripheral {
private:
    int pin;
    bool isOn;
    String statePath = baseStatePath + "is_on";

    void toggle(){
        digitalWrite(pin, !digitalRead(pin));
        systemContext.stateManager.set(statePath.c_str(), digitalRead(pin));
        systemContext.stateManager.save();
    }

public:
    PinOutput(int id, SystemContext& ctx, JsonObject cfg)
        : BasePeripheral(id, ctx)
    {
        pin = cfg["config"]["pin"];
        isOn = systemContext.stateManager.get(statePath.c_str());
    }

    void begin() override {
        pinMode(pin, OUTPUT);
        digitalWrite(pin, isOn ? HIGH : LOW);
    }

    void loop() override {}

    void onMessage(Message& msg) override {
        if(msg.command == "toggle") {
            toggle();
            JsonDocument payload;
            payload["is_on"] = isOn;
            payload["status"] = static_cast<uint8_t>(ActionResult::ACCEPTED);
            notify("on_toggle", msg.command, payload, msg.message_id);
        }
    }

    void triggerAction(String targetAction, String extraSettings) override {
         if(targetAction == "toggle") {
            toggle();
            JsonDocument payload;
            payload["is_on"] = isOn;
            notify("on_toggle", "on_toggle", payload);
         }
    }


};