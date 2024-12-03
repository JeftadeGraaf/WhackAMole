#ifndef DISPLAY_H
#define DISPLAY_H

#include <Adafruit_ILI9341.h>
#include "Fonts/InriaSans_Regular8pt7b.h"
#include "Fonts/IrishGrover_Regular8pt7b.h"

#include <SPI.h>
#include <SD.h>

class Display {
public:
    Display(int backlight_pin, int tft_cs, int tft_dc);
    void init();
    void refresh_backlight();
    void drawGraphicalCursor(int x, int y, int size, uint16_t color, const uint8_t cursor[]);
    void drawPixelArray(const uint32_t pixelArray[16][16], uint8_t pixelSize, uint16_t startX, uint16_t startY);
    void drawGameOverMenu(uint8_t player_score, uint8_t opponent_score);
    void drawGame();
    void updateGame(uint8_t score);
    void calcCenterScreenText(String text, uint8_t textSize);
    void clearScreen();
private:
    Adafruit_ILI9341 _tft;
    int _backlight_pin;
};

#endif
