#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>
#include "BasePeripheral.h"
#include "Message.h"
#include "ConfigManager.h"
#include "EventEngine.h"
#include <Adafruit_AHTX0.h>

class Aht10 : public BasePeripheral {
private:
    Adafruit_AHTX0 aht;
    int address;
    int readInterval;
    int nextRunTotalMin=0;
    int lastProcessedMin = -1;
    int isInitialized = false;
    unsigned long lastOnErrorMessage=0;

    void sendError(String msg) {
        if (millis() - lastOnErrorMessage > 3600000 || lastOnErrorMessage == 0) {
            // JsonDocument errorPayload;
            // errorPayload["message"] = msg;
            // errorPayload["type"] = "AHT10_ERROR";
            // notify("on_error", errorPayload);
            lastOnErrorMessage = millis();
        }
    }
    void sendData(){
        if (!isInitialized) {
            if (!aht.begin(&Wire, this->id, (uint8_t)address)) {
                sendError("Could not initialize AHT10");
                return;
            }
            isInitialized = true;
        }
        
        sensors_event_t humidity, temp;
        if (!aht.getEvent(&humidity, &temp)){
            isInitialized = false;
            sendError("Failed to get sensor events");
            return;
        }

        JsonDocument temperaturePayload;
        temperaturePayload["value"]=temp.temperature;
        notify("on_measure_temperature",temperaturePayload, temp.temperature);

        JsonDocument humidityPayload;
        humidityPayload["value"]= humidity.relative_humidity;
        notify("on_measure_humidity",humidityPayload, humidity.relative_humidity);
    }

    int getMinutesToNextRun(int currentTotalMin, int intervalMin) {
        if (intervalMin == 0) return 1; 
        int minsPastWindow = currentTotalMin % intervalMin;
        return intervalMin - minsPastWindow;
    }

public:
    Aht10(int id, SystemContext& ctx, JsonObject cfg)
        : BasePeripheral(id, ctx)
    {
        address = cfg["config"]["address"];
        readInterval = cfg["config"]["read_interval"];
    }

    void begin() override {
       if (aht.begin(&Wire, this->id, (uint8_t)address)) { 
            isInitialized = true;
            Serial.println("AHT10: zainicjalizowany");
        } else {
            isInitialized = false;
            Serial.println("AHT10: Not found at begin()");
            sendError("AHT10 not found at startup");
        }
    }

    void loop() override {
        if (!systemContext.timeManager.isActive) return;
        int currentMin = systemContext.timeManager.getTimeInMin();

        if (currentMin != lastProcessedMin && (currentMin >= nextRunTotalMin || nextRunTotalMin == 0)) {
            lastProcessedMin = currentMin;

            sendData();

            int delay = getMinutesToNextRun(currentMin, readInterval);
            nextRunTotalMin = currentMin + delay;

            if (nextRunTotalMin >= 1440) {
                nextRunTotalMin -= 1440;
            }

            Serial.printf("AHT10: Odczyt o %02d:%02d. Następny za %d min.\n", 
                            currentMin/60, currentMin%60, delay);
        }
    }

    void onMessage(Message& msg) override {}

    void triggerAction(String targetAction, String extraSettings) override {}

};