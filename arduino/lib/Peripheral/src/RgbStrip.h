#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>
#include "BasePeripheral.h"
#include "ButtonMessage.h"
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
    String baseStatePath;
    ConfigManager& stateManager;

public:
    RgbStrip(int id, EventEngine& engine, ConfigManager& stateManager,
              JsonObject config)
        : BasePeripheral(id, engine), stateManager(stateManager)
    {
        frequency = config["config"]["frequency"] | 1000;
        resolution = config["config"]["resolution_bits"] | 8;
        rPin  = config["config"]["r_pin"] | -1;
        gPin  = config["config"]["g_pin"] | -1;
        bPin  = config["config"]["b_pin"] | -1;
        
        baseStatePath = "states." + String(id) + ".";
        
    }
    
    void begin() override {
        if (rPin != -1) pinMode(rPin, OUTPUT);
        if (gPin != -1) pinMode(gPin, OUTPUT);
        if (bPin != -1) pinMode(bPin, OUTPUT);
        
        analogWriteRange(1023);
        if (frequency > 0) analogWriteFreq(frequency);
        
        currentR = stateManager.get((baseStatePath+"r_duty_cycle").c_str());
        currentG = stateManager.get((baseStatePath+"g_duty_cycle").c_str());
        currentB = stateManager.get((baseStatePath+"b_duty_cycle").c_str());
        brightness = stateManager.get((baseStatePath+"brightness").c_str());
        isOn = stateManager.get((baseStatePath+"is_on").c_str());
        apply();
    }

    void loop() override {}

    void onMessage(Message& msg) override {
        if(msg.command == "update_state"){
            updateState(msg);
        } else if(msg.command == "toggle"){
            toggle(msg);
        }
    }

    void triggerAction(String targetAction, String extraSettings) override {
        if(targetAction=="update_state"){
            if (extraSettings.length()==0 || extraSettings == "{}") {
                return;
            }

            JsonDocument payload;
            Serial.println(extraSettings);
            DeserializationError error = deserializeJson(payload, extraSettings);
            
            if (error){
                Serial.print("Parsing errors: ");
                Serial.println(error.f_str());
                return;
            }
            serializeJson(payload, Serial);

            currentR = payload["r_duty_cycle"] | currentR;
            currentG = payload["g_duty_cycle"] | currentG;
            currentB = payload["b_duty_cycle"] | currentB;
            isOn = payload["is_on"] | isOn;
            brightness = payload["brightness"] | brightness;

            stateManager.set((baseStatePath+"r_duty_cycle").c_str(), currentR);
            stateManager.set((baseStatePath+"g_duty_cycle").c_str(), currentG);
            stateManager.set((baseStatePath+"b_duty_cycle").c_str(), currentB);
            stateManager.set((baseStatePath+"brightness").c_str(), brightness);
            stateManager.set((baseStatePath+"is_on").c_str(), isOn);
            stateManager.save();

            apply();
            
            JsonDocument responsePayload;
            notify("on_state_updated", "on_state_updated", responsePayload);
            
        }else if ("toggle"){
            isOn = !isOn;
            apply();

            stateManager.set((baseStatePath+"is_on").c_str(), isOn);
            stateManager.save();

            JsonDocument payload;
            payload["is_on"] = isOn;
            notify("on_toggle", "on_toggle", payload, "",MessageType::EVENT);
        }

    }


    void updateState(Message& msg){
        JsonObject p = msg.payload.as<JsonObject>();
        currentR = p["r_duty_cycle"] | currentR;
        currentG = p["g_duty_cycle"] | currentG;
        currentB = p["b_duty_cycle"] | currentB;
        isOn = p["is_on"] | isOn;
        brightness = p["brightness"] | brightness;

        stateManager.set((baseStatePath+"r_duty_cycle").c_str(), currentR);
        stateManager.set((baseStatePath+"g_duty_cycle").c_str(), currentG);
        stateManager.set((baseStatePath+"b_duty_cycle").c_str(), currentB);
        stateManager.set((baseStatePath+"brightness").c_str(), brightness);
        stateManager.set((baseStatePath+"is_on").c_str(), isOn);
        stateManager.save();

        apply();

        JsonDocument payload;
        payload["status"] = static_cast<uint8_t>(ActionResult::ACCEPTED);
        notify("on_state_updated", msg.command, payload, msg.message_id);
    }

    void toggle(Message& msg){

        isOn = !isOn;
        apply();

        stateManager.set((baseStatePath+"is_on").c_str(), isOn);
        stateManager.save();

        JsonDocument payload;
        payload["is_on"] = isOn;
        payload["status"] = static_cast<uint8_t>(ActionResult::ACCEPTED);
        notify("on_toggle", msg.command, payload, msg.message_id);
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

};