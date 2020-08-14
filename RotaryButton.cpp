#include "RotaryButton.h"
#include "Arduino.h"


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
    
        //Serial.print("Encoder Position: ");
        Serial.println(encoderPosCount);
    }
    valueALast = valueA;
    return roundf(encoderPosCount / 2); // we divide by 2 because stepper has an inbetween step that is not required
}

boolean RotaryButton::getIsButtonPressed() {
    int switchValue = digitalRead(switchPin);
    // Serial.print("sw:");
    // Serial.println(switchValue);
    if (!isButtonPressed && switchValue == LOW) {
        isButtonPressed = true;
        return true;
    } else {
        if (switchValue == HIGH) {
            isButtonPressed = false;
        }
        return false;
    }
}
