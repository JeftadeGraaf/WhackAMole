#ifndef DISPLAY_H
#define DISPLAY_H

#include <Adafruit_ILI9341.h>
#include "Fonts/InriaSans_Regular8pt7b.h"
#include "Fonts/IrishGrover_Regular8pt7b.h"

#include <SPI.h>

class Display {
public:
    Display(int backlight_pin, int tft_cs, int tft_dc);
    void init(); //Initialize the display
    void refreshBacklight(); //Change the brightness of the display based on the potmeter value

    void updateGame(uint8_t score, bool buttonPressed); //Update the game, hammer position, mole position, score and time
    void updateChooseCharacter(bool buttonPressed); //Update the choose character menu based on user input
    void updateDifficulty(bool buttonPressed); //Update the difficulty menu based on user input
    void updateStartMenu(bool buttonPressed); //Update the startmenu based on user input

    void drawOrRemoveMole(uint8_t heapNumber, bool draw);
    void calculateHeapPosition(uint8_t heapNumber, uint16_t& xPos, uint16_t& yPos);

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

    void clearScreen(); //Turn screen black

    void setTimingVariable(uint32_t *timer1_overflows_32ms); //Used for keeping the time in the game

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
private:
    void calcCenterScreenText(String text, uint8_t textSize); //Used to calculate the center of the screen for a given text
    void drawPixelField(uint8_t y); //Used to draw a field of certain height. The field consists of different shades of green pixels
    void drawPixelArray(const uint8_t pixels[8][8], const uint8_t palette[], uint8_t pixelSize, int xStart, int yStart); // Draw a pixelarray with its corresponding palette
    void updateGameTimeScore(uint8_t score); //Update the time and score in the game screen


    Adafruit_ILI9341 _tft; //An instance of the display
    
    const uint32_t SCREEN_WIDTH = 320; //Displays screen width
    const uint16_t SCREEN_HEIGHT = 240; //Displays screen height
    const uint8_t picturePixelSize = 8; //the size per pixel of used images

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
    const uint8_t pixelSize = 10; //Size of the pixels

    //updateGame() variables
    uint8_t time = 60;    //starting time
    uint8_t oldScore = 0; //starting score
    uint32_t lastHammerUse = 0; //Time a hit with the hammer was performed
    bool hammerJustHit = false; //Saves if the hammer is in the process of hitting
    bool molePlaced; //If mole is placed
    uint32_t molePlacedTime; //Time mole is placed
    uint8_t molePlacedHeap; //Heap mole is placed in

    //Variables for selector and heap generation. updateGame(), drawGame(), updateChooseCharacter() functions
    uint16_t selectWidthHeight = 0; //The size of the selector in game
    uint8_t multiplySize = 0; //size of icons is based on difficulty, this saves the size

    uint8_t selectedHeap = 0; //Which molehole is selected
    uint8_t oldSelectedHeap; //Previouse selected molehole
    unsigned int oldDynamicStartX; //previous selector X position
    uint16_t oldDynamicStartY; //previous selector Y position

    unsigned int startX = 0; //Start X coordinate of selector
    unsigned int startY = 0; //Start Y coordinate of selector
    unsigned int dynamicStartX = 0; //Changes X based on input of nunchuk joystick and difficulty selected
    uint16_t dynamicStartY = 0; //Changes Y based on input of nunchuk joystick and difficulty selected
    uint8_t Xcrement = 0; //Amount to increase dynamicStartX
    uint8_t Ycrement = 0; //Amount to increase dynamicStartY
    uint16_t Xmax = 0; //Maximum amount dynamicStartX may reach
    uint8_t Ymax = 0; //Maximum amount dynamicStartY may reach
    uint8_t gridSize = 0; //2 for 4 heaps, 3 for 9 heaps, 4 for 16 heaps
};

#endif