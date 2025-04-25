#include <Arduino.h>
#include "aquarium.h"

Aquarium::Aquarium(int r_pin, int g_pin, int b_pin, int fluo_pin, CommunicationModule& communication_module)
  : r_pin(r_pin), g_pin(g_pin), b_pin(b_pin), fluo_pin(fluo_pin), communication_module(communication_module){
  pinMode(r_pin, OUTPUT);
  pinMode(g_pin, OUTPUT);
  pinMode(b_pin, OUTPUT);
  pinMode(fluo_pin, OUTPUT);
  r_value = 0;
  g_value = 0;
  b_value = 0;
  fluo_value = false;
  led_value = false;
}

void Aquarium::start() {
  check_message();
  update_led();
  update_fluo();
}

void Aquarium::check_message() {
  DeviceMessage* message = communication_module.get_message();
  if (!message) return;
  String method_name = communication_module.extract_method_name_from_message(*message);
  if (method_name == "_set_settings_response") {
    communication_module.send_message(set_settings_response(*message));
  } else if (method_name == "_set_settings_request") {
    communication_module.send_message(set_settings_request(*message));
  }
  delete message;
}

DeviceMessage Aquarium::set_settings_request(DeviceMessage& message) {
  return set_settings_response(message);
}

DeviceMessage Aquarium::set_settings_response(DeviceMessage& message) {
    if (message.payload["color_r"].is<int>()) {
        r_value = message.payload["color_r"].as<int>();
    }
    if (message.payload["color_g"].is<int>()) {
        g_value = message.payload["color_g"].as<int>();
    }
    if (message.payload["color_b"].is<int>()) {
        b_value = message.payload["color_b"].as<int>();
    }
    if (message.payload["fluo_mode"].is<bool>()) {
        fluo_value = message.payload["fluo_mode"].as<bool>();
    }
    if (message.payload["led_mode"].is<bool>()) {
        led_value = message.payload["led_mode"].as<bool>();
    }
    update_led();
    update_fluo();
    JsonDocument payload;
    payload["status"] = "accept";
    return DeviceMessage(message.message_id, message.message_event, "response", message.device_id, payload);
}

void Aquarium::update_led() {
  if (led_value) {
    analogWrite(r_pin, r_value);
    analogWrite(g_pin, g_value);
    analogWrite(b_pin, b_value);
  } else {
    digitalWrite(r_pin, LOW);
    digitalWrite(g_pin, LOW);
    digitalWrite(b_pin, LOW);
  }
}

void Aquarium::update_fluo() {
  if (fluo_value) {
    digitalWrite(fluo_pin, HIGH);
  } else {
    digitalWrite(fluo_pin, LOW);
  }
}