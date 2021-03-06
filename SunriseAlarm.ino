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
#include "Esp.h"
#include <ESP8266WiFi.h>      // ESP library for all WiFi functions
#include <timer.h>
#include <EEPROM.h>
#include <Wire.h> //required by pin expander
#include <Adafruit_MCP23017.h> //pin exapnder chip MCP23017
#include <math.h>

#include "OTA.h"
#include "DotMatix.h"
#include "RotaryButton.h"
#include "DisplayTime.h"
#include "Menu.h"
#include "LedStrip.h"
#include "Mqtt.h"
#include <TimeLib.h>

// #include "Http.h"
// #include "DFPlayer.h"

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

/*
Pin expander MCP23017
name  physical pin  Adafruit pin
GPB0	1             8	
GPB1	2             9	
GPB2	3             10	
GPB3	4             11	
GPB4	5             12	
GPB5	6             13	
GPB6	7             14	
GPB7	8             15
*/


//dot matrix led display
#define CLK_PIN           14 //D5
#define DATA_PIN          13 //D7
#define CS_PIN            15 //D8 

//rotary switch
#define ENCODER_A_PIN     5  //D1 (CLOCK)
#define ENCODER_B_PIN     4  //D2 (DT)
#define SWITCH_PIN        0   //D3
// digitalWrite(16, INPUT_PULLDOWN_16)

#define LDR_PIN           17  //A0  //LDR: Light Dependent Resistor 

#define DFPlayer_RX_PIN   4  //D2
#define DFPlayer_TX_PIN   16 //D0

#define LED_STRIP_PIN     12  //D6

#define NUMBER_OF_PIXELS_IN_LED_STRIP 150


// Turn on debug statements to the serial output
#define  DEBUG_ENABLE  0

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
  MODE_SET_ALARM_TOGGLE,
  MODE_MENU,
  MODE_ALARM,
  MODE_SET_LIGHT_SCENE
};


const char *ssid     = "telenet-Gert";
const char *password = "gertstogo1627";


// Define the number of 8x8 dot matrix devices and the hardware SPI interface
#define MAX_DEVICES 4
#define HARDWARE_TYPE MD_MAX72XX::DR1CR0RR0_HW


uint32_t rotaryPosition = 0;
uint32_t lastRotaryPosition = 0;
Timer timerLDR;
char lastTime[9] = "";
Mode mode;
Mode lastMode;
uint8_t lastWifiStatus;
uint32_t heapTrigger = millis() + 10000;
uint32_t lightSceneTrigger = 0;

// Instance of MCP23017 library
Adafruit_MCP23017 pinExpander;
OTA ota;
DotMatrix dotMatrix(MAX_DEVICES, HARDWARE_TYPE, CLK_PIN, DATA_PIN, CS_PIN);
RotaryButton rotaryButton(ENCODER_A_PIN, ENCODER_B_PIN, SWITCH_PIN);
DisplayTime displayTime;
Menu menu;
LedStrip ledStrip(LED_STRIP_PIN, NUMBER_OF_PIXELS_IN_LED_STRIP);
Mqtt mqtt;
// DFPlayer dfPlayer(DFPlayer_RX_PIN, DFPlayer_TX_PIN);

uint8_t intensity;
float intensityIncrease = 0.0;

void updateClock() {
  char *newTime = displayTime.getTime(mode == MODE_ALARM);
  if (strcmp(lastTime, newTime) != 0) {
    strncpy(lastTime, newTime, sizeof(lastTime));
    dotMatrix.showText(newTime);
  }
}

void updateSetAlarm() {
  dotMatrix.showText((char *)displayTime.getAlarmText(mode == MODE_SET_ALARM_HOURS ? 1 : 2).c_str());
}

void showAlarmToggle() {
  if (displayTime.getIsAlarmOn()) {
    dotMatrix.showText("On");
  } else {
    dotMatrix.showText("Off");
  }
}

