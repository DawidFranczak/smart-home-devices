#include <Arduino.h>
#include <ConfigManager.h>
#include "sensor.h"
#include "gate.h"
#include "device.h"

ConfigManager configManager("/config.json");
Mqtt mqtt(configManager);
Sensor sensor(configManager);
Gate gate(configManager);
Device device(mqtt, sensor, gate, configManager);

void setup() {
  // Serial.begin(9600);
  configManager.begin();
  sensor.begin();
  gate.begin();
  device.begin();
  mqtt.begin();
  mqtt.onMessage([](Message message) {
    device.onMessage(message);
  });
}

void loop() {
  mqtt.loop();
  device.loop();
  gate.loop();
  sensor.loop();
  delay(10);
}
