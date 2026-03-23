#pragma once
#include <ArduinoJson.h>

class BasePeripheral;
class BasePeripheralFactory;
class SystemContext;

constexpr uint8_t MAX_PERIPHERALS = 30;

class PeripheralManager {
public:
    BasePeripheralFactory& factory;
    BasePeripheral* peripherals[MAX_PERIPHERALS];
    uint8_t count = 0;
    bool blockLoop = false;

    PeripheralManager(BasePeripheralFactory& f);
    bool registerDevice(BasePeripheral* p);
    BasePeripheral* get(int id);
    void begin(SystemContext& systemContext);
    void loop();
    void buildPeripherals(SystemContext& ctx);
    void startSync();
};