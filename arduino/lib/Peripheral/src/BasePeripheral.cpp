#include "BasePeripheral.h"
#include "EventEngine.h" 
#include "SystemContext.h"

void BasePeripheral::notify(String eventType, String command, JsonDocument payload, String messageId, MessageType type, MessageTarget target, float eventvalue) {
    Message msg;
    msg.scope = Scope::PERIPHERAL;
    msg.direction = messageId.isEmpty() ? MessageDirection::INTENT : MessageDirection::RESULT;
    msg.type = type;
    msg.command = command;
    msg.message_id = messageId.isEmpty()? String(millis()) : messageId;
    msg.peripheral_id = this->id;
    msg.payload = payload;
    msg.device_id = mac;

    Event ev;
    ev.target = target;
    ev.type = eventType;
    ev.emitDeviceId = this->id;
    ev.msg.payload = msg.toJson();
    ev.msg.qos = 1;
    ev.msg.retain = true;
    ev.value = eventvalue;

    systemContext.eventEngine.emit(ev);
}