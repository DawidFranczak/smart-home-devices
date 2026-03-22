#include "Types.h"
#include "Cpu.h"
#include "BasePeripheral.h"
#include "PeripheralManager.h"
#include "BasicMessage.h"
#include <time.h>

Cpu::Cpu(
    EventEngine& engine,
    PeripheralManager& peripheralManager,
    ConfigManager& configManager,
    ConfigManager& stateManager,
    BasePeripheralFactory* factory
): 
    engine(engine),
    peripheralManager(peripheralManager),
    configManager(configManager),
    stateManager(stateManager),
    factory(factory)
    {};
    
void Cpu::begin(){
    buildPeripherals();
}
void Cpu::loop(){
    if(restarRequired && restartOnTick < millis()) ESP.restart();
    if(!rtcSetup) return;

    checkTime();
   
}

void Cpu::checkTime(){
    time_t now;
    time(&now);
    if (now != lastSec) {
        lastSec = now;
        struct tm timeinfo;
        localtime_r(&now, &timeinfo);
        if (timeinfo.tm_sec == 0) {
            Event tickEvent;
            tickEvent.type = "time";
            tickEvent.emitDeviceId = 0;
            tickEvent.target = MessageTarget::INTERNAL;
            engine.emit(tickEvent); 
        }
    }
}

void Cpu::buildPeripherals() {
    JsonObject peripherals = configManager.config["peripherals"].as<JsonObject>();
    for(JsonPair kv : peripherals) {
        int id = atoi(kv.key().c_str());
        JsonObject cfg = kv.value().as<JsonObject>();
        String statePath = "states." + String(id);
        BasePeripheral* p = factory->create(id, cfg, engine, stateManager);
        if(p) peripheralManager.registerDevice(p);
    }
}

void Cpu::onMessage(Message& message){
    if (syncInProgress && message.scope != Scope::CPU) return;
    
    if (message.scope == Scope::CPU) handleMessage(message);
    else if (message.scope == Scope::PERIPHERAL){
        BasePeripheral* bp = peripheralManager.get(message.peripheral_id);
        if(bp) bp->onMessage(message);
    }
}

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
    }else if(message.command == "health_check"){
        healthCheck(message);
    }
} 

void Cpu::syncStart(Message& message){
    auto syncType = message.payload["sync_type"].as<int>();
    if(syncType == static_cast<int>(StartSyncType::PERIPHERAL)){
        configManager.removeSection("peripherals");
        stateManager.removeSection("states"); 
        
    }else if (syncType == static_cast<int>(StartSyncType::RULE)){
        configManager.removeSection("rules");
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
    engine.emit(ev);
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

    engine.emit(ev);
}

void Cpu::healthCheck(Message& message){
    Serial.println("UPDATE1");
    if (message.payload["timestamp"].is<long>()) {
        Serial.println("UPDATE2");
        struct timeval tv;
        tv.tv_sec = message.payload["timestamp"].as<long>();; 
        tv.tv_usec = 0;
        settimeofday(&tv, NULL);    
        rtcSetup = true;
    }
}

void Cpu::syncEnd(Message& message){
    configManager.save();
    stateManager.save();

    Message resultMsg = basicCPUResult(message, true);
    Event ev;
    ev.target = MessageTarget::BACKEND;
    ev.msg.payload =resultMsg.toJson();
    ev.msg.qos =1;
    ev.msg.retain =true;
    ev.emitDeviceId = 0;
    ev.type = message.command;

    engine.emit(ev);
}

void Cpu::updatePeripheral(Message& message){
    int id = message.payload["id"];

    String pathName = "peripherals." + String(id) + ".name";
    configManager.set(pathName.c_str(), message.payload["name"]);

    String pathConfig = "peripherals." + String(id) + ".config";
    configManager.set(pathConfig.c_str(), message.payload["config"]);
    
    String statePath = "states." + String(id);
    stateManager.set(statePath.c_str(), message.payload["state"]);


    Message resultMsg = basicCPUResult(message, true);

    Event ev;
    ev.target = MessageTarget::BACKEND;
    ev.msg.payload =resultMsg.toJson();
    ev.msg.qos =1;
    ev.msg.retain =true;
    ev.emitDeviceId = 0;
    ev.type = message.command;

    engine.emit(ev);
}

void Cpu::updateRule(Message& message){
    JsonObject payload = message.payload.as<JsonObject>();

    int id = message.payload["id"];
    JsonObject trigger = message.payload["triggers"][0];
    JsonObject action = message.payload["actions"][0];
    
    String basePath = "rules." + String(id);

    configManager.set((basePath + ".triggerId").c_str(), trigger["peripheral"]);
    configManager.set((basePath + ".triggerEvent").c_str(), trigger["event"]);
    configManager.set((basePath + ".targetId").c_str(), action["peripheral"]);
    configManager.set((basePath + ".targetAction").c_str(), action["action"]);
    
    String settingsStr;
    serializeJson(action["extra_settings"], settingsStr);
    configManager.set((basePath + ".settings").c_str(), settingsStr.c_str());


    Message resultMsg = basicCPUResult(message, true);
    Event ev;
    ev.target = MessageTarget::BACKEND;
    ev.msg.payload = resultMsg.toJson();
    ev.msg.qos =1;
    ev.msg.retain =true;
    ev.emitDeviceId = 0;
    ev.type = message.command;

    engine.emit(ev);
}