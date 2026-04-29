#pragma once
#include <vector>
#include "PeripheralManager.h"
#include "BasePeripheral.h"
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
            int count = r["condCount"];
            if (count > 0) {
                JsonObject condsObj = r["cond"].as<JsonObject>();

                for (int i = 0; i < count; i++) {
                    String idx = String(i);
                    if (!condsObj[idx].isNull()) {
                        JsonObject c = condsObj[idx].as<JsonObject>();
                        
                        Condition cond;
                        cond.type = c["type"].as<String>();
                        cond.op = c["op"].as<String>();
                        cond.value = c["val"].as<float>();
                        cond.hysteresis = c["hyst"] | 0.0f;
                        
                        rule.conditions.push_back(cond);
                    }
                }
            }

            rules.push_back(rule);
        }
        printAllRules();
    }

    void emit(const Event& ev) {

        if (ev.target != MessageTarget::BACKEND){
            for (auto& rule : rules) { 
                if (rule.triggerId == ev.emitDeviceId && rule.triggerEvent == ev.type) {
                    
                    bool allConditionsNowMet = true;

                    for (auto& cond : rule.conditions) {
                        cond.triggered = checkCondition(ev.value, cond);
                        
                        if (!cond.triggered) {
                            allConditionsNowMet = false;
                        }
                    }

                    if (allConditionsNowMet) {
                        BasePeripheral* target = peripheralManager.get(rule.targetId);
                        if (target) {
                            target->triggerAction(rule.targetAction, rule.extraSettings);
                            Serial.printf("Rule %s triggered! Action: %s\n", rule.triggerEvent.c_str(), rule.targetAction.c_str());
                        }
                    } 
                }
            }
        }
        if (ev.target != MessageTarget::INTERNAL) {
            if (!ev.msg.payload.isEmpty()) mqtt.sendMessage(ev.msg);
        }
    }

    bool checkCondition(float incomingValue, Condition& cond) {
        if (cond.type == "boolean") {
            return (incomingValue > 0.5f) == (cond.value > 0.5f);
        }

        float threshold = cond.value;
        float hyst = cond.hysteresis;

        if (cond.op == ">" || cond.op == ">=") {
            if (hyst == 0.0) return (incomingValue > threshold);
            return cond.triggered ? (incomingValue > (threshold - hyst)) : (incomingValue > threshold);
        }

        if (cond.op == "<" || cond.op == "<=") {
            if (hyst == 0.0) return (incomingValue < threshold);
            return cond.triggered ? (incomingValue < (threshold + hyst)) : (incomingValue < threshold);
        }

        if (cond.op == "==") return abs(incomingValue - threshold) < 0.01f;
        
        return true;
    }

    void printAllRules() {
        Serial.println(F("\n--- STATUS WARUNKÓW REGUŁ ---"));
        for (const auto& rule : rules) {
            Serial.printf("Reguła [%s]: \n", 
                rule.triggerEvent.c_str());

            for (size_t i = 0; i < rule.conditions.size(); i++) {
                const auto& c = rule.conditions[i];
                Serial.printf("  [%d] %s %s %.2f | Stan: %d\n", 
                    i, c.type.c_str(), c.op.c_str(), c.value, c.triggered);
            }
        }
        Serial.println(F("-----------------------------\n"));
    }
};
