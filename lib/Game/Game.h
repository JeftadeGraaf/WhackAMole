#ifndef GAME_H
#define GAME_H

#include <avr/io.h>
#include "Display.h"
#include "Nunchuk.h"
#include "IRComm.h"

#define hammerHitMolePoints 1

class Game {
public:
    enum ProcessType {
        INVALID_PROCESS = 0,
        START_GAME = 1,
        MOLE_UP = 2,
        HAMMER_POSITION_HIT = 3,
        RECEIVE_SCORE = 4
    };

    Game(IRComm &ir, Display &display);

    uint8_t heapCoordinatesToHeapNumber(uint8_t x, uint8_t y, uint8_t gridWidth);
    void sendStart(bool opponentIsMole, uint8_t difficulty);
    void sendHammerMove(uint8_t grid_position, bool strike);
    void sendMoleUp(uint8_t grid_position);
    void sendScore(uint8_t score);
    void sendInvalidCommandSignal();
    void buttonListener();
    void reactToReceivedData(uint16_t data, uint32_t timer1_overflow_count);

    void loopReceivedProcess();

    void updateGame(bool ZPressed);
    void updateDifficulty(bool buttonPressed);

    enum process{
        startGame = 1,
        moleUp = 2,
        hammerPositionHit = 3,
        receiveScore = 4,
        invalidProcess = 5
    };
    process proc;
    process readReceivedProcess(uint16_t data);

    uint8_t score = 0; //Player score
    uint8_t opponentsScore = 0; //Opponents score

private:
    IRComm& ir;
    Display& display;

    const uint8_t timeMoleUp = 60; //Time mole is up
    const uint8_t timeHammerDown = 30; //Time hammer is down
    uint32_t scoreIncrementedTime; //Time score is incremented

    bool moleIsUp = false; //If mole is up
    uint32_t processCurrentTime; //Time the mole went up or hammer is hit
    uint8_t receivedMoleHeap; //Received mole heap
    uint8_t oldreceivedMoleHeap = 0; //previous received mole heap
    bool hammerHitting = false; //If hammer is hitting
    bool receivedHammerHitting = false; //Received hammer hitting

    const uint8_t NUNCHUK_ADDRESS = 0x52;       //Nunchuk I2c address
    //Save button state
    bool ZPressed;
    bool CPressed;
};

#endif