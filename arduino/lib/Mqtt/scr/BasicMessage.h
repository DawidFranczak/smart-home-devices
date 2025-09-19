#ifndef BASIC_MESSAGE_H
#define BASIC_MESSAGE_H

#include <ArduinoJson.h>
#include "Message.h"

Message connectRequest(String mac, String fun, int wifiStrength);
Message healthCheckRequest(String mac, int wifiStrength);
Message disconnectRequest(String mac);
Message getSettings(String mac);
Message basicResonse(Message& message, bool accept=true);
#endif
