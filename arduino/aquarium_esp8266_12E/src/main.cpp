#include <Arduino.h>
#include <Mqtt.h>
#include <ConfigManager.h>
#include "aquarium.h"

ConfigManager configManager("/config.json");
Mqtt mqtt(configManager);
Aquarium aquarium(configManager, mqtt);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  // Serial.begin(9600);
  configManager.begin();
  aquarium.begin();
  mqtt.begin();
  mqtt.onMessage([](Message msg) {
    aquarium.onMessage(msg);
  });
}

void loop() {
  mqtt.loop();
  if (mqtt.isConnected()) {
    digitalWrite(LED_BUILTIN, HIGH); 
  } else {
    digitalWrite(LED_BUILTIN, LOW); 
  }
  delay(10);
}

