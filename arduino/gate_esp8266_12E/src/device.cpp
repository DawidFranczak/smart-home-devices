#include <Arduino.h>
#include <Mqtt.h>
#include "device.h"
#include "BasicMessage.h"
#include "RfidMessage.h"
#include "ButtonMessage.h"


Device::Device(Mqtt& mqtt, Sensor& sensor, Gate& gate, ConfigManager& configManager):
    mqtt(mqtt),
    sensor(sensor),
    gate(gate),
    configManager(configManager){
    optoPin = configManager.get("device.optoPin").as<int>();
    openGateTimeout = configManager.get("device.openGateTimeout").as<int>();
    addTagTimeout = configManager.get("device.addTagTimeout").as<int>();
    pinMode(optoPin, INPUT_PULLUP);
    addTagStart = 0;
}
void Device::loop(){
    checkSensor();
    checkOpto();
}
void Device::onMessage(Message message){
    if (message.message_event == "add_tag") {
        addTag(message);
    } else if (message.message_event == "access_granted") {
        accessGrantedRequest(message);
    } else if (message.message_event == "access_denied") {
        accessDeniedRequest(message);
    } else if (message.message_event == "set_settings") {
        setSettingsResponse(message);
    } else if (message.message_event == "get_settings") {
        setSettingsResponse(message);
    }
}
void Device::checkSensor(){
    static unsigned long last_time_readed_tag = 0;
    String uid = sensor.readUid();
    if (addTagFlag){
        if (millis() - addTagStart > addTagTimeout) {
            if (addTagMessage.has_value()) {
                mqtt.sendMessage(addTagResponse(*addTagMessage, "0"));
            }
            addTagFlag=false;
            return;
        }
        if (uid == "") return; 
        if (addTagMessage.has_value()) {
            mqtt.sendMessage(addTagResponse(*addTagMessage, uid));
            addTagFlag=false;
        }
    }else{
        if (uid == "") return; 
        if (millis() - last_time_readed_tag < 1000) return;
        last_time_readed_tag = millis();
        mqtt.sendMessage(onReadRequest(mqtt.getMac(), uid));
    }
}

void Device::addTag(Message message){
    addTagStart = millis();
    addTagMessage = message;
    addTagFlag = true;
}

void Device::checkOpto(){
    static unsigned long lastCheck = 0;
    static int pressTime = 0;
    static bool sended = false;
  
    if (pressTime > 1000 && !sended) {
      sended = true;
      mqtt.sendMessage(onHoldRequest(mqtt.getMac()));
      pressTime = 0;
      gate.accessGranted(openGateTimeout);
    } else if (pressTime > 30 && pressTime < 1000 && digitalRead(optoPin) == HIGH){
      mqtt.sendMessage(onClickRequest(mqtt.getMac()));
      pressTime = 0;
      gate.accessGranted(openGateTimeout);
    }
  
    if (digitalRead(optoPin) == HIGH){
      lastCheck = millis();
      sended = false;
      pressTime = 0;
    } else {
      pressTime = millis() - lastCheck;
    }
}

void Device::accessGrantedRequest(Message message){
    gate.accessGranted(openGateTimeout);
    mqtt.sendMessage(basicResponse(message));
}
void Device::accessDeniedRequest(Message message){
    gate.accessDenied();
    mqtt.sendMessage(basicResponse(message));
}
void Device::setSettingsResponse(Message message){
    mqtt.sendMessage(basicResponse(message));
}