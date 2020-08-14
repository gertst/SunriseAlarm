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
      ntp.ruleDST("CEST", Last, Sun, Mar, 2, 120); // last sunday in march 2:00, timetone +120min (+1 GMT + 1h summertime offset)
      ntp.updateInterval(60 * 60 * 1000); //each hour
      ntp.begin();
      Serial.println("NTP init");
    }
    
    ntp.update();

    //return wifiOK && ntpOK;

}



String DisplayTime::getTime(bool showDot)
{
    
    String time = String(ntp.hours());
    if (showDot) {
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

bool DisplayTime::isRunning() {
    //Serial.println(timeClient.getHours());
    return ntp.year() > 0;
}
