#include <Arduino.h>
#include <Mqtt.h>
#include <SequentialLight.h>

#define SENSOR_UP 27
#define SENSOR_DOWN 26

ConfigManager configManager("/config.json");
Mqtt mqtt(configManager);
SequentialLight sequentialLight(mqtt, configManager);

void setup() {
  pinMode(SENSOR_UP,INPUT_PULLDOWN);
  pinMode(SENSOR_DOWN,INPUT_PULLDOWN);
  // Serial.begin(9600);
  configManager.begin();
  mqtt.begin();
  sequentialLight.begin();
  mqtt.onMessage([](Message message) {
    sequentialLight.onMessage(message);
  });
  configManager.displayFileContent();
}

void loop() {
  delay(10);

  static unsigned long lastActive = 0;
  mqtt.loop();
  sequentialLight.loop();
  if (mqtt.isConnected()) {
    digitalWrite(LED_BUILTIN, HIGH); 
  } else {
    digitalWrite(LED_BUILTIN, LOW); 
  }
  if (sequentialLight.isPending) lastActive = millis();
  if (lastActive + 1000 < millis()){
    if (digitalRead(SENSOR_DOWN) == HIGH){
      Serial.println("DOWN");
      sequentialLight.reverse = false;
      sequentialLight.blink();
    } else if (digitalRead(SENSOR_UP) == HIGH){
      Serial.println("UP");
        sequentialLight.reverse = true;
        sequentialLight.blink();

    }
  }
}
