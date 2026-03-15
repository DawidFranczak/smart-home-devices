#pragma once
#include <vector>
#include "PeripheralManager.h"
#include "Types.h"
#include "Mqtt.h"

class EventEngine {
private:
    PeripheralManager& peripheralManager;
    Mqtt& mqtt;
    ConfigManager& configManager;
    std::vector<Rule> rules;

public:
    EventEngine(PeripheralManager& peripheralManager, Mqtt& mqtt, ConfigManager& configManager)
        : peripheralManager(peripheralManager), mqtt(mqtt), configManager(configManager){}

    void begin(){
        rules.clear();
        JsonObject rulesObj = configManager.config["rules"].as<JsonObject>();
        for (JsonPair kv : rulesObj) {
        
            JsonObject r = kv.value().as<JsonObject>();
            Rule rule;
            rule.triggerId = r["triggerId"];
            rule.triggerEvent = r["triggerEvent"].as<String>();
            rule.targetId = r["targetId"];
            rule.targetAction = r["targetAction"].as<String>();
            rule.extraSettings = r["extraSettings"].as<String>();
            rules.push_back(rule);
        }
    }

    void emit(const Event& ev) {

        
        for (const auto& rule : rules) {
            if (rule.triggerId == ev.emitDeviceId && rule.triggerEvent == ev.type) {
                BasePeripheral* target = peripheralManager.get(rule.targetId);
                if (target) {
                    target->triggerAction(rule.targetAction, rule.extraSettings);
                }
            }
        }
        
        if (ev.target != MessageTarget::INTERNAL) {
            if (!ev.msg.payload.isEmpty()) mqtt.sendMessage(ev.msg);
        }
    }
};
