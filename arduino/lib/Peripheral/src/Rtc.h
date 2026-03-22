#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>
#include "BasePeripheral.h"
#include "ButtonMessage.h"
#include "ConfigManager.h"
#include "EventEngine.h"

class Rtc : public BasePeripheral {
private:
    bool rtcSetup = false;
    time_t lastSec = 0;
    ConfigManager& stateManager;
    unsigned long lastSync = 0;
    unsigned long now = millis();

    void requestSync() {
        JsonDocument payload;
        notify("on_sync_time", "on_sync_time", payload, "", MessageType::EVENT, MessageTarget::BACKEND);
    }

public:
    Rtc(int id, EventEngine& engine, ConfigManager& stateManager,
              JsonObject cfg)
        : BasePeripheral(id, engine), stateManager(stateManager)
    {
        lastSync = millis();
    }

    void begin() override {
        requestSync();
    }

    void loop() override {
        if(now - lastSync > 3600000){
          lastSync=now;
         requestSync();
        }

        if(!rtcSetup) return;

        time_t now_ts;
        time(&now_ts);

       if (now_ts != lastSec) {
            lastSec = now_ts;

            struct tm timeinfo = {0}; 
            if (localtime_r(&now_ts, &timeinfo)) {
                if (timeinfo.tm_sec == 0) {
                    float time = timeinfo.tm_hour *60 + timeinfo.tm_min;
                    JsonDocument payload;
                    notify("on_time", "on_time", payload, "", MessageType::EVENT, MessageTarget::INTERNAL, time);
                }
            }
        }
    }

    void onMessage(Message& msg) override {
        if(msg.command == "on_sync_time"){
            syncTime(msg);
        }
    }

    void triggerAction(String targetAction, String extraSettings) override {}

    void syncTime(Message& message){
    if (message.payload["timestamp"].is<long>()) {
        struct timeval tv;
        tv.tv_sec = message.payload["timestamp"].as<long>();; 
        tv.tv_usec = 0;
        settimeofday(&tv, NULL);    
        rtcSetup = true;
    }


}
};