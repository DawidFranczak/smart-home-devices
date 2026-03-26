#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>
#include "BasePeripheral.h"
#include "ButtonMessage.h"
#include "ConfigManager.h"
#include "EventEngine.h"

class ButtonMonostable : public BasePeripheral {
private:
    int pin;
    String mode;
    int activeState;
    unsigned long lastCheck = 0;
    int pressTime = 0;
    bool sended = false;
    JsonDocument payload;

public:
    ButtonMonostable(int id, SystemContext& ctx, JsonObject cfg)
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
        
    }

    void loop() override {
        bool isPressed = (digitalRead(pin) == activeState);

        if (isPressed) {
            pressTime = millis() - lastCheck;
        } else {
            if (pressTime > 30 && pressTime < 1000) {
                notify("on_click", "on_click", payload, "", MessageType::EVENT);
            }
            lastCheck = millis();
            pressTime = 0;
            sended = false;
        }

        if (pressTime > 1000 && !sended) {
            notify("on_hold", "on_hold", payload, "", MessageType::EVENT);
            sended = true;
        }
    }

    void onMessage(Message& msg) override {}

    void triggerAction(String targetAction, String extraSettings) override {}


};