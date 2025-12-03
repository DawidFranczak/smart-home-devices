#include <Arduino.h>
#include <Mqtt.h>
#include "lamp.h"

ConfigManager configManager("/config.json");
Mqtt mqtt(configManager);
Lamp lamp(mqtt, configManager);

void setup() {
  // Serial.begin(9600);
  configManager.begin();
  mqtt.begin();
  mqtt.onMessage([](Message message) {
    // Serial.println(message.toJson());
    lamp.onMessage(message);
  });
}

void loop() {
  mqtt.loop();
  if (mqtt.isConnected()) {
    digitalWrite(LED_BUILTIN, HIGH); 
  } else {
    digitalWrite(LED_BUILTIN, LOW); 
  }
  lamp.loop();
  delay(10);
}
