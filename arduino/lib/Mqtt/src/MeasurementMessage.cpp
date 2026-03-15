#include "MeasurementMessage.h"

// Message tempHumMeasurentRequest(String mac, float temperature, float humidity) {
//   JsonDocument payload;
//   payload["temperature"] = temperature;
//   payload["humidity"] = humidity;
//   return Message(String(millis()), "on_measurement_temp_hum", "request", mac, payload,1,true);
// }

// Message onTemperatureAbove(String mac) {
//   JsonDocument payload;
//   return Message(String(millis()), "on_temperature_above", "request", mac, payload);
// }

// Message onTemperatureBelow(String mac) {
//   JsonDocument payload;
//   return Message(String(millis()), "on_temperature_below", "request", mac, payload);
// }

// Message onHumidityAbove(String mac) {
//   JsonDocument payload;
//   return Message(String(millis()), "on_humidity_above", "request", mac, payload);
// }

// Message onHumidityBelow(String mac) {
//   JsonDocument payload;
//   return Message(String(millis()), "on_humidity_below", "request", mac, payload);
// }