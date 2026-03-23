#include "Types.h"
#include "Cpu.h"
#include "PeripheralManager.h"
#include "BasicMessage.h"
#include "SystemContext.h"
#include <time.h>

Cpu::Cpu(
    PeripheralManager& peripheralManager,
    SystemContext& systemContext
): 
    peripheralManager(peripheralManager),
    systemContext(systemContext)
    {};
    

void Cpu::loop(){
    if(restarRequired && restartOnTick < millis()) ESP.restart();
}

void Cpu::onMessage(Message& message){
    if (syncInProgress && message.scope != Scope::CPU) return;
    
    if (message.scope == Scope::CPU) handleMessage(message);
    else if (message.scope == Scope::PERIPHERAL){
        BasePeripheral* bp = peripheralManager.get(message.peripheral_id);
        if(bp) bp->onMessage(message);
    }
}
void Cpu::begin(){}

void Cpu::handleMessage(Message& message){
    if(message.command == "sync_start") {
        syncStart(message);
    }else if(message.command == "update_peripheral") {
        updatePeripheral(message);
    }else if(message.command == "update_rule") {
        updateRule(message);
    }else if(message.command == "sync_end") {
        syncEnd(message);
    }else if(message.command == "restart"){
        restart(message);
    }
} 

void Cpu::syncStart(Message& message){
    auto syncType = message.payload["sync_type"].as<int>();
    if(syncType == static_cast<int>(StartSyncType::PERIPHERAL)){
        systemContext.configManager.removeSection("peripherals");
        systemContext.stateManager.removeSection("states"); 
        
    }else if (syncType == static_cast<int>(StartSyncType::RULE)){
        systemContext.configManager.removeSection("rules");
    }
    peripheralManager.startSync();
    syncInProgress=true;


    Message resultMsg = basicCPUResult(message, true);

    Event ev;
    ev.target = MessageTarget::BACKEND;
    ev.msg.payload =resultMsg.toJson();
    ev.msg.qos = 1;
    ev.msg.retain =true;
    ev.emitDeviceId = 0;
    ev.type = message.command;
    systemContext.eventEngine.emit(ev);
}

void Cpu::restart(Message& message){
    restarRequired = true;
    restartOnTick = millis() + 10000; 
    Message resultMsg = basicCPUResult(message, true);
    Event ev;
    ev.target = MessageTarget::BACKEND;
    ev.msg.payload =resultMsg.toJson();
    ev.msg.qos =1;
    ev.msg.retain =true;
    ev.emitDeviceId = 0;
    ev.type = message.command;

    systemContext.eventEngine.emit(ev);
}

void Cpu::syncEnd(Message& message){
    systemContext.configManager.save();
    systemContext.stateManager.save();

    Message resultMsg = basicCPUResult(message, true);
    Event ev;
    ev.target = MessageTarget::BACKEND;
    ev.msg.payload =resultMsg.toJson();
    ev.msg.qos =1;
    ev.msg.retain =true;
    ev.emitDeviceId = 0;
    ev.type = message.command;

    systemContext.eventEngine.emit(ev);
}

void Cpu::updatePeripheral(Message& message){
    int id = message.payload["id"];

    String pathName = "peripherals." + String(id) + ".name";
    systemContext.configManager.set(pathName.c_str(), message.payload["name"]);

    String pathConfig = "peripherals." + String(id) + ".config";
    systemContext.configManager.set(pathConfig.c_str(), message.payload["config"]);
    
    String statePath = "states." + String(id);
    systemContext.stateManager.set(statePath.c_str(), message.payload["state"]);


    Message resultMsg = basicCPUResult(message, true);

    Event ev;
    ev.target = MessageTarget::BACKEND;
    ev.msg.payload =resultMsg.toJson();
    ev.msg.qos =1;
    ev.msg.retain =true;
    ev.emitDeviceId = 0;
    ev.type = message.command;

    systemContext.eventEngine.emit(ev);
}

void Cpu::updateRule(Message& message){
    JsonObject payload = message.payload.as<JsonObject>();

    int id = payload["id"];
    JsonObject trigger = payload["triggers"][0];
    JsonObject action = payload["actions"][0];
    JsonObject condtion = payload["conditions"][0];
    
    String basePath = "rules." + String(id);

    systemContext.configManager.set((basePath + ".triggerId").c_str(), trigger["peripheral"]);
    systemContext.configManager.set((basePath + ".triggerEvent").c_str(), trigger["event"]);
    systemContext.configManager.set((basePath + ".targetId").c_str(), action["peripheral"]);
    systemContext.configManager.set((basePath + ".targetAction").c_str(), action["action"]);
    systemContext.configManager.set((basePath + ".conditionsOperator").c_str(), condtion["operator"]);
    systemContext.configManager.set((basePath + ".conditionsValue").c_str(), (float)condtion["value"]);
    
    String settingsStr;
    serializeJson(action["extra_settings"], settingsStr);
    systemContext.configManager.set((basePath + ".settings").c_str(), settingsStr.c_str());


    Message resultMsg = basicCPUResult(message, true);
    Event ev;
    ev.target = MessageTarget::BACKEND;
    ev.msg.payload = resultMsg.toJson();
    ev.msg.qos =1;
    ev.msg.retain =true;
    ev.emitDeviceId = 0;
    ev.type = message.command;

    systemContext.eventEngine.emit(ev);
}