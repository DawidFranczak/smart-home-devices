#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <BasicMessage.h>
#include "Mqtt.h"

Mqtt::Mqtt(ConfigManager& configManager)
  : client(),
   configManager(configManager){}

void Mqtt::begin(){
  mac = WiFi.macAddress();
  deviceFunction = configManager.get("device.function").as<const char*>();
  
  brokerIp = configManager.get("mqtt.brokerIp").as<const char*>();
  brokerPort = configManager.get("mqtt.brokerPort").as<int>();
  brokerName = configManager.get("mqtt.brokerName").as<const char*>();
  healthCheckInterval = configManager.get("mqtt.healthCheckInterval").as<int>();

  ssid = configManager.get("wifi.ssid").as<const char*>();
  password = configManager.get("wifi.password").as<const char*>();
  client.setServer(brokerIp, brokerPort);
  willMessage = disconnectRequest(mac).toJson();
  client.setWill(
    brokerName,
    2,
    true,
    willMessage.c_str()
  );
  client.setKeepAlive(5);

  client.onConnect([this](bool sessionPresent) {
    Serial.println("MQTT connected");
    String topic = "device/" + this->mac + "/+";
    client.subscribe(topic.c_str(), 1);
    client.subscribe("device/broadcast/",1);
    sendMessage(connectRequest(this->mac, this->deviceFunction, WiFi.RSSI()));
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
    String msgStr;
    for (size_t i = 0; i < len; i++) msgStr += (char)payload[i];
    Message msg = Message::fromJson(msgStr);
    if (msg.message_event == "get_connected_devices"){
      sendMessage(connectRequest(this->mac, this->deviceFunction, WiFi.RSSI()));
      return;
    }else if (msg.message_event == "device_connect"){
      sendMessage(getSettings(this->getMac()));
      return;
    }
    // }else if (msg.message_event == "update_firmware") {
    //   Serial.println("=== OTA UPDATE REQUEST RECEIVED ===");

    //   String url = msg.url;

    //   Serial.print("Pobieram firmware z: ");
    //   Serial.println(url);

    //   t_httpUpdate_return ret = ESPhttpUpdate.update(url);

    //   if (ret == HTTP_UPDATE_OK) {
    //     Serial.println("OTA OK – restart...");
    //   } else {
    //     Serial.printf("OTA ERROR: %s\n",
    //                   ESPhttpUpdate.getLastErrorString().c_str());
    //   }
    //   return;
    // }
    if (messageHandler) messageHandler(msg);
  });

  healthTicker.attach(healthCheckInterval, [this](){
    this->healthCheck();
  });

  if (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid, password);
  }

  for(int i=0;i<10;i++){
    delay(1);
    if(WiFi.status() == WL_CONNECTED) break;
  }

  if (!client.connected()) {
    client.connect();
  }
}

void Mqtt::loop() {
    static unsigned long lastWifiCheck = 0;
    unsigned long now = millis();
    if (now - lastWifiCheck >= 10000) {
      lastWifiCheck = now;
      if (WiFi.status() != WL_CONNECTED) {
          WiFi.disconnect();
          WiFi.begin(ssid, password);
      }
      if (WiFi.status() == WL_CONNECTED && !client.connected()) {
        client.connect();
      }
    }
   
}

void Mqtt::sendToRouter(){
  while(pointer > 0){
    if (client.connected()) {
      Message* msg = messageBuffer[0];
      for (int  i = 0; i < pointer - 1; i++) {
        messageBuffer[i] = messageBuffer[i + 1];
      }
      pointer--;
      client.publish(brokerName, msg->qos, msg->retain, msg->toJson().c_str());
      delete msg;
    }else{
      break;
    }
  }
}

bool Mqtt::isConnected(){
  return connected;
}

void Mqtt::sendMessage(Message msg)  {
  if (!client.connected() && msg.qos == 0){
    return;
  }
  if (pointer < BUFFER_SIZE) {
    messageBuffer[pointer] = new Message(msg);
    pointer++ ;
  }
  sendToRouter();
}

void Mqtt::healthCheck() {
  sendMessage(healthCheckRequest(mac, WiFi.RSSI()));
}

void Mqtt::onMessage(std::function<void(Message&)> cb) {
  messageHandler = cb;
}

String Mqtt::getMac() {
  return mac;
}