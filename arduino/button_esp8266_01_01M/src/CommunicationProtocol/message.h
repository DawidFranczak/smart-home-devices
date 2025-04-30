#ifndef MESSAGE_H
#define MESSAGE_H

#include <ArduinoJson.h>
#include "communication_module.h"

DeviceMessage on_click_request(String mac);
DeviceMessage on_hold_request(String mac);

#endif