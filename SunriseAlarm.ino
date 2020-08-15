/**
 * 
 * Shows a clock and state of alarm (on/off)
 * Time is coming from NTP server
 * 
 * Modes:
 * 0. wifi status mode: when wifi connection is not (yet) established:
 * - show wifi status on the screen
 * 
 * 1. Alarm mode: When alarms goes of: 
 * - rotating button = alarm off and back to clock mode
 * - pressing button = snooze and back to clock mode
 * 
 * 2. Clock mode: when clock is shown
 * - rotating button = go to menu mode (with return to clock mode after idle time)
 * - pressing button = go to menu mode (with return to clock mode after idle time)
 * 
 * 3. Menu mode: when menu is shown
 * - rotating button = next menu item (with return to clock mode after idle time)
 * - pressing button = enter menu item
 */ 

#include "Arduino.h"
#include <ESP8266WiFi.h>      // ESP library for all WiFi functions
#include <SimpleTimer.h>


#include "DotMatix.h"
#include "RotaryButton.h"
#include "DisplayTime.h"
#include "Menu.h"

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

enum Mode {
  MODE_WIFI_STATUS,
  MODE_CLOCK,
  MODE_ALARM,
  MODE_MENU
};


const char *ssid     = "telenet-Gert";
const char *password = "gertstogo1627";

// Define the number of 8x8 dot matrix devices and the hardware SPI interface
#define MAX_DEVICES 4
#define HARDWARE_TYPE MD_MAX72XX::DR1CR0RR0_HW


int rotaryPosition = 0;
int lastRotaryPosition = 0;
SimpleTimer timerLDR;
String lastTime = "";
Mode mode;
Mode lastMode;

DotMatrix dotMatrix(MAX_DEVICES, HARDWARE_TYPE, CLK_PIN, DATA_PIN, CS_PIN);
RotaryButton rotaryButton(ENCODER_A_PIN, ENCODER_B_PIN, SWITCH_PIN);
DisplayTime displayTime;
Menu menu;

void updateClock() {
  String newTime = displayTime.getTime();
  if (lastTime != newTime) {
    lastTime = newTime;
    dotMatrix.showText(newTime);
  }
}

void updateWifiStatus() {
  //go to wifi mode if wifi not OK and return to last mode once wifi is OK
  String wifiStatus = displayTime.getWifiStatus();
  dotMatrix.showText(wifiStatus);

  if (wifiStatus == "OK") {
    setMode(MODE_CLOCK);
  }
}

void updateLDR() {
  int ldrValue = analogRead(LDR_PIN); //0 - 1023
  uint8_t intensity = map(ldrValue, 0, 1023, 4, 0);
  // Serial.print("intensity: ");
  // Serial.println(intensity);
  dotMatrix.setIntensity(intensity); //0 - 15
}

void setup() {
  Serial.begin(115200);
  Serial.println("Ready");
  //ArduinoOTA.begin(); 
  dotMatrix.setup();
  menu.setup();

  WiFi.begin(ssid, password);

  setMode(MODE_WIFI_STATUS);
  
  displayTime.setup();

  timerLDR.setInterval(1000, updateLDR);

}

void setMode(Mode newMode) {
  if (newMode == MODE_MENU) {
    //reset the menu state
    menu.initMenu();
  } else if (newMode == MODE_CLOCK) {
    //don't wait for next second: already set it now.
    updateClock();
  }
  mode = newMode;
}

void loop() {
  //ArduinoOTA.handle();
  dotMatrix.loop();
  displayTime.loop();
  
  timerLDR.run();


  switch (mode) {
    case MODE_WIFI_STATUS:
      updateWifiStatus();
      break;
    case MODE_CLOCK:
      updateClock();
      break;
    case MODE_MENU:
      if (menu.getActiveMenuItem()["id"] == "Root") {
        setMode(MODE_CLOCK);
      } else {
        String menuLabel = menu.getActiveMenuItem()["label"].as<String>();
        dotMatrix.showText(menuLabel);
      }
      break;
    

  }

  //if menu is idle, go back to clock mode
  if (mode == MODE_MENU && rotaryButton.getSecondsIdle() > 5) {
    setMode(MODE_CLOCK);
  }

  if (rotaryButton.getIsButtonPressed()) {
    if (mode == MODE_CLOCK) {
      setMode(MODE_MENU);
    } else if (mode == MODE_MENU) {
      String menuId = menu.commitMenu();
      Serial.println(F("menuId: "));
      Serial.println(menuId);
    }
  }

  rotaryPosition = rotaryButton.getPosition();
  if (lastRotaryPosition != rotaryPosition) {
    //dotMatrix.showText(String(rotaryPosition));
    if (mode != MODE_MENU) {
      setMode(MODE_MENU);
      return;
    }
    menu.rotateMenu(rotaryPosition - lastRotaryPosition);
    lastRotaryPosition = rotaryPosition;
  }
}

