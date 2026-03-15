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
  : espClient(),
    client(espClient),
    configManager(configManager){}

void Mqtt::begin(){
  mac = WiFi.macAddress();
  otaActive = false;
  firmwareVersion = configManager.get("device.firmwareVersion").as<float>();
  chipType = configManager.get("device.chipType").as<const char*>();
  brokerIp = configManager.get("mqtt.brokerIp").as<String>();
  brokerPort = configManager.get("mqtt.brokerPort").as<int>();
  brokerName = configManager.get("mqtt.brokerName").as<const char*>();
  healthCheckInterval = configManager.get("mqtt.healthCheckInterval").as<unsigned long>();
  healthCheckInterval = healthCheckInterval * 1000;
  ssid = configManager.get("wifi.ssid").as<const char*>();
  password = configManager.get("wifi.password").as<const char*>();
  
  willMessage = disconnectEvent(mac).toJson();
  WiFi.begin(ssid, password);

  for (size_t i = 0; i < 5; i++)
  {
    if (WiFi.status() == WL_CONNECTED) break;
     delay(5000);
     Serial.println(".");
  }

  IPAddress server;
  server.fromString(brokerIp);
  client.setServer(server, brokerPort);

  client.setBufferSize(2048);
  client.setKeepAlive(20);

  client.setCallback([this](char* topic, byte* payload, unsigned int length){
    String msgStr;
    for (unsigned int i = 0; i < length; i++) {
        msgStr += (char)payload[i];
    }
    Message msg = Message::fromJson(msgStr);
    if (msg.command == "get_connected_devices"){
      Message msg = connectEvent(this->mac, chipType, WiFi.RSSI(),this->firmwareVersion);
      QueuedMessage qm;
      qm.payload = msg.toJson();
      qm.qos=2;
      qm.retain=true;
      sendMessage(qm);
      return;
    // }else if (msg.command == "device_connect"){
    //   sendMessage(getSettings(this->getMac()));
    //   return;
    }else if (msg.command == "update_firmware") {
      if (msg.payload["url"].is<const char*>() && msg.payload["version"].is<float>()) {
          otaUrl = msg.payload["url"];
          otaActive = true;
          configManager.set("device.firmwareVersion",msg.payload["version"]);
          configManager.save();
      }
      return;
    }
    if (messageHandler) messageHandler(msg);
  });

  reconnect();
}

void Mqtt::loop() {
    client.loop();
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

    now = millis();
    static unsigned long lastHealthCheck = 0;
    if(now - lastHealthCheck > (unsigned long)healthCheckInterval){
      healthCheck();
      lastHealthCheck=now;
    }

    sendToRouter();
}


void Mqtt::reconnect() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WIFI reconnecting...");
        WiFi.disconnect();
        delay(1000);
        WiFi.begin(ssid, password);
    }else if (!client.connected()) {
        Serial.println("MQTT reconnecting...");
        if (client.connect(mac.c_str(),nullptr, nullptr, brokerName, 2, true, willMessage.c_str())) {
          Serial.println("MQTT OK");
            String topic = "device/" + mac + "/+";
            client.subscribe(topic.c_str(), 1);
            client.subscribe("device/broadcast/", 1);
            delay(100);
            Message msg = connectEvent(mac, chipType, WiFi.RSSI(), firmwareVersion); 
            QueuedMessage qm;
            qm.payload = msg.toJson();
            qm.qos=1;
            qm.retain=true;
            sendMessage(qm);
            IPAddress ip = WiFi.localIP();
            Serial.printf("IP: %d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);
            connected = true;
        } else {
            Serial.print("failed rc=");
            Serial.println(client.state());
        }
    }
}

void Mqtt::sendToRouter(){
  if (sending || pointer == 0 || !client.connected()) return;

  sending = true;
  QueuedMessage& qm = messageBuffer[0];

  bool ok = client.publish(
    brokerName, 
    qm.payload.c_str(), 
    qm.retain
  );

  if (ok) {
    Serial.print("MQTT sent [QoS:"); Serial.print(qm.qos); Serial.println("]:");
    Serial.println(qm.payload);
    for (int i = 0; i < pointer - 1; i++) {
      messageBuffer[i] = messageBuffer[i+1];
    }
    messageBuffer[pointer - 1].payload = "";
    pointer--;
  } 
  sending = false;
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

void Mqtt::healthCheck() {
  Message msg = healthCheckEvent(mac, WiFi.RSSI());
  QueuedMessage qm;
  qm.payload = msg.toJson();
  qm.qos=0;
  qm.retain=false;
  sendMessage(qm);
}

void Mqtt::onMessage(std::function<void(Message&)> cb) {
  messageHandler = cb;
}

String Mqtt::getMac() {
  return mac;
}