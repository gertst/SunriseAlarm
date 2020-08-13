#ifndef RotaryButton_h
#define RotaryButton_h

#include "Arduino.h"

class RotaryButton
{
  public:
    RotaryButton(uint8_t encoderAPin_, uint8_t encoderBPin_, uint8_t switchPin_);

    void setup();
    int loop();
  
  private:
    uint8_t encoderAPin;
    uint8_t encoderBPin;
    uint8_t switchPin;
    uint8_t valueA;
    uint8_t valueALast;
    int encoderPosCount;
    boolean isClockWise;
};

#endif