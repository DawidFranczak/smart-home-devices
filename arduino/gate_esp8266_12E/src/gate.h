#include <ConfigManager.h>
#ifndef GATE_H
#define GATE_H
class Gate{
    public:
        Gate(ConfigManager& configManager);
        void loop();
        void accessGranted(int openGateTime);
        void accessDenied();
    
    private:
        int gatePin;
        int buzzerPin;
        ConfigManager& configManager;
        unsigned long openGateTime;
        bool accessGrantedFlag;
        bool accessDeniedFlag;
        int accessDeniedCounter;
        unsigned long lastAccessGrantedTime;
        unsigned long lastAccessDeniedTime;
        void _accessGranted();
        void _accessDenied();
};

#endif