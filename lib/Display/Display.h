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

    void updateGame(uint8_t score, bool buttonPressed);
    void updateChooseCharacter(bool buttonPressed);
    void updateDifficulty(bool buttonPressed);
    void updateStartMenu(bool buttonPressed);

    enum Difficulty{
        four = 4,
        nine = 9,
        sixteen = 16
    };
    Difficulty selectedDifficulty = nine;

    void drawGame(Difficulty selectedDifficulty);
    void drawChooseCharacter();
    void drawDifficulty();
    void drawGameOverMenu(uint8_t player_score, uint8_t opponent_score, bool mol_win);
    void drawStartMenu();
    void drawHighscores();

    void setTimingVariable(uint32_t *timer1_overflows_32ms);

    enum Screens {
        game,
        gameOver,
        startMenu,
        chooseCharacter,
        difficulty,
        highscores
    };
    Screens displayedScreen;

    bool characterMole; //Saves the selected playable character
private:
    void calcCenterScreenText(String text, uint8_t textSize);
    void drawPixelField(uint8_t y);
    void drawDifficultyGrid(int multiplySize, int startX, int startY, int Xcrement, int Ycrement, int gridSize);
    void drawPixelArray(const uint8_t *pixels, const uint8_t palette[][3], uint8_t pixelSize, int xStart, int yStart, int xSize = 8, int ySize = 8);

    Adafruit_ILI9341 _tft;
    
    const uint32_t SCREEN_WIDTH = 320;
    const uint16_t SCREEN_HEIGHT = 240;
    const uint8_t picturePixelSize = 8;

    //refresh
    int _backlight_pin;

    //variables for calculating text bounds
    int16_t x1, y1;
    uint16_t textWidth, textHeight;
    uint16_t x, y;
    String text;

    //updateChooseCharacter() variables
    bool moleSelected = true;
    uint8_t moleTextXCoor;
    uint8_t hammerTextXCoor;
    uint8_t textYCoor;

    //updateDifficulty() variables
    uint8_t difficultyCircleX = 15;
    uint8_t difficultyCircleY = 115;

    //updateStartMenu() variables
    bool startButtonSelected = true;
    uint8_t startCircleX = 15;
    uint8_t startCircleY = 70;

    //Draw screens() variables
    const uint8_t pixelSize = 10;

    //updateGame() variables
    uint8_t time = 60;    //starting time
    uint8_t oldScore = 0; //starting score
    uint32_t lastHammerUse = 0;
    bool hammerJustHit = 0;

    //Variables for selector and heap generation. updateGame(), drawGame(), updateChooseCharacter() functions
    uint16_t selectWidthHeight = 0;
    uint8_t multiplySize = 0;

    uint8_t selectedHeap = 0;
    uint8_t oldSelectedHeap;
    unsigned int oldDynamicStartX;
    uint16_t oldDynamicStartY;

    unsigned int startX = 0;
    unsigned int startY = 0;
    unsigned int dynamicStartX = 0;
    uint16_t dynamicStartY = 0;
    uint8_t Xcrement = 0;
    uint8_t Ycrement = 0;
    uint16_t Xmax = 0;
    uint8_t Ymax = 0;
    uint8_t gridSize = 0;

    //Mole pixel array, for putting down mole after 2 seconds
    uint32_t moleArray[4];
};

#endif
