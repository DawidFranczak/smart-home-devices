#ifndef LAMP_H
#define LAMP_H

#include "CommunicationProtocol/communication_module.h"
#include "Adafruit_PWMServoDriver.h"

enum LampState {
    IDLE,
    TURNING_ON,
    TURNING_OFF,
    BLINKING,
};

class Lamp{
    public:
        Lamp(CommunicationModule& communication_module);
        void start();

    private:
        int lamp_count;
        int current_lamp_index;
        bool is_pending;
        bool lamp_on;
        int brightness;
        unsigned long step;
        unsigned long lighting_time;
        bool reverse;
        unsigned long last_update;
        int current_lamp;
        int current_brightness;
        LampState state;
        unsigned long blink_start_time;

        CommunicationModule& communication_module;
        Adafruit_PWMServoDriver pwm;
        void check_message();
        void update_lamp();
        void turn_on_lamp_request();
        void turn_off_lamp_request();
        void blink_lamp_request();
        void toggle_lamp_request();
        bool _turn_on();
        bool _turn_off();
        DeviceMessage set_settings_response(DeviceMessage& message);
        DeviceMessage set_settings_request(DeviceMessage& message);
        DeviceMessage accept_request(DeviceMessage& message);
};
#endif 