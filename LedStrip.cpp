#include "LedStrip.h"
#include "Arduino.h"

#include <Adafruit_NeoPixel.h>    //https://github.com/adafruit/Adafruit_NeoPixel

LedStrip::LedStrip(int pin_, int nrOfPixels_) {
    nrOfPixels = nrOfPixels_;
    pin = pin_;
    Adafruit_NeoPixel strip(nrOfPixels, pin, NEO_GRBW + NEO_KHZ800);
}

void LedStrip::setup() {
    // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
    // Any other board, you can remove this part (but no harm leaving it):
    #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
        clock_prescale_set(clock_div_1);
    #endif
    // END of Trinket-specific code.

    strip.begin(); 
    strip.show();            // Turn OFF all pixels ASAP
    //strip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)
    Serial.print("strip.numPixels(: )");
    Serial.println(strip.numPixels());
    
}

void LedStrip::loop() {
    colorWipe(strip.Color(255,   0,   0));
}

void LedStrip::colorWipe(uint32_t color) {
    cnt ++;
    if (cnt >= nrOfPixels) {
        cnt = 0;
    } 

    strip.setPixelColor(cnt, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
}

int LedStrip::GetNrOfPixels() {
    return strip.numPixels();
}