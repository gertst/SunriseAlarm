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
      parola.displayReset();  
      parola.setTextBuffer(newText.c_str());
      //textLengthInPixels = parola.getTextColumns(newText.c_str());
      
      if (textLengthInPixels > 32) {
        parola.setTextEffect(PA_SCROLL_LEFT, PA_SCROLL_LEFT); //in and out effect
      } else {
        parola.setTextEffect(PA_PRINT, PA_NO_EFFECT); //in and out effect
      }
      newMessageAvailable = false;
    }
  }
}

void DotMatrix::showText(String text) {
  if (newText != text) {
    newText = text;
    newMessageAvailable = true;
  }
}

//0 to 15
void DotMatrix::setIntensity(uint8_t intensity) {
  parola.setIntensity(intensity);
}
