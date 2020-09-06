#include "DisplayTime.h"
#include "Arduino.h"

#include <ESP8266WiFi.h> 
#include <WiFiUdp.h>
#include <NTP.h> //https://github.com/sstaub/NTP
#include <EEPROM.h>

#define EEPROM_ADDR_ALARM_ON_OFF 0
#define EEPROM_ADDR_ALARM_HOURS 1
#define EEPROM_ADDR_ALARM_MINUTES 2

WiFiUDP wifiUdp;
NTP ntp(wifiUdp);

bool wifiOK = false;

void pollForWifi() {
  Serial.print("wifi status:");
  Serial.println(WiFi.status());
}


void DisplayTime::setup() {
    isAlarmOn = (bool) EEPROM.read(EEPROM_ADDR_ALARM_ON_OFF);
    alarmHour = EEPROM.read(EEPROM_ADDR_ALARM_HOURS);
    alarmMinute = EEPROM.read(EEPROM_ADDR_ALARM_MINUTES);
}

void DisplayTime::loop() {

    if (!wifiOK && WiFi.status() == WL_CONNECTED) {
        wifiOK = true;

        Serial.print("Connected to ");
        Serial.println(WiFi.SSID());     
        Serial.print("IP address:\t");
        Serial.println(WiFi.localIP());

        //define the start of summertime for Central Europe
        ntp.ruleDST("CEST", Last, Sun, Mar, 2, 2 * 60); // last sunday in march 2:00, timetone +120min (+1 GMT + 1h summertime offset)
        //define the standard time for Central Europe
        ntp.ruleSTD("CET", Last, Sun, Oct, 3, 60); // last sunday in october 3:00, timezone +60min (+1 GMT)
        ntp.updateInterval(60 * 60 * 1000); //each hour
        ntp.begin();
        Serial.println("NTP init");
    } 
    
    ntp.update();

    //return wifiOK && ntpOK;

}

bool DisplayTime::getIsAlarmOn() {
    return isAlarmOn;
}

void DisplayTime::setIsAlarmOn(bool value) {
    isAlarmOn = value;
    EEPROM.write((byte)EEPROM_ADDR_ALARM_ON_OFF, isAlarmOn);
    EEPROM.commit();
}

String DisplayTime::getTime()
{
    
    String time = String(ntp.hours());
    if (ntp.seconds() % 2) {
        time = time + ":" ;
    } else {
        time = time + " " ;
    }
    if (ntp.minutes() < 10) {
        time = time + "0";
    }
    time = time + String(ntp.minutes());
    return time;
}

String DisplayTime::getWifiStatus() {
    //Serial.println(ntp.year());
    switch (WiFi.status()) {
        case WL_IDLE_STATUS: return "Wifi idle";
        case WL_NO_SSID_AVAIL: return "No SSID available";
        case WL_CONNECTED: return "OK"; //don't change, used for checking when ready
        case WL_CONNECT_FAILED: return "Wifi connection failed";
        case WL_DISCONNECTED: return "Wifi ...";
    }
}

void DisplayTime::updateAlarmHours(int rotation) {
    if (rotation > 0) {
        alarmHour ++;
        if (alarmHour > 23) {
            alarmHour = 0;
        }
    } else {
        if (alarmHour == 0) {
            alarmHour = 23;
        } else {
            alarmHour --;
        }
    }
    alarmIsTriggered = false;
    EEPROM.write(EEPROM_ADDR_ALARM_HOURS, alarmHour);
    EEPROM.commit();
}

void DisplayTime::updateAlarmMinutes(int rotation) {
    if (rotation > 0) {
        alarmMinute += 5;
        if (alarmMinute > 55) {
            alarmMinute = 0;
        }
    } else {
        if (alarmMinute <= 0) {
            alarmMinute = 55;
        } else {
            alarmMinute -= 5;
        }
    }
    alarmIsTriggered = false;
    EEPROM.write(EEPROM_ADDR_ALARM_MINUTES, alarmMinute);
    EEPROM.commit();
}

bool DisplayTime::alarmGoesOff() {
    
    if (isAlarmOn) {

        if (!alarmIsTriggered && alarmHour == ntp.hours() && alarmMinute == ntp.minutes() ) {
            alarmIsTriggered = true;
            //reset alarm once the exact alarm minute has passed
            if (alarmMinute != ntp.minutes()) {
                alarmIsTriggered = false;
                return false;
            } else {
                return true;
            }
        } else {
            return false;
        }
        
    } else {
        return false;
    }
    
}

/**
 * Text that should be shown when alarm is set
 * @param alarmMode: 
 *   0 = alarm hours + minutes
 *   1 = alarm hours blinking
 *   2 = alarm minutes blinking
 */ 
String DisplayTime::getAlarmText(byte alarmMode) {
    String time;
    if (alarmHour < 10) {
        time = time + "$"; //add space with size of a digit (the & sign is replaced with a space the size of a digit)
    }
    time = time + String(alarmHour) + ":";
    if (alarmMinute < 10) {
        time = time + "0";
    }
    time = time + String(alarmMinute);

    // if (alarmMode == 0 || ntp.seconds() % 2) {
    //     return time;
    // } else {
    //     return "--:--";
    // }

    return time;
}

void DisplayTime::command(String topic, String msg) {
    // if (topic == "alarmHour") {
    //     this->updateAlarmHours(msg.toInt());
    // } else if (topic == "alarmMinute") {
    //     this->updateAlarmMinutes(msg.toInt());
    // } else if (topic == "alarm") {
    //     this->setIsAlarmOn(msg.toInt());
    // }
}


