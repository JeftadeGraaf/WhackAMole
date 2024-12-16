#ifndef GAME_H
#define GAME_H

#include <avr/io.h>
#include <IRComm.h>

#include "Display.h"
#include "Nunchuk.h"
#include "IRComm.h"

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

    enum process{
        startGame = 1,
        moleUp = 2,
        hammerPositionHit = 3,
        recieveScore = 4,
        invalidProcess = 5
    };

    process readRecievedProcess(uint16_t data);

    process lastReceivedProcess;

private:
    // Add private member variables and methods here
    IRComm& ir;
    Display& display;

    bool moleIsUp; //If mole is up
    uint32_t moleUpCurrentTime; //Time mole is up
    uint16_t recievedData; //!TEMP recieved data

    const uint8_t NUNCHUK_ADDRESS = 0x52;       //Nunchuk I2c address
    //Save button state
    bool ZPressed;
    bool CPressed;
};

#endif