#include <Arduino.h>
#include <BasicMessage.h>
#include "aquarium.h"

Aquarium::Aquarium(int rPin, int gPin, int bPin, int fluoPin, Mqtt& mqtt)
  : rPin(rPin), gPin(gPin), bPin(bPin), fluoPin(fluoPin), mqtt(mqtt){
  pinMode(rPin, OUTPUT);
  pinMode(gPin, OUTPUT);
  pinMode(bPin, OUTPUT);
  pinMode(fluoPin, OUTPUT);
  rValue = 0;
  gValue = 0;
  bValue = 0;
  fluoValue = false;
  ledValue = false;
}

void Aquarium::onMessage(Message message){
  if(message.message_event == "set_settings"){
    setSettings(message);
    mqtt.sendMessage(basicResonse(message));
  }else if(message.message_event == "get_settings"){
    setSettings(message);
  }
}

void Aquarium::setSettings(Message message) {
  Serial.println(message.toJson());
    if (message.payload["color_r"].is<int>()) {
        rValue = message.payload["color_r"].as<int>();
    }
    if (message.payload["color_g"].is<int>()) {
        gValue = message.payload["color_g"].as<int>();
    }
    if (message.payload["color_b"].is<int>()) {
        bValue = message.payload["color_b"].as<int>();
    }
    if (message.payload["fluo_mode"].is<bool>()) {
        fluoValue = message.payload["fluo_mode"].as<bool>();
    }
    if (message.payload["led_mode"].is<bool>()) {
        ledValue = message.payload["led_mode"].as<bool>();
    }
    updateLed();
    updateFluo();
}

void Aquarium::updateLed() {
  if (ledValue) {
    analogWrite(rPin, rValue);
    analogWrite(gPin, gValue);
    analogWrite(bPin, bValue);
  } else {
    digitalWrite(rPin, LOW);
    digitalWrite(gPin, LOW);
    digitalWrite(bPin, LOW);
  }
}

void Aquarium::updateFluo() {
  if (fluoValue) {
    digitalWrite(fluoPin, HIGH);
  } else {
    digitalWrite(fluoPin, LOW);
  }
}