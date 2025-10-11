
#ifndef RFID_MESSAGE_H
#define RFID_MESSAGE_H

#include <ArduinoJson.h>
#include "Message.h"
Message addTagResponse(Message message, String uid);
Message onReadRequest(String mac, String uid);

#endif