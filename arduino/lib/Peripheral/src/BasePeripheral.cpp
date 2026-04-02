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


void BasePeripheral::notify(const Message& originalMsg, ActionResult result){
    JsonDocument payload;
    payload["status"] = static_cast<uint8_t>(result);
    Message msg;
    msg.scope = originalMsg.scope;
    msg.direction = MessageDirection::RESULT;
    msg.type = originalMsg.type;
    msg.command = originalMsg.command;
    msg.message_id = originalMsg.message_id;
    msg.peripheral_id = this->id;
    msg.payload = payload;
    msg.device_id = mac;

    emitToEngine(msg, originalMsg.command, MessageTarget::BACKEND);
}
void BasePeripheral::notify(String eventType, JsonDocument payload, float eventValue){
    Message msg;
    msg.scope = Scope::PERIPHERAL;
    msg.direction = MessageDirection::INTENT;
    msg.type = MessageType::EVENT;
    msg.command = eventType;
    msg.message_id = String(millis());
    msg.peripheral_id = this->id;
    msg.payload = payload;
    msg.device_id = mac;

    emitToEngine(msg, eventType, MessageTarget::BOTH, eventValue);
}

void BasePeripheral::emitToEngine(Message& msg, String evType, MessageTarget target, float val){
    Event ev;
    ev.target = target;
    ev.type = evType;
    ev.emitDeviceId = this->id;
    ev.msg.payload = msg.toJson();
    ev.msg.qos = 1;
    ev.msg.retain = true;
    ev.value = val;

    systemContext.eventEngine.emit(ev);
}
bool BasePeripheral::getSettings(const String& extra, JsonDocument& doc, JsonObject& obj){
    if (extra.length() == 0 || extra == "null") {
        return false;
    }

    DeserializationError error = deserializeJson(doc, extra);

    if (error) {
        Serial.print(F("Błąd parsowania JSON: "));
        Serial.println(error.f_str());
        return false;
    }

    obj = doc.as<JsonObject>();
    
    return !obj.isNull();
}