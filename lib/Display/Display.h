#ifndef DISPLAY_H
#define DISPLAY_H

#include <Adafruit_ILI9341.h>
#include "Fonts/InriaSans_Regular8pt7b.h"
#include "Fonts/IrishGrover_Regular8pt7b.h"

#include <SPI.h>

class Display {
public:
    Display(int backlight_pin, int tft_cs, int tft_dc);
    void init();
    void refreshBacklight();

    void updateGame(uint8_t score);

    void drawGame(uint8_t heaps);
    void drawGameOverMenu(uint8_t player_score, uint8_t opponent_score, bool mol_win);
    void drawStartMenu();
    void drawChooseCharacter();
    void drawHighscores();

    void clearScreen();

    enum Screens {
        game,
        gameOver,
        startMenu,
        chooseCharacter,
        highscores
    };

    Screens displayedScreen;

private:
    void calcCenterScreenText(String text, uint8_t textSize);
    void drawPixelField(uint8_t y);
    void drawPixelArray(const uint8_t pixels[8][8], const uint8_t palette[], uint8_t pixelSize, int xStart, int yStart);

    Adafruit_ILI9341 _tft;
    
    const uint32_t SCREEN_WIDTH = 320;
    const uint16_t SCREEN_HEIGHT = 240;
    const uint8_t picturePixelSize = 8;

    //refresh
    int _backlight_pin;

    //calcCenterScreenText() variables
    int16_t x1, y1;
    uint16_t textWidth, textHeight;
    uint16_t x, y;
    String text;

    //Draw screens() variables
    const uint8_t pixelSize = 10;

    //updateGame() variables
    uint8_t time = 60;    //starting time
    uint8_t oldScore = 0; //starting score

    //Variables for selector and heap generation. updateGame() and drawGame() functions
    uint16_t selectWidthHeight = 0;
    uint8_t multiplySize = 0;

    unsigned int startX = 0;
    unsigned int startY = 0;
    unsigned int dynamicStartX = 0;
    unsigned int dynamicStartY = 0;
    uint8_t Xcrement = 0;
    uint8_t Ycrement = 0;
    uint16_t Xmax = 0;
    uint8_t Ymax = 0;
};

#endif
