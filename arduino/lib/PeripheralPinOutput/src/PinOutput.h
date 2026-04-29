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
        isOn = digitalRead(pin);
        systemContext.stateManager.set(statePath.c_str(), isOn);
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
            notify(msg, ActionResult::ACCEPTED);
            toggle();
            if (isOn){
                notify("on_on");
            }else{
                notify("on_off");
            }
        }
    }

    void triggerAction(String targetAction, String extraSettings) override {
         if(targetAction == "toggle") {
            toggle();
            if (isOn){
                notify("on_on");
            }else{
                notify("on_off");
            }
         }
    }


};