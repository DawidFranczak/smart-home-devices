#pragma once
#include "Rtc.h"
#include "PinInput.h"
#include "RgbStrip.h"
#include "PinOutput.h"
#include "SequentialLight.h"
#include "BasePeripheralFactory.h"

class ESP8266PeripheralFactory : public BasePeripheralFactory {
public:
    BasePeripheral* create(int id, JsonObject cfg, SystemContext& ctx) override {
        String type = cfg["name"].as<String>();
        if(type == "pin_output") {
            return new PinOutput(id, ctx, cfg);
        } else if(type == "pin_input") {
            return new PinInput(id, ctx, cfg);
        } else if(type == "rgb_strip") {
            return new RgbStrip(id, ctx, cfg);
        } else if(type == "rtc") {
            return new Rtc(id, ctx, cfg);
        } else if(type == "sequential_light") {
            return new SequentialLight(id, ctx, cfg);
        } 
        return nullptr;
    }
};