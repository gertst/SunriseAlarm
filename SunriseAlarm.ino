#ifndef SunriseAlarm_ino
#define SunriseAlarm_ino

#include "Arduino.h"
#include <ESP8266WiFi.h>      // ESP library for all WiFi functions
#include <ArduinoOTA.h>       // Library for uploading sketch over the air (OTA)
#include <SPI.h>              // Library for hardware or software driven SPI
//#include <EEPROM.h>           // Library for handle the EEPROM (on device memory, even stored when current is off)
//#include <U8g2lib.h>          // Library to control the 128x64 Pixel OLED display with SH1106 chip  https://github.com/olikraus/u8g2
#include <DNSServer.h>        // used in AP mode (config mode) to connect direct to the web page 
#include <ESP8266WebServer.h> // used in AP mode (config mode)
//#include <WiFiManager.h>    
#include "DotMatix.h"
#include "RotaryButton.h"

// Define the number of 8x8 dot matrix devices and the hardware SPI interface
#define MAX_DEVICES 4
#define HARDWARE_TYPE MD_MAX72XX::DR1CR0RR0_HW

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

DotMatrix dotMatrix(MAX_DEVICES, HARDWARE_TYPE, CLK_PIN, DATA_PIN, CS_PIN);
RotaryButton rotaryButton(ENCODER_A_PIN, ENCODER_B_PIN, SWITCH_PIN);

int rotaryPosition = 0;
int lastRotaryPosition = 0;


void setup() {
  Serial.begin(115200);
  Serial.println("Ready");
  ArduinoOTA.begin(); 
  dotMatrix.setup();
  dotMatrix.showText("Ready.");

}

void loop() {
  ArduinoOTA.handle();
  dotMatrix.loop();
  rotaryPosition = rotaryButton.loop();
  if (lastRotaryPosition != rotaryPosition) {
    dotMatrix.showText(String(rotaryPosition));
    lastRotaryPosition = rotaryPosition;
  }
  
}


#endif