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
    client(espClient)
    {
      mac = WiFi.macAddress();
      client.setServer(brokerIp, brokerPort);
    }

void Mqtt::reconnect() {
  while (!client.connected()) {
    if (client.connect(deviceFunction)) {
      String topic = "device/"+mac+"/+";
      client.subscribe(topic.c_str());
      sendMessage(connectMessage(mac, deviceFunction, WiFi.RSSI()));
    } else {
      delay(5000);
    }
  }
}

void Mqtt::loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Łączenie z WiFi: ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("\nWiFi OK");
  }

  if (!client.connected()) {
    reconnect();
  }

  client.loop();
  sendToRouter();
  unsigned long now = millis();
  if (now - lastHealthCheck > healthCheckInterval) {
    lastHealthCheck = now;
  }
}

void Mqtt::sendToRouter(){
  if (pointer > 0 && client.connected()) {
    Message* msg = messageBuffer[0];
    for (int  i = 0; i < pointer - 1; i++) {
      messageBuffer[i] = messageBuffer[i + 1];
    }
    pointer--;
    Serial.println(msg->toJson());
    client.publish(brokerName, msg->toJson().c_str(), msg->retain);
    delete msg;
  }
}

void Mqtt::sendMessage(Message msg)  {
  if (pointer < BUFFER_SIZE) {
    messageBuffer[pointer] = new Message(msg);
    pointer++ ;
  }
}

void Mqtt::setCallback(std::function<void(Message&)> cb) {
  messageHandler = cb;
  client.setCallback([this](char* topic, byte* payload, unsigned int length) {
    String msgStr;
    for (unsigned int i = 0; i < length; i++) msgStr += (char)payload[i];

    Message msg = Message::fromJson(msgStr);
    if (messageHandler) messageHandler(msg);
  });
}

void Mqtt::updateClinet(){
    client.loop();
}

String Mqtt::getMac() {
  return mac;
}

