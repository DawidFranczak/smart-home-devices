#pragma once
#include <Arduino.h>

struct TimeData {
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint8_t day;
    uint8_t month;
    uint16_t year;
};

class TimeManager {
private:
    TimeData currentTime;

public:
    TimeManager();
    
    void update(uint8_t h, uint8_t m, uint8_t s, uint8_t d, uint8_t mo, uint16_t y);
    
    TimeData now() const { return currentTime; }
    uint8_t getHour() const { return currentTime.hour; }
    uint8_t getMinute() const { return currentTime.minute; }
    uint8_t getSecond() const { return currentTime.second; }
    uint8_t getTimeInMin() const { return currentTime.hour * 60 + currentTime.minute; }
    uint8_t getTimeInSec() const { return currentTime.hour * 3600 + currentTime.minute * 60 + currentTime.second; }
    
};