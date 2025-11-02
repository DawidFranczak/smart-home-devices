#pragma once
#include <Arduino.h>
#include <Adafruit_AHTX0.h>
#include <Mqtt.h>
#include <MeasurementMessage.h>

enum SensorType {
  SENSOR_AHT,
};

struct DeviceSettings {
  unsigned long waiting_time;
  float temperature_hysteresis;
  float humidity_hysteresis;
  float trigger_temp_up;
  float trigger_temp_down;
  float trigger_hum_up;
  float trigger_hum_down;
};

class TempHum {
public:
  TempHum(Mqtt& mqttClient, SensorType sensorType, const unsigned long checkEventInterval);

  void begin();
  void onMessage(Message msg);  
  void loop();

private:
  Mqtt& mqtt;
  SensorType sensorType;
  const unsigned long checkEventInterval;
  unsigned long lastReadTime;
  unsigned long lastCheckTime;
  bool tempAboveSent;
  bool tempBelowSent;
  bool humAboveSent;
  bool humBelowSent;
  Adafruit_AHTX0 aht;
  DeviceSettings settings;

  void sendAggregateData(float temp,float hum);
  void updateSettings(Message msg);
  void checkTemperatureEvents(float temp);
  void checkHumidityEvents(float hum);
};
