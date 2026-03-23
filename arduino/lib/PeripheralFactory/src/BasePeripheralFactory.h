#pragma once
#include <ArduinoJson.h>
#include "BasePeripheral.h"
#include "ConfigManager.h"
#include "EventEngine.h"

class BasePeripheralFactory {
public:
    virtual ~BasePeripheralFactory() {}
    virtual BasePeripheral* create(int id, JsonObject cfg, SystemContext& ctx) = 0;
};