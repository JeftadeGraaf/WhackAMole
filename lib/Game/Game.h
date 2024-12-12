#ifndef GAME_H
#define GAME_H

#include <avr/io.h>
#include <IRComm.h>

class Game {
public:
    Game(IRComm &ir);
    ~Game();

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