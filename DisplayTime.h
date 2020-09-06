#ifndef DisplayTime_h
#define DisplayTime_h

#include "Arduino.h"

class DisplayTime
{
  public:
    void setup();
    String getTime();
    String getWifiStatus();
    void loop();
    bool getIsAlarmOn();
    void setIsAlarmOn(bool value);
    void updateAlarmHours(int rotation);
    void updateAlarmMinutes(int rotation);
    String getAlarmText(byte alarmMode);
    bool alarmGoesOff();
    void command(String topic, String msg);
  
  private:
    bool isAlarmOn = false;
    bool alarmIsTriggered = false;
    byte alarmHour = 6;
    byte alarmMinute = 50;
};

#endif