#include <Arduino.h>
#include <Mqtt.h>
#include <BasicMessage.h>
#include "Relay.h"

Relay::Relay(int outputPin, Mqtt& mqtt):outputPin(outputPin), mqtt(mqtt){
    pinMode(outputPin,OUTPUT);
};
void Relay::on(){
    digitalWrite(outputPin,HIGH);
    mqtt.sendMessage(deviceStateRequest(mqtt.getMac(),"on"));
};
void Relay::off(){
    digitalWrite(outputPin,LOW);
    mqtt.sendMessage(deviceStateRequest(mqtt.getMac(),"off"));
};
void Relay::toggle(){
    if (digitalRead(outputPin)) off();
    else on();
};
void Relay::onMessage(Message msg){
    if (msg.message_event == "off") {
        off();
    } else if (msg.message_event == "on") {
        on();
    } else if (msg.message_event == "toggle") {
        toggle();
    }
};