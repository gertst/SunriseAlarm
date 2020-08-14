#include "DotMatix.h"
#include "Arduino.h"

#include <MD_Parola.h> // Parola library to scroll and display text on the display (needs MD_MAX72xx library)  https://github.com/MajicDesigns/MD_Parola

const int FRAME_DELAY = 20;

void DotMatrix::setup()
{
  parola.begin();
  //Serial.println("DotMatrix::setup");
  parola.displayClear();
  parola.displaySuspend(false);
  byte i = 2;                                            //EEPROM.read(0);
  parola.setIntensity(i);                               // Values from 0 to 15
  parola.setTextEffect(PA_PRINT, PA_NO_EFFECT); //in and out effect
  parola.setTextAlignment(PA_CENTER);
  parola.setSpeed(FRAME_DELAY);
  parola.setPause(FRAME_DELAY); //how long to pause the animation between the in and out text effect
}

void DotMatrix::loop()
{
  
  if (parola.displayAnimate()) // animates and returns true when an animation is completed
  {
    if (newMessageAvailable)
    {
      parola.setTextBuffer(newText.c_str());
      parola.setTextEffect(PA_PRINT, PA_NO_EFFECT); //in and out effect
      //parola.setTextEffect(PA_SCROLL_DOWN, PA_NO_EFFECT); //in and out effect
      newMessageAvailable = false;
      parola.displayReset();  
    } else {
      //parola.setTextEffect(PA_PRINT, PA_NO_EFFECT);
    }
  }
}

void DotMatrix::showText(String text) {
  newText = text;
  newMessageAvailable = true;
}

//0 to 15
void DotMatrix::setIntensity(uint8_t intensity) {
  parola.setIntensity(intensity);
}
