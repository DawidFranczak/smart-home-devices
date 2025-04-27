#include <Arduino.h>
#include "sensor.h"

Sensor::Sensor(int SS_PIN, int RST_PIN) {
    MFRC522 rfid(SS_PIN, RST_PIN); 
    SPI.begin(); 
    rfid.PCD_Init(); 
}

void Sensor::start(){
  if (rfid.PCD_PerformSelfTest()) return;
  rfid.PCD_Init(); 
}
String Sensor::read_uid(){
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