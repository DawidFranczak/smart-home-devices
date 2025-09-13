#ifndef BASIC_MESSAGE_H
#define BASIC_MESSAGE_H

#include <ArduinoJson.h>
#include "Message.h"

Message connectMessage(String mac, String fun, int wifiStrength);
Message healthCheck(String mac, int wifiStrength);
Message basicResonse(Message& message);
#endif
