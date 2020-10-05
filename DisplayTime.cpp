#include "DisplayTime.h"
#include "Arduino.h"

#include <EEPROM.h>
#include <Time.h>
#include <TimeLib.h>
#include "Mqtt.h"

extern Mqtt mqtt;

#define EEPROM_ADDR_ALARM_ON_OFF 0
#define EEPROM_ADDR_ALARM_HOURS 1
#define EEPROM_ADDR_ALARM_MINUTES 2

void DisplayTime::setup() {
    isAlarmOn = (bool) EEPROM.read(EEPROM_ADDR_ALARM_ON_OFF);
    alarmHour = EEPROM.read(EEPROM_ADDR_ALARM_HOURS);
    alarmMinute = EEPROM.read(EEPROM_ADDR_ALARM_MINUTES);
}

void DisplayTime::updateTime(uint32_t millis_) {
 setTime(millis_);
}

byte DisplayTime::seconds() {
    return second(); 
}

byte DisplayTime::minutes() {
    return minute();
}

byte DisplayTime::hours() {
    return hour();
}

void DisplayTime::loop() {

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
    
    String time = String(hours());
    if (seconds() % 2) {
        time = time + ":" ;
    } else {
        time = time + " " ;
    }
    if (minutes() < 10) {
        time = time + "0";
    }
    time = time + String(minutes());
    if (minutes() != lastMinutes) {
        //mqtt.publish("sunriseAlarm/npt/time", time);
        lastMinutes = minutes();
    }
    return time;
}

void DisplayTime::updateAlarmHours(int rotation, bool isAbsolute) {
    if (isAbsolute) {
        alarmHour = rotation;
    } else {
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
    }
    alarmIsTriggered = false;
    EEPROM.write(EEPROM_ADDR_ALARM_HOURS, alarmHour);
    EEPROM.commit();
    mqtt.publish("sunriseAlarm/alarmHour", (String)alarmHour, true);
}

void DisplayTime::updateAlarmMinutes(int rotation, bool isAbsolute) {
    if (isAbsolute) {
        alarmMinute = rotation;
    } else {
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
    }
    alarmIsTriggered = false;
    EEPROM.write(EEPROM_ADDR_ALARM_MINUTES, alarmMinute);
    EEPROM.commit();
    mqtt.publish("sunriseAlarm/alarmMinute", (String)alarmMinute, true);
}

bool DisplayTime::alarmGoesOff() {
    
    if (isAlarmOn) {

        if (!alarmIsTriggered && alarmHour == hours() && alarmMinute == minutes() ) {
            alarmIsTriggered = true;
            //reset alarm once the exact alarm minute has passed
            if (alarmMinute != minutes()) {
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

    // if (alarmMode == 0 || 
    //     return time;
    // } else {
    //     return "--:--";
    // }

    return time;
}

void DisplayTime::command(String topic, String msg) {
    if (topic == "sunriseAlarm/alarmHour/set") {
        this->updateAlarmHours(msg.toInt(), true);
    } else if (topic == "sunriseAlarm/alarmMinute/set") {
        this->updateAlarmMinutes(msg.toInt(), true);
    } else if (topic == "sunriseAlarm/updateTime") {
        this->updateTime(msg.toInt());
    } 
}


