#include <avr/io.h>
#include <HardwareSerial.h>

#include "Game.h"

// Constructor
Game::Game(IRComm &ir, Display &display) :
    ir(ir),
    display(display)
    {
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
4: opponentIsHammer
5: gridSize if 16 else 0
6: gridSize if 9 else 0
LSB: gridSize if 4 else 0
*/
void Game::sendStart(bool opponentIsHammer, uint8_t gridSize) {

    uint16_t command = 0;

    command |= 0x0100; // command id as 16-bit value

    if (!opponentIsHammer) {
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
            updateGame(ZPressed);
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
            updateDifficulty(ZPressed);
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

void Game::reactToReceivedData(uint16_t data, uint32_t timer1_overflow_count){
    proc = readReceivedProcess(data);

    switch(proc){
        case Game::startGame: {
            if(display.displayedScreen != Display::game){
                display.characterMole = (data & 0x08) != 0; //set character based on bit 3

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
            }
            break;
        }
        //TODO wanneer nieuwe game gestart wordt, wordt mol in verkeerde hol geplaatst
        case Game::moleUp:{
            receivedMoleHeap = data & 0xF; //Get mole heap from 4 LSBs
            //If mole is not up, draw mole
            if(!moleIsUp){
                display.drawOrRemoveMole(receivedMoleHeap, true);
                moleIsUp = true;
                processCurrentTime = display.get_t1_overflows();
                oldreceivedMoleHeap = receivedMoleHeap;
            }
            break;
        }

        case Game::hammerPositionHit:
            receivedMoleHeap = data & 0xF; //Get mole heap from 4 LSBs
            receivedHammerHitting = (data & 0x10) != 0; //Get hammer hitting from 5th LSB

            Serial.println(receivedHammerHitting);

            if(receivedMoleHeap != oldreceivedMoleHeap){
                display.drawOrRemoveHammer(oldreceivedMoleHeap, false, false); //remove cursor from old heap
                display.drawOrRemoveHammer(receivedMoleHeap, true, false); //draw cursor on new heap
            }

            //Recieved that the hammer should be hitting
            if(receivedHammerHitting && !hammerHitting){
                display.drawOrRemoveHammer(receivedMoleHeap, false, false); //remove selector
                display.drawOrRemoveHole(receivedMoleHeap, true); //place hole
                display.drawOrRemoveHammer(receivedMoleHeap, true, true); //draw hitting hammer
                hammerHitting = true;
            }
            //Hammer is not hitting anymore, but it was before
            else if(!receivedHammerHitting && hammerHitting){
                display.drawOrRemoveHammer(receivedMoleHeap, false, true); //remove hitting hammer
                display.drawOrRemoveHole(receivedMoleHeap, true); //remove hole
                display.drawOrRemoveHammer(receivedMoleHeap, true, false); //place selector
                hammerHitting = false;
            }

            oldreceivedMoleHeap = receivedMoleHeap;
            break;
        
        case Game::receiveScore:
            if(display.displayedScreen != Display::gameOver){
                opponentsScore = (uint8_t) data; //Get score from 8 LSBs    
                gameOver();

                if((display.characterMole && score > opponentsScore) || (!display.characterMole && !(score < opponentsScore))){
                    moleWon = true;
                }
                else{
                    moleWon = false;
                }
                display.updateGameOver(score, opponentsScore, moleWon);
            break;

        default:
            Serial.println("Error: Unknown process");
            break;
    }
    }

}


//TODO joystick (debounce)
//TODO calculate score
// Function to update the game state
void Game::updateGame(bool ZPressed){
    //Dynamic Time and Score
    display.updateGameTimeScore(score);

    //Read movement
    if((!display.characterMole && !display.hammerJustHit) || display.characterMole){
        if(Nunchuk.state.joy_x_axis > Nunchuk.centerValue + Nunchuk.deadzone && display.dynamicStartX != display.Xmax){
            display.dynamicStartX += display.Xcrement; //Move right
            display.selectedHeap += 1;
        } else if (Nunchuk.state.joy_x_axis < Nunchuk.centerValue - Nunchuk.deadzone && display.dynamicStartX != display.startX){
            display.dynamicStartX -= display.Xcrement; //Move left
            display.selectedHeap -= 1;
        }

        if(Nunchuk.state.joy_y_axis < Nunchuk.centerValue - Nunchuk.deadzone && display.dynamicStartY != display.Ymax){
            display.dynamicStartY += display.Ycrement; //Move down
            display.selectedHeap += display.gridSize;
        } else if (Nunchuk.state.joy_y_axis > Nunchuk.centerValue + Nunchuk.deadzone && display.dynamicStartY != display.startY){
            display.dynamicStartY -= display.Ycrement; //Move up
            display.selectedHeap -= display.gridSize;
        }
    }

    //If character is mole
    if(display.characterMole){
        //Draw selector rectangle
        display._tft.drawRect(display.dynamicStartX-2, display.dynamicStartY-2, display.selectWidthHeight+4, display.selectWidthHeight+4, ILI9341_BLACK);
        //If other heap is selected, remove old selector
        if(display.oldSelectedHeap != display.selectedHeap){
            display._tft.drawRect(display.oldDynamicStartX-2, display.oldDynamicStartY-2, display.selectWidthHeight+4, display.selectWidthHeight+4, ILI9341_GREEN);
        }

        //If Z is pressed and mole is not placed, draw mole
        if(ZPressed && !display.molePlaced){
            sendMoleUp(display.selectedHeap); //Send placed mole to other console
            display.drawOrRemoveMole(display.selectedHeap, true); //Draw the mole on the selected heap
            display.molePlaced = true; //A mole has been placed
            display.molePlacedTime = display.get_t1_overflows(); //Save the time the mole was placed
            display.molePlacedHeap = display.selectedHeap; //Save the heap the mole was placed in
        }

        //If mole is placed and time is up, remove mole
        if(display.molePlaced && (display.get_t1_overflows() - display.molePlacedTime >= timeMoleUp)){ //Check if mole has been placed for 2 seconds
            display.drawOrRemoveMole(display.molePlacedHeap, false);
            display.molePlaced = false;
        }
        if(display.molePlaced && (display.get_t1_overflows() - display.molePlacedTime < timeMoleUp)){
            if((display.molePlaced && !hammerHitting) && (display.get_t1_overflows() - scoreIncrementedTime >= timeMoleUp)){
                score += moleAvoidPoints;
                scoreIncrementedTime = display.get_t1_overflows();
            }
        }
    }

    //If character is hammer
    else{
        //If the hammers movement is not blocked
        if (display.get_t1_overflows() - display.lastHammerUse >= timeHammerDown) { // 30 overflows ≈ 1 second
            //If hammer finished hitting
            if(display.hammerJustHit){
                //Remove horizontal hammer
                display.drawOrRemoveHammer(display.selectedHeap, false, true);
                //Place selector hammer and hole
                display.drawOrRemoveHammer(display.selectedHeap, true, false);
                display.drawOrRemoveHole(display.selectedHeap, true);
                display.hammerJustHit = false;
            }
            //If other heap is selected
            if(display.oldSelectedHeap != display.selectedHeap){
                //remove old selector
                display.drawOrRemoveHammer(display.oldSelectedHeap, false, false);
                //Draw selector hammer
                display.drawOrRemoveHammer(display.selectedHeap, true, false);
            }
            if(ZPressed) {
                // Update last usage timestamp
                display.lastHammerUse = display.get_t1_overflows();
            }
        }
        //If the hammer is blocked
        else if(!display.hammerJustHit){
            //Remove selector hammer
            display.drawOrRemoveHammer(display.selectedHeap, false, false);
            // Perform hammer action
            display.drawOrRemoveHammer(display.selectedHeap, true, true);
            display.hammerJustHit = true;
        }
        sendHammerMove(display.selectedHeap, display.hammerJustHit); //Send hammer position to other console
    }

    display.oldSelectedHeap = display.selectedHeap;
    display.oldDynamicStartX = display.dynamicStartX;
    display.oldDynamicStartY = display.dynamicStartY;

    if (display.time == 0) {
        // Game over
        sendScore(score); //Send score to other console
        // gameOver();
    }
}

void Game::updateDifficulty(bool buttonPressed){
    display._tft.fillCircle(display.difficultyCircleX, display.difficultyCircleY, 5, ILI9341_GREEN);
    if(Nunchuk.state.joy_y_axis < Nunchuk.centerValue - Nunchuk.deadzone && display.selectedDifficulty != Display::sixteen){
        //move down
        display.difficultyCircleY += 50;
        //When moving down, change the difficulty to the value under it
        if(display.selectedDifficulty == Display::four){
            display.selectedDifficulty = Display::nine;
        }
        else if(display.selectedDifficulty == Display::nine){
            display.selectedDifficulty = Display::sixteen;
        }
    } else if (Nunchuk.state.joy_y_axis > Nunchuk.centerValue + Nunchuk.deadzone && display.selectedDifficulty != Display::four){
        //move up
        display.difficultyCircleY -= 50;
        //When moving down, change the difficulty to the value above it
        if(display.selectedDifficulty == Display::sixteen){
            display.selectedDifficulty = Display::nine;
        }
        else if(display.selectedDifficulty == Display::nine){
            display.selectedDifficulty = Display::four;
        }
    }
    display._tft.fillCircle(display.difficultyCircleX, display.difficultyCircleY, 5, ILI9341_BLACK);

    //Start the game with the selected difficulty when button is pressed
    if(buttonPressed){
        sendStart(display.characterMole, display.selectedDifficulty); //Send start game process to other console
        display.drawGame(display.selectedDifficulty);
    }
}

void Game::loopReceivedProcess(){
    if(proc == moleUp){
        if((moleIsUp && display.hammerJustHit) && (display.selectedHeap == receivedMoleHeap) && (display.get_t1_overflows() - scoreIncrementedTime >= timeHammerDown)){
            score += hammerHitMolePoints;
            scoreIncrementedTime = display.get_t1_overflows();
        }
        //If mole is up, check if it has been up for 2 seconds
        if(moleIsUp && (display.get_t1_overflows() - processCurrentTime >= timeMoleUp)) {
            //Remove mole after 2 seconds
            display.drawOrRemoveMole(oldreceivedMoleHeap, false);
            moleIsUp = false;
        }
    }

}

Game::process Game::readReceivedProcess(uint16_t data)
{
    data = data >> 8;
    switch (data) {
        case INVALID_PROCESS:
            return invalidProcess;
        case START_GAME:
            return startGame;
        case MOLE_UP:
            return moleUp;
        case HAMMER_POSITION_HIT:
            return hammerPositionHit;
        case RECEIVE_SCORE:
            return receiveScore;
        default:
            return invalidProcess;
    }
}

void Game::gameOver()
{
    // Reset variables for next game
    display.selectedHeap = 0;
    display.oldSelectedHeap = 0;
    display.drawGameOverMenu();
    score = 0;
}
