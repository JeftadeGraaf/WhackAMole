#ifndef GAME_H
#define GAME_H

#include <avr/io.h>
#include <IRComm.h>

class Game {
public:
    Game(IRComm &ir);

    uint8_t heapCoordinatesToHeapNumber(uint8_t x, uint8_t y, uint8_t gridWidth);
    void sendStart(bool opponentIsMole, uint8_t difficulty);
    void hammerMove(uint8_t grid_position, bool strike);
    void moleUp(uint8_t grid_position);
    void sendScore(uint8_t score);
    void sendInvalidCommandSignal();

private:
    // Add private member variables and methods here
    IRComm ir;

};

#endif