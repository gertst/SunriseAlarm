#ifndef DotMatrix_h
#define DotMatrix_h

#include <MD_MAX72xx.h> 
#include <MD_Parola.h> 

class DotMatrix
{
  public:
    DotMatrix(uint8_t maxDevices, MD_MAX72XX::moduleType_t hardwareType, uint8_t clkPin, uint8_t dataPin, uint8_t csPin) 
    : _parola(hardwareType, csPin, maxDevices)
    {};

    void setup();
    void loop();
    void showText(char *text);
  
  private:
    MD_Parola _parola;
};

#endif