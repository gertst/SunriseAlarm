#ifndef Mqtt_h
#define Mqtt_h

#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

class Mqtt
{
  public:
    // Mqtt() 
    // : mqttClient(wifiClient)
    // {}

    void setup();
    void loop();
    void setCallback(std::function<void(String, String)> callback);
    void publish(String topic, String payload);
    // void subscribe(String topic);
  
  private:
    WiFiClient wifiClient;
    PubSubClient mqttClient;
    void mqttConnect();
    void mqttCallback(String topic, byte* message, unsigned int length);
    std::function<void(String, String)> callback;
    // list<String> subscriptionsQueue;
};

#endif