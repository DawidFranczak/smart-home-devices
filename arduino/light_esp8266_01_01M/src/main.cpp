#include <Arduino.h>
#include <Mqtt.h>
#include <Button.h>
#include <Relay.h>
#include "settings.h"

Mqtt mqtt(BROKER_IP, BROKER_PORT, BROKER_NAME, SSID, PASSWORD, DEVICE_FUNCTION, HEALT_CHECK_INTERVAL);
Button button(BUTTON_PIN, mqtt);
Relay relay(OUTPUT_PIN);

ButtonType buttonType;
unsigned long lastCheck = 0;

void check_button_mono();
void check_button_bi();

void setup() {
  Serial.begin(9600);
  mqtt.begin();
  mqtt.onMessage([](Message msg) {
    Serial.println(msg.toJson());
    button.onMessage(msg);
    relay.onMessage(msg);
  });
}

void loop() {
  mqtt.loop();
  button.loop();

  buttonType = button.getButtonType();
  lastCheck = millis();
  if (buttonType == 0){
    check_button_mono();
  }else if(buttonType == 1){
    check_button_bi();
  }
  delay(10);
}

void check_button_mono() {
  static unsigned long lastCheck = 0;
  static int pressTime = 0;
  static bool switched = false;
  if (pressTime > 30 && digitalRead(BUTTON_PIN) == HIGH && !switched){
    relay.toggle();
    switched = true;
    pressTime = 0;
  }

  if (digitalRead(BUTTON_PIN) == HIGH){
    lastCheck = millis();
    switched = false;
    pressTime = 0;
  }else {
    pressTime = millis() - lastCheck;
  }
}

void check_button_bi(){
  static bool lastState = digitalRead(BUTTON_PIN);
  static unsigned long lastCheck = 0;

  if (digitalRead(BUTTON_PIN) == lastState){
    lastCheck = millis();
  }else if (millis() - lastCheck > 50){
    relay.toggle();
    lastState = digitalRead(BUTTON_PIN);
  } 
}