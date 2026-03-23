#include <Arduino.h>
#include "PeripheralManager.h"
#include "BasePeripheral.h"       
#include "BasePeripheralFactory.h"
#include "SystemContext.h"


PeripheralManager::PeripheralManager(BasePeripheralFactory& f) : factory(f), count(0) {}
bool PeripheralManager::registerDevice(BasePeripheral* peripheral) {
    if (count >= MAX_PERIPHERALS) return false;
    peripherals[count++] = peripheral;

    Serial.println("=== Lista zarejestrowanych peryferiów ===");
    for (uint8_t i = 0; i < count; i++) {
        BasePeripheral* p = peripherals[i];
        Serial.print("Index: "); Serial.print(i);
        Serial.print(" | ID: "); Serial.print(p->getId());
    }
    Serial.println("=== Koniec listy ===");

    return true;
}

BasePeripheral* PeripheralManager::get(int id) {
    for (uint8_t i = 0; i < count; i++) {
        if (peripherals[i]->getId() == id)
            return peripherals[i];
    }
    return nullptr;
}

void PeripheralManager::begin(SystemContext& systemContext) {
    buildPeripherals(systemContext);
    for (uint8_t i = 0; i < count; i++)
        peripherals[i]->begin();
}

void PeripheralManager::loop() {
    if (blockLoop) return;
    for (uint8_t i = 0; i < count; i++){
        peripherals[i]->loop();
    }
}

void PeripheralManager::startSync(){
    blockLoop = true;
}

void PeripheralManager::buildPeripherals(SystemContext& systemContext) {
    JsonObject peripherals = systemContext.configManager.config["peripherals"].as<JsonObject>();
    for(JsonPair kv : peripherals) {
        int id = atoi(kv.key().c_str());
        JsonObject cfg = kv.value().as<JsonObject>();
        BasePeripheral* p = factory.create(id, cfg, systemContext);
        if(p) registerDevice(p);
    }
}

