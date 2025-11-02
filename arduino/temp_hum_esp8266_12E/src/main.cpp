#include <Arduino.h>
#include <Mqtt.h>
#include <MeasurementMessage.h>
#include "settings.h"
#include "TempHum.h"


Mqtt mqtt(BROKER_IP, BROKER_PORT, BROKER_NAME, SSID, PASSWORD, DEVICE_FUNCTION, HEALT_CHECK_INTERVAL);
TempHum tempHum(mqtt, SENSOR_TYPE, CHECK_EVENT_INTERVAL*1000);

void setup() {
  tempHum.begin();
  mqtt.begin();
  mqtt.onMessage([](Message msg) {
    tempHum.onMessage(msg);
  });

}

void loop() {
  tempHum.loop();
}

