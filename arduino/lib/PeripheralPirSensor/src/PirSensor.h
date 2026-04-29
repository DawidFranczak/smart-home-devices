#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>
#include "BasePeripheral.h"
#include "Message.h"
#include "ConfigManager.h"
#include "EventEngine.h"

class PirSensor : public BasePeripheral {
private:
    int pin;
    int coolDownTime;
    bool sended=false;
    unsigned long lastCheck;

public:
    PirSensor(int id, SystemContext& ctx, JsonObject cfg)
        : BasePeripheral(id, ctx)
    {
        pin = cfg["config"]["pin"];
        coolDownTime = cfg["config"]["cool_down_time"];
        coolDownTime = coolDownTime * 1000;
    }

    void begin() override {
        pinMode(pin, INPUT);
    }

    void loop() override {
        if (digitalRead(pin) == HIGH){
            lastCheck = millis();
            if (!sended){
                sended=true;
                JsonDocument payload;
                payload["is_on"] = true;
                notify("on_motion", payload, 1.0);
            }
        }else if (sended){
            if (millis()-lastCheck > (unsigned long)coolDownTime){
                sended = false;
                JsonDocument payload;
                payload["is_on"] = false;
                notify("on_motion", payload, 0.0);
            }
        }
    }

    void onMessage(Message& msg) override {}

    void triggerAction(String targetAction, String extraSettings) override {}

};