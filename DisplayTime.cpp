#include "DisplayTime.h"
#include "Arduino.h"

#include <ESP8266WiFi.h> 
#include <WiFiUdp.h>
#include <NTP.h> //https://github.com/sstaub/NTP

WiFiUDP wifiUdp;
NTP ntp(wifiUdp);

bool wifiOK = false;

void pollForWifi() {
  Serial.print("wifi status:");
  Serial.println(WiFi.status());
}


void DisplayTime::setup() {
    
}

void DisplayTime::loop() {

    if (!wifiOK && WiFi.status() == WL_CONNECTED) {
      wifiOK = true;
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
        case WL_DISCONNECTED: return "Wifi disconnected";
    }
}
