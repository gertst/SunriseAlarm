#include "DotMatix.h"
#include "Arduino.h"

#include <MD_Parola.h> // Parola library to scroll and display text on the display (needs MD_MAX72xx library)  https://github.com/MajicDesigns/MD_Parola



const int FRAME_DELAY = 25;

//to add/replace characters for the font: https://pjrp.github.io/MDParolaFontEditor

uint8_t dig0[] = {  5, 62, 127, 65, 127, 62 }; 	// 32 - '0'
uint8_t dig1[] = {	5, 4, 66, 127, 127, 64 }; 	// 33 - '1'
uint8_t dig2[] = {	5, 113, 121, 73, 79, 70 }; 	// 34 - '2'
uint8_t dig3[] = {	5, 65, 73, 73, 127, 54 }; 	// 35 - '3'
uint8_t dig4[] = {	5, 15, 15, 8, 127, 127 }; 	// 36 - '4'
uint8_t dig5[] = {	5, 47, 79, 73, 121, 49 }; 	// 37 - '5'
uint8_t dig6[] = {	5, 62, 127, 73, 121, 50 }; 	// 38 - '6'
uint8_t dig7[] = {	5, 1, 1, 121, 125, 3 }; 	// 39 - european style '7'
uint8_t dig8[] = {	5, 54, 127, 73, 127, 54 }; 	// 40 - '8'
uint8_t dig9[] = {	5, 6, 79, 73, 127, 62 }; 	// 41 - '9'

uint8_t digitSpace[] = { 5, 0, 0, 0, 0, 0 }; 
uint8_t pipe[] = {2, 56, 56}; 	// 124Pipe


void DotMatrix::setup()
{
  parola.begin();
  parola.addChar('0', dig0);
  parola.addChar('1', dig1);
  parola.addChar('2', dig2);
  parola.addChar('3', dig3);
  parola.addChar('4', dig4);
  parola.addChar('5', dig5);
  parola.addChar('6', dig6);
  parola.addChar('7', dig7);
  parola.addChar('8', dig8);
  parola.addChar('9', dig9);
  parola.addChar('$', digitSpace);
  parola.addChar('|', pipe);

  parola.displayClear();
  parola.displaySuspend(false);
  parola.setTextEffect(PA_PRINT, PA_NO_EFFECT); //in and out effect
  parola.setTextAlignment(PA_CENTER);
  parola.setSpeed(FRAME_DELAY);
  parola.setScrollSpacing(10);
  parola.setPause(FRAME_DELAY); //how long to pause the animation between the in and out text effect

}

void DotMatrix::loop()
{
  if (parola.displayAnimate()) // animates and returns true when an animation is completed
  {
    if (newMessageAvailable) {
      refreshDot = true;
      parola.setTextBuffer(newText.c_str());
      //getTextColumns is a new method
      textExceedsDisplay = parola.getTextColumns(newText.c_str()) > 32;
      
      if (textExceedsDisplay) {
        parola.setTextEffect(PA_SCROLL_LEFT, PA_SCROLL_LEFT); //in and out effect
        parola.setTextAlignment(PA_LEFT);
        //parola.displayScroll(newText.c_str(), PA_LEFT, PA_SCROLL_LEFT, FRAME_DELAY);
      } else {
        parola.setTextEffect(PA_PRINT, PA_NO_EFFECT); //in and out effect
        //parola.displayText(newText.c_str(), PA_CENTER, FRAME_DELAY, 100, PA_PRINT, PA_NO_EFFECT);
        parola.setTextAlignment(PA_CENTER);
      }

      //parola.setIntensity(intensity);
      parola.displayReset();  
      newMessageAvailable = false;
    } else {
      if (textExceedsDisplay) {
        //keep on scrolling if prev scroll animation is finished
        parola.displayReset();  
      }
    } 
    
  }
  
  //alarm dot
  if (!textExceedsDisplay && refreshDot) {
    parola.getGraphicObject()->setPoint(0, 0, isAlarmOn);
    parola.getGraphicObject()->setPoint(1, 0, isAlarmOn);
    parola.getGraphicObject()->setPoint(1, 1, isAlarmOn);
    parola.getGraphicObject()->setPoint(0, 1, isAlarmOn);
  }

  //underline?
  if (!textExceedsDisplay && hoursUnderlined) {
    if (((long)roundf(millis()/100) % 20 > 10)) {
      for (uint8_t i = 0; i < 11; i++) {
        parola.getGraphicObject()->setPoint(7, 18 + i, 1);
      }
    } else {
      for (uint8_t i = 0; i < 12; i++) {
        parola.getGraphicObject()->setPoint(7, 18 + i, 0);
      }
    } 
  } 
  if (!textExceedsDisplay && minutesUnderlined) {
    if (((long)roundf(millis()/100) % 20 > 10)) {
      for (uint8_t i = 0; i < 11; i++) {
        parola.getGraphicObject()->setPoint(7, 3 + i, 1);
      }
    } else {
      for (uint8_t i = 0; i < 12; i++) {
        parola.getGraphicObject()->setPoint(7, 3 + i, 0);
      }
    } 
  } 
  //parola.setIntensity(intensity);
  parola.getGraphicObject()->control(MD_MAX72XX::INTENSITY, intensity);
}

void DotMatrix::showText(String text) {
  if (newText != text) {
    newText = text;
    newMessageAvailable = true;
  }
}

//0 to 15
void DotMatrix::setIntensity(uint8_t intensity_) {
  intensity = intensity_;
}

void DotMatrix::underlineHours(bool value) {
    hoursUnderlined = value;
}

void DotMatrix::underlineMinutes(bool value) {
    minutesUnderlined = value;
}

void DotMatrix::setAlarmDot(bool value) {
  this->isAlarmOn = value;
  this->refreshDot = true;
}

void DotMatrix::command(String topic, String msg) {
    //if (topic == "alarm") {
    //    this->setAlarmDot(msg.toInt());
    //}
}
