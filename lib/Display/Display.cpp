#include "Display.h"
#include <Adafruit_ILI9341.h>


// Initialize the display

Display::Display(int backlight_pin, int tft_cs, int tft_dc)
    : _tft(tft_cs, tft_dc) {
    // Constructor
    _backlight_pin = backlight_pin;
}

void Display::init() {
    // Reference voltage set to AVCC (5V), ADC0 as input and left adjusted
    ADMUX &= ~((1<<MUX0)|(1<<MUX1)|(1<<MUX2)|(1<<MUX3));
    ADMUX |= (1<<REFS0) | (1<<ADLAR);
    // Enable ADC, set prescaler to 128 for accuracy (16MHz / 128 = 125kHz)
    ADCSRA = (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);
    // Enable ADC
    ADCSRA |= (1<<ADEN);

    // Fast PWM mode, non-inverting
    TCCR0A |= (1<<WGM00) | (1<<WGM01);
    TCCR0A |= (1<<COM0B1);
    //Prescaler 64
    TCCR0B |= (1<<CS00) | (1<<CS01);

    // Set the backlight pin as output
    DDRD |= (1<<_backlight_pin);

    _tft.begin();
    _tft.setRotation(1);

}

void Display::refresh_backlight() {
    // Add code to refresh the backlight as needed
    if(!(ADCSRA & (1<<ADSC))){
        OCR0B = ADCH;
    }

    ADCSRA |= (1<<ADSC);
}

void Display::drawGraphicalCursor(int x, int y, int size, uint16_t color, const uint8_t cursor[]) {
    // Use the tft object
    _tft.drawBitmap(x, y, cursor, size, size, color);
}

void Display::clearScreen() {
    _tft.fillScreen(ILI9341_BLACK);
}

void Display::showStartMenu() {
    _tft.fillScreen(ILI9341_BLACK);

    _tft.fillRect(0, 0, 320, 160, 0x6B5B); // Light blue color in RGB565 format

    for(int j = 0; j < 12; j++){
        for (int i = 0; i < 32; i++)
        {
            // Generate random RGB values biased towards green
            int red = 32 + rand() % 32;     // Red: 32 to 63 (brighter)
            int green = 200 + rand() % 56; // Green: 200 to 255 (dominant)
            int blue = 16 + rand() % 32;   // Blue: 16 to 47 (reduced range)

            // Convert to RGB565
            uint16_t color = ((red >> 3) << 11) | ((green >> 2) << 5) | (blue >> 3);

            // Draw the rectangle with the random green shade
            _tft.fillRect(i * 10, 160 + j * 10, 10, 10, color);
        }
    }

    

    _tft.setCursor(10, 10);
    _tft.setTextColor(ILI9341_WHITE);
    _tft.setTextSize(2);
    _tft.println("Start Menu");
    _tft.setCursor(10, 40);
    _tft.setTextSize(1);
    _tft.println("Press C to start");
}