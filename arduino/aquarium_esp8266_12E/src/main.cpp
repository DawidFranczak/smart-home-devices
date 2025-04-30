#include <Arduino.h>
#include "CommunicationProtocol/communication_module.h"
#include "aquarium.h"

CommunicationModule communication_module;
//14, 12, 13, 0
Aquarium aquarium(D5, D6, D7, D3, communication_module);
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  if (communication_module.is_connected()) {
    digitalWrite(LED_BUILTIN, HIGH); 
  } else {
    digitalWrite(LED_BUILTIN, LOW); 
  }
  communication_module.start();
  aquarium.start();
  delay(10);
}

