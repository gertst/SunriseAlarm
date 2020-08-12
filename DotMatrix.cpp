#include "DotMatix.h"
#include "Arduino.h"

#include <MD_Parola.h>  // Parola library to scroll and display text on the display (needs MD_MAX72xx library)  https://github.com/MajicDesigns/MD_Parola
#include <MD_MAX72xx.h> // Library to control the Maxim MAX7219 chip on the dot matrix module   https://github.com/MajicDesigns/MD_MAX72XX

const int FRAME_DELAY = 25;

void DotMatrix::setup()
{
  _parola.begin();
  //Serial.println("DotMatrix::setup");
  _parola.displayClear();
  _parola.displaySuspend(false);
  byte i = 15;                                            //EEPROM.read(0);
  _parola.setIntensity(i);                               // Values from 0 to 15
  _parola.setTextEffect(PA_SCROLL_LEFT, PA_SCROLL_DOWN); //in and out effect
  //_parola.displayScroll("Hallokes ...", PA_LEFT, PA_SCROLL_LEFT, FRAME_DELAY);
  //_parola.displayText("Hallo!", PA_CENTER, FRAME_DELAY, 1000, PA_SCROLL_LEFT, PA_CLOSING_CURSOR);
}

void DotMatrix::loop()
{
  if (_parola.displayAnimate()) {
    _parola.displayReset();
  }
}

void DotMatrix::showText(char *text) {
  char *stringBuffer[] = {text};
  uint8_t textLength = (++textLength) % ARRAY_SIZE(stringBuffer);
  _parola.displayScroll(stringBuffer[textLength], PA_CENTER, PA_CLOSING_CURSOR, FRAME_DELAY);
  //_parola.displayText(text, PA_CENTER, FRAME_DELAY, 2000, PA_SCROLL_LEFT, PA_OPENING_CURSOR);
}
