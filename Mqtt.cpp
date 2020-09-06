#include "Mqtt.h"
#include "Arduino.h"
#include <PubSubClient.h>

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
    // if (mqttClient.connected() && subscriptionsQueue.size() > 0) {
    //     while(subscriptionsQueue.size() > 0) {
    //         mqttClient.subscribe(subscriptionsQueue.front(), 1); //subscribe item
    //         subscriptionsQueue.pop_front(); //remove item
    //     }
    // }
    mqttClient.loop();
    //mqttClient.publish("room/temperature", temperatureTemp);
}

void Mqtt::mqttCallback(String topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String msg;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    msg += (char)message[i];
  }
  Serial.println();

  callback(topic, msg);
}

void Mqtt::mqttConnect() {
    // Loop until we're reconnected
    if (!mqttClient.connected()) {
        Serial.println("Attempting MQTT connection...");
        // Attempt to connect
        /*
            YOU MIGHT NEED TO CHANGE THIS LINE, IF YOU'RE HAVING PROBLEMS WITH MQTT MULTIPLE CONNECTIONS
            To change the ESP device ID, you will have to give a new name to the ESP8266.
            Here's how it looks:
            if (client.connect("ESP8266Client")) {
            You can do it like this:
            if (client.connect("ESP1_Office")) {
            Then, for the other ESP:
            if (client.connect("ESP2_Garage")) {
            That should solve your MQTT multiple connections problem
        */
        if (mqttClient.connect("SunriseAlarm")) {
            Serial.println("connected");  
            // Subscribe or resubscribe to a topic
            mqttClient.subscribe("sunriseAlarm/#");
            //mqttClient.setServer("192.168.68.10", 1883);

            mqttClient.setCallback([this] (String topic, byte* message, unsigned int length) {
                this->mqttCallback (topic, message, length);
            }); 
            
        } else {
            Serial.print("MQTT connction failed. State: ");
            Serial.println(mqttClient.state());
            reconnectTime = millis() + 5000;
        }
    }
}

void Mqtt::setCallback(std::function<void(String, String)> callback_) {
    this->callback = callback_;
}

void Mqtt::publish(String topic, String payload) {
    mqttClient.publish(topic.c_str(), payload.c_str());
}

// void Mqtt::subscribe(String topic) {
//     if (mqttClient.connected()) {
//         mqttClient.subscribe(topic.c_str(), 1); 
//     } else {
//         subscriptionsQueue.push_back(topic);
//     }
// }
