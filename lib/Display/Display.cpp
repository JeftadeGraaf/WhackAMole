#include "Display.h"
#include <Adafruit_ILI9341.h>
#include "Fonts/InriaSans_Regular8pt7b.h"
#include "Fonts/IrishGrover_Regular8pt7b.h"

const uint32_t SCREEN_WIDTH = 320;
const uint16_t SCREEN_HEIGHT = 240;

//calcCenterScreenText function:
int16_t x1, y1;
uint16_t textWidth, textHeight;
int16_t x, y;
String text;

//Draw screens functions
const uint8_t pixelSize = 10;

//updateGame function
uint8_t time = 60;    //starting time
uint8_t oldScore = 0;   //starting score


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

//TODO Mol of hamer tekenen
//TODO knoppen reageren
void Display::drawGameOverMenu(uint8_t player_score, uint8_t opponent_score){
    _tft.fillRect(0, 0, SCREEN_WIDTH, 37, SKY_BLUE);

    for(uint16_t j = 0; j < SCREEN_HEIGHT / pixelSize; j++){
        for (uint16_t i = 0; i < SCREEN_WIDTH / pixelSize; i++)
        {
            // Generate random RGB values biased towards green
            uint8_t red = 32 + rand() % 32;     // Red: 32 to 63 (brighter)
            uint8_t green = 200 + rand() % 56; // Green: 200 to 255 (dominant)
            uint8_t blue = 16 + rand() % 32;   // Blue: 16 to 47 (reduced range)

            // Convert to RGB565
            uint16_t color = ((red >> 3) << 11) | ((green >> 2) << 5) | (blue >> 3);

            // Draw the rectangle with the random green shade
            _tft.fillRect(i * pixelSize, 37 + j * pixelSize, pixelSize, pixelSize, color);
        }
    }

    _tft.setTextColor(ILI9341_BLACK);

    _tft.setFont(&IrishGrover_Regular8pt7b);

    text = "Whack a Mole";
    calcCenterScreenText(text, 2);
    _tft.setCursor(x, 30);
    _tft.print(text);

    if(player_score > opponent_score){
        text = "You Won!";
    } else {
        text = "You Lost!";
    }
    calcCenterScreenText(text, 2);
    _tft.setCursor(x, 90);
    _tft.print(text);

    _tft.setFont(&InriaSans_Regular8pt7b);

    text = "Your score: " + String(player_score);
    calcCenterScreenText(text, 1);
    _tft.setCursor(x, 120);
    _tft.print(text);

    text = "Opponents score: " + String(opponent_score);
    calcCenterScreenText(text, 1);
    _tft.setCursor(x, 136);
    _tft.print(text);

    text = "Z: Return to menu";
    _tft.setCursor(11, 200);
    _tft.print(text);

    text = "C: Save name";
    _tft.setCursor(11, 220);
    _tft.print(text);
}

void Display::drawGame(){
    _tft.fillRect(0, 0, SCREEN_WIDTH, 37, SKY_BLUE);

    for(uint16_t j = 0; j < SCREEN_HEIGHT / pixelSize; j++){
        for (uint16_t i = 0; i < SCREEN_WIDTH / pixelSize; i++)
        {
            // Generate random RGB values biased towards green
            uint8_t red = 32 + rand() % 32;     // Red: 32 to 63 (brighter)
            uint8_t green = 200 + rand() % 56; // Green: 200 to 255 (dominant)
            uint8_t blue = 16 + rand() % 32;   // Blue: 16 to 47 (reduced range)

            // Convert to RGB565
            uint16_t color = ((red >> 3) << 11) | ((green >> 2) << 5) | (blue >> 3);

            // Draw the rectangle with the random green shade
            _tft.fillRect(i * pixelSize, 37 + j * pixelSize, pixelSize, pixelSize, color);
        }
    }

    _tft.setTextColor(ILI9341_BLACK);

    _tft.setFont(&IrishGrover_Regular8pt7b);
        text = "Whack a Mole";
        calcCenterScreenText(text, 2);
        _tft.setCursor(x, 30);
        _tft.print(text);

    _tft.setFont(&InriaSans_Regular8pt7b);
        _tft.setTextSize(1);
        _tft.setCursor(2, 15);
        _tft.print("Time");
        
        text = "Score";
        calcCenterScreenText(text, 1);
        _tft.setCursor(SCREEN_WIDTH - textWidth - 2, 15);
        _tft.print(text);
}

//TODO tijd afnemen
//TODO score incrementen
void Display::updateGame(uint8_t score){
    _tft.setFont(&InriaSans_Regular8pt7b);
    _tft.setTextSize(1);

    _tft.setTextColor(SKY_BLUE);
        _tft.setCursor(2, 30);
        _tft.print(String(time));
        
        text = String(oldScore);
        calcCenterScreenText(text, 1);
        _tft.setCursor(SCREEN_WIDTH - textWidth - 2, 30);
        _tft.print(text);

    _tft.setTextColor(ILI9341_BLACK);
        _tft.setCursor(2, 30);
        _tft.print(String(time--));
        
        text = String(score);
        calcCenterScreenText(text, 1);
        _tft.setCursor(SCREEN_WIDTH - textWidth - 2, 30);
        _tft.print(text);
}

void Display::calcCenterScreenText(String text, uint8_t textSize){
    _tft.setTextSize(textSize);
    _tft.getTextBounds(text, 0, 0, &x1, &y1, &textWidth, &textHeight);
    // Center the text on the screen
    x = (SCREEN_WIDTH - textWidth) / 2;
    y = (SCREEN_HEIGHT - textHeight) / 2;
}

void Display::clearScreen() {
    _tft.fillScreen(ILI9341_BLACK);
}