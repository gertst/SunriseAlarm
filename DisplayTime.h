#ifndef DisplayTime_h
#define DisplayTime_h

#include "Arduino.h"
#include <NTPClient.h>
#include <WiFiUdp.h>

class DisplayTime
{
  public:
    DisplayTime(){};

    void setup();
    String getTime();
    bool isRunning();
  
  private:
    // WiFiUDP ntpUDP;
    // NTPClient timeClient = (WiFiUDP){};
};

#endif