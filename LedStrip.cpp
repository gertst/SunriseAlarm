// #include <stdio.h>
#include "LedStrip.h"
#include "Arduino.h"

#include <Adafruit_NeoPixel.h>    //https://github.com/adafruit/Adafruit_NeoPixel

#include "Mqtt.h"

extern Mqtt mqtt;

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
        strip.setPixelColor(i, 0);
    }
    
}

void LedStrip::loop() {
    if (!this->initDone && millis() > 5000) {
        //init to black
        for (uint8_t i = 0; i < nrOfPixels; i++) {
            strip.setPixelColor(i, 0);
        }
        this->initDone = true;
    }

    if (initDone) {
        updateFade();
        updateImageEffect();
    }
}

void LedStrip::fadeTo(uint8_t pixelNumber, uint32_t color, uint32_t targetTime) {
    int delayBetweenPixels = lightScenes[currentScene].delayInSeconds * 1;
    pixelData[pixelNumber].startColor = strip.getPixelColor(pixelNumber);
    pixelData[pixelNumber].targetColor = strip.gamma32(color);
    pixelData[pixelNumber].startTime = millis() + (delayBetweenPixels * pixelNumber); //time is dependant of pixelNumber, to make sue no pixels are set at the same time
    pixelData[pixelNumber].targetTime = targetTime + (delayBetweenPixels * pixelNumber);
}

void LedStrip::updateFade() {

    if (millis() > nextFadeMillis) {

        nextFadeMillis = millis() + 10;

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
}

void LedStrip::command(String topic, String msg) {
    if (topic == "sunriseAlarm/fadeTo") {
        //cancel sunrise effect
        this->nextSunriseMillis = 0;

        String color = "#000000";
        int delay = 8000;
        if (msg.lastIndexOf(",") > -1) {
            color = msg.substring(0, msg.lastIndexOf(","));
            String secondParam = msg.substring(msg.lastIndexOf(",") + 1);
            delay = secondParam.toInt() * 1000;
            //mqtt.publish("sunriseAlarm/debug", color + "--" + secondParam);
        } else {
            color = msg;
        }
        color.replace("#", "0x"); 
        uint32_t value = strtol(color.c_str(), NULL, 16);
        for (uint8_t i = 0; i < nrOfPixels; i++) {
            this->fadeTo(i, value, millis() + delay);   
        }
    } else if (topic.startsWith("sunriseAlarm/picture/get/")) {
        //keep the name of the picture
        this->picture = topic.substring(topic.lastIndexOf("/") + 1);
        //mqtt.publish(topic, "1");
    }  else if (topic.startsWith("sunriseAlarm/picture/return/pixelRow")) {
        
        this->nextRow = topic.substring(topic.lastIndexOf("/") + 1).toInt();

        if (this->nextRow > -1 || lightScenes[currentScene].loop) {
            this->nextSunriseMillis = millis() + (lightScenes[currentScene].delayInSeconds * 1000);
        } else {
            this->nextSunriseMillis = 0;
        }

        //msg contains: rrggbbaa,rrggbbaa, ...
        //eg: f4f1e8ff,f6f2eaff,f6f4ebff,f7f5edff,f8f6eeff,f8f6efff,...

        uint8_t cnt = 0;

        //iterate msg from mqtt
        for(uint32_t msgPos = 0; msgPos < msg.length(); msgPos+=9){
            // mqtt.publish("sunriseAlarm/picture/debug", 
            // (String)msg.substring(msgPos, msgPos + 2) + ">" + (String)strtoul(msg.substring(msgPos, msgPos + 2).c_str(), NULL, 16) + ", " +
            // (String)msg.substring(msgPos + 2, msgPos + 4) + ">" +  (String)strtoul(msg.substring(msgPos + 2, msgPos + 4).c_str(), NULL, 16) + ", " +
            // (String)msg.substring(msgPos + 4, msgPos + 6) + ">" +  (String)strtoul(msg.substring(msgPos + 4, msgPos + 6).c_str(), NULL, 16)
            // );
            fadeTo(cnt, strip.Color(
                    (uint8_t)strtoul(msg.substring(msgPos, msgPos + 2).c_str(), NULL, 16),
                    (uint8_t)strtoul(msg.substring(msgPos + 2, msgPos + 4).c_str(), NULL, 16),
                    (uint8_t)strtoul(msg.substring(msgPos + 4, msgPos + 6).c_str(), NULL, 16),
                    255 - (uint8_t)strtoul(msg.substring(msgPos + 6, msgPos + 8).c_str(), NULL, 16)
                ), 
                millis() + (lightScenes[currentScene].delayInSeconds * 950)
            );
            cnt++;
        }
       
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

void LedStrip::updateImageEffect() {
    
    //ready to get next row of pixels from MQTT/node-red?
    if (this->nextSunriseMillis > 0 && this->nextSunriseMillis < millis()) {

        if (lightScenes[currentScene].loop && this->nextRow == -1) {
            this->nextRow = 0;
        }

        mqtt.publish("sunriseAlarm/picture/get/" + this->picture, (String)this->nextRow);

        //tmp set to 0 (disable, as it will ne set with the return)
        this->nextSunriseMillis = 0;

    }
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