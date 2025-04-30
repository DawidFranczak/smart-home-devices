#include <Arduino.h>
#include "CommunicationProtocol/communication_module.h"
#include "lamp.h"
CommunicationModule communication_module;
Lamp lamp(communication_module);
void setup() {
  Serial.begin(9600);
}

void loop() {
  communication_module.start();
  lamp.start();
  delay(10);
}