#include "MearurementMessage.h"

Message tempHumMeasurentRequest(String mac, float temperature, float humidity) {
  JsonDocument payload;
  payload["temperature"] = temperature;
  payload["humidity"] = humidity;
  return Message(String(millis()), "on_measurement_temp_hum", "request", mac, payload,1,true);
}