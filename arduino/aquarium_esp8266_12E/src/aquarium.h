#ifndef AQUARIUM_H
#define AQUARIUM_H

#include <Mqtt.h>


class Aquarium {
public:
  Aquarium(ConfigManager& configManager, Mqtt& mqtt);
  void onMessage(Message message);

private:
    int rPin;
    int gPin;
    int bPin;
    int fluoPin;
    int rValue;
    int gValue;    
    int bValue;
    bool fluoValue;
    bool ledValue;
    Mqtt& mqtt;
    ConfigManager& configManager;

    void setSettings(Message message);
    void updateLed();
    void updateFluo();

};
#endif