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
            rule.conditionsOperator = r["conditionsOperator"].as<String>();
            rule.conditionsValue = r["conditionsValue"].as<float>();
            rules.push_back(rule);
        }
        printAllRules();
    }

    void emit(const Event& ev) {

        if (ev.target != MessageTarget::BACKEND){
            for (const auto& rule : rules) {
                if (rule.triggerId == ev.emitDeviceId && rule.triggerEvent == ev.type) {
                    if (checkCondition(ev.value, rule.conditionsOperator, rule.conditionsValue)) {
                        BasePeripheral* target = peripheralManager.get(rule.targetId);
                        if (target) {
                            target->triggerAction(rule.targetAction, rule.extraSettings);
                        }
                    }
                }
            }
        }
        if (ev.target != MessageTarget::INTERNAL) {
            if (!ev.msg.payload.isEmpty()) mqtt.sendMessage(ev.msg);
        }
    }

    bool checkCondition(float incomingValue, String op, float ruleValue) {
        Serial.print(incomingValue);
        Serial.print(op);
        Serial.println(ruleValue);
        if (op == "==") return incomingValue == ruleValue;
        if (op == "!=") return incomingValue != ruleValue;
        if (op == ">")  return incomingValue > ruleValue;
        if (op == "<")  return incomingValue < ruleValue;
        if (op == ">=") return incomingValue >= ruleValue;
        if (op == "<=") return incomingValue <= ruleValue;
        return true;
    }
    void printAllRules() {
        Serial.println(F("--- LISTA ZAREJESTROWANYCH REGUŁ ---"));
        int i = 0;
        for (const auto& rule : rules) {
            Serial.print(i++);
            Serial.print(F(". [Trigger: "));
            Serial.print(rule.triggerId);
            Serial.print(F(" ("));
            Serial.print(rule.triggerEvent);
            Serial.print(F(")] Condition: "));
            Serial.print(rule.conditionsOperator);
            Serial.print(F(" "));
            Serial.print(rule.conditionsValue);
            Serial.print(F(" -> Target: "));
            Serial.print(rule.targetId);
            Serial.print(F(" Action: "));
            Serial.println(rule.targetAction);
        }
        Serial.println(F("-----------------------------------"));
    }
};
