#include "Mqtt.h"

class Relay{
    public:
        Relay(ConfigManager& configManager, Mqtt& mqtt);
        void on();
        void off();
        void toggle();
        void onMessage(Message msg);
    private:
        int outputPin;
        ConfigManager& configManager;
        Mqtt& mqtt;
};