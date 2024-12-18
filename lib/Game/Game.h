#ifndef GAME_H
#define GAME_H

#include <avr/io.h>
#include "Display.h"
#include "Nunchuk.h"
#include "IRComm.h"
#include "Timer1Overflow.h"

class Game {
public:
    Game(IRComm &ir, Display &display, Timer1Overflow &timer1);

    uint8_t heapCoordinatesToHeapNumber(uint8_t x, uint8_t y, uint8_t gridWidth);
    void sendStart(bool opponentIsMole, uint8_t difficulty);
    void sendHammerMove(uint8_t grid_position, bool strike);
    void sendMoleUp(uint8_t grid_position);
    void sendScore(uint8_t score);
    void sendInvalidCommandSignal();
    void buttonListener();
    void reactToRecievedData(uint16_t data, uint32_t timer1_overflow_count);

    void updateGame(uint8_t score, bool ZPressed);
    void updateDifficulty(bool buttonPressed);

    enum process{
        startGame = 1,
        moleUp = 2,
        hammerPositionHit = 3,
        recieveScore = 4,
        invalidProcess = 5
    };

    process readRecievedProcess(uint16_t data);

    uint8_t score = 0; //TODO wordt niet gebruikt
    uint8_t opponentsScore = 0; //Opponents score

private:
    IRComm& ir;
    Display& display;
    Timer1Overflow& timer1;

    bool moleIsUp; //If mole is up
    uint32_t processCurrentTime; //Time the mole went up or hammer is hit
    uint8_t recievedMoleHeap = 0; //Recieved mole heap
    uint8_t oldRecievedMoleHeap = 0; //previous recieved mole heap
    bool hammerHitting; //If hammer is hitting
    bool recievedHammerHitting; //Recieved hammer hitting

    const uint8_t NUNCHUK_ADDRESS = 0x52;       //Nunchuk I2c address
    //Save button state
    bool ZPressed;
    bool CPressed;
};

#endif