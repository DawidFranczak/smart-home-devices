#include <Arduino.h>
#include <Mqtt.h>
#include <Adafruit_AHTX0.h>
#include <MearurementMessage.h>
#include "settings.h"

Mqtt mqtt(BROKER_IP, BROKER_PORT, BROKER_NAME, SSID, PASSWORD, DEVICE_FUNCTION, HEALT_CHECK_INTERVAL);
Adafruit_AHTX0 aht;
int sleepTime;
bool goToSleep = false;
unsigned long start;
void setup() {
  start = millis();
  mqtt.begin();
  mqtt.onMessage([](Message msg) {
    Serial.println(msg.toJson());
    if(msg.message_type != "response") return;
    if(msg.message_event == "on_measurement_temp_hum"){
      if (msg.payload["sleeping_time"].is<int>()){
        sleepTime = msg.payload["sleeping_time"].as<int>();
        Serial.println("Going to deep sleep for " + String(sleepTime) + " seconds");
        goToSleep = true;
      }
    }
  });
  goToSleep = false;
  Serial.begin(9600);
  aht.begin();
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);
  Message measurement = tempHumMeasurentRequest(mqtt.getMac(),temp.temperature, humidity.relative_humidity);
  Serial.print("Temperature: "); Serial.print(temp.temperature); Serial.println(" degrees C");
  Serial.print("Humidity: "); Serial.print(humidity.relative_humidity); Serial.println("% rH");
  mqtt.sendMessage(measurement);
}

void loop() {
  if (goToSleep){
    Serial.println("czas ");
    Serial.print(millis()-start);
    ESP.deepSleep((uint64_t)sleepTime * 1000000);
  }
}