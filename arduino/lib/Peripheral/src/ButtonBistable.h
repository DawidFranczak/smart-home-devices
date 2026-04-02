#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>
#include "BasePeripheral.h"
#include "ButtonMessage.h"
#include "ConfigManager.h"
#include "EventEngine.h"

class ButtonBistable : public BasePeripheral {
private:
    int pin;
    String mode;
    bool lastState;
    int activeState;

public:
    ButtonBistable(int id, SystemContext& ctx, JsonObject cfg)
        : BasePeripheral(id, ctx)
    {
        pin = cfg["config"]["pin"];
        mode = cfg["config"]["mode"].as<String>();
    }

    void begin() override {
        
        uint8_t arduinoMode = INPUT;

        if (mode == "PULL_UP") {
            arduinoMode = INPUT_PULLUP;
            activeState = LOW;
        } 
        else if (mode == "PULL_DOWN") {
            activeState = HIGH;
            #ifdef ESP8266
            if (pin == 16) {
                arduinoMode = INPUT_PULLDOWN_16;
            } else {
                arduinoMode = INPUT;
        }
            #else
            arduinoMode = INPUT_PULLDOWN;
            #endif
        } 
        pinMode(pin, arduinoMode);
        
        lastState = digitalRead(pin);
    }

    void loop() override {
        int currentState = digitalRead(pin);
        static unsigned long lastDebounceTime = 0;
        static int stableState = -1;
        if (currentState != lastState) {
            lastDebounceTime = millis();
            lastState = currentState;
        }
        if ((millis() - lastDebounceTime) > 30) {
            if (currentState != stableState) {
                stableState = currentState;
                
                if (stableState) notify("on_on");
                else notify("on_off");   
            }
        }
    }

    void onMessage(Message& msg) override {}

    void triggerAction(String targetAction, String extraSettings) override {}


};