#include "LedStrip.h"
#include "Arduino.h"

#include <Adafruit_NeoPixel.h>    //https://github.com/adafruit/Adafruit_NeoPixel

void LedStrip::setup() {
    // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
    // Any other board, you can remove this part (but no harm leaving it):
    #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
        clock_prescale_set(clock_div_1);
    #endif
    strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
    strip.show();            // Turn OFF all pixels ASAP
    strip.setBrightness(255); // Set BRIGHTNESS to about 1/5 (max = 255)

    randomSeed(millis()); //init random 

    //init array
    for (uint8_t i = 0; i < nrOfPixels; i++) {
        pixelData[i].startColor = 0; 
        pixelData[i].targetColor = 0; 
        pixelData[i].startTime = 0;
        pixelData[i].targetTime = 0;
    }

    //test fade
    for (uint8_t i = 0; i < nrOfPixels; i++) {
        fadeTo(i, strip.Color(0,0,0,1), millis() + (10 * 1000));
    }
    
}

void LedStrip::loop() {
    //no current effect: start a new one after 2 secs
    // if (pixelData[0].targetTime + 2000 < millis()) {
    //     uint8_t red = random(0, 155);
    //     uint8_t green = random(0, 155);
    //     uint8_t blue = random(0, 155);
    //     uint8_t white = 0; //random(0, 155);
        
    //     for (uint8_t i = 0; i < nrOfPixels; i++) {
    //         fadeTo(i, strip.Color(red, green, blue, white), millis() + (5 * 1000));
    //     }
    // }
    updateFade();
}

void LedStrip::fadeTo(uint8_t pixelNumber, uint32_t color, uint32_t targetTime) {
    pixelData[pixelNumber].startColor = strip.getPixelColor(pixelNumber);
    pixelData[pixelNumber].targetColor = color;
    pixelData[pixelNumber].startTime = millis();
    pixelData[pixelNumber].targetTime = targetTime;
}

void LedStrip::updateFade() {
    for (uint8_t i = 0; i < nrOfPixels; i++) {
        //time is passed: set color to targetColor
        if (pixelData[i].targetTime <= millis()) {
            strip.setPixelColor(i, pixelData[i].targetColor);
        } else {
            //map syntax: map(value, fromLow, fromHigh, toLow, toHigh)
            uint8_t redValue = map(millis(),pixelData[i].startTime, pixelData[i].targetTime, red(pixelData[i].startColor), red(pixelData[i].targetColor));
            uint8_t greenValue = map(millis(),pixelData[i].startTime, pixelData[i].targetTime, green(pixelData[i].startColor), green(pixelData[i].targetColor));
            uint8_t blueValue = map(millis(),pixelData[i].startTime, pixelData[i].targetTime, blue(pixelData[i].startColor), blue(pixelData[i].targetColor));
            uint8_t whiteValue = map(millis(),pixelData[i].startTime, pixelData[i].targetTime, white(pixelData[i].startColor), white(pixelData[i].targetColor));
            strip.setPixelColor(i, strip.Color(redValue, greenValue, blueValue, whiteValue));
        }
    }
    strip.show();
}

// Returns the Red component of a 32-bit color
uint8_t LedStrip::red(uint32_t color){
    return (color >> 16) & 0xFF;
}

// Returns the Green component of a 32-bit color
uint8_t LedStrip::green(uint32_t color) {
    return (color >> 8) & 0xFF;
}

// Returns the Blue component of a 32-bit color
uint8_t LedStrip::blue(uint32_t color) {
    return color & 0xFF;
}

// Returns the White component of a 32-bit color
uint8_t LedStrip::white(uint32_t color) {
    return (color >> 32) & 0xFF;
}