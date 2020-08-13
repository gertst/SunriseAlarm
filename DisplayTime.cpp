#include "DisplayTime.h"
#include "Arduino.h"

#include <WiFiUdp.h>
#include <NTPClient.h>

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, 12 * 60 * 60 * 1000); //update each 12 hours

void DisplayTime::setup()
{
    //timeClient(ntpUDP);
    timeClient.setTimeOffset(2 * 3600); // add 2 hrs
    timeClient.begin();
}

String DisplayTime::getTime()
{
    timeClient.update();
    String time = String(timeClient.getHours());
    time = time + ":" ;
    if (timeClient.getMinutes() < 10) {
        time = time + "0";
    }
    time = time + String(timeClient.getMinutes());
    return time;
}

bool DisplayTime::isRunning() {
    //Serial.println(timeClient.getHours());
    return timeClient.getHours() > 0;
}
