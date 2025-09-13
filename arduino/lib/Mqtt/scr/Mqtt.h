#ifndef MQTT_H
#define MQTT_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Message.h>

#define BUFFER_SIZE 10

class Mqtt {
  private:
    const char* brokerIp;
    int brokerPort;
    const char* brokerName;
    const char* ssid;
    const char* password;
    const char* deviceFunction;
    unsigned long healthCheckInterval;

    WiFiClient espClient;
    PubSubClient client;

    unsigned long lastHealthCheck = 0;
    std::function<void(Message&)> messageHandler;
    int pointer = 0; 
    Message* messageBuffer[BUFFER_SIZE];

    void reconnect();
    void sendToRouter();

  public:
    Mqtt(const char* brokerIp,
         int brokerPort,
         const char* brokerName,
         const char* ssid,
         const char* password,
         const char* deviceFunction,
         unsigned long healthCheckInterval);

    String mac;
    void loop();
    void sendMessage(const Message message);
    void updateClinet();
    void setCallback(std::function<void(Message&)> cb);
    String getMac();
};

#endif
