#include <Arduino.h>
#include "CommunicationProtocol/communication_module.h"
#include "button.h"

CommunicationModule communication_module;
Button button(2, communication_module);

void setup() {
}

void loop() {
  communication_module.start();
  button.start();
  delay(10);
}