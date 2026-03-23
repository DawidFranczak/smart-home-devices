#include <Arduino.h>

#if defined(ESP32)

  #include <WiFi.h>
  #include <HTTPClient.h>
  #include <HTTPUpdate.h>

#elif defined(ESP8266)

  #include <ESP8266WiFi.h>
  #include <ESP8266HTTPClient.h>
  #include <ESP8266httpUpdate.h>

#endif

#include <BasicMessage.h>
#include "Mqtt.h"

#if defined(ESP8266)
  #define OTA_UPDATE(client, url) ESPhttpUpdate.update(client, url)
  #define OTA_ERROR_STRING() ESPhttpUpdate.getLastErrorString().c_str()
#elif defined(ESP32)
  #define OTA_UPDATE(client, url) httpUpdate.update(client, url)
  #define OTA_ERROR_STRING() httpUpdate.getLastErrorString().c_str()
#endif


Mqtt::Mqtt(ConfigManager& configManager)
  : client(),
    configManager(configManager){}

void Mqtt::begin(){
  mac = WiFi.macAddress();
  otaActive = false;
  firmwareVersion = configManager.get("device.firmwareVersion").as<float>();
  chipType = configManager.get("device.chipType").as<const char*>();
  brokerIp = configManager.get("mqtt.brokerIp").as<String>();
  brokerPort = configManager.get("mqtt.brokerPort").as<int>();
  brokerName = configManager.get("mqtt.brokerName").as<const char*>();
  ssid = configManager.get("wifi.ssid").as<const char*>();
  password = configManager.get("wifi.password").as<const char*>();
  
  
  willMessage = disconnectEvent(mac).toJson();
  client.setWill(brokerName, 1, true, willMessage.c_str());
  client.setKeepAlive(5);

  client.onConnect([this](bool sessionPresent) {
    String topic = "device/" + this->mac + "/+";
    client.subscribe(topic.c_str(), 1);
    client.subscribe("device/broadcast/",1);
    Message msg = connectEvent(mac, chipType, WiFi.RSSI(), firmwareVersion); 
    QueuedMessage qm;
    qm.payload = msg.toJson();
    qm.qos=1;
    qm.retain=true;
    sendMessage(qm);
    Serial.println("MQTT OK");
    connected = true;
  });

  client.onDisconnect([this](AsyncMqttClientDisconnectReason reason) {
    client.connect();
    connected = false;
  });

  client.onMessage([this](char* topic, char* payload,
                      AsyncMqttClientMessageProperties properties,
                      size_t len, size_t index, size_t total) {
    for (size_t i = 0; i < len; i++) _tempPayload += (char)payload[i];

   if (index + len == total) {
      Message msg = Message::fromJson(_tempPayload);
      _tempPayload = "";
      if (msg.command == "get_connected_devices"){
        Message msg = connectEvent(this->mac, chipType, WiFi.RSSI(),this->firmwareVersion);
        QueuedMessage qm;
        qm.payload = msg.toJson();
        qm.qos=2;
        qm.retain=true;
        sendMessage(qm);
        return;
      }
      else if (msg.command == "update_firmware") {
        if (msg.payload["url"].is<const char*>() && msg.payload["version"].is<float>()) {
            otaUrl = msg.payload["url"];
            otaActive = true;
            configManager.set("device.firmwareVersion",msg.payload["version"]);
            configManager.save();
        }
        return;
      }
      if (messageHandler) messageHandler(msg);
   }
  });

  
  WiFi.begin(ssid, password);

  IPAddress server;
  server.fromString(brokerIp);
  client.setServer(server, brokerPort);
  client.setKeepAlive(5);

  reconnect();
}

void Mqtt::loop() {
    if (otaActive){
      otaActive = false;
      WiFiClient otaClient;
       t_httpUpdate_return ret = OTA_UPDATE(otaClient, otaUrl);
      if (ret == HTTP_UPDATE_OK) {
        Serial.println("OTA OK – restart...");
      } else {
        configManager.set("device.firmwareVersion",firmwareVersion);
        configManager.save();
        Message msg = firmwareUpdateErrorEvent(getMac(),OTA_ERROR_STRING());
        QueuedMessage qm;
        qm.payload = msg.toJson();
        qm.qos=2;
        qm.retain=true;
        sendMessage(qm);
        Serial.printf("OTA ERROR: %s\n",OTA_ERROR_STRING());
      }
    }

    static unsigned long lastWifiCheck = 0;
    unsigned long now = millis();
    if (now - lastWifiCheck >= 10000) {
      lastWifiCheck = now;
      reconnect();
    }

    sendToRouter();
}


void Mqtt::reconnect() {
 if (WiFi.status() == WL_CONNECTED && !client.connected()) {
        Serial.println("Reconnecting to MQTT...");
        client.connect();
    }
}

void Mqtt::sendToRouter(){
  if (pointer == 0 || !client.connected()) return;

  QueuedMessage& qm = messageBuffer[0];

  uint16_t packetId = client.publish(
    brokerName,
    qm.qos, 
    qm.retain,
    qm.payload.c_str() 
  );

  if (packetId>0) {
    Serial.print("MQTT sent [QoS:"); Serial.print(qm.qos); Serial.println("]:");
    Serial.println(qm.payload);
    for (int i = 0; i < pointer - 1; i++) {
      messageBuffer[i] = messageBuffer[i+1];
    }
    messageBuffer[pointer - 1].payload = "";
    pointer--;
  } 
}

bool Mqtt::isConnected(){
  return connected;
}

void Mqtt::sendMessage(const QueuedMessage& msg) {
  if (!client.connected() && msg.qos == 0){
    return;
  }
if (pointer < BUFFER_SIZE) {
    messageBuffer[pointer].payload = msg.payload;
    messageBuffer[pointer].qos = msg.qos;
    messageBuffer[pointer].retain = msg.retain;
    pointer++;
  }
  sendToRouter();
}

void Mqtt::onMessage(std::function<void(Message&)> cb) {
  messageHandler = cb;
}

String Mqtt::getMac() {
  return mac;
}