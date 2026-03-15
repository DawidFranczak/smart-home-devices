#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>
#include "BasePeripheral.h"
#include "ButtonMessage.h"
#include "ConfigManager.h"
#include "EventEngine.h"

class PinOutput : public BasePeripheral {
private:
    int pin;
    bool isOn;
    ConfigManager& stateManager;
    String statePath;

    void toggle(){
        digitalWrite(pin, !digitalRead(pin));
        stateManager.set(statePath.c_str(), digitalRead(pin));
        stateManager.save();
    }

public:
    PinOutput(int id, EventEngine& engine, ConfigManager& stateManager,
              JsonObject cfg, JsonObject state)
        : BasePeripheral(id, engine), stateManager(stateManager)
    {
        pin = cfg["config"]["pin"];
        isOn = state["is_on"] | false;
        statePath = "states." + String(id)+ ".is_on";
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