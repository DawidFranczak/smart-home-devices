#include <Arduino.h>
#include <Mqtt.h>
#include <Button.h>
#include <ConfigManager.h>

ConfigManager configManager("/config.json");
Mqtt mqtt(configManager);
Button button(configManager, mqtt);

void setup() {
  // Serial.begin(9600);
  configManager.begin();
  button.begin();
  mqtt.begin();
  mqtt.onMessage([](Message msg) {
    button.onMessage(msg);
  });

}

void loop() {
  mqtt.loop();
  button.loop();
  delay(10);
}