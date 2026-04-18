#include "TimeManager.h"

TimeManager::TimeManager() {
    currentTime = {0, 0, 0, 1, 1, 2000};
}

void TimeManager::update(int h, int m, int s, int d, int mo, uint16_t y) {
    currentTime.hour = h;
    currentTime.minute = m;
    currentTime.second = s;
    currentTime.day = d;
    currentTime.month = mo;
    currentTime.year = y;

    isActive = true;
}