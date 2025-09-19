#ifndef BUTTON_H
#define BUTTON_H

#include <Mqtt.h>

class Button {
public:
  Button(int input_pin, Mqtt& mqtt);
  void loop();

private:
  int button_pin;
  Mqtt& mqtt;
  void check_button();
};

#endif