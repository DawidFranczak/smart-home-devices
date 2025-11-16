#ifndef BUTTON_H
#define BUTTON_H

#include "Mqtt.h"

enum ButtonType {
  MONO = 0,
  BI = 1
};

class Button {
public:
  Button(int inputPin, Mqtt& mqtt);
  void loop();
  void onMessage(Message msg);
  ButtonType getButtonType();

private:
  int buttonPin;
  Mqtt& mqtt;
  void setSettings(Message msg);
  void check_button_mono();
  void check_button_bi();
  ButtonType buttonType;
  ButtonType parseButtonType(const String& typeStr);
};

#endif