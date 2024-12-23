#include <SevenSegment.h>
#include <Wire.h>
#include <avr/io.h>

SevenSegment::SevenSegment(uint8_t address)
    : _address(address) {}

void SevenSegment::begin() {
    Wire.begin();
}

void SevenSegment::displayDigit(uint8_t digit) {
    if (digit > 9) {
        return; // Invalid digit, do nothing
    }
    Wire.beginTransmission(_address);
    Wire.write(digitPattern[digit]); // Send hex digit pattern to the display
    Wire.endTransmission();
}

void SevenSegment::clearDisplay() {
    Wire.beginTransmission(_address);
    Wire.write(0xFF); // Send 0 to clear the display
    Wire.endTransmission();
}