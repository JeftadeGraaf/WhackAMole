#include <avr/io.h>

#include "Game.h"

// Constructor
Game::Game(IRComm &ir, Display &display, Timer1Overflow &timer1) :
    ir(ir),
    display(display),
    timer1(timer1)
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
            loopRecievedProcess();
            break;

        case Display::gameOver:
            //Update the game over screen when data is recieved
            if(!display.gameOverUpdated){
                sendScore(score);
                if(proc == recieveScore){
                    display.updateGameOver(score, opponentsScore, moleWon);
                    display.gameOverUpdated = true;
                    score = 0;
                }
            }

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
            //Serial.println("ERROR, unknown screen");
            break;
    }
}

void Game::reactToRecievedData(uint16_t data, uint32_t timer1_overflow_count){
    proc = readRecievedProcess(data);

    switch(proc){
        case Game::startGame: {
                score = 0; //Reset score
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
                    //Serial.println("Difficulty set error");
                    //TODO terugsturen en terug ontvangen voor correcte check
                }
                display.drawGame(display.selectedDifficulty);
            break;
        }
        //TODO wanneer nieuwe game gestart wordt, wordt mol in verkeerde hol geplaatst
        case Game::moleUp:{
            recievedMoleHeap = data & 0xF; //Get mole heap from 4 LSBs
            //If mole is not up, draw mole
            if(!recievedMoleIsUp){
            if (display.hammerPlaced && display.hammerPlacedHeap == recievedMoleHeap) {
                // Draw the mole under the hammer
                display.drawOrRemoveMole(recievedMoleHeap, true);
                display.drawOrRemoveHammer(recievedMoleHeap, true, true);
            } else {
                display.drawOrRemoveMole(recievedMoleHeap, true);
            }
            recievedMoleIsUp = true;
            processCurrentTime = timer1.overflowCount;
            oldRecievedMoleHeap = recievedMoleHeap;
            }
            break;
        }

        case Game::hammerPositionHit:
                recievedMoleHeap = data & 0xF; //Get mole heap from 4 LSBs
                recievedHammerHitting = (data & 0x10) != 0; //Get hammer hitting from 5th LSB

                //Recieved hammers heap has changed
                if(recievedMoleHeap != oldRecievedMoleHeap){
                    display.drawOrRemoveHammer(oldRecievedMoleHeap, false, false); //remove cursor from old heap
                    display.drawOrRemoveHammer(recievedMoleHeap, true, false); //draw cursor on new heap
                }

                //Recieved that the hammer should be hitting
                if(recievedHammerHitting && !hammerHitting){
                    display.drawOrRemoveHammer(recievedMoleHeap, false, false); //remove selector
                    display.drawOrRemoveHole(recievedMoleHeap, true); //place hole
                    display.drawOrRemoveHammer(recievedMoleHeap, true, true); //draw hitting hammer
                    hammerHitting = true;
                }
                //Hammer is not hitting anymore, but it was before
                else if(!recievedHammerHitting && hammerHitting){
                    display.drawOrRemoveHammer(recievedMoleHeap, false, true); //remove hitting hammer
                    display.drawOrRemoveHole(recievedMoleHeap, true); //remove hole
                    display.drawOrRemoveHammer(recievedMoleHeap, true, false); //place selector
                    hammerHitting = false;
                }

                oldRecievedMoleHeap = recievedMoleHeap;
            break;
        
        case Game::recieveScore:
                opponentsScore = (uint8_t) data; //Get score from 8 LSBs    
                sendScore(score); //Send own score to other console
                if((display.characterMole && score > opponentsScore) || (!display.characterMole && score < opponentsScore)){
                    moleWon = true;
                }
                else{
                    moleWon = false;
                }
            break;

        default:
            //Serial.println("Error: Unknown process");
            break;
    }

}

