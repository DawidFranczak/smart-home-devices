#include <Arduino.h>
#include <MFRC522.h>

#ifndef SENSOR_H
#define SENSOR_H
class Sensor{
    public:
        Sensor(int SS_PIN, int RST_PIN);
        void start();
        String read_uid();

    private:
        MFRC522 rfid;
};

#endif