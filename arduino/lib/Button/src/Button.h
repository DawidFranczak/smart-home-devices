#ifndef BUTTON_H
#define BUTTON_H

#include "Mqtt.h"

enum ButtonType {
  MONO = 0,
  BI = 1
};

class Button {
public:
  Button(ConfigManager& configManager, Mqtt& mqtt);
  void loop();
  void begin();
  void onMessage(Message msg);
  ButtonType getButtonType();

private:
  ConfigManager& configManager;
  Mqtt& mqtt;
  u_int8_t buttonPin;
  void setSettings(Message msg);
  void check_button_mono();
  void check_button_bi();
  ButtonType buttonType;
  ButtonType parseButtonType(const String& typeStr);
};

#endif