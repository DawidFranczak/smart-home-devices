#ifndef AQUARIUM_H
#define AQUARIUM_H

#include "CommunicationProtocol/communication_module.h"


class Aquarium {
public:
  Aquarium(int r_pin, int g_pin, int b_pin, int fluo_pin, CommunicationModule& communication_module);
  void start();

private:
    int r_pin;
    int g_pin;
    int b_pin;
    int fluo_pin;
    int r_value;
    int g_value;    
    int b_value;
    bool fluo_value;
    bool led_value;
    CommunicationModule& communication_module;

    void update_led();
    void update_fluo();
    void check_message();

    DeviceMessage set_settings_response(DeviceMessage& message);
    DeviceMessage set_settings_request(DeviceMessage& message);
};
#endif