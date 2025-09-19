
#ifndef GATE_H
#define GATE_H
class Gate{
    public:
        Gate(int gatePin, int buzzerPin);
        void loop();
        void accessGranted(int openGateTime);
        void accessDenied();
    
    private:
        int gatePin;
        int buzzerPin;
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