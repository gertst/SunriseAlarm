#include "DotMatix.h"
#include "Arduino.h"

#include <MD_Parola.h>  // Parola library to scroll and display text on the display (needs MD_MAX72xx library)  https://github.com/MajicDesigns/MD_Parola
#include <MD_MAX72xx.h> // Library to control the Maxim MAX7219 chip on the dot matrix module   https://github.com/MajicDesigns/MD_MAX72XX

const int FRAME_DELAY = 20;

void DotMatrix::setup()
{
  _parola.begin();
  //Serial.println("DotMatrix::setup");
  _parola.displayClear();
  _parola.displaySuspend(false);
  byte i = 0;                                            //EEPROM.read(0);
  _parola.setIntensity(i);                               // Values from 0 to 15
  _parola.setTextEffect(PA_PRINT, PA_NO_EFFECT); //in and out effect
  _parola.setTextAlignment(PA_CENTER);
  _parola.setSpeed(FRAME_DELAY);
  _parola.setPause(FRAME_DELAY); //how long to pause the animation between the in and out text effect
}

void DotMatrix::loop()
{
  // animates and returns true when an animation is completed
  // if (_parola.displayAnimate()) {
  //   _parola.displayReset();
  //   //_parola.displaySuspend(true);
  // }

  if (_parola.displayAnimate()) // animates and returns true when an animation is completed
  {
    if (_newMessageAvailable)
    {
      Serial.print("new text: ");
      Serial.println(_newText);
      //uint8_t textLength = (++textLength) % ARRAY_SIZE(_newText);
      // _parola.displayText(_newText.c_str(), PA_CENTER, FRAME_DELAY, 2000, PA_SCROLL_LEFT, PA_OPENING_CURSOR);
      _parola.setTextBuffer(_newText.c_str());
      _parola.setTextEffect(PA_SCROLL_DOWN, PA_NO_EFFECT); //in and out effect
      _newMessageAvailable = false;
      _parola.displayReset();  
    } else {
      _parola.setTextEffect(PA_PRINT, PA_NO_EFFECT);
    }
  }



}

void DotMatrix::showText(String text) {
  // char *stringBuffer[] = {text};
  // // uint8_t textLength = (++textLength) % ARRAY_SIZE(stringBuffer);
  // //_parola.displayScroll(stringBuffer[textLength], PA_CENTER, PA_CLOSING_CURSOR, FRAME_DELAY);
  // uint8_t textLength = (++textLength) % ARRAY_SIZE(text);
  // _newText = text[textLength];
  // strcpy(*_newText, text);
  _newText = text;
  _newMessageAvailable = true;
  // _parola.setTextBuffer(text);
  // _parola.displayReset();
  //_parola.displayText(text, PA_CENTER, FRAME_DELAY, 2000, PA_SCROLL_LEFT, PA_OPENING_CURSOR);
}
