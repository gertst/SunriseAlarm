#ifndef RotaryButton_h
#define RotaryButton_h

#include "Arduino.h"

class RotaryButton
{
  public:
    RotaryButton(uint8_t encoderAPin, uint8_t encoderBPin, uint8_t switchPin);

    void setup();
    int loop();
  
  private:
    uint8_t _encoderAPin;
    uint8_t _encoderBPin;
    uint8_t _switchPin;
    uint8_t _valueA;
    uint8_t _valueALast;
    int _encoderPosCount;
    boolean _isClockWise;
};

#endif