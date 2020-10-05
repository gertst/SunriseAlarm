// #include <stdio.h>
#include "LedStrip.h"
#include "Arduino.h"

#include <Adafruit_NeoPixel.h>    //https://github.com/adafruit/Adafruit_NeoPixel

#include "Mqtt.h"

extern Mqtt mqtt;
std::vector<uint8_t> sunrise_map;

void LedStrip::setup() {
    // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
    // Any other board, you can remove this part (but no harm leaving it):
    #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
        clock_prescale_set(clock_div_1);
    #endif
    strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
    strip.show();            // Turn OFF all pixels ASAP
    strip.setBrightness(255); // Set BRIGHTNESS to max to have best resolution

    randomSeed(millis()); //init random 

    //init array
    for (uint8_t i = 0; i < nrOfPixels; i++) {
        pixelData[i].startColor = 0; 
        pixelData[i].targetColor = 0; 
        pixelData[i].startTime = 0;
        pixelData[i].targetTime = 0;
    }
    
}

void LedStrip::loop() {
    if (!this->initDone) {
        //init to black
        for (uint8_t i = 0; i < nrOfPixels; i++) {
            strip.setPixelColor(i, 0);
        }
        this->initDone = true;
    }
    updateFade();
    updateSunrise();
}

void LedStrip::fadeTo(uint8_t pixelNumber, uint32_t color, uint32_t targetTime) {
    
    pixelData[pixelNumber].startColor = strip.getPixelColor(pixelNumber);
    pixelData[pixelNumber].targetColor = strip.gamma32(color);
    pixelData[pixelNumber].startTime = millis() + (100 * pixelNumber);
    pixelData[pixelNumber].targetTime = targetTime + (100 * pixelNumber);
}

void LedStrip::updateFade() {
    bool isChanged = false;
    uint8_t redValue;
    uint8_t greenValue;
    uint8_t blueValue;
    uint8_t whiteValue;
    
    for (uint8_t i = 0; i < nrOfPixels; i++) {
        //time is passed: set color to targetColor
        if (pixelData[i].targetTime <= millis()) {
            if (strip.getPixelColor(i) != pixelData[i].targetColor) {
                strip.setPixelColor(i, pixelData[i].targetColor);
                isChanged = true;
            }
        } else {
            //startTime passed? No
            if (pixelData[i].startTime <= millis()) {
                //map syntax: map(value, fromLow, fromHigh, toLow, toHigh)
                redValue = map(millis(),pixelData[i].startTime, pixelData[i].targetTime, red(pixelData[i].startColor), red(pixelData[i].targetColor));
                greenValue = map(millis(),pixelData[i].startTime, pixelData[i].targetTime, green(pixelData[i].startColor), green(pixelData[i].targetColor));
                blueValue = map(millis(),pixelData[i].startTime, pixelData[i].targetTime, blue(pixelData[i].startColor), blue(pixelData[i].targetColor));
                whiteValue = map(millis(),pixelData[i].startTime, pixelData[i].targetTime, white(pixelData[i].startColor), white(pixelData[i].targetColor));
                uint32_t newColor = strip.Color(redValue, greenValue, blueValue, whiteValue);
                if (strip.getPixelColor(i) != newColor) {
                    strip.setPixelColor(i, newColor);
                    isChanged = true;
                }
            }
        }
    }
    if (isChanged) {
        strip.show();
    }
}

void LedStrip::command(String topic, String msg) {
    if (topic == "sunriseAlarm/fadeTo") {
        //cancel sunrise effect
        this->nextRow = -1;
        msg.replace("#", "0x"); 
        uint32_t value = strtol(msg.c_str(), NULL, 16);
        for (uint8_t i = 0; i < nrOfPixels; i++) {
            //middle leds: keep black to keep leds below the clock to black
            if (i >= 71 && i <= 79) {
                this->fadeTo(i, 0, millis()+ 8000);   
            } else {
                this->fadeTo(i, value, millis()+ 8000);   
            }
        }
    } else if (topic == "sunriseAlarm/sunrise") {
        this->sunrise();
    } else if (topic.startsWith("sunriseAlarm/picture/return/pixelRow")) {
        
        this->nextRow = topic.substring(topic.lastIndexOf("/")).toInt();

        //delete all earlier elements first
        sunrise_map.erase(sunrise_map.begin(), sunrise_map.end());  
        int j=0;
        for(int i =0; i < msg.length(); i++){
            if(msg.charAt(i) == ','){
                sunrise_map.push_back(msg.substring(j,i).toInt());
                j = i+1;
            }
        }
        sunrise_map.push_back(msg.substring(j,msg.length()).toInt()); //to grab the last value of the string
        
    }
    
    
}

// Returns the White component of a 32-bit color
uint8_t LedStrip::white(uint32_t color) {
    return (color >> 24) & 0xFF;
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

void LedStrip::updateSunrise() {
    //iterate the sunrise_map array 
    if (sizeof(sunrise_map) > 0  && this->nextSunriseMillis < millis()) {
        this->nextSunriseMillis = millis() + 30000;
        //el 1 = red   pixel 1
        //el 2 = green pixel 1
        //el 3 = blue  pixel 1
        //el 4 = alpha pixel 1 (used as white)
        //el 1 = red   pixel 2
        //el 2 = green pixel 2
        //el 3 = blue  pixel 2
        //el 4 = alpha pixel 2 (used as white)
        // ...
        for (uint8_t i = 0; i < nrOfPixels; i+=4) {
            fadeTo(i, strip.Color(
                    sunrise_map[i + 0], 
                    sunrise_map[i + 1], 
                    sunrise_map[i + 2], 
                    sunrise_map[i + 3]
                ), 
                this->nextSunriseMillis
            );
        }
        //delete all elements of array
        sunrise_map.erase(sunrise_map.begin(), sunrise_map.end());  

        //get next row, if existing
        if (this->nextRow > -1) {
            mqtt.publish("sunriseAlarm/picture/get/sunriseForRow", (String)this->nextRow);
        }
    }
}

void LedStrip::sunrise() {
    //get the first 150 pixels from MQTT
    mqtt.publish("sunriseAlarm/picture/get/sunriseForRow", "0");
}

LightScene LedStrip::getCurrentLightScene() {
    return lightScenes[currentScene];
}

void LedStrip::setNextOrPreviousLightScene(int rotation) {
    uint8_t size = sizeof(lightScenes)/sizeof(lightScenes[0]);
    if (rotation > 0) {
        currentScene++;
        if (currentScene >= size) {
            currentScene = 0;
        }
    } else {
        if (currentScene == 0) {
            currentScene = size - 1;
        } else {
            currentScene--;
        }
    }
}