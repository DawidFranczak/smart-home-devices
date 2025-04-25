#include <Arduino.h>
#include "button.h"

Button::Button(int input_pin, CommunicationModule& communication_module)
  : button_pin(input_pin), communication_module(communication_module) {
  pinMode(button_pin, INPUT_PULLUP);
}

void Button::start() {
  check_message();
  check_button();
}

void Button::check_button() {
  static unsigned long last_check = 0;
  static int press_time = 0;
  static bool sended = false;

  if (press_time > 1000 && !sended) {
    sended = true;
    DeviceMessage message = on_hold_request(communication_module.get_mac());
    communication_module.send_message(message);
    press_time = 0;
  } else if (press_time > 30 && press_time < 1000 and digitalRead(button_pin) == HIGH){
    DeviceMessage message = on_click_request(communication_module.get_mac());
    communication_module.send_message(message);
    press_time = 0;
  }

  if (digitalRead(button_pin) == HIGH){
    last_check = millis();
    sended = false;
    press_time = 0;
  }else {
    press_time = millis() - last_check;
  }
}

void Button::check_message() {
  DeviceMessage* message = communication_module.get_message();
  if (!message) return;
  String method_name = communication_module.extract_method_name_from_message(*message);
  if (method_name == "_set_settings_response") {
    communication_module.send_message(set_settings_response(*message));
  }else{
  }
  delete message;
}

DeviceMessage Button::set_settings_response(DeviceMessage& message) {
  JsonDocument payload;
  payload["status"] = "accept";
  return DeviceMessage(message.message_id, message.message_event, "response", message.device_id, payload);
}