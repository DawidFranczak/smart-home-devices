#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <BasicMessage.h>
#include "Mqtt.h"

Mqtt::Mqtt(const char* brokerIp,
           int brokerPort,
           const char* brokerName,
           const char* ssid,
           const char* password,
           const char* deviceFunction,
           unsigned long healthCheckInterval)
  : brokerIp(brokerIp),
    brokerPort(brokerPort),
    brokerName(brokerName),
    ssid(ssid),
    password(password),
    deviceFunction(deviceFunction),
    healthCheckInterval(healthCheckInterval),
    client()
    {
      mac = WiFi.macAddress();
      client.setServer(brokerIp, brokerPort);
      willMessage = disconnectRequest(mac).toJson();
      client.setWill(brokerName, 1, true, willMessage.c_str());
      client.setKeepAlive(5);

      client.onConnect([this](bool sessionPresent) {
        String topic = "device/" + this->mac + "/+";
        client.subscribe(topic.c_str(), 1);
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
        if (msg.message_event == "device_connect"){
          sendMessage(getSettings(this->getMac()));
          return;
        }
        if (messageHandler) messageHandler(msg);
      });

      healthTicker.attach(healthCheckInterval, [this](){
        this->healthCheck();
      });
    }

void Mqtt::begin(){
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
    }
    Serial.println("WiFi OK");
  }

  if (!client.connected()) {
    client.connect();
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