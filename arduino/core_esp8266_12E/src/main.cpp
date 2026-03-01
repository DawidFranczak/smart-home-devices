#include <Arduino.h>
#include <Mqtt.h>
#include <Types.h>
#include <Cpu.h>
#include <EventEngine.h>
#include <ConfigManager.h>
#include <PeripheralManager.h>
#include <BasePeripheral.h>
#include <ESP8266PeripheralFactory.h>

ConfigManager configManager("/config.json");
ConfigManager stateManager("/state.json");
ESP8266PeripheralFactory factory;
PeripheralManager peripheralManager;
Mqtt mqtt(configManager);
EventEngine eventEngine(peripheralManager, mqtt);
Cpu cpu(eventEngine, peripheralManager, configManager, stateManager, &factory);

void setup() {
  Serial.begin(9600);
  configManager.begin();
  stateManager.begin();
  mqtt.begin();
  mqtt.onMessage([](Message msg) {
    cpu.onMessage(msg);
  });
  cpu.begin();
  peripheralManager.begin();
}

void loop() {
  mqtt.loop();
  cpu.loop();
  peripheralManager.loop();
  delay(10);
}