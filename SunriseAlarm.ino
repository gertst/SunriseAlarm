/**
 * 
 * Shows a clock.
 * Time is coming from NTP server
 * 
 * 
 */ 

#include "Arduino.h"
#include <ESP8266WiFi.h>      // ESP library for all WiFi functions
#include <SimpleTimer.h>


#include "DotMatix.h"
#include "RotaryButton.h"
#include "DisplayTime.h"

/* 
static const uint8_t D0 = 16;
static const uint8_t D1 = 5;
static const uint8_t D2 = 4;
static const uint8_t D3 = 0;
static const uint8_t D4 = 2;
static const uint8_t D5 = 14;
static const uint8_t D6 = 12;
static const uint8_t D7 = 13;
static const uint8_t D8 = 15;
static const uint8_t D9 = 3;
static const uint8_t D10 = 1; */

//dot matrix led display
#define CLK_PIN   14 //D5
#define DATA_PIN  13 //D7
#define CS_PIN    2  //D4

//rotary switch
#define ENCODER_A_PIN   5  //D1
#define ENCODER_B_PIN   4  //D2
#define SWITCH_PIN      0  //D3

#define LDR_PIN      17 //A0  //LDR: Light Dependent Resistor 

const char *ssid     = "telenet-Gert";
const char *password = "gertstogo1627";

// Define the number of 8x8 dot matrix devices and the hardware SPI interface
#define MAX_DEVICES 4
#define HARDWARE_TYPE MD_MAX72XX::DR1CR0RR0_HW


DotMatrix dotMatrix(MAX_DEVICES, HARDWARE_TYPE, CLK_PIN, DATA_PIN, CS_PIN);
RotaryButton rotaryButton(ENCODER_A_PIN, ENCODER_B_PIN, SWITCH_PIN);
DisplayTime displayTime;

int rotaryPosition = 0;
int lastRotaryPosition = 0;
SimpleTimer timerEachSecond;
SimpleTimer timerEachMinute;



void eachSecond() {
  //only show time if the time is initialized
  String wifiStatus = displayTime.getWifiStatus();
  if (wifiStatus == "OK") {
    dotMatrix.showText(displayTime.getTime());
  } else {
    dotMatrix.showText(wifiStatus);
  }

  int ldrValue = analogRead(LDR_PIN); //0 - 1023
  uint8_t intensity = map(ldrValue, 0, 1023, 6, 0);
  dotMatrix.setIntensity(intensity); //0 - 15
  // Serial.print("LDR: intensity: ");
  // Serial.println(intensity); 
}

void eachMinute() {

}

void setup() {
  Serial.begin(115200);
  Serial.println("Ready");
  //ArduinoOTA.begin(); 
  dotMatrix.setup();
  dotMatrix.showText("Wifi...");

  WiFi.begin(ssid, password);
  
  displayTime.setup();

  timerEachSecond.setInterval(1000, eachSecond);
  timerEachMinute.setInterval(60 * 1000, eachMinute);

}

void loop() {
  //ArduinoOTA.handle();
  dotMatrix.loop();
  displayTime.loop();
  
  timerEachSecond.run();
  timerEachMinute.run();

  if (rotaryButton.getIsButtonPressed()) {
    //Serial.println("pressed");
    dotMatrix.showText("Press!");
  }

  rotaryPosition = rotaryButton.getPosition();
  if (lastRotaryPosition != rotaryPosition) {
    dotMatrix.showText(String(rotaryPosition));
    lastRotaryPosition = rotaryPosition;
  }
}

