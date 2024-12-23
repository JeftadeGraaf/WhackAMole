#ifndef SEVENSEGMENT_H
#define SEVENSEGMENT_H

#include <avr/io.h>

class SevenSegment {
public:
    SevenSegment(uint8_t address);
    void begin();
    void displayDigit(uint8_t digit);
    void clearDisplay();

private:
    // Hex digit patterns for the 7-segment display (common anode)
    const uint8_t digitPattern[11] = {
        0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8,
        0x80, 0x90
    };
    uint8_t _address;
};

#endif // SEVENSEGMENT_H