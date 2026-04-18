#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>
#include "BasePeripheral.h"
#include "Message.h"
#include "ConfigManager.h"
#include "EventEngine.h"

class RgbStrip : public BasePeripheral {
private:
    int rPin, gPin, bPin;
    int resolution;
    int frequency;
    int currentR = 0, currentG = 0, currentB = 0;
    int brightness = 100;
    bool isOn = false;

public:
    RgbStrip(int id, SystemContext& systemContext, JsonObject cfg)
        : BasePeripheral(id, systemContext)
    {
        frequency = cfg["config"]["frequency"] | 1000;
        resolution = cfg["config"]["resolution_bits"] | 8;
        rPin  = cfg["config"]["r_pin"] | -1;
        gPin  = cfg["config"]["g_pin"] | -1;
        bPin  = cfg["config"]["b_pin"] | -1;
    }
    
    void begin() override {
        if (rPin != -1) pinMode(rPin, OUTPUT);
        if (gPin != -1) pinMode(gPin, OUTPUT);
        if (bPin != -1) pinMode(bPin, OUTPUT);
        
        analogWriteRange(1023);
        if (frequency > 0) analogWriteFreq(frequency);
        currentR = systemContext.stateManager.get((baseStatePath+"r_duty_cycle").c_str());
        currentG = systemContext.stateManager.get((baseStatePath+"g_duty_cycle").c_str());
        currentB = systemContext.stateManager.get((baseStatePath+"b_duty_cycle").c_str());
        brightness = systemContext.stateManager.get((baseStatePath+"brightness").c_str());
        isOn = systemContext.stateManager.get((baseStatePath+"is_on").c_str());
        apply();
    }

    void loop() override {}

    void onMessage(Message& msg) override {
        if(msg.command == "update_state"){
            notify(msg, ActionResult::ACCEPTED);
            JsonObject payload = msg.payload.as<JsonObject>();
            updateState(payload);
            saveState();
            apply();
            sendOnStateUpdate();

        } else if(msg.command == "toggle"){
            notify(msg, ActionResult::ACCEPTED);
            toggle();
        } else if(msg.command == "on"){
            notify(msg, ActionResult::ACCEPTED);
            on();
        } else if(msg.command == "off"){
            notify(msg, ActionResult::ACCEPTED);
            off();
        }
    }

    void triggerAction(String targetAction, String extraSettings) override {
        if(targetAction=="update_state"){
            JsonDocument doc;
            JsonObject settings;
            bool hasSettings = getSettings(extraSettings, doc, settings);
            if(!hasSettings) return;

            updateState(settings);
            saveState();
            apply();
            sendOnStateUpdate();
            
        }else if (targetAction=="toggle"){
            toggle();
        }else if(targetAction == "on"){
            on();
        } else if(targetAction == "off"){
            off();
        }

    }

private:

    void toggle(){
        if (isOn) off();
        else if (!isOn) on();
    }
    
    void on(){
        if(isOn) return;
        isOn = true;
        apply();
        saveState();
        notify("on_on");
    }

    void off(){
        if(!isOn) return;
        isOn = false;
        apply();
        saveState();
        notify("on_off");
    }

    void apply() {
        if (!isOn) {
            if (rPin != -1) analogWrite(rPin, 0);
            if (gPin != -1) analogWrite(gPin, 0);
            if (bPin != -1) analogWrite(bPin, 0);
            return;
        }

        float factor = brightness / 100.0f;

        auto calculateDuty = [&](int val, int res) {
            int maxVal = (1 << res) - 1;
            if (maxVal <= 0) return 0;
            return (int)((val / (float)maxVal) * 1023 * factor);
        };

        if (rPin != -1) analogWrite(rPin, calculateDuty(currentR, resolution));
        if (gPin != -1) analogWrite(gPin, calculateDuty(currentG, resolution));
        if (bPin != -1) analogWrite(bPin, calculateDuty(currentB, resolution));
    }
    
    void updateState(JsonObject state){
        currentR = state["r_duty_cycle"] | currentR;
        currentG = state["g_duty_cycle"] | currentG;
        currentB = state["b_duty_cycle"] | currentB;
        isOn = state["is_on"] | isOn;
        brightness = state["brightness"] | brightness;
    }

    void saveState(){
        systemContext.stateManager.set((baseStatePath+"r_duty_cycle").c_str(), currentR);
        systemContext.stateManager.set((baseStatePath+"g_duty_cycle").c_str(), currentG);
        systemContext.stateManager.set((baseStatePath+"b_duty_cycle").c_str(), currentB);
        systemContext.stateManager.set((baseStatePath+"brightness").c_str(), brightness);
        systemContext.stateManager.set((baseStatePath+"is_on").c_str(), isOn);
        systemContext.stateManager.save();
    }

    void sendOnStateUpdate(){
        JsonDocument payload;
        payload["r_duty_cycle"] = currentR;
        payload["g_duty_cycle"] = currentG;
        payload["b_duty_cycle"] = currentB;
        payload["is_on"] = isOn;
        payload["brightness"] = brightness;

        notify("on_state_updated", payload);
    }
};