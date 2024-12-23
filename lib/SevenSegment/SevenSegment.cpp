#include <SevenSegment.h>
#include <Wire.h>
#include <avr/io.h>

// Define the digit patterns for the seven-segment display
const uint8_t digitPattern[11] = {
    0b00111111, // 0
    0b00000110, // 1
    0b01011011, // 2
    0b01001111, // 3
    0b01100110, // 4
    0b01101101, // 5
    0b01111101, // 6
    0b00000111, // 7
    0b01111111, // 8
    0b01101111  // 9
};

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