void updateWifiStatus() {
  //go to wifi mode if wifi not OK and return to last mode once wifi is OK
  if (lastWifiStatus != WiFi.status()) {
    switch (WiFi.status()) {
        case WL_IDLE_STATUS: dotMatrix.showText("Wifi idle"); break;
        case WL_NO_SSID_AVAIL: dotMatrix.showText("No SSID available"); break;
        case WL_CONNECTED: dotMatrix.showText("OK"); break;
        case WL_CONNECT_FAILED: dotMatrix.showText("Wifi connection failed"); break;
        case WL_DISCONNECTED: dotMatrix.showText("Wifi ..."); break;
    }
    lastWifiStatus = WiFi.status();
  }

  if (WiFi.status() == WL_CONNECTED) {
    setMode(MODE_CLOCK);
  }
}

long mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return round((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min);
}

void updateLDR() {
  int ldrValue = analogRead(LDR_PIN); //0 - 1024
  int newIntensity = 1;
  newIntensity = mapFloat((float)ldrValue, 0.0, 1024.0, 15.0, intensityIncrease);
  
  //higher intensity when not idle for 30 secs
  newIntensity = rotaryButton.getSecondsIdle() < 30 ? (newIntensity) * 2  : newIntensity;


  //keep into limits
  newIntensity = max(0, newIntensity);
  newIntensity = min(15, newIntensity);

  if (intensity != newIntensity) {
    dotMatrix.setIntensity(newIntensity); //0 - 15
    mqtt.publish("sunriseAlarm/intensity", (String)newIntensity);
    intensity = newIntensity;
  }

  //restart ESP at 3 'o clock and if the ESP runs for more then 2 hours (to avoid resetting in the same hour)
  if (hour() == 3 && millis() > 2 * 60 * 60 * 1000) {
    ESP.restart();
  }

}

/*******************
 ****   SETUP    ***
 *******************/
void setup() {

/// change pin to gpio pins - IMPORTANT: Serial monitor will NOT work anymore with this code
/// revert back to serial pins with FUNCTION_0
// pinMode(1, FUNCTION_3);  //GPIO 1 (TX) swap the pin to a GPIO.
// pinMode(3, FUNCTION_3); //GPIO 3 (RX) swap the pin to a GPIO.
///////////////////////

  pinExpander.begin();
  // Define GPA0 (physical pin 21) as output pin
  pinExpander.pinMode(0, OUTPUT);

  dotMatrix.setup();
  dotMatrix.showText("Hello!");
  dotMatrix.loop();
  delay(500);
  EEPROM.begin(512); 
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.setSleepMode(WIFI_NONE_SLEEP);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {  
    Serial.println("Connection Failed! Rebooting...");  
    delay(5000);  
    ESP.restart();  
  }  

  ota.setup();
  displayTime.setup();
  displayTime.updateTime(millis());
  menu.setup();
  ledStrip.setup();
  mqtt.setup();
  // dfPlayer.setup();
  // mqtt.subscribe("sunriseAlarm/#");
  mqtt.setCallback(mqttCallback);

  setMode(MODE_WIFI_STATUS);

  timerLDR.setInterval(2000);
  timerLDR.setCallback(updateLDR);
  timerLDR.start();
  
  dotMatrix.setAlarmDot(displayTime.getIsAlarmOn());

}

void mqttCallback(String topic, String message) {
  
  ledStrip.command(topic, message);
  displayTime.command(topic, message);
  dotMatrix.command(topic, message);
  // dfPlayer.command(topic, message);

  if (topic == "sunriseAlarm/showAlarmHours") {
    setMode(MODE_SET_ALARM_HOURS);
  } else if (topic == "sunriseAlarm/showAlarmMinutes") {
    setMode(MODE_SET_ALARM_MINUTES);
  } else if (topic == "sunriseAlarm/alarmOnOff") {
    processMenuCommand("AlarmOnOff");
  } else if (topic == "sunriseAlarm/setMode") {
    setMode((Mode)message.toInt());
  } else if (topic == "sunriseAlarm/intensityIncrease") {
    intensityIncrease = message.toFloat();
  } else if (topic == "sunriseAlarm/ping") {
    mqtt.publish("sunriseAlarm/pong", displayTime.getTime(false));
  } else if (topic == "sunriseAlarm/restart") {
   ESP.restart();
  }
}

