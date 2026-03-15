#pragma once
#include <Arduino.h>
#include <map>

class Message;

enum class ActionResult{
    ACCEPTED = 1,
    REJECTED = 2
};

enum class Scope{
    CPU = 1,
    PERIPHERAL = 2
};

enum class MessageType{
    ACTION = 1,
    EVENT = 2
};

enum class MessageDirection{
    INTENT = 1,
    RESULT = 2
};

enum class StartSyncType{
    PERIPHERAL = 0,
    RULE = 1
};


enum class EventScope {
    SYSTEM, 
    RULE 
};


enum class MessageTarget {
    INTERNAL,
    BACKEND,
    BOTH
};

struct QueuedMessage {
    String payload;
    uint8_t qos;
    bool retain;
};

struct Event {
    MessageTarget target;
    QueuedMessage msg;
    int emitDeviceId;
    String type;
};

struct Rule {
    int triggerId;
    String triggerEvent;
    int targetId;
    String targetAction;
    String extraSettings;
};

