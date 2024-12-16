#include <avr/io.h>
#include <HardwareSerial.h>

#include "Game.h"


// Constructor
Game::Game(IRComm &ir, Display &display)
    : ir(ir),           // Initialize references in constructor initializer list
    display(display)
    {
    // this->ir = ir;
    // this->display = display;
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

void Game::buttonListener() {
    //update button state
    Nunchuk.getState(NUNCHUK_ADDRESS);
    ZPressed = Nunchuk.state.z_button;
    CPressed = Nunchuk.state.c_button;

    //Switch between different screens
    switch(display.displayedScreen) {
        case Display::game:
            display.updateGame(0, ZPressed);
            break;

        case Display::gameOver:
            //Go to start menu
            if(ZPressed){
                display.drawStartMenu();
            }
            break;

        case Display::startMenu:
            //Update selection
            display.updateStartMenu(ZPressed);
            break;

        case Display::chooseCharacter:
            //Update selection
            display.updateChooseCharacter(ZPressed);
            //Go back to start menu
            if(CPressed){
                display.drawStartMenu();
            }
            break;

        case Display::difficulty:
            //Update selection
            display.updateDifficulty(ZPressed);
            //Go back to choose character screen
            if(CPressed){
                display.drawChooseCharacter();
            }
            break;

        case Display::highscores:
            //Go back to start menu
            if(CPressed){
                display.drawStartMenu();
            }
            break;

        default:
            Serial.println("ERROR, unknown screen");
    }
}

void Game::reactToRecievedData(uint16_t data, uint32_t timer1_overflow_count){
    process proc = readRecievedProcess(data);

    switch(proc){
        case Game::startGame: {
            display.characterMole = (data >> 3) & 1; //set character based on bit 3

            uint8_t lastThreeBits = data & 0x7; //Set difficulty based on 3 LSBs
            if(lastThreeBits == 1){ //Bit 0 is set -> 2x2
                display.selectedDifficulty = Display::four;
            }
            else if(lastThreeBits == 2){ //Bit 1 is set -> 3x3
                display.selectedDifficulty = Display::nine;
            }
            else if(lastThreeBits == 4){ //Bit 2 is set -> 4x4
                display.selectedDifficulty = Display::sixteen;
            }
            else{ //If invalid difficulty is recieved
                Serial.println("Difficulty set error");
                //TODO terugsturen en terug ontvangen voor correcte check
            }
            display.drawGame(display.selectedDifficulty);
            break;
        }

        case Game::moleUp:{
            uint8_t recievedMoleHeap = data & 0xF; //Get mole heap from 4 LSBs
            //If mole is not up, draw mole
            if(!moleIsUp){
                display.drawOrRemoveMole(recievedMoleHeap, true);
                moleUpCurrentTime = timer1_overflow_count;
                moleIsUp = true;
            }
            //If mole is up, check if it has been up for 2 seconds
            else{
                if (timer1_overflow_count - moleUpCurrentTime >= 60) {
                    //Remove mole after 2 seconds
                    display.drawOrRemoveMole(recievedMoleHeap, false);
                    moleIsUp = false;
                }
            }
            break;
        }

        case Game::hammerPositionHit:
            Serial.println("error");
            break;
        
        case Game::recieveScore:
            Serial.println("error");
            break;

        default:
            Serial.println("error");
            break;
    }
}

Game::process Game::readRecievedProcess(uint16_t data){
    data = data >> 8;
    if(data == 1){
        return startGame;
    }
    else if(data == 2){
        return moleUp;
    }
    else if(data == 3){
        return hammerPositionHit;
    }
    else if(data == 4){
        return recieveScore;
    }
    return invalidProcess;
}