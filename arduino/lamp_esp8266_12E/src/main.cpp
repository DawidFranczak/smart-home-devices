#include <Arduino.h>
#include <Mqtt.h>
#include "settings.h"
#include "lamp.h"

Mqtt mqtt(BROKER_IP, BROKER_PORT, BROKER_NAME, SSID, PASSWORD, DEVICE_FUNCTION, HEALT_CHECK_INTERVAL);
Lamp lamp(mqtt, LAMP_COUNT);

void setup() {
  Serial.begin(9600);
  mqtt.begin();
  mqtt.onMessage([](Message message) {
    lamp.onMessage(message);
  });
}

void loop() {
  if (mqtt.isConnected()) {
    digitalWrite(LED_BUILTIN, HIGH); 
  } else {
    digitalWrite(LED_BUILTIN, LOW); 
  }
  lamp.loop();
  delay(10);
}