#include "EventEngine.h"
#include "Mqtt.h"
#include "BasePeripheralFactory.h"
class Cpu {
private:
    PeripheralManager& peripheralManager;
    SystemContext& systemContext;
    bool syncInProgress = false;
    bool restarRequired = false;
    unsigned long restartOnTick = 0;
    
    void handleMessage(Message& message);
    void syncStart(Message& message);
    void updatePeripheral(Message& message);
    void syncEnd(Message& message);
    void restart(Message& message);
    void updateRule(Message& message);
public:
    Cpu(
        PeripheralManager& peripheralManager,
        SystemContext& systemContext
    );

    void begin();
    void loop();
    void onMessage(Message& message);
};