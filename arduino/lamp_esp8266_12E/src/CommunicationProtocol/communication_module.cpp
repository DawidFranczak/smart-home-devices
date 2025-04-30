#include <Arduino.h>
#include "communication_module.h"
#include "settings.h"

CommunicationModule::CommunicationModule()
  : host_name(SERVER_IP), host_port(SERVER_PORT), ssid(SSID), password(PASSWORD), fun(DEVICE_FUNCTION){
  mac = WiFi.macAddress();

}

bool CommunicationModule::is_connected() {
  return WiFi.status() == WL_CONNECTED;
}

DeviceMessage* CommunicationModule::get_message() {
  if (from_server_count > 0) {
    DeviceMessage* msg = from_server_queue[0];
    for (int i = 0; i < from_server_count - 1; i++) {
      from_server_queue[i] = from_server_queue[i + 1];
    }
    from_server_count--;
    return msg;
  }
  return nullptr;
}

String CommunicationModule::extract_method_name_from_message(DeviceMessage& message) {
  return "_" + message.message_event + "_" + message.message_type;
}

void CommunicationModule::send_message(DeviceMessage message) {
  if (to_server_count < MAX_QUEUE_SIZE) {
    to_server_queue[to_server_count++] = new DeviceMessage(message);
  }
}

String CommunicationModule::get_mac() {
  return mac;
}

void CommunicationModule::start() {
  connect_to_network();
  if (!client.connected()) {
    if (client.connect(host_name.c_str(), host_port)) {
      DeviceMessage connect_msg = get_connect_message();
      client.println(connect_msg.toJson());
    }
  }
  send_to_router();
  receive_from_router();
  send_health_check();
}

void CommunicationModule::connect_to_network() {
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid.c_str(), password.c_str());
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
    }
  }
}

void CommunicationModule::receive_from_router() {
  if (client.connected() && client.available()) {
    client.setTimeout(100);
    String data = client.readStringUntil('\n');
    if (from_server_count < MAX_QUEUE_SIZE) {
      from_server_queue[from_server_count++] = new DeviceMessage(DeviceMessage::fromJson(data));
    }
  }
}

void CommunicationModule::send_to_router() {
  if (to_server_count > 0 && client.connected()) {
    DeviceMessage* msg = to_server_queue[0];
    for (int i = 0; i < to_server_count - 1; i++) {
      to_server_queue[i] = to_server_queue[i + 1];
    }
    to_server_count--;
    client.println(msg->toJson());
    delete msg;
  }
}

void CommunicationModule::send_health_check() {
  static unsigned long last_health_check = 0;
  if (millis() - last_health_check >= HEALT_CHECK_INTERVAL) {
    JsonDocument payload;
    payload["wifi_strength"] = WiFi.RSSI();
    send_message(DeviceMessage(String(millis()), "health_check", "request", mac, payload));
    last_health_check = millis();
  }
}

DeviceMessage CommunicationModule::get_connect_message() {
  JsonDocument payload;
  payload["fun"] = fun;
  payload["wifi_strength"] = WiFi.RSSI();
  return DeviceMessage(String(millis()), "device_connect", "request", mac, payload);
}