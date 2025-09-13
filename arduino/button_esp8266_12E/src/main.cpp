#include <Arduino.h>
#include <Mqtt.h>
#include "button.h"
#include "settings.h"

Mqtt mqtt(BROKER_IP, BROKER_PORT, BROKER_NAME, SSID, PASSWORD, DEVICE_FUNCTION, HEALT_CHECK_INTERVAL);
Button button(2, mqtt);

void setup() {
  // Serial.begin(9600);

  mqtt.setCallback([](Message msg) {
    Serial.println(msg.toJson());
  });
}

void loop() {
  mqtt.loop();
  button.loop();
  delay(10);
}