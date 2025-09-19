#include "Arduino.h"
#include "Mqtt.h"
#include "lamp.h"
#include "Adafruit_PWMServoDriver.h"
#include "ArduinoJson.h"
#include "BasicMessage.h"

Lamp::Lamp(Mqtt& mqtt, int lampCount) : mqtt(mqtt),lampCount(lampCount) {
    isPending = false;
    lampOn = false;
    brightness = 4095;
    step = 20;
    lightingTime = 10000;
    reverse = false;
    state = IDLE;
    currentLampIndex = 0;
    pwm = Adafruit_PWMServoDriver();
    pwm.begin();    
    pwm.setPWMFreq(50);
    for (int i = 0; i < 15; i++) {
        pwm.setPWM(i, 0, 0);
    }
};

void Lamp::loop() {
    updateLamp();
}

void Lamp::onMessage(Message message) {
    reverse = false;
    if (message.payload["reverse"].is<bool>()) {
        reverse = message.payload["reverse"].as<bool>();
    }
    if (message.message_event == "get_settings") {
        setSettings(message);
        mqtt.sendMessage(basicResponse(message));
    } else if (message.message_event == "set_settings") {
        setSettings(message);
        mqtt.sendMessage(basicResponse(message));
    } else if (message.message_event == "off") {
        turnOffLampRequest();
        mqtt.sendMessage(basicResponse(message));
    } else if (message.message_event == "on") {
        turnOnLampRequest();
        mqtt.sendMessage(basicResponse(message));
    } else if (message.message_event == "blink") {
        blinkLampRequest();
        mqtt.sendMessage(basicResponse(message));
    } else if (message.message_event == "toggle") {
        toggleLampRequest();
        mqtt.sendMessage(basicResponse(message));
    }
}

void Lamp::updateLamp() {

    switch (state) {
        case IDLE:
            break;

        case TURNING_ON:
            if(!_turnOn()) break; 
            lampOn = true;
            isPending = false;
            state = IDLE;
            break;

        case TURNING_OFF:
            if(!_turnOff()) break; 
            lampOn = false;
            isPending = false;
            state = IDLE;
            break;

        case BLINKING:
            if (!lampOn) {
                lampOn = _turnOn();
                lastUpdate = millis();
                break;
            }
            if (millis() - lastUpdate < lightingTime) break;
            isPending = false;
            turnOffLampRequest();
            break;
    }
}

void Lamp::turnOnLampRequest() {
    if (isPending || lampOn) return;
    isPending = true;
    state = TURNING_ON;
    lastUpdate = millis();
    currentBrightness = 0;
    currentLampIndex = reverse ? lampCount - 1 : 0;
}

void Lamp::turnOffLampRequest() {
    if (isPending || !lampOn) return;
    isPending = true;
    state = TURNING_OFF;
    lastUpdate = millis();
    currentBrightness = brightness;
    currentLampIndex = reverse ? lampCount - 1 : 0;  
}

void Lamp::blinkLampRequest() {
    if (isPending) return;
    isPending = true;
    state = BLINKING;
    currentBrightness = 0;
    currentLampIndex = reverse ? lampCount - 1 : 0;  
}

void Lamp::toggleLampRequest() {
    if (isPending) return;
    isPending = true;
    state = lampOn ? TURNING_OFF : TURNING_ON;
    lastUpdate = millis();
    currentBrightness = lampOn ? brightness : 0;
    currentLampIndex = reverse ? lampCount - 1 : 0;  
}

void Lamp::setSettings(Message message) {
    if (message.payload["brightness"].is<int>()) {
        brightness = message.payload["brightness"].as<int>() * 40.95;
    }
    if (message.payload["step"].is<int>()) {
        step = message.payload["step"].as<int>();
    }
    if (message.payload["lightingTime"].is<int>()) {
        lightingTime = message.payload["lightingTime"].as<int>()*1000;
    }
}

bool Lamp::_turnOn(){
    if (currentBrightness < brightness) {
        currentBrightness += step;
        if (currentBrightness > brightness) currentBrightness = brightness;
        pwm.setPWM(currentLampIndex, 0, currentBrightness);
    } else {
        currentLampIndex = reverse ? currentLampIndex - 1 : currentLampIndex + 1;
        if (currentLampIndex >= 0 && currentLampIndex < lampCount) {
            currentBrightness = 0;
            pwm.setPWM(currentLampIndex, 0, currentBrightness);
        }
        return (reverse && currentLampIndex < 0) || (!reverse && currentLampIndex >= lampCount);
    }
    return false;
};

bool Lamp::_turnOff(){
    if (currentBrightness > 0) {
        currentBrightness -= step;
        if (currentBrightness < 0) currentBrightness = 0;
        pwm.setPWM(currentLampIndex, 0, currentBrightness);
    } else {
        currentLampIndex = reverse ? currentLampIndex - 1 : currentLampIndex + 1;
        if (currentLampIndex >= 0 && currentLampIndex < lampCount) {
            currentBrightness = brightness;
            pwm.setPWM(currentLampIndex, 0, currentBrightness);
        }
        return (reverse && currentLampIndex < 0) || (!reverse && currentLampIndex >= lampCount);
    }
    return false;
};