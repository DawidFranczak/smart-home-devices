#include <Arduino.h>
#include "gate.h"

Gate::Gate(int gatePin, int buzzerPin) :
 gatePin(gatePin), buzzerPin(buzzerPin) {
    pinMode(gatePin, OUTPUT);
    pinMode(buzzerPin, OUTPUT);
    accessGrantedFlag = false;
    accessDeniedFlag = false;
    openGateTime = 10000;
    lastAccessGrantedTime = 0;
    lastAccessDeniedTime = 0;
    accessDeniedCounter = 0;
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