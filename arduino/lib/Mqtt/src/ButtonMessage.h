
#ifndef BUTTON_MESSAGE_H
#define BUTTON_MESSAGE_H

#include <ArduinoJson.h>
#include "Message.h"

// Message onClickRequest(String mac);
// Message onHoldRequest(String mac);
// Message onToggleRequest(String mac);
Message toggleResult(Message& message, int peripheralId, bool isOn, bool accept=true);
Message toggleEvent(int peripheralId, bool isOn);
#endif
