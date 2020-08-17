#include "RotaryButton.h"
#include "Arduino.h"

int lastActivityTime = 0;

RotaryButton::RotaryButton(uint8_t encoderAPin_, uint8_t encoderBPin_, uint8_t switchPin_) {
    encoderAPin = encoderAPin_;
    encoderBPin = encoderBPin_;
    switchPin = switchPin_;
    
    pinMode (encoderAPin, INPUT);
    pinMode (encoderBPin, INPUT);
    pinMode (switchPin, INPUT);
    valueALast = digitalRead(encoderAPin);
    encoderPosCount = 0;
}

int RotaryButton::getPosition()
{
    valueA = digitalRead(encoderAPin);
    if (valueA != valueALast){ // Means the knob is rotating
        // if the knob is rotating, we need to determine direction
        // We do that by reading pin B.
        if (digitalRead(encoderBPin) == valueA) { // Means pin A Changed first - We're Rotating Clockwise
            encoderPosCount ++;
            isClockWise = true;
        } else {// Otherwise B changed first and we're moving CCW
            isClockWise = false;
            encoderPosCount--;
        }
    
        lastActivityTime = millis();

        Serial.print("Rotary: ");
        Serial.println(encoderPosCount);
    }
    valueALast = valueA;
    //return encoderPosCount;
    return roundf(encoderPosCount / 2); // we divide by 2 because stepper has an inbetween step that is not required
}

unsigned long RotaryButton::getSecondsIdle() {
    return long((millis() - lastActivityTime) / 1000);
}

boolean RotaryButton::getIsButtonPressed() {
    int switchValue = digitalRead(switchPin);

    //disable for 500 millis
    if (millis() - lastActivityTime > 500) {
        if (!isButtonPressed && switchValue == LOW) {
            isButtonPressed = true;
            lastActivityTime = millis();
            return true;
        } else {
            if (switchValue == HIGH) {
                isButtonPressed = false;
            }
            return false;
        }
    } else {
        return false;
    }
}
