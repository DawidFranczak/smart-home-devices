#ifndef COMMUNICATION_MODULE_H
#define COMMUNICATION_MODULE_H

#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

#define MAX_QUEUE_SIZE 100

struct DeviceMessage {
  String message_id;
  String message_event;
  String message_type;
  String device_id;
  JsonDocument payload;

  DeviceMessage(String id, String event, String type, String dev_id, JsonDocument pl)
    : message_id(id), message_event(event), message_type(type), device_id(dev_id), payload(pl) {}

  String toJson() {
    JsonDocument doc;
    doc["message_id"] = message_id;
    doc["message_event"] = message_event;
    doc["message_type"] = message_type;
    doc["device_id"] = device_id;
    doc["payload"] = payload;

    String output;
    serializeJson(doc, output);
    return output;
  }

  static DeviceMessage fromJson(String json_str) {
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, json_str);
    if (error) {
      JsonDocument empty_payload;
      return DeviceMessage("", "", "", "", empty_payload);
    }
    JsonDocument payload;
    payload.set(doc["payload"]);
    return DeviceMessage(
      doc["message_id"].as<String>(),
      doc["message_event"].as<String>(),
      doc["message_type"].as<String>(),
      doc["device_id"].as<String>(),
      payload
    );
  }
};

class CommunicationModule {
public:
  CommunicationModule();
  bool is_connected();
  DeviceMessage* get_message();
  String extract_method_name_from_message(DeviceMessage& message);
  void send_message(DeviceMessage message);
  String get_mac();
  void start();

private:
  String host_name;
  int host_port;
  String ssid;
  String password;
  String fun;
  DeviceMessage* to_server_queue[MAX_QUEUE_SIZE];
  DeviceMessage* from_server_queue[MAX_QUEUE_SIZE];
  int to_server_count = 0;
  int from_server_count = 0;
  WiFiClient client;
  String mac;
  void connect_to_network();
  void receive_from_router();
  void send_to_router();
  void send_health_check();
  DeviceMessage get_connect_message();
};

#endif