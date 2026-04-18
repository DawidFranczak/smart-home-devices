#include "SequentialLight.h"

SequentialLight::SequentialLight(int id, SystemContext& systemContext, JsonObject config)
    : BasePeripheral(id, systemContext)
{
    address = config["config"]["address"] | 0x40;
    frequency = config["config"]["frequency"] | 50;
    lightCount  = config["config"]["light_count"] | 16;
    isPending = false;
    lightOn = false;
    reverse = false;
    state = IDLE;
    currentLightIndex = 0;
}

void SequentialLight::begin(){
    speedBase = systemContext.stateManager.get((baseStatePath+"speed").c_str());
    brightnessBase = systemContext.stateManager.get((baseStatePath+"brightness").c_str());
    lightingTimeBase = systemContext.stateManager.get((baseStatePath+"lighting_time").c_str());
    mapSettings();
    
    JsonArray periodsArr = systemContext.stateManager.config["states"][String(id)]["lighting_period"].as<JsonArray>();
    
    updateLightningPeriod(periodsArr);

    pwm = Adafruit_PWMServoDriver();
    pwm.begin();    
    pwm.setPWMFreq(100);
    for (int i = 0; i < 16; i++) {
        pwm.setPWM(i, 0, 0);
    }
}

void SequentialLight::loop() {
    static unsigned long lastAnimStep =0;
    if (millis() - lastAnimStep < 20) return; 
    lastAnimStep = millis();
    update();
}
void SequentialLight::mapSettings(){
    speed = speedBase *  3;
    brightness = brightnessBase * 40.95;
    lightingTime = lightingTimeBase * 1000;
}

void SequentialLight::onMessage(Message& message) {
    reverse = message.payload["reverse"].is<bool>() ? message.payload["reverse"].as<bool>(): false;
    lightingTime = message.payload["lighting_time"].is<int>() ? message.payload["lighting_time"].as<int>()*1000 : lightingTimeBase * 1000;
    speed = message.payload["speed"].is<int>() ? message.payload["speed"].as<int>()*3 : speedBase *3;
    
    JsonDocument payload;
    payload["status"] = static_cast<uint8_t>(ActionResult::ACCEPTED);
    if (message.command == "update_state") {
        notify(message, ActionResult::ACCEPTED);
        updateState(message);
        JsonDocument payload2;
        payload2["brightness"]=brightnessBase;
        payload2["speed"]=speedBase;
        payload2["lighting_time"]=lightingTimeBase;
        payload2["status"] = static_cast<uint8_t>(OnBlinkStatus::START);
        notify("on_off");
    } else if (message.command == "off") {
        notify(message, ActionResult::ACCEPTED);
        turnOff();
        JsonDocument payload2;
        notify("on_off");
    } else if (message.command == "on") {
        notify(message, ActionResult::ACCEPTED);
        turnOn();
        JsonDocument payload2;
        notify("on_on");
    } else if (message.command == "blink") {
        notify(message, ActionResult::ACCEPTED);
        blink();
    } else if (message.command == "toggle") {
        notify(message, ActionResult::ACCEPTED);
        toggle();
        if(!lightOn) notify("on_on");
        else  notify("on_off");
    }
}

void SequentialLight::triggerAction(String targetAction, String extraSettings){

    JsonDocument payload;
    DeserializationError error = deserializeJson(payload, extraSettings);
    JsonDocument payloadResponse;
    
    if (error){
        Serial.print("Parsing errors: ");
        Serial.println(error.f_str());
        return;
    }
    reverse = payload["reverse"] | false;
    lightingTime = payload["lighting_time"].is<int>() 
                ? payload["lighting_time"].as<uint32_t>() * 1000 
                : lightingTimeBase * 1000;

    speed = payload["speed"].is<int>() 
        ? payload["speed"].as<int>() * 3 
        : speedBase *3;

    if (targetAction == "off") {
        turnOff();
        notify("on_off");
    } else if (targetAction == "on") {
        turnOn();
        notify("on_on");
    } else if (targetAction == "blink") {
        blink();
    } else if (targetAction == "toggle") {
        toggle();
        if(!lightOn) notify("on_on");
        else  notify("on_off");
    }
}

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
        brightnessBase = message.payload["brightness"].as<int>();
        systemContext.stateManager.set((baseStatePath+"brightness").c_str(), brightnessBase);
    }
    if (message.payload["speed"].is<int>()) {
        speedBase = message.payload["speed"].as<int>();
        systemContext.stateManager.set((baseStatePath+"speed").c_str(), speedBase);
    }
    if (message.payload["lighting_time"].is<int>()) {
        lightingTimeBase = message.payload["lighting_time"].as<int>();
        systemContext.stateManager.set((baseStatePath+"lighting_time").c_str(), lightingTimeBase);
    }
    if (message.payload["lighting_period"].is<JsonArray>()) {
        JsonArray arr = message.payload["lighting_period"].as<JsonArray>();
        updateLightningPeriod(arr);
        systemContext.stateManager.set((baseStatePath+"lighting_period").c_str(), arr);
    }
    mapSettings();
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
        currentBrightness += speed;
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
        currentBrightness -= speed;
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