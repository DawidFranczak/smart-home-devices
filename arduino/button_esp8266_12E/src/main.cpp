#include <Arduino.h>
#include <Mqtt.h>
#include "button.h"
#include "settings.h"

Mqtt mqtt(BROKER_IP, BROKER_PORT, BROKER_NAME, SSID, PASSWORD, DEVICE_FUNCTION, HEALT_CHECK_INTERVAL);
Button button(BUTTON_PIN, mqtt);

void setup() {
  // Serial.begin(9600);
  mqtt.begin();
  mqtt.onMessage([](Message msg) {
  });
}

void loop() {
  button.loop();
  delay(10);
}