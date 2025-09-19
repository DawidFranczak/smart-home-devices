
#ifndef BUTTON_MESSAGE_H
#define BUTTON_MESSAGE_H

#include <ArduinoJson.h>
#include "Message.h"

Message onClickRequest(String mac);
Message onHoldRequest(String mac);

#endif
