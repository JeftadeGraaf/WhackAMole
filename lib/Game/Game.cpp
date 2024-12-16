#include <avr/io.h>
#include <HardwareSerial.h>

#include "IRComm.h"
#include "Game.h"

// Constructor
Game::Game(IRComm &ir) {
    this->ir = ir;
}

// commands are built up like this:
// 0-3: -
// 4-7: command id
// 8-15: data

uint8_t Game::heapCoordinatesToHeapNumber(uint8_t x, uint8_t y, uint8_t gridWidth) {
    return (y * gridWidth) + x;
}

/*
command id: 0x1

data bits:
mSB-3: -
4: opponentIsMole
5: gridSize if 16 else 0
6: gridSize if 9 else 0
LSB: gridSize if 4 else 0
*/
void Game::sendStart(bool opponentIsMole, uint8_t gridSize) {

    uint16_t command = 0;

    command |= 0x0100; // command id as 16-bit value

    if (opponentIsMole) {
        command |= (1 << 3);
    }

    if (gridSize == 4) {
        command |= (1 << 0);
    } else if (gridSize == 9) {
        command |= (1 << 1);
    } else if (gridSize == 16) {
        command |= (1 << 2);
    }

    // send command
    ir.sendFrame(command);
}

/* 
command id: 0x2

data bits:
mSB-3: -
4-7: grid_position 
*/
void Game::sendMoleUp(uint8_t grid_position) {

    uint16_t command = 0;

    command |= 0x0200; // command id as 16-bit value
    command |= grid_position;

    // send command
    ir.sendFrame(command);
}

/*
command id: 0x3

data bits:
mSB-2: -
3: strike
4-7: grid_position
*/
void Game::sendHammerMove(uint8_t grid_position, bool strike) {
    uint16_t command = 0;

    command |= 0x0300; // command id as 16-bit value
    
    if (strike) {
        command |= (1 << 4);
    }

    command |= grid_position;

    // send command
    ir.sendFrame(command);
}

/* 
command id: 0x4

data bits:
mSB-3: -
4-7: score 
*/
void Game::sendScore(uint8_t score) {
    uint16_t command = 0;

    command |= 0x0400; // command id as 16-bit value
    command |= score;

    // send command
    ir.sendFrame(command);
}

/*
command id: 0x5

data bits:
none
*/
void Game::sendInvalidCommandSignal() {
    uint16_t command = 0;

    command |= 0x0500; // command id as 16-bit value

    // send command
    ir.sendFrame(command);
}
