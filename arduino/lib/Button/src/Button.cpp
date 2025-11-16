#include <Arduino.h>
#include "Mqtt.h"
#include "ButtonMessage.h"
#include "Button.h"

Button::Button(int inputPin, Mqtt& mqtt)
  : buttonPin(inputPin), mqtt(mqtt), buttonType() {
  pinMode(buttonPin, INPUT_PULLUP);
}

void Button::loop() {
  if (buttonType == 0){
    check_button_mono();
  }else if(buttonType == 1){
    check_button_bi();
  }
}

void Button::check_button_mono() {
  static unsigned long lastCheck = 0;
  static int pressTime = 0;
  static bool sended = false;

  if (pressTime > 1000 && !sended) {
    mqtt.sendMessage(onHoldRequest(mqtt.getMac()));
    sended = true;
    pressTime = 0;
  } else if (pressTime > 30 && pressTime < 1000 && digitalRead(buttonPin) == HIGH){
    mqtt.sendMessage(onClickRequest(mqtt.getMac()));
    pressTime = 0;
  }

  if (digitalRead(buttonPin) == HIGH){
    lastCheck = millis();
    sended = false;
    pressTime = 0;
  }else {
    pressTime = millis() - lastCheck;
  }
}

void Button::check_button_bi(){
  static bool lastState = digitalRead(buttonPin);
  static unsigned long lastCheck = 0;

  if (digitalRead(buttonPin) == lastState){
    lastCheck = millis();
  } else if (millis() - lastCheck >50){
    mqtt.sendMessage(onToggleRequest(mqtt.getMac()));
    lastState = digitalRead(buttonPin);
  } 
}

ButtonType Button::parseButtonType(const String& typeStr) {
  if (typeStr == "BI") {
    return BI;
  } else {
    return MONO;
  }
}
void Button::onMessage(Message msg){
  if(msg.message_event == "get_settings" || msg.message_event == "set_settings"){
    setSettings(msg);
  }
}
void Button::setSettings(Message msg){
  if (msg.payload["button_type"].is<const char*>()) {
     buttonType = parseButtonType(msg.payload["button_type"]);
  }
}
ButtonType Button::getButtonType() {
  return buttonType;
}

