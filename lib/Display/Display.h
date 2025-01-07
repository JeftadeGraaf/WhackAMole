#ifndef DISPLAY_H
#define DISPLAY_H

#include "Adafruit_ILI9341.h"
#include "Fonts/InriaSans_Regular8pt7b.h"
#include "Fonts/IrishGrover_Regular8pt7b.h"
#include <Timer1Overflow.h>
#include <SevenSegment.h>
#include <Audio.h>

#include <SPI.h>

class Display {
public:
    Timer1Overflow &timer1; //Instance of the timer1 overflow object
    SevenSegment &sevenSegment; //Instance of the seven segment object
    Audio &audio; //Instance of the audio object

    Adafruit_ILI9341 _tft; //An instance of the display
    Display(int backlight_pin, int tft_cs, int tft_dc, Timer1Overflow &timer1, SevenSegment &sevenSegment, Audio &audio);
    void init(); //Initialize the display
    void refreshBacklight(); //Change the brightness of the display based on the potmeter value
    void updateGameTimeScore(uint8_t score); //Update the time and score in the game screen
    void updateChooseCharacter(bool buttonPressed); //Update the choose character menu based on user input
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

    void drawPixelArray(const uint8_t *pixels PROGMEM, uint8_t pixelSize, int xStart, int yStart, int xSize, int ySize);

    void drawGame(Difficulty selectedDifficulty); // Draw the game
    void drawChooseCharacter(); //Draw the choose character menu
    void drawDifficulty(); //Draw the choose difficulty menu
    void drawGameOverMenu(); //Draw the game over menu
    void updateGameOver(uint8_t player_score, uint8_t opponent_score, bool mole_win); //Update the game over screen based on the winner
    void drawStartMenu(); //Draw the start menu
    void drawHighscores(); //Draw the highscores screen

    int calcCenterScreenText(const char *text, uint8_t textSize);

    void clearScreen(); //Turn screen black

    enum Screens {
        game,
        gameOver,
        startMenu,
        chooseCharacter,
        difficulty,
        highscores
    };
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
    bool hammerPlaced = false; //If mole is placed
    uint32_t hammerPlacedTime; //Time mole is placed
    uint8_t hammerPlacedHeap; //Heap mole is placed in
    uint8_t hammerSelectedHeap;
    //refresh
    int _backlight_pin; //Pin of the backlight

    //variables for calculating text bounds
    int16_t x1, y1; //returned boundary postion
    uint16_t textWidth, textHeight; //returned values of the text width and height
    uint16_t x, y; //Used for postioning
    const char* text; //Used to save the to be printed text

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

    //Variables for selector and heap generation. updateGame(), drawGame(), updateChooseCharacter() functions
    uint8_t selectedHeap = 0; //Which molehole is selected
    bool gameOverUpdated = false;

    void redrawBackGround(uint16_t x, uint8_t y, uint8_t width, uint8_t height);

private:
    void drawPixelField(uint8_t y); //Used to draw a field of certain height. The field consists of different shades of green pixels

    const uint32_t SCREEN_WIDTH = 320; //Displays screen width
    const uint16_t SCREEN_HEIGHT = 240; //Displays screen height
    const uint8_t picturePixelSize = 8; //the size per pixel of used images

    const char title[13] = "Whack A Mole"; //The title of the game
    const char highscoresText[11] = "Highscores"; //The highscores button text
    const char character[10] = "Character"; //The character button text
    const char chooseYour[13] = "Choose your"; //The choose your text
    const char scoreText[6] = "Score"; //The score text
    const char moleText[5] = "Mole"; //The mole text
    const char hammerText[7] = "Hammer"; //The hammer text

    const uint64_t backgroundBitmap[24] = {
        0xC8A6D57B93E1F04A, 0x1D5E7A2C0B9F4638, 0x7E3D9B1F6C5A2048, 0x4B1E0D8F3A6C7592,
        0x9F2B8C1D6E5A0437, 0x3D8E1B5F7A2C0649, 0x1F0C5A2E8D7B9364, 0x6C5A1D3E7B9F0428,
        0xE9B4F16D2C8A5037, 0x5A1D3E7B9F0C8642, 0x8D1B5F3A6C7E2049, 0x2C8A6D1B5F3E9074,
        0x7B4F1D9E3A6C8502, 0x1D3E7B9F0C5A2846, 0x6C8A2D1B5F3E9074, 0xF1D2E8B4C7A5036,
        0x4A1D3E7B9F0C8652, 0x9E3A6C8D1B5F2074, 0x2C8A6D1B5F3E9074, 0x7B4F1D9E3A6C8502,
        0xD2E8B4C7A5F1036, 0x5A1D3E7B9F0C8642, 0x8D1B5F3A6C7E2049, 0xE9B4F16D2C8A5037
    };  //The background bitmap

   const uint16_t greenBiasedColors[4] = {
        0x07E0,  // Base green color
        0x0760,  // Slightly darker green
        0x07C0,  // Slightly lighter green
        0x0620   // Muted green with more blue
    };
};

#endif
