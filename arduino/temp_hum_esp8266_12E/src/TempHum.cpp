#include "TempHum.h"

TempHum::TempHum(Mqtt& mqttClient, SensorType sensorType, const unsigned long checkEventInterval)
  : mqtt(mqttClient),
    sensorType(sensorType),
    checkEventInterval(checkEventInterval),
    lastReadTime(0),
    lastCheckTime(0),
    tempAboveSent(false),
    tempBelowSent(false),
    humAboveSent(false),
    humBelowSent(false)
{

  settings.waiting_time = 10000;
  settings.temperature_hysteresis = 1.0;
  settings.humidity_hysteresis = 5.0;
  settings.trigger_temp_up = 25.0;
  settings.trigger_temp_down = 15.0;
  settings.trigger_hum_up = 70.0;
  settings.trigger_hum_down = 30.0;
}

void TempHum::begin() {
  aht.begin();
  mqtt.onMessage([this](Message msg) {
    this->onMessage(msg);
  });
}

void TempHum::onMessage(Message msg) {

  if (msg.message_event == "get_settings" || msg.message_event == "set_settings") {
    updateSettings(msg);
  }

  if (msg.message_event == "on_measurement_temp_hum") {
    if (msg.payload["waiting_time"].is<unsigned long>()) {
      settings.waiting_time = msg.payload["waiting_time"].as<unsigned long>()*1000;
    }
  }
}

void TempHum::loop() {

  if (millis() - lastReadTime > settings.waiting_time) {
    lastReadTime = millis();
     if (sensorType == SENSOR_AHT){
      sensors_event_t humidity, temp;
      aht.getEvent(&humidity, &temp);
      sendAggregateData(temp.temperature,humidity.relative_humidity);
    }
  }
  if (millis() - lastCheckTime > checkEventInterval){
    if(sensorType == SENSOR_AHT){
       sensors_event_t humidity, temp;
       aht.getEvent(&humidity, &temp);
       checkTemperatureEvents(temp.temperature);
       checkHumidityEvents(humidity.relative_humidity);
      }
    lastCheckTime = millis();
  }
}

void TempHum::sendAggregateData(float temp, float hum) {
  
    Message measurement = tempHumMeasurentRequest(
      mqtt.getMac(),
      temp,
      hum
    );
    mqtt.sendMessage(measurement);
}

void TempHum::checkTemperatureEvents(float temp) {
  String mac = mqtt.getMac();
  if (!tempAboveSent && temp >= settings.trigger_temp_up) {
    mqtt.sendMessage(onTemperatureAbove(mac));
    tempAboveSent = true;
  }
  if (tempAboveSent && temp < (settings.trigger_temp_up - settings.temperature_hysteresis)) {
    tempAboveSent = false;
  }
  if (!tempBelowSent && temp <= settings.trigger_temp_down) {
    mqtt.sendMessage(onTemperatureBelow(mac));
    tempBelowSent = true;
  }
  if (tempBelowSent && temp > (settings.trigger_temp_down + settings.temperature_hysteresis)) {
    tempBelowSent = false;
  }
}

void TempHum::checkHumidityEvents(float hum){
  String mac = mqtt.getMac();
  if (!humAboveSent && hum >= settings.trigger_hum_up) {
    mqtt.sendMessage(onHumidityAbove(mac));
    humAboveSent = true;
  }
  if (humAboveSent && hum < (settings.trigger_hum_up - settings.humidity_hysteresis)) {
    humAboveSent = false;
  }

  if (!humBelowSent && hum <= settings.trigger_hum_down) {
    mqtt.sendMessage(onHumidityBelow(mac));
    humBelowSent = true;
  }
  if (humBelowSent && hum > (settings.trigger_hum_down + settings.humidity_hysteresis)) {
    humBelowSent = false;
  }
}

void TempHum::updateSettings(Message msg) {
  auto payload = msg.payload;

  if (payload["waiting_time"].is<int>()) settings.waiting_time = payload["waiting_time"].as<int>();
  if (payload["temperature_hysteresis"].is<float>()) settings.temperature_hysteresis = payload["temperature_hysteresis"].as<float>();
  if (payload["humidity_hysteresis"].is<float>()) settings.humidity_hysteresis = payload["humidity_hysteresis"].as<float>();
  if (payload["trigger_temp_up"].is<float>()) settings.trigger_temp_up = payload["trigger_temp_up"].as<float>();
  if (payload["trigger_temp_down"].is<float>()) settings.trigger_temp_down = payload["trigger_temp_down"].as<float>();
  if (payload["trigger_hum_up"].is<float>()) settings.trigger_hum_up = payload["trigger_hum_up"].as<float>();
  if (payload["trigger_hum_down"].is<float>()) settings.trigger_hum_down = payload["trigger_hum_down"].as<float>();
}
