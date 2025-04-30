#include "Arduino.h"
#include "lamp.h"
#include "Adafruit_PWMServoDriver.h"
#include "ArduinoJson.h"
Lamp::Lamp(CommunicationModule& communication_module) : communication_module(communication_module) {
    is_pending = false;
    lamp_on = false;
    brightness = 4095;
    step = 20;
    lighting_time = 10000;
    reverse = false;
    state = IDLE;
    lamp_count = 4;
    current_lamp_index = 0;
    pwm = Adafruit_PWMServoDriver();
    pwm.begin();    
    pwm.setPWMFreq(50);
    for (int i = 0; i < 15; i++) {
        pwm.setPWM(i, 0, 0);
    }
};

void Lamp::start() {
    check_message();
    update_lamp();
}

void Lamp::check_message() {
    DeviceMessage* message = communication_module.get_message();
    if (message == nullptr) return;
    String method_name = communication_module.extract_method_name_from_message(*message);
    reverse = false;
    if (message->payload["reverse"].is<bool>()) {
        reverse = message->payload["reverse"].as<bool>();
    }
    if (method_name == "_set_settings_response") {
        communication_module.send_message(set_settings_response(*message));
    } else if (method_name == "_set_settings_request") {
        communication_module.send_message(set_settings_request(*message));
    } else if (method_name == "_off_request") {
        turn_off_lamp_request();
        // communication_module.send_message(accept_request(*message));
    } else if (method_name == "_on_request") {
        turn_on_lamp_request();
        // communication_module.send_message(accept_request(*message));
    } else if (method_name == "_blink_request") {
        blink_lamp_request();
        // communication_module.send_message(accept_request(*message));
    } else if (method_name == "_toggle_request") {
        toggle_lamp_request();
        // communication_module.send_message(accept_request(*message));

    }
}

void Lamp::update_lamp() {

    switch (state) {
        case IDLE:
            break;

        case TURNING_ON:
            if(!_turn_on()) break; 
            lamp_on = true;
            is_pending = false;
            state = IDLE;
            break;

        case TURNING_OFF:
            if(!_turn_off()) break; 
            lamp_on = false;
            is_pending = false;
            state = IDLE;
            break;

        case BLINKING:
            if (!lamp_on) {
                lamp_on = _turn_on();
                last_update = millis();
                break;
            }
            if (millis() - last_update < lighting_time) break;
            is_pending = false;
            turn_off_lamp_request();
            break;
    }
}

void Lamp::turn_on_lamp_request() {
    if (is_pending || lamp_on) return;
    is_pending = true;
    state = TURNING_ON;
    last_update = millis();
    current_brightness = 0;
    current_lamp_index = reverse ? lamp_count - 1 : 0;
}

void Lamp::turn_off_lamp_request() {
    if (is_pending || !lamp_on) return;
    is_pending = true;
    state = TURNING_OFF;
    last_update = millis();
    current_brightness = brightness;
    current_lamp_index = reverse ? lamp_count - 1 : 0;  
}

void Lamp::blink_lamp_request() {
    if (is_pending) return;
    is_pending = true;
    state = BLINKING;
    current_brightness = 0;
    current_lamp_index = reverse ? lamp_count - 1 : 0;  
}

void Lamp::toggle_lamp_request() {
    if (is_pending) return;
    is_pending = true;
    state = lamp_on ? TURNING_OFF : TURNING_ON;
    last_update = millis();
    current_brightness = 0;
    current_lamp_index = reverse ? lamp_count - 1 : 0;  
}

DeviceMessage Lamp::set_settings_request(DeviceMessage& message) {
    return set_settings_response(message);
}

DeviceMessage Lamp::set_settings_response(DeviceMessage& message) {
    if (message.payload["brightness"].is<int>()) {
        brightness = message.payload["brightness"].as<int>() * 40.95;
    }
    if (message.payload["step"].is<int>()) {
        step = message.payload["step"].as<int>();
    }
    if (message.payload["lighting_time"].is<int>()) {
        lighting_time = message.payload["lighting_time"].as<int>()*1000;
    }
   return accept_request(message);
}

DeviceMessage Lamp::accept_request(DeviceMessage& message){
    JsonDocument payload;
    payload["status"] = "accept";
    return DeviceMessage(message.message_id, message.message_event, "response", message.device_id, payload);
}

bool Lamp::_turn_on(){
    if (current_brightness < brightness) {
        current_brightness += step;
        if (current_brightness > brightness) current_brightness = brightness;
        pwm.setPWM(current_lamp_index, 0, current_brightness);
    } else {
        current_lamp_index = reverse ? current_lamp_index - 1 : current_lamp_index + 1;
        if (current_lamp_index >= 0 && current_lamp_index < lamp_count) {
            current_brightness = 0;
            pwm.setPWM(current_lamp_index, 0, current_brightness);
        }
        return (reverse && current_lamp_index < 0) || (!reverse && current_lamp_index >= lamp_count);
    }
    return false;
};

bool Lamp::_turn_off(){
    if (current_brightness > 0) {
        current_brightness -= step;
        if (current_brightness < 0) current_brightness = 0;
        pwm.setPWM(current_lamp_index, 0, current_brightness);
    } else {
        current_lamp_index = reverse ? current_lamp_index - 1 : current_lamp_index + 1;
        if (current_lamp_index >= 0 && current_lamp_index < lamp_count) {
            current_brightness = brightness;
            pwm.setPWM(current_lamp_index, 0, current_brightness);
        }
        return (reverse && current_lamp_index < 0) || (!reverse && current_lamp_index >= lamp_count);
    }
    return false;
};