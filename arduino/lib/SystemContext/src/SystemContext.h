#pragma once
#include <Arduino.h>
class ConfigManager;
class EventEngine;
class TimeManager;

class SystemContext {
public:
    ConfigManager& configManager;
    ConfigManager& stateManager;
    EventEngine& eventEngine;
    TimeManager& timeManager;

    SystemContext(ConfigManager& cm, ConfigManager& sm, EventEngine& ee, TimeManager& tm) 
        : configManager(cm), stateManager(sm), eventEngine(ee), timeManager(tm) {}
};