#include "Mqtt.h"
#include "Arduino.h"
#include <PubSubClient.h>
#include <time.h>
#include <TimeLib.h>
#include <ArduinoJson.h>

uint16_t reconnectTime = millis() + 5000;

void Mqtt::setup() {
    //WiFi.mode(WIFI_STA);
    //mqttConnect();
    mqttClient.setClient(wifiClient);
    mqttClient.setServer("192.168.68.10", 1883);
}

void Mqtt::loop() {
    if (!mqttClient.connected() && reconnectTime < millis()) {
        mqttConnect();
    }
    
    mqttClient.loop();
    
    //sync time from Mqtt broker (node red) each 20 minutes
    if (mqttClient.connected() && millis() - lastTimeSync > 1000 * 60 * 20) {
        lastTimeSync = millis();
        mqttClient.publish("time/get", "");
    }
}

void Mqtt::mqttCallback(String topic, byte* message, unsigned int length) {

    String msg;  
    for (int i = 0; i < length; i++) {
        msg += (char)message[i];
    }
    callback(topic, msg);
}

void Mqtt::mqttConnect() {
    // Loop until we're reconnected
    if (!mqttClient.connected()) {
        
        if (mqttClient.connect("SunriseAlarm")) {
            mqttClient.subscribe("sunriseAlarm/#");
            
            mqttClient.setCallback([this] (String topic, byte* message, unsigned int length) {
                this->mqttCallback (topic, message, length);
                if (topic == "time/return") {
                    
                    DynamicJsonDocument json(1024);
                    deserializeJson(json, message);

                    int year = json["year"]; // 2020
                    int month = json["month"]; // 8
                    int date = json["date"]; // 26
                    int hour = json["hour"]; // 19
                    int minute = json["minute"]; // 26
                    int second = json["second"]; // 10
                    //int millisecond = json["millisecond"]; // 928

                    setTime(hour, minute, second, date, month, year);

                }
            }); 

            mqttClient.subscribe("time/#");
            mqttClient.publish("time/get", "");
            
        } else {
            reconnectTime = millis() + 2500;
        }
    }
}

void Mqtt::setCallback(std::function<void(String, String)> callback_) {
    this->callback = callback_;
}

void Mqtt::publish(String topic, String payload = "") {
    mqttClient.publish(topic.c_str(), payload.c_str());
}