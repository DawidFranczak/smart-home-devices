#include "CommunicationProtocol/communication_module.h"
#include "sensor.h"
#include "gate.h"
class Device{
    public:
        Device(CommunicationModule& comunication_module, Sensor& sensor, Gate& gate, int opto_pin);
        void start();
    private:
        CommunicationModule& communication_module;
        Sensor& sensor;
        Gate& gate;
        int opto_pin;
        int open_gate_timeout;
        unsigned long add_tag_timeout;
        unsigned long add_tag_start;
        DeviceMessage* add_tag_message;
        bool add_tag_flag;
        void check_message();
        void check_sensor();
        void check_opto();
        void add_tag();
        void add_tag_request(DeviceMessage message);
        void access_granted_request(DeviceMessage message);
        void access_denied_request(DeviceMessage message);
        void set_settings_response(DeviceMessage message);

};