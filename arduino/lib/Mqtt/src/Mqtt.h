#ifndef MQTT_H
#define MQTT_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <AsyncMqttClient.h>
#include <Message.h>
#include <Ticker.h>
#include <ConfigManager.h>

#define BUFFER_SIZE 10

class Mqtt {
  private:
    WiFiClient espClient;
    AsyncMqttClient client;
    Ticker healthTicker;
    ConfigManager& configManager;

    const char* deviceFunction;
    const char* brokerIp;
    int brokerPort;
    const char* brokerName;
    const char* ssid;
    const char* password;
    
    int healthCheckInterval;
    unsigned long lastHealthCheck = 0;
    std::function<void(Message&)> messageHandler;
    int pointer = 0; 
    Message* messageBuffer[BUFFER_SIZE];
    String willMessage;
    void sendToRouter();
    void healthCheck();

  public:
    Mqtt(ConfigManager& configManager);

    String mac;
    bool connected;
    bool isConnected();
    void begin();
    void loop();
    void sendMessage(const Message message);
    void onMessage(std::function<void(Message&)> cb);
    String getMac();
};

#endif
