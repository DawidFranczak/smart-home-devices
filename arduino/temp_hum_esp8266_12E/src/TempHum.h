#pragma once
#include <Arduino.h>
#include <Adafruit_AHTX0.h>
#include <Mqtt.h>
#include <MeasurementMessage.h>

enum SensorType {
  SENSOR_AHT=0,
};

class TempHum {
public:
  TempHum(Mqtt& mqttClient, ConfigManager& configManager);

  void begin();
  void onMessage(Message msg);  
  void loop();

private:
  Mqtt& mqtt;
  ConfigManager& configManager;
  SensorType sensorType;
  unsigned long checkEventInterval;
  unsigned long lastReadTime;
  unsigned long lastCheckTime;
  bool tempAboveSent;
  bool tempBelowSent;
  bool humAboveSent;
  bool humBelowSent;
  Adafruit_AHTX0 aht;
  unsigned long waitingTime;
  float temperatureHysteresis;
  float humidityHysteresis;
  float triggerTempUp;
  float triggerTempDown;
  float triggerHumUp;
  float triggerHumDown;

  void sendAggregateData(float temp,float hum);
  void updateSettings(Message msg);
  void checkTemperatureEvents(float temp);
  void checkHumidityEvents(float hum);
};
