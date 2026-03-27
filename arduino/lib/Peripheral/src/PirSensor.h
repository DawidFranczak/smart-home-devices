#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>
#include "BasePeripheral.h"
#include "ButtonMessage.h"
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
        uint8_t arduinoMode = INPUT;
        #ifdef ESP8266
            if (pin == 16) {
                arduinoMode = INPUT_PULLDOWN_16;
            } else {
                arduinoMode = INPUT;
            }
        #else
            arduinoMode = INPUT_PULLDOWN;
        #endif
        pinMode(pin, arduinoMode);
        
    }

    void loop() override {
        if (digitalRead(pin) == HIGH){
            lastCheck = millis();
            if (!sended){
                sended=true;
                JsonDocument payload;
                payload["is_on"] = HIGH;
                notify("on_motion", "on_motion", payload, "", MessageType::EVENT);
            }
        }else if (sended){
            if (millis()-lastCheck > (unsigned long)coolDownTime){
                 sended = false;
                
            }
        }
    }

    void onMessage(Message& msg) override {}

    void triggerAction(String targetAction, String extraSettings) override {}


};