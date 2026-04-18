#pragma once
#include <Arduino.h>

struct TimeData {
    int hour;
    int minute;
    int second;
    int day;
    int month;
    uint16_t year;
};

class TimeManager {
private:
    TimeData currentTime;

public:
    bool isActive = false;
    TimeManager();
    
    void update(int h, int m, int s, int d, int mo, uint16_t y);
    
    TimeData now() const { return currentTime; }
    int getHour() const { return currentTime.hour; }
    int getMinute() const { return currentTime.minute; }
    int getSecond() const { return currentTime.second; }
    int getTimeInMin() const { return currentTime.hour * 60 + currentTime.minute; }
    int getTimeInSec() const { return currentTime.hour * 3600 + currentTime.minute * 60 + currentTime.second; }
    
};