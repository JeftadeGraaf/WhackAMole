#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <Adafruit_ILI9341.h>

class Display {
public:
    Display(int backlight_pin, int tft_cs, int tft_dc);
    void init();
    void refresh_backlight();

    void drawGraphicalCursor(int x, int y, int size, uint16_t color, const uint8_t cursor[]);
    void drawGameOverMenu(uint8_t player_score, uint8_t opponent_score);
    void drawGame();
    void updateGame(uint8_t score);
    void calcCenterScreenText(String text, uint8_t textSize);
    void clearScreen();

private:
    // Add private members and methods as needed
    int _backlight_pin;
    Adafruit_ILI9341 _tft;
};

#endif // DISPLAY_H