#pragma once
#include <vector>
#include <Arduino.h>
#include <ArduinoJson.h>
#include "BasePeripheral.h"
#include "ButtonMessage.h"
#include "SystemContext.h"
#include "ConfigManager.h"
#include "Adafruit_PWMServoDriver.h"

enum SequentialLightState {
    IDLE,
    TURNING_ON,
    TURNING_OFF,
    BLINKING,
};

struct LightingPeriod {
    int start_time;
    int end_time;
    int brightness;
};

class SequentialLight : public BasePeripheral {

public:
    SequentialLight(int id, SystemContext& systemContext, JsonObject config);
    void begin();
    void loop();
    void onMessage(Message& message);
    void triggerAction(String targetAction, String extraSettings);
    void turnOn();
    void turnOff();
    void blink();
    void toggle();
    bool isPending;
    
    private:
    std::vector<LightingPeriod> lightingPeriods;    
    int lightCount;
    int address;
    int frequency;
    int currentLightIndex;
    bool lightOn;
    int brightnessBase;
    int stepBase;
    int brightness;
    int step;
    bool reverse;
    unsigned long lightingTimeBase;
    unsigned long lightingTime;
    unsigned long lastUpdate;
    int currentLight;
    int currentBrightness;
    SequentialLightState state;
    unsigned long blinkStartTime;

    Adafruit_PWMServoDriver pwm;
    void update();
    void updateState(Message message);
    void updateLightningPeriod(JsonArray periods);
    bool _turnOn();
    bool _turnOff();
};