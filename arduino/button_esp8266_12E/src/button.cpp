#include <Arduino.h>
#include <Mqtt.h>
#include <ButtonMessage.h>
#include "button.h"

Button::Button(int input_pin, Mqtt& mqtt)
  : button_pin(input_pin), mqtt(mqtt) {
  pinMode(button_pin, INPUT_PULLUP);
}

void Button::loop() {
  check_button();
}

void Button::check_button() {
  static unsigned long last_check = 0;
  static int press_time = 0;
  static bool sended = false;

  if (press_time > 1000 && !sended) {
    mqtt.sendMessage(onHoldMessage(mqtt.getMac()));
    sended = true;
    press_time = 0;
  } else if (press_time > 30 && press_time < 1000 and digitalRead(button_pin) == HIGH){
    mqtt.sendMessage(onClickMessage(mqtt.getMac()));
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