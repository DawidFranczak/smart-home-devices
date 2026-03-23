#include "SequentialLight.h"

SequentialLight::SequentialLight(int id, SystemContext& systemContext, JsonObject config)
    : BasePeripheral(id, systemContext)
{
    address = config["config"]["address"] | 0x40;
    frequency = config["config"]["frequency"] | 50;
    lightCount  = config["config"]["light_count"] | 16;
}

void SequentialLight::begin(){
    step = systemContext.stateManager.get((baseStatePath+".step").c_str());
    brightness = systemContext.stateManager.get((baseStatePath+".brightness").c_str());
    lightingTime = systemContext.stateManager.get((baseStatePath+".lighting_time").c_str());
    
    JsonArray periodsArr = systemContext.stateManager.config["states"][String(id)]["lighting_period"].as<JsonArray>();
    
    updateLightningPeriod(periodsArr);

    pwm = Adafruit_PWMServoDriver();
    pwm.begin();    
    pwm.setPWMFreq(50);
    for (int i = 0; i < 16; i++) {
        pwm.setPWM(i, 0, 0);
    }
    Serial.println("SequentialLight started");
}

void SequentialLight::loop() {
    update();
}

void SequentialLight::onMessage(Message& message) {
    reverse = false;
    if (message.payload["reverse"].is<bool>()) {
        reverse = message.payload["reverse"].as<bool>();
    }
    if (message.command == "update_state") {
        updateState(message);
    } else if (message.command == "off") {
        turnOff();
    } else if (message.command == "on") {
        turnOn();
    } else if (message.command == "blink") {
        blink();
    } else if (message.command == "toggle") {
        toggle();
    }
}

void SequentialLight::triggerAction(String targetAction, String extraSettings){}

void SequentialLight::update() {

    switch (state) {
        case IDLE:
            break;

        case TURNING_ON:
            if(!_turnOn()) break; 
            lightOn = true;
            isPending = false;
            state = IDLE;
            break;

        case TURNING_OFF:
            if(!_turnOff()) break; 
            lightOn = false;
            isPending = false;
            state = IDLE;
            break;

        case BLINKING:
            if (!lightOn) {
                lightOn = _turnOn();
                lastUpdate = millis();
                break;
            }
            if (millis() - lastUpdate < lightingTime) break;
            isPending = false;
            turnOff();
            break;
    }
}

void SequentialLight::turnOn() {
    if (isPending || lightOn) return;
    isPending = true;
    state = TURNING_ON;
    lastUpdate = millis();
    currentBrightness = 0;
    currentLightIndex = reverse ? lightCount - 1 : 0;
}

void SequentialLight::turnOff() {
    if (isPending || !lightOn) return;
    isPending = true;
    state = TURNING_OFF;
    lastUpdate = millis();
    currentBrightness = brightness;
    currentLightIndex = reverse ? lightCount - 1 : 0;  
}

void SequentialLight::blink() {
    if (isPending) return;
    isPending = true;
    state = BLINKING;
    currentBrightness = 0;
    currentLightIndex = reverse ? lightCount - 1 : 0;  
}

void SequentialLight::toggle() {
    if (isPending) return;
    isPending = true;
    state = lightOn ? TURNING_OFF : TURNING_ON;
    lastUpdate = millis();
    currentBrightness = lightOn ? brightness : 0;
    currentLightIndex = reverse ? lightCount - 1 : 0;  
}

void SequentialLight::updateState(Message message) {
    if (message.payload["brightness"].is<int>()) {
        brightness = message.payload["brightness"].as<int>() * 40.95;
        systemContext.stateManager.set((baseStatePath+"brightness").c_str(), brightness);
    }
    if (message.payload["step"].is<int>()) {
        step = message.payload["step"].as<int>()*3;
        systemContext.stateManager.set((baseStatePath+"step").c_str(), step);
    }
    if (message.payload["lighting_time"].is<int>()) {
        lightingTime = message.payload["lighting_time"].as<int>()*1000;
        systemContext.stateManager.set((baseStatePath+"lighting_time").c_str(), lightingTime);
    }
    if (message.payload["lighting_period"].is<JsonArray>()) {
        JsonArray arr = message.payload["lighting_period"].as<JsonArray>();
        updateLightningPeriod(arr);
        systemContext.stateManager.set((baseStatePath+"lighting_period").c_str(), arr);
    }
    systemContext.stateManager.save();
}

void SequentialLight::updateLightningPeriod(JsonArray periods){
    lightingPeriods.clear();
    if (!periods.isNull()) {
        for (JsonObject v : periods) {
            LightingPeriod lp;
            lp.start_time = v["start_time"] | 0;
            lp.end_time = v["end_time"] | 0;
            lp.brightness = v["brightness"] | 100;
            
            lightingPeriods.push_back(lp);
        }
    }
}

bool SequentialLight::_turnOn(){
    if (currentBrightness < brightness) {
        currentBrightness += step;
        if (currentBrightness > brightness) currentBrightness = brightness;
        pwm.setPWM(currentLightIndex, 0, currentBrightness);
    } else {
        currentLightIndex = reverse ? currentLightIndex - 1 : currentLightIndex + 1;
        if (currentLightIndex >= 0 && currentLightIndex < lightCount) {
            currentBrightness = 0;
            pwm.setPWM(currentLightIndex, 0, currentBrightness);
        }
        return (reverse && currentLightIndex < 0) || (!reverse && currentLightIndex >= lightCount);
    }
    return false;
};

bool SequentialLight::_turnOff(){
    if (currentBrightness > 0) {
        currentBrightness -= step;
        if (currentBrightness < 0) currentBrightness = 0;
        pwm.setPWM(currentLightIndex, 0, currentBrightness);
    } else {
        currentLightIndex = reverse ? currentLightIndex - 1 : currentLightIndex + 1;
        if (currentLightIndex >= 0 && currentLightIndex < lightCount) {
            currentBrightness = brightness;
            pwm.setPWM(currentLightIndex, 0, currentBrightness);
        }
        return (reverse && currentLightIndex < 0) || (!reverse && currentLightIndex >= lightCount);
    }
    return false;
};