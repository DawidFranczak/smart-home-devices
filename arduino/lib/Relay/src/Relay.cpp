#include <Arduino.h>
#include "Relay.h"
#include <Mqtt.h>

Relay::Relay(int outputPin):outputPin(outputPin){
    pinMode(outputPin,OUTPUT);
};
void Relay::on(){
    digitalWrite(outputPin,HIGH);
};
void Relay::off(){
    digitalWrite(outputPin,LOW);
};
void Relay::toggle(){
     digitalWrite(outputPin,!digitalRead(outputPin));
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