#include <SevenSegment.h>
#include <Wire.h>
#include <Arduino.h>

SevenSegment::SevenSegment(uint8_t address)
    : _address(address) {}

void SevenSegment::begin()
{
    Wire.begin();
}

void SevenSegment::displayDigit(uint8_t digit)
{
    if (digit > 10)
    {
        return; // Invalid digit, do nothing
    }
    Wire.beginTransmission(_address);
    Wire.write(digitPattern[digit]); // Send hex digit pattern to the display
    Wire.endTransmission();
}