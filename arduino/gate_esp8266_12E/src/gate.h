
#ifndef GATE_H
#define GATE_H
class Gate{
    public:
        Gate(int gate_pin, int buzzer_pin);
        void start();
        void access_granted(int open_gate_time);
        void access_denied();
    
    private:
        int gate_pin;
        int buzzer_pin;
        unsigned long open_gate_time;
        bool access_granted_flag;
        bool access_denied_flag;
        int access_denied_counter;
        unsigned long last_access_granted_time;
        unsigned long last_access_denied_time;
        void _access_granted();
        void _access_denied();
};

#endif