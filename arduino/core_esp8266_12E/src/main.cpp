#include <Arduino.h>
#include <Mqtt.h>
#include <Types.h>
#include <Cpu.h>
#include <EventEngine.h>
#include <TimeManager.h>
#include <ConfigManager.h>
#include <PeripheralManager.h>
#include <BasePeripheral.h>
#include <SystemContext.h>
#include <ESP8266PeripheralFactory.h>

ConfigManager configManager("/config.json");
ConfigManager stateManager("/state.json");
ESP8266PeripheralFactory factory;
PeripheralManager peripheralManager(factory);
Mqtt mqtt(configManager);
EventEngine eventEngine(peripheralManager, mqtt, configManager);
TimeManager timeManager;
SystemContext systemContext(configManager, stateManager, eventEngine, timeManager);
Cpu cpu(peripheralManager, systemContext);

void setup() {
  Serial.begin(9600);
  configManager.begin();
  stateManager.begin();
  mqtt.begin();
  mqtt.onMessage([](Message msg) {
    Serial.println(msg.toJson());
    cpu.onMessage(msg);
  });

  cpu.begin();
  eventEngine.begin();
  peripheralManager.begin(systemContext);

  Serial.printf("Free RAM: %d\n", ESP.getFreeHeap());
}

void loop() {
  mqtt.loop();
  cpu.loop();
  peripheralManager.loop();
  delay(10);
}