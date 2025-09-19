#ifndef MQTT_H
#define MQTT_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <AsyncMqttClient.h>
#include <Message.h>
#include <Ticker.h>

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
    AsyncMqttClient client;
    Ticker healthTicker;

    unsigned long lastHealthCheck = 0;
    std::function<void(Message&)> messageHandler;
    int pointer = 0; 
    Message* messageBuffer[BUFFER_SIZE];
    String willMessage;
    void reconnect();
    void sendToRouter();
    void healthCheck();

  public:
    Mqtt(const char* brokerIp,
         int brokerPort,
         const char* brokerName,
         const char* ssid,
         const char* password,
         const char* deviceFunction,
         unsigned long healthCheckInterval);

    String mac;
    bool connected;
    bool isConnected();
    void begin();
    void sendMessage(const Message message);
    void onMessage(std::function<void(Message&)> cb);
    String getMac();
};

#endif
