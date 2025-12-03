#include <Arduino.h>
#include <Mqtt.h>
#include <MeasurementMessage.h>
#include "TempHum.h"

ConfigManager configManager("/config.json");
Mqtt mqtt(configManager);
TempHum tempHum(mqtt, configManager);

void setup() {
  configManager.begin();
  tempHum.begin();
  mqtt.begin();
  mqtt.onMessage([](Message msg) {
    tempHum.onMessage(msg);
  });

}

void loop() {
  mqtt.loop();
  tempHum.loop();
}

