#pragma once
#include "BasePeripheralFactory.h"
#include "PinOutput.h"
#include "PinInput.h"
#include "RgbStrip.h"

class ESP8266PeripheralFactory : public BasePeripheralFactory {
public:
    BasePeripheral* create(int id, JsonObject cfg, EventEngine& engine, ConfigManager& stateManager) override {
        String type = cfg["name"].as<String>();
        if(type == "pin_output") {
            return new PinOutput(id, engine, stateManager, cfg);
        } else if(type == "pin_input") {
            return new PinInput(id, engine, stateManager, cfg);
        } else if(type == "rgb_strip") {
            return new RgbStrip(id, engine, stateManager, cfg);
        } 
        return nullptr;
    }
};