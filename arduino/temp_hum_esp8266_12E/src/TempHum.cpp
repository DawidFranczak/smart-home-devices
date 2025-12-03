#include "TempHum.h"

TempHum::TempHum(Mqtt& mqttClient, ConfigManager& configManager)
  : mqtt(mqttClient),
    configManager(configManager),
    lastReadTime(0),
    lastCheckTime(0),
    tempAboveSent(false),
    tempBelowSent(false),
    humAboveSent(false),
    humBelowSent(false)
{
  sensorType = configManager.get("device.checkEventInterval").as<SensorType>(); 
  checkEventInterval = configManager.get("device.checkEventInterval").as<unsigned long>(); 
  waitingTime = configManager.get("device.waitingTime").as<unsigned long>();
  temperatureHysteresis = configManager.get("device.temperatureHysteresis").as<float>();
  humidityHysteresis = configManager.get("device.humidityHysteresis").as<float>();
  triggerTempUp = configManager.get("device.triggerTempUp").as<float>();
  triggerTempDown = configManager.get("device.triggerTempDown").as<float>();
  triggerHumUp = configManager.get("device.triggerHumUp").as<float>();
  triggerHumDown = configManager.get("device.triggerHumDown").as<float>();

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
      waitingTime = msg.payload["waiting_time"].as<unsigned long>()*1000;
    }
  }
}

void TempHum::loop() {

  if (millis() - lastReadTime > waitingTime) {
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
  if (!tempAboveSent && temp >= triggerTempUp) {
    mqtt.sendMessage(onTemperatureAbove(mac));
    tempAboveSent = true;
  }
  if (tempAboveSent && temp < (triggerTempUp - temperatureHysteresis)) {
    tempAboveSent = false;
  }
  if (!tempBelowSent && temp <= triggerTempDown) {
    mqtt.sendMessage(onTemperatureBelow(mac));
    tempBelowSent = true;
  }
  if (tempBelowSent && temp > (triggerTempDown + temperatureHysteresis)) {
    tempBelowSent = false;
  }
}

void TempHum::checkHumidityEvents(float hum){
  String mac = mqtt.getMac();
  if (!humAboveSent && hum >= triggerHumUp) {
    mqtt.sendMessage(onHumidityAbove(mac));
    humAboveSent = true;
  }
  if (humAboveSent && hum < (triggerHumUp - humidityHysteresis)) {
    humAboveSent = false;
  }

  if (!humBelowSent && hum <= triggerHumDown) {
    mqtt.sendMessage(onHumidityBelow(mac));
    humBelowSent = true;
  }
  if (humBelowSent && hum > (triggerHumDown + humidityHysteresis)) {
    humBelowSent = false;
  }
}

void TempHum::updateSettings(Message msg) {
  auto payload = msg.payload;

  if (payload["waiting_time"].is<int>()) waitingTime = payload["waiting_time"].as<int>();
  if (payload["temperature_hysteresis"].is<float>()){
     temperatureHysteresis = payload["temperature_hysteresis"].as<float>();
     configManager.set("device.temperatureHysteresis",temperatureHysteresis);
  }
  if (payload["humidity_hysteresis"].is<float>()) {
      humidityHysteresis = payload["humidity_hysteresis"].as<float>();
      configManager.set("device.humidityHysteresis", humidityHysteresis);
  }

  if (payload["trigger_temp_up"].is<float>()) {
      triggerTempUp = payload["trigger_temp_up"].as<float>();
      configManager.set("device.triggerTempUp", triggerTempUp);
  }

  if (payload["trigger_temp_down"].is<float>()) {
      triggerTempDown = payload["trigger_temp_down"].as<float>();
      configManager.set("device.triggerTempDown", triggerTempDown);
  }

  if (payload["trigger_hum_up"].is<float>()) {
      triggerHumUp = payload["trigger_hum_up"].as<float>();
      configManager.set("device.triggerHumUp", triggerHumUp);
  }

  if (payload["trigger_hum_down"].is<float>()) {
      triggerHumDown = payload["trigger_hum_down"].as<float>();
      configManager.set("device.triggerHumDown", triggerHumDown);
  }
}
