#include <Arduino.h>
#include <BasicMessage.h>
#include "aquarium.h"

Aquarium::Aquarium(ConfigManager& configManager, Mqtt& mqtt)
  : configManager(configManager), mqtt(mqtt){
  rPin = configManager.get("device.rPin").as<int>();
  gPin = configManager.get("device.gPin").as<int>();
  bPin = configManager.get("device.bPin").as<int>();
  fluoPin = configManager.get("device.fluoPin").as<int>();
  rValue = configManager.get("device.rValue").as<int>();
  gValue = configManager.get("device.gValue").as<int>();
  bValue = configManager.get("device.bValue").as<int>();
  ledValue = configManager.get("device.ledValue").as<bool>(); 
  fluoValue = configManager.get("device.fluoValue").as<bool>();

  pinMode(rPin, OUTPUT);
  pinMode(gPin, OUTPUT);
  pinMode(bPin, OUTPUT);
  pinMode(fluoPin, OUTPUT);
}

void Aquarium::onMessage(Message message){
  if(message.message_event == "set_settings"){
    setSettings(message);
    mqtt.sendMessage(basicResponse(message));
  }else if(message.message_event == "get_settings"){
    setSettings(message);
  }
}

void Aquarium::setSettings(Message message) {
    if (message.payload["color_r"].is<int>()) {
        rValue = message.payload["color_r"].as<int>();
        configManager.set("device.rValue",rValue);
    }
    if (message.payload["color_g"].is<int>()) {
        gValue = message.payload["color_g"].as<int>();
        configManager.set("device.gValue",rValue);
    }
    if (message.payload["color_b"].is<int>()) {
        bValue = message.payload["color_b"].as<int>();
        configManager.set("device.bValue",rValue);
    }
    if (message.payload["fluo_mode"].is<bool>()) {
        fluoValue = message.payload["fluo_mode"].as<bool>();
        configManager.set("device.fluoValue",fluoValue);
    }
    if (message.payload["led_mode"].is<bool>()) {
        ledValue = message.payload["led_mode"].as<bool>();
        configManager.set("device.ledValue",ledValue);
    }
    configManager.save();
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