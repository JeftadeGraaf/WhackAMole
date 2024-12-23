#ifndef GAME_H
#define GAME_H

#include <avr/io.h>
#include "Display.h"
#include "Nunchuk.h"
#include "IRComm.h"
#include <Timer1Overflow.h>

#define hammerHitMolePoints 1
#define moleAvoidPoints 1
#define timeMoleUp (uint8_t)25
#define timeHammerDown (uint8_t)30

class Game {
public:
    Game(IRComm &ir, Display &display, Timer1Overflow &timer1); //Game constructor

    uint8_t heapCoordinatesToHeapNumber(uint8_t x, uint8_t y, uint8_t gridWidth); //Convert heap coordinates to heap number
    void sendStart(bool opponentIsMole, uint8_t difficulty); //Send start game process to other console
    void sendHammerMove(uint8_t grid_position, bool strike); //Send hammer position and state to other console
    void sendMoleUp(uint8_t grid_position); //Send mole position and state to other console
    void sendScore(uint8_t score); //Send score to other console
    void sendInvalidCommandSignal(); //TODO
    void buttonListener(); //Listen and react to button presses
    void reactToRecievedData(uint16_t data, uint32_t timer1_overflow_count); //React to recieved data

    void loopRecievedProcess(); //For the mole up process, needs to be constantly looped because of time check

    void updateGame(bool ZPressed); //Update the game screen
    void updateDifficulty(bool buttonPressed); //Update the difficulty screen

    //Recieved processes and their IDs
    enum process{
        startGame = 1,
        moleUp = 2,
        hammerPositionHit = 3,
        recieveScore = 4,
        invalidProcess = 5
    };
    process proc; //Most recently recieved process
    process readRecievedProcess(uint16_t data); //Read the process ID from the recieved data

    uint8_t score = 0; //Player score
    uint8_t opponentsScore = 0; //Opponents score
    bool moleWon = false; //Saves if the mole has won after the game

private:
    void gameOver(); //Resets variables for next game and shows game over screen
    void moleScoreCalculator(); //Calculate the moles score based on recieved and local data
    void hammerScoreCalculator(); //Calculate the hammers score based on recieved and local data

    IRComm& ir; //Instance of infraread class
    Display& display; //Instance of display class
    Timer1Overflow &timer1; //Instance of the timer1 overflow class

    //Variables if player is playing as the mole
    bool recievedHammerHitting = false; //If recieved data says hammer is hitting
    bool moleWasHit = false; //If mole was hit, for score calculation

    //Variables if the player is playing as the hammer
    bool hammerHitting = false; //If hammer is hitting
    bool recievedMoleIsUp = false; //If recieved data says mole is up

    //For both mole and hammer
    uint8_t recievedMoleHeap; //What mole heap is recieved
    uint8_t oldRecievedMoleHeap = 0; //previously recieved mole heap
    uint32_t processCurrentTime; //Time the mole went up or hammer is hit
    uint32_t scoreIncrementedTime; //Time score is incremented

    const uint8_t NUNCHUK_ADDRESS = 0x52; //Nunchuk I2c address

    //Save button state
    bool ZPressed; //Z button state
    bool CPressed; //C button state
};

#endif