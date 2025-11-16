
#ifndef MEASUREMENT_MESSAGE_H
#define MEASUREMENT_MESSAGE_H

#include <ArduinoJson.h>
#include "Message.h"

Message tempHumMeasurentRequest(String mac, float temperature, float humidity);

Message onTemperatureAbove(String mac);
Message onTemperatureBelow(String mac);
Message onHumidityAbove(String mac);
Message onHumidityBelow(String mac);

#endif