Game::process Game::readRecievedProcess(uint16_t data){
    data = data >> 8;
    if(data == 0){
        return invalidProcess;
    }
    else if(data == 1){
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

//TODO joystick (debounce)
//TODO calculate score
void Game::updateGame(bool ZPressed){
    //Dynamic Time and Score
    display.updateGameTimeScore(score);

    //Read movement
    if((!display.characterMole && !display.hammerPlaced) || display.characterMole){
        if(timer1.joystickDebounceCount > 5){
            if(Nunchuk.state.joy_x_axis > Nunchuk.centerValue + Nunchuk.deadzone && display.dynamicStartX != display.Xmax){
                display.dynamicStartX += display.Xcrement; //Move right
                display.selectedHeap += 1;
                timer1.resetJoystickDebounce();
            } else if (Nunchuk.state.joy_x_axis < Nunchuk.centerValue - Nunchuk.deadzone && display.dynamicStartX != display.startX){
                display.dynamicStartX -= display.Xcrement; //Move left
                display.selectedHeap -= 1;
                timer1.resetJoystickDebounce();
            }

            if(Nunchuk.state.joy_y_axis < Nunchuk.centerValue - Nunchuk.deadzone && display.dynamicStartY != display.Ymax){
                display.dynamicStartY += display.Ycrement; //Move down
                display.selectedHeap += display.gridSize;
                timer1.resetJoystickDebounce();
            } else if (Nunchuk.state.joy_y_axis > Nunchuk.centerValue + Nunchuk.deadzone && display.dynamicStartY != display.startY){
                display.dynamicStartY -= display.Ycrement; //Move up
                display.selectedHeap -= display.gridSize;
                timer1.resetJoystickDebounce();
            }
        }
    }

    //If character is mole
    if(display.characterMole){
        //Draw selector rectangle
        uint16_t dynamicStartX = display.dynamicStartX;
        uint16_t dynamicStartY = display.dynamicStartY;
        display.calculateHeapPosition(display.selectedHeap, dynamicStartX, dynamicStartY);
        display._tft.drawRect(dynamicStartX-2, dynamicStartY-2, display.selectWidthHeight+4, display.selectWidthHeight+4, ILI9341_BLACK);
        //If other heap is selected, remove old selector
        if(display.oldSelectedHeap != display.selectedHeap){
            display.redrawBackGround(display.oldDynamicStartX, display.oldDynamicStartY, display.selectWidthHeight, display.selectWidthHeight);
            display.drawOrRemoveHole(display.oldSelectedHeap, true);
            if(display.molePlaced){
                display.drawOrRemoveMole(display.molePlacedHeap, true);
            }
        }

        //If Z is pressed and mole is not placed, draw mole
        if(ZPressed && !display.molePlaced){
            sendMoleUp(display.selectedHeap); //Send placed mole to other console
            display.drawOrRemoveMole(display.selectedHeap, true); //Draw the mole on the selected heap
            display.molePlaced = true; //A mole has been placed
            display.molePlacedTime = timer1.overflowCount; //Save the time the mole was placed
            display.molePlacedHeap = display.selectedHeap; //Save the heap the mole was placed in
        }

        if((display.molePlaced && recievedHammerHitting) && (display.selectedHeap == recievedMoleHeap)){
            moleWasHit = true;
        }

        //If mole is placed and time is up, remove mole
        if(display.molePlaced && (timer1.overflowCount - display.molePlacedTime >= timeMoleUp)){ //Check if mole has been placed for 2 seconds
            display.drawOrRemoveMole(display.molePlacedHeap, false);
            if (!moleWasHit && (timer1.overflowCount - scoreIncrementedTime >= timeMoleUp)) {
                score += moleAvoidPoints;
                scoreIncrementedTime = timer1.overflowCount;
            }
            display.molePlaced = false;
            moleWasHit = false;
        }

        
    }

    //If character is hammer
    else{
        // If the hammer is currently placed
        if (display.hammerPlaced)
        {
            if (timer1.overflowCount - display.hammerPlacedTime >= timeHammerDown)
            {
                // Remove the horizontal hammer
                display.drawOrRemoveHammer(display.hammerPlacedHeap, false, true);

                // Place a hole where the hammer was
                display.drawOrRemoveHole(display.hammerPlacedHeap, true);

                display.drawOrRemoveHammer(display.hammerPlacedHeap, true, false);

                // Reset hammer state
                display.hammerPlaced = false;
            }
            else
            {
                // Hammer still active; do not process further actions
                return;
            }
        }

        // If Z is pressed and the hammer is not already placed
        if (ZPressed)
        {
            // Remove the selector hammer from the current heap
            display.drawOrRemoveHammer(display.selectedHeap, false, false);

            // Place the horizontal hammer (hammer hit action)
            display.drawOrRemoveHammer(display.selectedHeap, true, true);

            // Update hammer state
            display.hammerPlaced = true;
            display.hammerPlacedTime = timer1.overflowCount;
            display.hammerPlacedHeap = display.selectedHeap;
        }

        // Handle heap selection changes
        if (display.selectedHeap != display.oldSelectedHeap)
        {
            // Remove any existing selector hammer from the old heap
            display.drawOrRemoveHammer(display.oldSelectedHeap, false, false);

            // If Z is pressed, ensure the horizontal hammer is displayed correctly on the new heap
            if (ZPressed)
            {
                display.drawOrRemoveHammer(display.selectedHeap, true, true);
            }
            else
            {
                // Draw the selector hammer on the new heap
                display.drawOrRemoveHammer(display.selectedHeap, true, false);
            }
        }
    }

    display.oldSelectedHeap = display.selectedHeap;
    display.oldDynamicStartX = display.dynamicStartX;
    display.oldDynamicStartY = display.dynamicStartY;

    if (display.time == 0) {
        // Game over
        gameOver();
    }
}

void Game::updateDifficulty(bool buttonPressed){
    if(Nunchuk.state.joy_y_axis < Nunchuk.centerValue - Nunchuk.deadzone && display.selectedDifficulty != Display::sixteen){
        display.redrawBackGround(display.difficultyCircleX - 5, display.difficultyCircleY - 5, 10, 10);
        //move down
        display.difficultyCircleY += 50;
        //When moving down, change the difficulty to the value under it
        if(display.selectedDifficulty == Display::four){
            display.selectedDifficulty = Display::nine;
        }
        else if(display.selectedDifficulty == Display::nine){
            display.selectedDifficulty = Display::sixteen;
        }
        display._tft.fillCircle(display.difficultyCircleX, display.difficultyCircleY, 5, ILI9341_BLACK);
    } else if (Nunchuk.state.joy_y_axis > Nunchuk.centerValue + Nunchuk.deadzone && display.selectedDifficulty != Display::four){
        display.redrawBackGround(display.difficultyCircleX - 5, display.difficultyCircleY - 5, 10, 10);
        //move up
        display.difficultyCircleY -= 50;
        //When moving down, change the difficulty to the value above it
        if(display.selectedDifficulty == Display::sixteen){
            display.selectedDifficulty = Display::nine;
        }
        else if(display.selectedDifficulty == Display::nine){
            display.selectedDifficulty = Display::four;
        }
        display._tft.fillCircle(display.difficultyCircleX, display.difficultyCircleY, 5, ILI9341_BLACK);
    }

    //Start the game with the selected difficulty when button is pressed
    if(buttonPressed){
        sendStart(display.characterMole, display.selectedDifficulty); //Send start game process to other console
        display.drawGame(display.selectedDifficulty);
    }
}

void Game::loopRecievedProcess(){
    if(proc == moleUp){
        if((recievedMoleIsUp && display.hammerPlaced) &&
        (display.selectedHeap == recievedMoleHeap) &&
        (timer1.overflowCount - scoreIncrementedTime >= timeHammerDown)){
            score += hammerHitMolePoints;
            scoreIncrementedTime = timer1.overflowCount;
        }
        //If mole is up, check if it has been up for 2 seconds
        if(recievedMoleIsUp && (timer1.overflowCount - processCurrentTime >= timeMoleUp)) {
            //Remove mole after 2 seconds
            if(oldRecievedMoleHeap == display.hammerPlacedHeap){
                if(display.hammerPlaced){
                    display.drawOrRemoveHammer(oldRecievedMoleHeap, false, true);
                    display.drawOrRemoveMole(oldRecievedMoleHeap, false);
                    display.drawOrRemoveHole(oldRecievedMoleHeap, true);
                    display.drawOrRemoveHammer(oldRecievedMoleHeap, true, true);
                } else {
                    display.drawOrRemoveHammer(oldRecievedMoleHeap, false, false);
                    display.drawOrRemoveMole(oldRecievedMoleHeap, false);
                    display.drawOrRemoveHammer(oldRecievedMoleHeap, true, false);
                }
            } else {
                display.drawOrRemoveMole(oldRecievedMoleHeap, false);
            }
            recievedMoleIsUp = false;
        }
    }

}

void Game::gameOver(){
//Reset variables for next game
    display.selectedHeap = 0;
    display.oldSelectedHeap = 0;
    display.drawGameOverMenu();
}