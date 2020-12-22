#ifndef DisplayTime_h
#define DisplayTime_h

#include "Arduino.h"

class DisplayTime
{
  public:
    void setup();
    char *getTime(bool alarmIsOn);
    void loop();
    bool getIsAlarmOn();
    void setIsAlarmOn(bool value);
    void updateAlarmHours(int rotation, bool isAbsolute);
    void updateAlarmMinutes(int rotation, bool isAbsolute);
    String getAlarmText(byte alarmMode);
    bool alarmGoesOff();
    void command(String topic, String msg);
    void updateTime(uint32_t millis_);
    byte seconds();
    byte minutes();
    byte hours();
    bool alarmIsTriggered = false;
    
  private:
    bool isAlarmOn = false;
    byte alarmHour = 6;
    byte alarmMinute = 50;
    int8_t lastMinutes;
    long startMillis = millis();
};

#endif