#ifndef LAMP_H
#define LAMP_H

#include <Mqtt.h>
#include "Adafruit_PWMServoDriver.h"

enum LampState {
    IDLE,
    TURNING_ON,
    TURNING_OFF,
    BLINKING,
};

class Lamp{
    public:
        Lamp(Mqtt& mqtt, int lampCount);
        void loop();
        void onMessage(Message message);

    private:
        int lampCount;
        int currentLampIndex;
        bool isPending;
        bool lampOn;
        int brightness;
        int step;
        unsigned long lightingTime;
        bool reverse;
        unsigned long lastUpdate;
        int currentLamp;
        int currentBrightness;
        LampState state;
        unsigned long blinkStartTime;

        Mqtt& mqtt;
        Adafruit_PWMServoDriver pwm;
        void updateLamp();
        void turnOnLampRequest();
        void turnOffLampRequest();
        void blinkLampRequest();
        void toggleLampRequest();
        void setSettings(Message message);
        bool _turnOn();
        bool _turnOff();
};
#endif 