#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>
#include "BasePeripheral.h"
#include "ButtonMessage.h"
#include "ConfigManager.h"
#include "EventEngine.h"

class Relay : public BasePeripheral {
private:
    int pin;
    bool isOn;
    String statePath = baseStatePath + "is_on";
    unsigned long timerStart = 0;
    unsigned long durationMs = 0;
    bool isTimerActive = false;
    bool targetStateAfterTimer = false;

    void setPhysicalState(bool state) {
        isOn = state;
        digitalWrite(pin, isOn ? HIGH : LOW);
        
        systemContext.stateManager.set(statePath.c_str(), isOn);
        systemContext.stateManager.save();
        if (isOn){
            notify("on_on");
        }else{
            notify("on_off");
        }
    }

    void handlePowerAction(bool state, int delay, int duration) {
        if (delay > 0) {
            timerStart = millis();
            durationMs = delay;
            targetStateAfterTimer = state;
            isTimerActive = true;
        } else {
            setPhysicalState(state);
            
            if (duration > 0 && state == true) {
                timerStart = millis();
                durationMs = duration;
                targetStateAfterTimer = false;
                isTimerActive = true;
            } else {
                isTimerActive = false;
            }
        }
    }

public:
    Relay(int id, SystemContext& ctx, JsonObject cfg)
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

        int duration = msq.payload["duration"] | 0;
        int delay = msq.payload["delay"] | 0;

        if(msg.command == "toggle") {
            notify(msg, ActionResult::ACCEPTED);
            handlePowerAction(!isOn, 0, 0);
          
        }else if(msg.command == "on"){
            notify(msg, ActionResult::ACCEPTED);
            handlePowerAction(!isOn, delay, duration);
           
        }else if(msg.command == "off"){
            notify(msg, ActionResult::ACCEPTED);
            handlePowerAction(!isOn, delay, duration);
        }
    }

    void triggerAction(String targetAction, String extraSettings) override {
        JsonDocument doc;
        JsonObject settings;
        
        bool hasSettings = getSettings(extraSettings, doc, settings);

        int duration = settings["duration"] | 0;
        int delay = settings["delay"] | 0;

        if (targetAction == "on") {
            handlePowerAction(true, delay, duration);
        } 
        else if (targetAction == "off") {
            handlePowerAction(false, delay, duration);
        }
        else if (targetAction == "toggle") {
            handlePowerAction(!isOn, 0, 0);
        }
    }
};