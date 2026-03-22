#include "EventEngine.h"
#include "Mqtt.h"
#include "BasePeripheralFactory.h"
class Cpu {
private:
    EventEngine& engine;
    PeripheralManager& peripheralManager;
    ConfigManager& configManager;
    ConfigManager& stateManager;
    BasePeripheralFactory* factory;

    bool syncInProgress = false;
    bool restarRequired = false;
    unsigned long restartOnTick = 0;
    
    void handleMessage(Message& message);
    void syncStart(Message& message);
    void updatePeripheral(Message& message);
    void syncEnd(Message& message);
    void restart(Message& message);
    void updateRule(Message& message);
    void healthCheck(Message& message);
    void buildPeripherals();
public:
    Cpu(
        EventEngine& engine,
        PeripheralManager& peripheralManager,
        ConfigManager& configManager,
        ConfigManager& stateManager,
        BasePeripheralFactory* factory
    );

    void begin();
    void loop();
    void onMessage(Message& message);
};