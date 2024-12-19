#ifndef DISPLAY_H
#define DISPLAY_H

#include "Adafruit_ILI9341.h"
#include "Fonts/InriaSans_Regular8pt7b.h"
#include "Fonts/IrishGrover_Regular8pt7b.h"
#include <Timer1Overflow.h>
#include <SevenSegment.h>

#include <SPI.h>

class Display {
public:
    Display(int backlight_pin, int tft_cs, int tft_dc, Timer1Overflow &timer1, SevenSegment &sevenSegment);
    void init(); //Initialize the display
    void refreshBacklight(); //Change the brightness of the display based on the potmeter value
    // void updateGameTimeScore(uint8_t score); //Update the time and score in the game screen

    // void updateGame(uint8_t score, bool buttonPressed); //Update the game, hammer position, mole position, score and time
    void updateChooseCharacter(bool buttonPressed); //Update the choose character menu based on user input
    // void updateDifficulty(bool buttonPressed); //Update the difficulty menu based on user input
    void updateStartMenu(bool buttonPressed); //Update the startmenu based on user input

    void drawOrRemoveHole(uint8_t heapNumber, bool draw); //Draw or remove the hole
    void drawOrRemoveHammer(uint8_t heapNumber, bool draw, bool horizontal); //Draw or remove the hammer
    void drawOrRemoveMole(uint8_t heapNumber, bool draw); //Draw or remove the mole
    void calculateHeapPosition(uint8_t heapNumber, uint16_t& xPos, uint16_t& yPos); //Calculate the position of the mole or hammer based on the heap number

    enum Difficulty{
        four = 4,
        nine = 9,
        sixteen = 16
    };
    Difficulty selectedDifficulty = nine; //The current selected difficulty

    void drawGame(Difficulty selectedDifficulty); //Draw the game
    void drawChooseCharacter(); //Draw the choose character menu
    void drawDifficulty(); //Draw the choose difficulty menu
    void drawGameOverMenu(uint8_t player_score, uint8_t opponent_score, bool mol_win); //Draw the game over menu
    void drawStartMenu(); //Draw the start menu
    void drawHighscores(); //Draw the highscores screen

    enum Screens {
        game,
        gameOver,
        startMenu,
        chooseCharacter,
        difficulty,
        highscores,
    };

     const uint32_t SCREEN_WIDTH = 320; //Displays screen width
    const uint16_t SCREEN_HEIGHT = 240; //Displays screen height
    const uint8_t picturePixelSize = 8; //the size per pixel of used images
    Screens displayedScreen; //The current displayed screen
    bool characterMole = true; //Saves the selected playable character
    bool molePlaced; //If mole is placed
    uint32_t molePlacedTime; //Time mole is placed
    uint8_t molePlacedHeap; //Heap mole is placed in
    uint8_t oldSelectedHeap; //Previouse selected molehole
    unsigned int oldDynamicStartX; //previous selector X position
    uint16_t oldDynamicStartY; //previous selector Y position
    unsigned int dynamicStartX = 0; //Changes X based on input of nunchuk joystick and difficulty selected
    uint16_t dynamicStartY = 0; //Changes Y based on input of nunchuk joystick and difficulty selected
    uint16_t selectWidthHeight = 0; //The size of the selector in game
    uint8_t multiplySize = 0; //size of icons is based on difficulty, this saves the size
    unsigned int startX = 0; //Start X coordinate of selector
    unsigned int startY = 0; //Start Y coordinate of selector
    uint8_t Xcrement = 0; //Amount to increase dynamicStartX
    uint8_t Ycrement = 0; //Amount to increase dynamicStartY
    uint16_t Xmax = 0; //Maximum amount dynamicStartX may reach
    uint8_t Ymax = 0; //Maximum amount dynamicStartY may reach
    uint8_t gridSize = 0; //2 for 4 heaps, 3 for 9 heaps, 4 for 16 heaps
    //updateGame() variables
    uint8_t time = 60;    //starting time
    uint8_t oldScore = 0; //starting score
    uint32_t lastHammerUse = 0; //Time a hit with the hammer was performed
    bool hammerJustHit = false; //Saves if the hammer is in the process of hitting
    //refresh
    int _backlight_pin; //Pin of the backlight

    //variables for calculating text bounds
    int16_t x1, y1; //returned boundary postion
    uint16_t textWidth, textHeight; //returned values of the text width and height
    uint16_t x, y; //Used for postioning
    String text; //Used to save the to be printed text

    //updateChooseCharacter() variables
    uint8_t moleTextXCoor; //X coordinate of the printed mole text
    uint8_t hammerTextXCoor; //X coordinate of the printed hammer text
    uint8_t textYCoor; //Y coordinate of the printed texts

    //updateDifficulty() variables
    uint8_t difficultyCircleX = 15; //X coordinate of the postion of the selector circle
    uint8_t difficultyCircleY = 115; //Y coordinate of the postion of the selector circle

    //updateStartMenu() variables
    bool startButtonSelected = true; //Changes based on which option is selected
    uint8_t startCircleX = 15; //X coordinate of the postion of the selector circle
    uint8_t startCircleY = 70; //Y coordinate of the postion of the selector circle

    //Draw screens() variables
    const uint8_t backgroundPixelSize = 10; //Size of the pixels

    uint8_t selectedHeap = 0; //Which molehole is selected

    const uint32_t SCREEN_WIDTH = 320; //Displays screen width
    const uint16_t SCREEN_HEIGHT = 240; //Displays screen height
    const uint8_t picturePixelSize = 8; //the size per pixel of used images

    Adafruit_ILI9341 _tft; //An instance of the display

private:
    void calcCenterScreenText(String text, uint8_t textSize); //Used to calculate the center of the screen for a given text
    void drawPixelField(uint8_t y); //Used to draw a field of certain height. The field consists of different shades of green pixels
    void drawPixelField(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t pixelSize);
    void drawPixelArray(const uint8_t *pixels, const uint8_t palette[][3], uint8_t pixelSize, int xStart, int yStart, int xSize, int ySize);
    void updateGameTimeScore(uint8_t score); //Update the time and score in the game screen

    Adafruit_ILI9341 _tft; //An instance of the display

    void updateGameTimeScore(uint8_t score); //Update the time and score in the game screen

    struct DifficultyLevel {
        uint8_t multiplySize;
        uint16_t startX;
        uint16_t startY;
        uint16_t Xincrement;
        uint16_t Yincrement;
        uint8_t gridSize;
        uint16_t Xmax;
        uint16_t Ymax;
        uint16_t dynamicStartX;
        uint8_t dynamicStartY;
    };

    DifficultyLevel difficultyLevels[3] = {
        {6, 60, 70, 150, 100, 2, 210, 170},   // Difficulty 4
        {5, 50, 55, 90, 70, 3, 230, 195},     // Difficulty 9
        {4, 15, 54, 88, 45, 4, 279, 189},     // Difficulty 16
    };

    DifficultyLevel level;    

private:
    void calcCenterScreenText(String text, uint8_t textSize); //Used to calculate the center of the screen for a given text
    void drawPixelField(uint8_t y); //Used to draw a field of certain height. The field consists of different shades of green pixels
    void drawPixelField(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t pixelSize);
    void drawPixelArray(const uint8_t *pixels, const uint8_t palette[][3], uint8_t pixelSize, int xStart, int yStart, int xSize, int ySize);

    Timer1Overflow* _timer1;
    SevenSegment* _sevenSegment;

};

#endif