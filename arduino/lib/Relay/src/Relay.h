#include "Mqtt.h"

class Relay{
    public:
        Relay(int outputPin, Mqtt& mqtt);
        void on();
        void off();
        void toggle();
        void onMessage(Message msg);
    private:
        int outputPin;
        Mqtt& mqtt;
};