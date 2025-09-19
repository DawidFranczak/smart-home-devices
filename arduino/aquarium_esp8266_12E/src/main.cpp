#include <Arduino.h>
#include <Mqtt.h>
#include "aquarium.h"
#include "settings.h"

Mqtt mqtt(BROKER_IP, BROKER_PORT, BROKER_NAME, SSID, PASSWORD, DEVICE_FUNCTION, HEALT_CHECK_INTERVAL);

Aquarium aquarium(R_PIN, G_PIN, B_PIN, FLUO_PIN, mqtt);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  mqtt.begin();
  mqtt.onMessage([](Message msg) {
    aquarium.onMessage(msg);
  });
}

void loop() {
  if (mqtt.isConnected()) {
    digitalWrite(LED_BUILTIN, HIGH); 
  } else {
    digitalWrite(LED_BUILTIN, LOW); 
  }
  delay(10);
}

