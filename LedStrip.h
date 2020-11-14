#ifndef LedStrip_h
#define LedStrip_h

#include "Arduino.h"
#include <Adafruit_NeoPixel.h>    //https://github.com/adafruit/Adafruit_NeoPixel

struct pixelData_t {
  uint32_t startColor; //set by fadeTo() method.
  uint32_t targetColor;
  uint32_t startTime; //time in milliseconds past 1970 when color fade starts (set by fadeTo() method)
  uint32_t targetTime; //time in milliseconds past 1970 when color should be reached
};

struct LightScene {
  String label;
  String type;
  String hex;
  uint8_t delayInSeconds;
  bool loop;
};

class LedStrip
{
  public:
    LedStrip(int pin_, int nrOfPixels_)
    : strip(nrOfPixels_, pin_, NEO_GRBW + NEO_KHZ800), pin(pin_), nrOfPixels(nrOfPixels_)
    {};
    Adafruit_NeoPixel strip; //warning: strip gets already initialized here!
    void setup();
    void loop();
    void command(String topic, String msg);
    void sunrise();
    LightScene getCurrentLightScene();
    void setNextOrPreviousLightScene(int rotation);
  
  private:
    int nrOfPixels;
    int pin;
    void colorWipe(uint32_t color);
    void fadeTo(uint8_t pixelNumber, uint32_t color, uint32_t targetTime);
    pixelData_t pixelData[150];
    void updateFade();
    void updateImageEffect();
    uint8_t red(uint32_t color);
    uint8_t green(uint32_t color);
    uint8_t blue(uint32_t color);
    uint8_t white(uint32_t color);
    uint32_t nextSunriseMillis = 0;
    bool initDone = false;
    LightScene lightScenes[16] = {
      {"Off",    "fadeTo", "#00000000", 10, false},
      {"Soft",   "fadeTo", "#007C4318", 5, false},
      {"Cyan",   "fadeTo", "#002a727f", 5, false},
      {"Red",    "fadeTo", "#007c0018", 5, false},
      {"Orange", "fadeTo", "#009e5721", 5, false},
      {"White",  "fadeTo", "#77FCD795", 5, false},
      {"Banaan",  "picture", "", 3, true},
      {"Chaos",  "picture", "", 3, true},
      {"Dream",  "picture", "", 3, true},
      {"Flames",  "picture", "", 3, true},
      {"Pink",  "picture", "", 3, true},
      {"Kawaii",  "picture", "", 3, true},
      {"Leaves",  "picture", "", 3, true},
      {"Rainbow",  "picture", "", 3, true},
      {"Sea",  "picture", "", 3, true},
      {"Sunrise",  "picture", "", 20, false}
    };
    uint8_t currentScene = 0;
    int nextRow = -1;
    String picture = "";
};

#endif