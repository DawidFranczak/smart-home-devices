#ifndef BASIC_MESSAGE_H
#define BASIC_MESSAGE_H

#include <ArduinoJson.h>
#include "Message.h"

Message connectEvent(String mac, String fun, int wifiStrength, float firmwareVersion);
Message disconnectEvent(String mac);
Message healthCheckEvent(String mac, int wifiStrength);
// Message getSettings(String mac);
// Message deviceStateEvent(String mac, String state);
Message firmwareUpdateErrorEvent(String mac, String error);
Message basicCPUResult(Message& message, bool accept=true);
Message basicPeripheralResult(Message& message, int peripheralId, bool accept=true);
#endif
