#ifndef MESSAGE_H
#define MESSAGE_H

#include <ArduinoJson.h>
#include "communication_module.h"

DeviceMessage on_click_request(String mac);
DeviceMessage on_hold_request(String mac);
DeviceMessage add_tag_response(DeviceMessage message, String uid);
DeviceMessage on_read_request(String mac, String uid);
DeviceMessage accept_response(DeviceMessage message);

#endif