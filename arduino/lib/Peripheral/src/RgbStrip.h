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
    int rRes, gRes, bRes;
    int rFreq, gFreq, bFreq;
    int currentR = 0, currentG = 0, currentB = 0;
    int brightness = 100;
    bool isOn = false;
    String baseStatePath;
    ConfigManager& stateManager;

public:
    RgbStrip(int id, EventEngine& engine, ConfigManager& stateManager,
              JsonObject config, JsonObject state)
        : BasePeripheral(id, engine), stateManager(stateManager)
    {
        rPin  = config["config"]["r_pin"]["pin"]["pin"] | -1;
        rFreq = config["config"]["r_pin"]["frequency"] | 1000;
        rRes  = config["config"]["r_pin"]["resolution_bits"] | 8;

        gPin  = config["config"]["g_pin"]["pin"]["pin"] | -1;
        gFreq = config["config"]["g_pin"]["frequency"] | 1000;
        gRes  = config["config"]["g_pin"]["resolution_bits"] | 8;

        bPin  = config["config"]["b_pin"]["pin"]["pin"] | -1;
        bFreq = config["config"]["b_pin"]["frequency"] | 1000;
        bRes  = config["config"]["b_pin"]["resolution_bits"] | 8;
        baseStatePath = "states." + String(id) + ".";
    }

    void begin() override {
        if (rPin != -1) pinMode(rPin, OUTPUT);
        if (gPin != -1) pinMode(gPin, OUTPUT);
        if (bPin != -1) pinMode(bPin, OUTPUT);

        analogWriteRange(1023);
        if (rFreq > 0) analogWriteFreq(rFreq);

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

    void triggerAction(String targetAction, String extraSettings) override {}


    void updateState(Message& msg){
        JsonObject p = msg.payload.as<JsonObject>();
        currentR = p["r_pin"]["duty_cycle"] | currentR;
        currentG = p["g_pin"]["duty_cycle"] | currentG;
        currentB = p["b_pin"]["duty_cycle"] | currentB;
        isOn = p["is_on"] | isOn;
        brightness = p["brightness"] | brightness;

        stateManager.set((baseStatePath+"r_pin.duty_cycle").c_str(), currentR);
        stateManager.set((baseStatePath+"g_pin.duty_cycle").c_str(), currentG);
        stateManager.set((baseStatePath+"b_pin.duty_cycle").c_str(), currentB);
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

        if (rPin != -1) analogWrite(rPin, calculateDuty(currentR, rRes));
        if (gPin != -1) analogWrite(gPin, calculateDuty(currentG, gRes));
        if (bPin != -1) analogWrite(bPin, calculateDuty(currentB, bRes));
    }

};