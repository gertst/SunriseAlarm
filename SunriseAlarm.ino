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
#include <EEPROM.h>

#include "OTA.h"
#include "DotMatix.h"
#include "RotaryButton.h"
#include "DisplayTime.h"
#include "Menu.h"
#include "LedStrip.h"
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

#define LDR_PIN        17  //A0  //LDR: Light Dependent Resistor 

#define LED_STRIP_PIN  12  //D6
#define NUMBER_OF_PIXELS_IN_LED_STRIP 150
// Adafruit_NeoPixel strip(NUMBER_OF_PIXELS_IN_LED_STRIP, LED_STRIP_PIN, NEO_GRBW + NEO_KHZ800);
// int cnt = 0;

// Turn on debug statements to the serial output
#define  DEBUG_ENABLE  1

#if  DEBUG_ENABLE
#define DEBUG(s, x) { Serial.print(F(s)); Serial.print(x); }
#define DEBUGS(x) Serial.print(F(x))
#define DEBUGX(x) Serial.println(x, HEX)
#else
#define DEBUG(s, x)
#define DEBUGS(x)
#define DEBUGX(x)
#endif

enum Mode {
  MODE_WIFI_STATUS,
  MODE_CLOCK,
  MODE_SET_ALARM_HOURS,
  MODE_SET_ALARM_MINUTES,
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

OTA ota;
DotMatrix dotMatrix(MAX_DEVICES, HARDWARE_TYPE, CLK_PIN, DATA_PIN, CS_PIN);
RotaryButton rotaryButton(ENCODER_A_PIN, ENCODER_B_PIN, SWITCH_PIN);
DisplayTime displayTime;
Menu menu;
LedStrip ledStrip(LED_STRIP_PIN, NUMBER_OF_PIXELS_IN_LED_STRIP);

void updateClock() {
  String newTime = displayTime.getTime();
  if (lastTime != newTime) {
    lastTime = newTime;
    dotMatrix.showText(newTime);
  }
}

void updateAlarm() {
  dotMatrix.showText(displayTime.getAlarmText(mode == MODE_SET_ALARM_HOURS ? 1 : 2));
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
  Serial.print("intensity: ");
  Serial.println(intensity);
  dotMatrix.setIntensity(intensity); //0 - 15
}

void setup() {
  dotMatrix.setup();
  dotMatrix.showText("Hello!");
  dotMatrix.loop();
  delay(500);
  EEPROM.begin(512); 
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {  
    Serial.println("Connection Failed! Rebooting...");  
    delay(5000);  
    ESP.restart();  
  }  
  
  ota.setup();
  displayTime.setup();
  menu.setup();
  ledStrip.setup();

  // #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  //   clock_prescale_set(clock_div_1);
  // #endif
  // strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  // strip.show();            // Turn OFF all pixels ASAP
  // strip.setBrightness(40); // Set BRIGHTNESS to about 1/5 (max = 255)

  setMode(MODE_WIFI_STATUS);

  timerLDR.setInterval(1000, updateLDR);
  
  dotMatrix.isAlarmOn = displayTime.getIsAlarmOn();

}

void processCommand(String command) {
  if (command == "AlarmOnOff") {
    displayTime.setIsAlarmOn(!displayTime.getIsAlarmOn());
    dotMatrix.isAlarmOn = displayTime.getIsAlarmOn();
  } else if (command == "SetAlarm") {
    setMode(MODE_SET_ALARM_HOURS);
  }
}

void setMode(Mode newMode) {
  if (newMode == MODE_MENU) {
    //reset the menu state
    menu.initMenu();
  } else if (newMode == MODE_CLOCK) {
    //don't wait for next second: already set it now.
    updateClock();
  } else if (newMode == MODE_SET_ALARM_HOURS || newMode == MODE_SET_ALARM_MINUTES) {
    //don't wait for next second: already set it now.
    updateAlarm();
  } 
  mode = newMode;
  dotMatrix.underlineHours(mode == MODE_SET_ALARM_HOURS);
  dotMatrix.underlineMinutes(mode == MODE_SET_ALARM_MINUTES);
}

void loop() {
  //ArduinoOTA.handle();
  dotMatrix.loop();
  displayTime.loop();
  
  timerLDR.run();

  ota.loop();
  ledStrip.loop();
  // colorWipe(strip.Color(cnt % 150,   40,   cnt % 39)     , 50); // Red
  // cnt++;
  // if (cnt > 10000) {
  //   cnt = 0;
  // }

  switch (mode) {
    case MODE_WIFI_STATUS:
      updateWifiStatus();
      break;
    case MODE_CLOCK:
      updateClock();
      break;
    case MODE_SET_ALARM_HOURS:
      updateAlarm();
      break;
    case MODE_SET_ALARM_MINUTES:
      updateAlarm();
      break;
    case MODE_MENU:
      if (menu.getActiveMenuItem()["id"] == "Root") {
        setMode(MODE_CLOCK);
      } else {
        String menuLabel = menu.getActiveMenuItem()["label"].as<String>();
        if (menu.getActiveMenuItem()["id"] == "Alarm") { 
          menuLabel = menuLabel + ": " + displayTime.getAlarmText(0);
        }
        dotMatrix.showText(menuLabel);
      }
      break;
  }

  //if menu is idle, go back to clock mode
  if (mode != MODE_CLOCK && mode != MODE_WIFI_STATUS && rotaryButton.getSecondsIdle() > 8) {
    setMode(MODE_CLOCK);
  }

  if (rotaryButton.getIsButtonPressed()) {
    if (mode == MODE_CLOCK) {
      setMode(MODE_MENU);
    } else if (mode == MODE_MENU) {
      String menuId = menu.commitMenu();
      Serial.println(F("menuId: "));
      Serial.println(menuId);
      processCommand(menuId);
    } else if (mode == MODE_SET_ALARM_HOURS) {
      setMode(MODE_SET_ALARM_MINUTES);
    } else if (mode == MODE_SET_ALARM_MINUTES) {
      setMode(MODE_MENU);
    }
  }

  rotaryPosition = rotaryButton.getPosition();
  if (lastRotaryPosition != rotaryPosition) {
    //dotMatrix.showText(String(rotaryPosition));
    if (mode == MODE_CLOCK) {
      setMode(MODE_MENU);
    } else if (mode == MODE_SET_ALARM_HOURS) {
      displayTime.updateAlarmHours(rotaryPosition - lastRotaryPosition);
    } else if (mode == MODE_SET_ALARM_MINUTES) {
      displayTime.updateAlarmMinutes(rotaryPosition - lastRotaryPosition);
    } else {
      menu.rotateMenu(rotaryPosition - lastRotaryPosition);
    }
    lastRotaryPosition = rotaryPosition;
  }
}

// void colorWipe(uint32_t color, int wait) {
//   //for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
//     strip.setPixelColor(cnt % 150, color);         //  Set pixel's color (in RAM)
//     strip.show();                          //  Update strip to match
//     //delay(wait);                           //  Pause for a moment
//   //}
// }