#ifndef AQUARIUM_H
#define AQUARIUM_H

#include <Mqtt.h>


class Aquarium {
public:
  Aquarium(int rPin, int gPin, int bPin, int fluoPin, Mqtt& mqtt);
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

    void setSettings(Message message);
    void updateLed();
    void updateFluo();

};
#endif