#include <Arduino.h>
#include "CommunicationProtocol/communication_module.h"
#include "sensor.h"
#include "gate.h"
#include "device.h"

CommunicationModule communication_module;
Sensor sensor(16, 15);
Gate gate(4,5);
Device device(communication_module, sensor, gate, 0);

void setup() {
  Serial.begin(9600);
}

void loop() {
  if (communication_module.is_connected()) {
    digitalWrite(LED_BUILTIN, HIGH); 
  } else {
    digitalWrite(LED_BUILTIN, LOW); 
  }
  communication_module.start();
  device.start();
  gate.start();
  sensor.start();
  delay(10);
}
