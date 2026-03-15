#ifndef MQTT_H
#define MQTT_H

#include <Arduino.h>
#include <PubSubClient.h>
#include <Message.h>
#include <Ticker.h>
#include <ConfigManager.h>
#include <Types.h>

#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#define BUFFER_SIZE 10



class Mqtt {
  private:
    WiFiClient espClient;
    PubSubClient client;
    ConfigManager& configManager;
    
    const char* deviceFunction;
    String brokerIp;
    int brokerPort;
    const char* brokerName;
    const char* ssid;
    const char* password;
    float firmwareVersion;
    const char* chipType;
    
    bool otaActive;
    const char* otaUrl;
    int healthCheckInterval;
    unsigned long lastHealthCheck = 0;
    bool sending = false;
    std::function<void(Message&)> messageHandler;
    int pointer = 0; 
    QueuedMessage messageBuffer[BUFFER_SIZE];
    String willMessage;
    void sendToRouter();
    void healthCheck();
    void reconnect();
  public:
    Mqtt(ConfigManager& configManager);

    String mac;
    bool connected;
    bool isConnected();
    void begin();
    void loop();
    void sendMessage(const QueuedMessage& msg);
    void onMessage(std::function<void(Message&)> cb);
    String getMac();
};

#endif
