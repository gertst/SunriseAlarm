#include "DotMatix.h"
#include "Arduino.h"

#include <MD_Parola.h>  // Parola library to scroll and display text on the display (needs MD_MAX72xx library)  https://github.com/MajicDesigns/MD_Parola
#include <MD_MAX72xx.h> // Library to control the Maxim MAX7219 chip on the dot matrix module   https://github.com/MajicDesigns/MD_MAX72XX

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
  // animates and returns true when an animation is completed
  // if (parola.displayAnimate()) {
  //   parola.displayReset();
  //   //parola.displaySuspend(true);
  // }

  if (parola.displayAnimate()) // animates and returns true when an animation is completed
  {
    if (newMessageAvailable)
    {
      // Serial.print("new text: ");
      // Serial.println(newText);
      //uint8_t textLength = (++textLength) % ARRAY_SIZE(_newText);
      // parola.displayText(_newText.c_str(), PA_CENTER, FRAME_DELAY, 2000, PA_SCROLL_LEFT, PA_OPENING_CURSOR);
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
  // char *stringBuffer[] = {text};
  // // uint8_t textLength = (++textLength) % ARRAY_SIZE(stringBuffer);
  // //parola.displayScroll(stringBuffer[textLength], PA_CENTER, PA_CLOSING_CURSOR, FRAME_DELAY);
  // uint8_t textLength = (++textLength) % ARRAY_SIZE(text);
  // _newText = text[textLength];
  // strcpy(*_newText, text);
  newText = text;
  newMessageAvailable = true;
  // parola.setTextBuffer(text);
  // parola.displayReset();
  //parola.displayText(text, PA_CENTER, FRAME_DELAY, 2000, PA_SCROLL_LEFT, PA_OPENING_CURSOR);
}
