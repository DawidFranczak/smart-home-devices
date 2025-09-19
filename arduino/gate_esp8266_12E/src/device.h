#include <Mqtt.h>
#include "sensor.h"
#include "gate.h"
class Device{
    public:
        Device(Mqtt& mqtt, Sensor& sensor, Gate& gate, int optoPin);
        void loop();
        void onMessage(Message message);
    private:
        Mqtt& mqtt;
        Sensor& sensor;
        Gate& gate;
        std::optional<Message> addTagMessage;
        int optoPin;
        int openGateTimeout;
        unsigned long addTagTimeout;
        unsigned long addTagStart;
        bool addTagFlag;
        void checkSensor();
        void checkOpto();
        void addTag(Message message);
        void accessGrantedRequest(Message message);
        void accessDeniedRequest(Message message);
        void setSettingsResponse(Message message);

};