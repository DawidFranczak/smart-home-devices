#pragma once
#include <map>
#include "BasePeripheral.h"

constexpr uint8_t MAX_PERIPHERALS = 30;
class PeripheralManager {

private:
    BasePeripheral* peripherals[MAX_PERIPHERALS];
    uint8_t count = 0;
    bool blockLoop = false;
    
public:
    bool registerDevice(BasePeripheral* peripheral) {
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
    
    BasePeripheral* get(int id) {
        for (uint8_t i = 0; i < count; i++) {
            if (peripherals[i]->getId() == id)
                return peripherals[i];
        }
        return nullptr;
    }

    void begin() {
        for (uint8_t i = 0; i < count; i++)
            peripherals[i]->begin();
    }

    void loop() {
        if (blockLoop) return;
        for (uint8_t i = 0; i < count; i++){
            peripherals[i]->loop();
        }
    }

    void startSync(){
        blockLoop = true;
    }
};
