#ifndef LedStrip_h
#define LedStrip_h

#include "Arduino.h"
#include <Adafruit_NeoPixel.h>    //https://github.com/adafruit/Adafruit_NeoPixel

class LedStrip
{
  public:
    LedStrip(int pin_, int nrOfPixels_);

    void setup();
    void loop();
    int GetNrOfPixels();
  
  private:
    int nrOfPixels;
    int pin;
    Adafruit_NeoPixel strip;
    int cnt = 0;
    void colorWipe(uint32_t color);
};

#endif