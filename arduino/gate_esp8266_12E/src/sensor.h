#include <Arduino.h>
#include <MFRC522.h>
#include <ConfigManager.h>

#ifndef SENSOR_H
#define SENSOR_H
class Sensor{
    public:
        Sensor(ConfigManager& configManager);
        void loop();
        void begin();
        String readUid();

    private:
        MFRC522 rfid;
        ConfigManager& configManager;
};

#endif