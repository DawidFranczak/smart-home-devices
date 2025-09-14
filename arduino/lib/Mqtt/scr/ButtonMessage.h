
#ifndef BUTTON_MESSAGE_H
#define BUTTON_MESSAGE_H

#include <ArduinoJson.h>
#include "Message.h"

Message onClickMessage(String mac);
Message onHoldMessage(String mac);

#endif
