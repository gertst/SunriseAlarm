#ifndef DotMatrix_h
#define DotMatrix_h

#include <MD_Parola.h>

class DotMatrix
{
  public:
    DotMatrix(uint8_t maxDevices, MD_MAX72XX::moduleType_t hardwareType, uint8_t clkPin, uint8_t dataPin, uint8_t csPin) 
    : parola(hardwareType, csPin, maxDevices)
    {};

    void setup();
    void loop();
    void showText(String Text);
    void setIntensity(uint8_t intensity);
    bool isAlarmOn = false;
  
  private:
    MD_Parola parola;
    bool newMessageAvailable = false;
    String newText;
    bool textExceedsDisplay = false;
};

#endif