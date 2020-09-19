#ifndef DFPlayer_h
#define DFPlayer_h

#include "Arduino.h"
#include <SoftwareSerial.h>

//https://github.com/PowerBroker2/DFPlayerMini_Fast
//https://forum.arduino.cc/index.php?topic=513128.15
#include <DFPlayerMini_Fast.h>

class DFPlayer
{
  public:
    DFPlayer(uint8_t rxPin_, uint8_t txPin_) 
    : rxPin(rxPin_), txPin(txPin_), softwareSerial(rxPin_, txPin_)
    {}
    void setup();
    void loop();
    void command(String topic, String msg);
  
  private:
    uint8_t rxPin;
    uint8_t txPin;
    SoftwareSerial softwareSerial;
    DFPlayerMini_Fast dfPlayer;
    bool isStarted = false;
    uint8_t volumeTarget = 0;
    void increaseVolume();
    uint32_t timerTrigger = 0;
    uint8_t volume = 0;
    
};

#endif