#include "TimeManager.h"

TimeManager::TimeManager() {
    currentTime = {0, 0, 0, 1, 1, 2000};
}

void TimeManager::update(uint8_t h, uint8_t m, uint8_t s, uint8_t d, uint8_t mo, uint16_t y) {
    currentTime = {h, m, s, d, mo, y};
}