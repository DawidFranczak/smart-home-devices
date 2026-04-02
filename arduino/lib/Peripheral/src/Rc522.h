#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>
#include "BasePeripheral.h"
#include "ButtonMessage.h"
#include "ConfigManager.h"
#include "EventEngine.h"
#include <MFRC522.h>

class Rc522 : public BasePeripheral {
private:
    MFRC522 rfid;
    int ssPin;
    int rstPin;
    unsigned long lastRead = 0;
    bool addTagFlag = false;
    String addTagMessageId = "";
    unsigned long addTagStart = 0;
public:
    Rc522(int id, SystemContext& ctx, JsonObject cfg)
        : BasePeripheral(id, ctx)
    {
        ssPin = cfg["config"]["ss"];
        rstPin = cfg["config"]["rst"];
        MFRC522 rfid(ssPin,rstPin); 
        
    }

    void begin() override {
        SPI.begin(); 
        rfid.PCD_Init(); 
    }

    void loop() override {
        static unsigned long lastHardwareCheck = 0;
        if (millis() - lastHardwareCheck > 5000) {
            if (!rfid.PCD_PerformSelfTest()) {
                rfid.PCD_Init();
            }
            lastHardwareCheck = millis();
        }

        if (addTagFlag) {
            if (millis() - addTagStart > 30000) {
                JsonDocument payload;
                payload["uid"] = "";
                payload["status"] = static_cast<uint8_t>(ActionResult::REJECTED);
                notify("add_tag", "add_tag", payload, addTagMessageId);
                addTagFlag = false;
                addTagMessageId="";
                return;
            }
        }

        if(millis() - lastRead < 1000) return;
        String uid = readUid();
        if (uid == "") return;

        lastRead = millis();
        JsonDocument payload;
        payload["uid"] = uid;
        Serial.println(uid);

        if (addTagFlag){
            payload["status"] = static_cast<uint8_t>(ActionResult::ACCEPTED);
            notify("add_tag", "add_tag", payload, addTagMessageId);
            addTagMessageId="";
            addTagFlag=false;
        }else{
            notify("on_read", "on_read", payload, "", MessageType::EVENT);
        }
    }

    String readUid() {
        if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
            return "";
        }

        String uidStr = "";
        for (byte i = 0; i < rfid.uid.size; i++) {
            uidStr += (rfid.uid.uidByte[i] < 0x10 ? "0" : "");
            uidStr += String(rfid.uid.uidByte[i], HEX);
        }
        uidStr.toUpperCase();

        rfid.PICC_HaltA();
        rfid.PCD_StopCrypto1();
        return uidStr;
    }

    void onMessage(Message& msg) override {
         if(msg.command == "add_tag") {
            addTagFlag = true;
            addTagMessageId = msg.message_id;
            addTagStart = millis();
        }if(msg.command == "on_read") {
            int statusInt = msg.payload["status"] | -1; 
            ActionResult result = static_cast<ActionResult>(statusInt);
            if (result == ActionResult::ACCEPTED){
                notify("on_read_success");
            }else if (result == ActionResult::REJECTED){
                notify("on_read_failure");
            }
 
        }
    }

    void triggerAction(String targetAction, String extraSettings) override {}


};