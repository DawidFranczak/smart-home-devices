#include <Arduino.h>
#include "gate.h"

Gate::Gate(int gate_pin, int buzzer_pin) :
 gate_pin(gate_pin), buzzer_pin(buzzer_pin) {
    pinMode(gate_pin, OUTPUT);
    pinMode(buzzer_pin, OUTPUT);
    access_granted_flag = false;
    access_denied_flag = false;
    open_gate_time = 10000;
    last_access_granted_time = 0;
    last_access_denied_time = 0;
    access_denied_counter = 0;
}
void Gate::start(){
    if (access_granted_flag) {
        _access_granted();
    } else if (access_denied_flag) {
        _access_denied();
    }
}
void Gate::access_granted(int open_gate_time){    
    access_denied_flag = false;
    access_granted_flag = true;
    this->open_gate_time = open_gate_time;
    last_access_granted_time = millis();
}

void Gate::access_denied(){
    access_denied_flag = true;
    access_granted_flag = false;
    last_access_denied_time = millis();
}

void Gate::_access_granted(){
    digitalWrite(gate_pin, HIGH);
    digitalWrite(buzzer_pin, HIGH); 
    if (millis() - last_access_granted_time < open_gate_time) return;
    digitalWrite(gate_pin, LOW);
    digitalWrite(buzzer_pin, LOW);
    access_granted_flag = false;
    last_access_granted_time = millis();

}
void Gate::_access_denied(){
    digitalWrite(gate_pin, LOW);
    if (millis() - last_access_denied_time > 600) {
        access_denied_counter++;
        last_access_denied_time = millis();
        if (access_denied_counter < 4) return; 
        access_denied_flag = false;
        access_denied_counter = 0;
        digitalWrite(buzzer_pin, LOW);
    }else if (millis() - last_access_denied_time > 400) {
        digitalWrite(buzzer_pin, LOW);
        return;
    }else if (millis() - last_access_denied_time > 200) {
        digitalWrite(buzzer_pin, HIGH);
        return;
    }
}