#pragma once
#include "BasePeripheralFactory.h"
#include "Rtc.h"
#include "PinInput.h"
#include "PinOutput.h"
#include "ButtonBistable.h"
#include "ButtonMonostable.h"
#include "Relay.h"

class ESP826601PeripheralFactory : public BasePeripheralFactory {
public:
    BasePeripheral* create(int id, JsonObject cfg, SystemContext& ctx) override {
        String type = cfg["name"].as<String>();
        if(type == "pin_output") {
            return new PinOutput(id, ctx, cfg);
        } else if(type == "pin_input") {
            return new PinInput(id, ctx, cfg);
        } else if(type == "rtc") {
            return new Rtc(id, ctx, cfg);
        } else if(type == "button_bistable") {
            return new ButtonBistable(id, ctx, cfg);
        } else if(type == "button_monostable") {
            return new ButtonMonostable(id, ctx, cfg);
        } else if(type == "relay") {
            return new Relay(id, ctx, cfg);
        } 
        return nullptr;
    }
};