#include <Arduino.h>
#include "CommunicationProtocol/communication_module.h"
#include "CommunicationProtocol/message.h"
#include "device.h"


Device::Device(CommunicationModule& comunication_module, Sensor& sensor, Gate& gate, int opto_pin):
    communication_module(comunication_module),
    sensor(sensor),
    gate(gate),
    opto_pin(opto_pin){
    pinMode(opto_pin, INPUT_PULLUP);
    open_gate_timeout = 10000;
    add_tag_timeout = 10000;
    add_tag_flag = false;
    add_tag_start = 0;
    add_tag_message = nullptr;
}
void Device::start(){
    check_message();
    if (add_tag_flag){
        add_tag();
    }else{
        check_sensor();
    }
    check_opto();
}
void Device::check_message(){
    DeviceMessage* message = communication_module.get_message();
    if (message == nullptr) return;
    String method_name = communication_module.extract_method_name_from_message(*message);
    if (method_name == "_add_tag_request") {
        add_tag_request(*message);
    } else if (method_name == "_access_granted_request") {
        access_granted_request(*message);
    } else if (method_name == "_access_denied_request") {
        access_denied_request(*message);
    } else if (method_name == "_set_settings_response") {
        set_settings_response(*message);
    }
    delete message;
}
void Device::check_sensor(){
    String uid = sensor.read_uid();
    if (uid == "") return; 
    DeviceMessage message = on_read_request(communication_module.get_mac(), uid);
    communication_module.send_message(message); 
}

void Device::add_tag(){
    if (millis() - add_tag_start > add_tag_timeout) {
        add_tag_flag = false;
        communication_module.send_message(add_tag_response(*add_tag_message, ""));
        delete add_tag_message;
        add_tag_message = nullptr;
        return;
    }
    String uid = sensor.read_uid();
    if (uid == "") return; 
    communication_module.send_message(add_tag_response(*add_tag_message, uid)); 
    delete add_tag_message;
    add_tag_message = nullptr;
    add_tag_flag = false;
}

void Device::check_opto(){
    static unsigned long last_check = 0;
    static int press_time = 0;
    static bool sended = false;
  
    if (press_time > 1000 && !sended) {
      sended = true;
      DeviceMessage message = on_hold_request(communication_module.get_mac());
      communication_module.send_message(message);
      press_time = 0;
      gate.access_granted(open_gate_timeout);
    } else if (press_time > 30 && press_time < 1000 and digitalRead(opto_pin) == HIGH){
      DeviceMessage message = on_click_request(communication_module.get_mac());
      communication_module.send_message(message);
      press_time = 0;
      gate.access_granted(open_gate_timeout);
    }
  
    if (digitalRead(opto_pin) == HIGH){
      last_check = millis();
      sended = false;
      press_time = 0;
    }else {
      press_time = millis() - last_check;
    }
}
void Device::add_tag_request(DeviceMessage message){
    add_tag_flag = true;
    add_tag_message = new DeviceMessage(message);
    add_tag_start = millis();
}

void Device::access_granted_request(DeviceMessage message){
    gate.access_granted(open_gate_timeout);
    communication_module.send_message(accept_response(message));
}
void Device::access_denied_request(DeviceMessage message){
    gate.access_denied();
    communication_module.send_message(accept_response(message));
}
void Device::set_settings_response(DeviceMessage message){
    communication_module.send_message(accept_response(message));
}