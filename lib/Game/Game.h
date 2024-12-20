#ifndef GAME_H
#define GAME_H

#include <avr/io.h>
#include "Display.h"
#include "Nunchuk.h"
#include "IRComm.h"

#define hammerHitMolePoints 1
#define moleAvoidPoints 1
#define timeMoleUp (uint8_t)25
#define timeHammerDown (uint8_t)30

class Game {
public:
    Game(IRComm &ir, Display &display);

    uint8_t heapCoordinatesToHeapNumber(uint8_t x, uint8_t y, uint8_t gridWidth);
    void sendStart(bool opponentIsMole, uint8_t difficulty);
    void sendHammerMove(uint8_t grid_position, bool strike);
    void sendMoleUp(uint8_t grid_position);
    void sendScore(uint8_t score);
    void sendInvalidCommandSignal();
    void buttonListener();
    void reactToRecievedData(uint16_t data, uint32_t timer1_overflow_count);

    void loopRecievedProcess();

    void updateGame(bool ZPressed);
    void updateDifficulty(bool buttonPressed);

    enum process{
        startGame = 1,
        moleUp = 2,
        hammerPositionHit = 3,
        recieveScore = 4,
        invalidProcess = 5
    };
    process proc;
    process readRecievedProcess(uint16_t data);

    uint8_t score = 0; //Player score
    uint8_t opponentsScore = 0; //Opponents score
    bool moleWon = false;

private:
    void gameOver();

    IRComm& ir;
    Display& display;


    uint32_t scoreIncrementedTime; //Time score is incremented

    bool moleIsUp = false; //If mole is up
    uint32_t processCurrentTime; //Time the mole went up or hammer is hit
    uint8_t recievedMoleHeap; //Recieved mole heap
    uint8_t oldRecievedMoleHeap = 0; //previous recieved mole heap
    bool hammerHitting = false; //If hammer is hitting
    bool recievedHammerHitting = false; //Recieved hammer hitting

    const uint8_t NUNCHUK_ADDRESS = 0x52;       //Nunchuk I2c address
    //Save button state
    bool ZPressed;
    bool CPressed;
};

#endif