void processMenuCommand(String command) {
  if (command == "Light") {
    setMode(MODE_SET_LIGHT_SCENE);
  } 
  // else if (command == "LightWhite") {
  //   ledStrip.command("sunriseAlarm/fadeTo", "#77FCD795");
  // } else if (command == "LightSoft") {
  //   ledStrip.command("sunriseAlarm/fadeTo", "#7C4318");
  // } else if (command == "LightOff") {
  //   ledStrip.command("sunriseAlarm/fadeTo", "#00000000");
  // } 
}

void setMode(Mode newMode) {
  if (mode == newMode) {
    return;
  }

  mqtt.publish("sunriseAlarm/mode", (String)newMode);

  //if oldMode is alarm: stop alarm
  if (mode == MODE_ALARM) {
    // dfPlayer.command("sunriseAlarm/stopMusic", "");
    ledStrip.command("sunriseAlarm/fadeTo", "#000000,5");
    displayTime.alarmIsTriggered = false;
  }

  if (newMode == MODE_MENU) {
    //reset the menu state
    menu.initMenu();
  } else if (newMode == MODE_CLOCK) {
    //don't wait for next second: already set it now.
    updateClock();
  } else if (newMode == MODE_SET_ALARM_HOURS || newMode == MODE_SET_ALARM_MINUTES) {
    //don't wait for next second: already set it now.
    updateSetAlarm();
  } else if (newMode == MODE_ALARM) {
    updateClock();
    // dfPlayer.command("sunriseAlarm/volumeRaiseTo", "30");
    // dfPlayer.command("sunriseAlarm/play", "1");
    //ledStrip.command("sunriseAlarm/picture/get/sunrise", "1");
    mqtt.publish("sunriseAlarm/picture/get/Sunrise", "1");
  } else if (newMode == MODE_SET_ALARM_TOGGLE) {
    showAlarmToggle();
  } else if (newMode == MODE_SET_LIGHT_SCENE) {
    dotMatrix.showText((char *)ledStrip.getCurrentLightScene().label.c_str());
  } 
  mode = newMode;
  dotMatrix.underlineHours(mode == MODE_SET_ALARM_HOURS);
  dotMatrix.underlineMinutes(mode == MODE_SET_ALARM_MINUTES);
}


/*******************
 ****   LOOP    ****
 *******************/
