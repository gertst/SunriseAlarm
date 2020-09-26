#ifndef DisplayTime_h
#define DisplayTime_h

#include "Arduino.h"

class DisplayTime
{
  public:
    void setup();
    String getTime();
    void loop();
    bool getIsAlarmOn();
    void setIsAlarmOn(bool value);
    void updateAlarmHours(int rotation);
    void updateAlarmMinutes(int rotation);
    String getAlarmText(byte alarmMode);
    bool alarmGoesOff();
    void command(String topic, String msg);
    void updateTime(uint32_t millis_);
    byte seconds();
    byte minutes();
    byte hours();
    
  private:
    bool isAlarmOn = false;
    bool alarmIsTriggered = false;
    byte alarmHour = 6;
    byte alarmMinute = 50;
    int8_t lastMinutes;
    long startMillis = millis();
};

#endif