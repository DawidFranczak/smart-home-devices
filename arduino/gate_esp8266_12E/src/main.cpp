#include <Arduino.h>
#include "sensor.h"
#include "gate.h"
#include "device.h"
#include "settings.h"

Mqtt mqtt(BROKER_IP, BROKER_PORT, BROKER_NAME, SSID, PASSWORD, DEVICE_FUNCTION, HEALT_CHECK_INTERVAL);
Sensor sensor(SS_PIN, RST_PIN);
Gate gate(GATE_PIN,BUZZER_PIN);
Device device(mqtt, sensor, gate, OPTO_PIN);

void setup() {
  // Serial.begin(9600);
  mqtt.begin();
  mqtt.onMessage([](Message message) {
    device.onMessage(message);
  });
}

void loop() {
  device.loop();
  gate.loop();
  sensor.loop();
  delay(10);
}