void loop() {
  //ArduinoOTA.handle();
  dotMatrix.loop();
  displayTime.loop();
  ota.loop();
  ledStrip.loop();
  // dfPlayer.loop();
  
  
  if (heapTrigger < millis()) {
    heapTrigger = millis() + (1000 * 120);
    mqtt.publish("sunriseAlarm/debug/heap", (String)ESP.getFreeHeap());
    mqtt.publish("sunriseAlarm/debug/fragmentation", (String)ESP.getHeapFragmentation());
  }

  timerLDR.update();
  
  if (mode != MODE_WIFI_STATUS) {
    mqtt.loop();
  }

  if (displayTime.alarmGoesOff()) {
    setMode(MODE_ALARM);
  }

  switch (mode) {
    case MODE_WIFI_STATUS:
      updateWifiStatus();
      break;
    case MODE_CLOCK:
    case MODE_ALARM:
      updateClock();
      break;
    case MODE_SET_ALARM_HOURS:
      updateSetAlarm();
      break;
    case MODE_SET_ALARM_MINUTES:
      updateSetAlarm();
      break;
    case MODE_SET_ALARM_TOGGLE:
      showAlarmToggle();
      break;
    case MODE_MENU:
      if (menu.getActiveMenuItem()["id"] == "Root") {
        setMode(MODE_CLOCK);
      } else {
        char menuLabel[20];
        if (menu.getActiveMenuItem()["id"] == "Alarm") { 
          strncpy(menuLabel, displayTime.getAlarmText(0).c_str(), sizeof(menuLabel));
        } else {
          strncpy(menuLabel, menu.getActiveMenuItem()["label"], sizeof(menuLabel));
        }
        dotMatrix.showText(menuLabel);
      }
      break;
    case MODE_SET_LIGHT_SCENE:
      //dotMatrix.showText(ledStrip.getCurrentLightScene().label);
      break;
  }

  //if menu is idle, go back to clock mode
  if (mode != MODE_CLOCK && mode != MODE_ALARM && mode != MODE_WIFI_STATUS && rotaryButton.getSecondsIdle() > 15) {
    setMode(MODE_CLOCK);
  }

  if (rotaryButton.getIsButtonPressed()) {
    if (mode == MODE_CLOCK) {
      setMode(MODE_SET_ALARM_HOURS);
    } else if (mode == MODE_MENU) {
      String menuId = menu.commitMenu();
      // Serial.println(F("menuId: "));
      // Serial.println(menuId);
      processMenuCommand(menuId);
    } else if (mode == MODE_SET_ALARM_HOURS) {
      setMode(MODE_SET_ALARM_MINUTES);
    } else if (mode == MODE_SET_ALARM_MINUTES) {
      setMode(MODE_SET_ALARM_TOGGLE);
    } else if (mode == MODE_SET_ALARM_TOGGLE) {
      setMode(MODE_CLOCK);
    } else if (mode == MODE_ALARM) {
      setMode(MODE_CLOCK);
    } else if (mode == MODE_SET_LIGHT_SCENE) {
      setMode(MODE_CLOCK);
    }
    mqtt.publish("sunriseAlarm/rotaryButtonPressed", "mode: " + mode);
  }

  rotaryPosition = rotaryButton.getPosition();
  if (lastRotaryPosition != rotaryPosition) {
    //dotMatrix.showText(String(rotaryPosition));
    //mqtt.publish("sunriseAlarm/rotaryPosition", (String)rotaryPosition);
    if (mode == MODE_CLOCK) {
      setMode(MODE_MENU);
    } else if (mode == MODE_SET_ALARM_HOURS) {
      displayTime.updateAlarmHours(rotaryPosition - lastRotaryPosition, false);
    } else if (mode == MODE_SET_ALARM_MINUTES) {
      displayTime.updateAlarmMinutes(rotaryPosition - lastRotaryPosition, false);
    } else if (mode == MODE_SET_ALARM_TOGGLE) {
      displayTime.setIsAlarmOn(!displayTime.getIsAlarmOn());
      dotMatrix.setAlarmDot(displayTime.getIsAlarmOn()); 
    } else if (mode == MODE_SET_LIGHT_SCENE) {
      ledStrip.setNextOrPreviousLightScene(rotaryPosition - lastRotaryPosition);
      dotMatrix.showText((char *)ledStrip.getCurrentLightScene().label.c_str());

      lightSceneTrigger = millis() + 2000;

    } else {
      menu.rotateMenu(rotaryPosition - lastRotaryPosition);
    }
    lastRotaryPosition = rotaryPosition;
  }

  if (lightSceneTrigger > 0 && lightSceneTrigger < millis()) {
    lightSceneTrigger = 0;
    if (ledStrip.getCurrentLightScene().type == "fadeTo") {
      ledStrip.command("sunriseAlarm/fadeTo", ledStrip.getCurrentLightScene().hex + ",5");
    } else if (ledStrip.getCurrentLightScene().type == "picture") {
      mqtt.publish("sunriseAlarm/picture/get/" + ledStrip.getCurrentLightScene().label, "1");
    }
  }

}//end loop

// void colorWipe(uint32_t color, int wait) {
//   //for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
//     strip.setPixelColor(cnt % 150, color);         //  Set pixel's color (in RAM)
//     strip.show();                          //  Update strip to match
//     //delay(wait);                           //  Pause for a moment
//   //}
// }