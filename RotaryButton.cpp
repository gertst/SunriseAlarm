#include "RotaryButton.h"
#include "Arduino.h"

RotaryButton::RotaryButton(uint8_t encoderAPin, uint8_t encoderBPin, uint8_t switchPin) {
    _encoderAPin = encoderAPin;
    _encoderBPin = encoderBPin;
    _switchPin = switchPin;
    pinMode (_encoderAPin, INPUT);
    pinMode (_encoderBPin, INPUT);
    _valueALast = digitalRead(_encoderAPin);
    _encoderPosCount = 0;
    // Serial.println("RotaryButton instanciated");
    // Serial.print("RotaryButton _valueALast");
    // Serial.println(_valueALast);
}

int RotaryButton::loop()
{
    _valueA = digitalRead(_encoderAPin);
    if (_valueA != _valueALast){ // Means the knob is rotating
        // if the knob is rotating, we need to determine direction
        // We do that by reading pin B.
        if (digitalRead(_encoderBPin) == _valueA) { // Means pin A Changed first - We're Rotating Clockwise
            _encoderPosCount ++;
            _isClockWise = true;
        } else {// Otherwise B changed first and we're moving CCW
            _isClockWise = false;
            _encoderPosCount--;
        }
    
        Serial.print("Encoder Position: ");
        Serial.println(_encoderPosCount);
    }
    _valueALast = _valueA;
    return _encoderPosCount;
}
