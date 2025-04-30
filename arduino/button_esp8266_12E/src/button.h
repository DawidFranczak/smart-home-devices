#ifndef BUTTON_H
#define BUTTON_H

#include "CommunicationProtocol/communication_module.h"
#include "CommunicationProtocol/message.h"

class Button {
public:
  Button(int input_pin, CommunicationModule& communication_module);
  void start();

private:
  int button_pin;
  CommunicationModule& communication_module;
  void check_button();
  void check_message();
  DeviceMessage set_settings_response(DeviceMessage& message);
};

#endif