#include <Arduino.h>
#include "sensor.h"

Sensor::Sensor(ConfigManager& configManager):configManager(configManager) {

    MFRC522 rfid(
      configManager.get("device.ssPin").as<int>(),
      configManager.get("device.rstPin").as<int>()
    ); 
    SPI.begin(); 
    rfid.PCD_Init(); 
}

void Sensor::loop(){
  Serial.println(rfid.PCD_PerformSelfTest());
  if (rfid.PCD_PerformSelfTest()) return;
  rfid.PCD_Init(); 
}

String Sensor::readUid(){
    String UID = "";
    if(!rfid.PICC_IsNewCardPresent()) return UID;
    if (rfid.PICC_ReadCardSerial()) {
      for (byte i = 0; i<rfid.uid.size; i++){
        UID = UID + rfid.uid.uidByte[i];
      }
      rfid.PICC_HaltA();
    }
    return UID;
  }