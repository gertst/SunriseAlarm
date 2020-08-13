#ifndef DotMatrix_h
#define DotMatrix_h

#include <MD_MAX72xx.h> 
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
  
  private:
    MD_Parola parola;
    bool newMessageAvailable = false;
    String newText;
};

#endif