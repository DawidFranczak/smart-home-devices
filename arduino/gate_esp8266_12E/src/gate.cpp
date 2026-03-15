#include <Arduino.h>
#include "gate.h"

Gate::Gate(ConfigManager& configManager) :
 configManager(configManager){
    accessGrantedFlag = false;
    accessDeniedFlag = false;
    lastAccessGrantedTime = 0;
    lastAccessDeniedTime = 0;
    accessDeniedCounter = 0;
}
void Gate::begin(){
    gatePin = configManager.get("device.gatePin").as<int>();
    buzzerPin = configManager.get("device.buzzerPin").as<int>();
    openGateTime = configManager.get("device.openGateTimeout").as<int>();;
    pinMode(gatePin, OUTPUT);
    pinMode(buzzerPin, OUTPUT);
}
void Gate::loop(){
    if (accessGrantedFlag) {
        _accessGranted();
    } else if (accessDeniedFlag) {
        _accessDenied();
    }
}
void Gate::accessGranted(int openGateTime){    
    accessDeniedFlag = false;
    accessGrantedFlag = true;
    this->openGateTime = openGateTime;
    lastAccessGrantedTime = millis();
}

void Gate::accessDenied(){
    accessDeniedFlag = true;
    accessGrantedFlag = false;
    lastAccessDeniedTime = millis();
}

void Gate::_accessGranted(){
    digitalWrite(gatePin, HIGH);
    digitalWrite(buzzerPin, HIGH); 
    if (millis() - lastAccessGrantedTime < openGateTime) return;
    digitalWrite(gatePin, LOW);
    digitalWrite(buzzerPin, LOW);
    accessGrantedFlag = false;
    lastAccessGrantedTime = millis();

}
void Gate::_accessDenied(){
    digitalWrite(gatePin, LOW);
    if (millis() - lastAccessDeniedTime > 600) {
        accessDeniedCounter++;
        lastAccessDeniedTime = millis();
        if (accessDeniedCounter < 4) return; 
        accessDeniedFlag = false;
        accessDeniedCounter = 0;
        digitalWrite(buzzerPin, LOW);
    }else if (millis() - lastAccessDeniedTime > 400) {
        digitalWrite(buzzerPin, LOW);
        return;
    }else if (millis() - lastAccessDeniedTime > 200) {
        digitalWrite(buzzerPin, HIGH);
        return;
    }
}