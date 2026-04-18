#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>
#include "BasePeripheral.h"
#include "Message.h"
#include "ConfigManager.h"
#include "EventEngine.h"

class ActiveBuzzer : public BasePeripheral {
private:
    int pin;
    bool isPlaying = false;
    long pattern[16]; 
    size_t patternSize = 0;
    int repeatCount = 0;
    
    int currentStep = 0;
    unsigned long lastStepTime = 0;
    unsigned long stepDuration = 0;

void startSequence(JsonVariant root) {
        if (!root["pattern"].is<JsonArray>()) return;

        JsonArray arr = root["pattern"].as<JsonArray>();
        patternSize = arr.size();
        
        if (patternSize > 16) patternSize = 16; 
        if (patternSize == 0) return;

        for (size_t i = 0; i < patternSize; i++) {
            pattern[i] = arr[i];
        }

        repeatCount = root["repeat"] | 1;
        isPlaying = true;
        currentStep = 0;
        executeStep();
    }

    void executeStep() {
        stepDuration = pattern[currentStep % patternSize];
        lastStepTime = millis();
        
        digitalWrite(pin, (currentStep % 2 == 0) ? HIGH : LOW);
    }

public:
    ActiveBuzzer(int id, SystemContext& ctx, JsonObject cfg)
        : BasePeripheral(id, ctx)
    {
        pin = cfg["config"]["pin"];
    }

    void begin() override {
        pinMode(pin, OUTPUT);
        digitalWrite(pin, LOW);
    }

    void loop() override {
      if (!isPlaying) return;

        if (millis() - lastStepTime >= stepDuration) {
            currentStep++;
            
            if (currentStep >= (int)patternSize * repeatCount) {
                isPlaying = false;
                digitalWrite(pin, LOW);
            } else {
                executeStep();
            }
        }
    }

    void onMessage(Message& msg) override {
        if(msg.command=="play_sequence"){
            startSequence(msg.payload);
        }
    }

    void triggerAction(String targetAction, String extraSettings) override {
        if(targetAction=="play_sequence"){
            JsonDocument doc;
            JsonObject settings;
            getSettings(extraSettings, doc, settings);
            startSequence(settings);
        }
    }

};