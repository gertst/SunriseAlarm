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
    void showText(char *text);
    void setIntensity(uint8_t intensity_);
    void underlineHours(bool value);
    void underlineMinutes(bool value);
    void setAlarmDot(bool value);
    void command(String topic, String msg);
  
  private:
    MD_Parola parola;
    bool newMessageAvailable = false;
    char newText[20];
    bool textExceedsDisplay = false;
    bool hoursUnderlined = false;
    bool minutesUnderlined = false;
    bool isAlarmOn = false;
    bool refreshDot = true;
    uint8_t intensity = 0;
};

#endif