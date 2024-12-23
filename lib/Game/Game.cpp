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
            }
            if(!display.gameOverUpdated && opponentsScore != 255){
                display.updateGameOver(score, opponentsScore, moleWon);
                display.gameOverUpdated = true;
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
                score = 0; //Reset score when recieving start game
                opponentsScore = 255; //Reset opponents score when recieving start game
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

        case Game::moleUp:{
            recievedMoleHeap = data & 0xF; //Get mole heap from 4 LSBs
            //If mole is not up, draw mole
            if(!recievedMoleIsUp){
                display.drawOrRemoveMole(recievedMoleHeap, true);
                recievedMoleIsUp = true;
                processCurrentTime = timer1.overflowCount;
                oldRecievedMoleHeap = recievedMoleHeap;
            }
            break;
        }

        case Game::hammerPositionHit:
                if(display.displayedScreen == Display::game){
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
                }
            break;
        
        case Game::recieveScore:
                opponentsScore = (uint8_t) data; //Get score from 8 LSBs    
                if((display.characterMole && score > opponentsScore) || (!display.characterMole && score < opponentsScore)){
                    moleWon = true;
                }
                else{
                    moleWon = false;
                }
            break;

        default:
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
void Game::updateGame(bool ZPressed){
    //Dynamic Time and Score
    display.updateGameTimeScore(score);

    //Change the selector position based on nunchuk input. If the hammer is hitting, the selector is blocked
    if((!display.characterMole && !display.hammerJustHit) || display.characterMole){
        //Checks are performed to see which direction the nunchuk is moved, and if the selector is at the edge of the screen
        if(Nunchuk.state.joy_x_axis > Nunchuk.centerValue + Nunchuk.deadzone && display.dynamicStartX != display.Xmax){
            display.dynamicStartX += display.Xcrement; //Move right
            display.selectedHeap += 1; //Selected heap is changed to the right
        } else if (Nunchuk.state.joy_x_axis < Nunchuk.centerValue - Nunchuk.deadzone && display.dynamicStartX != display.startX){
            display.dynamicStartX -= display.Xcrement; //Move left
            display.selectedHeap -= 1; //Selected heap is changed to the left
        }

        if(Nunchuk.state.joy_y_axis < Nunchuk.centerValue - Nunchuk.deadzone && display.dynamicStartY != display.Ymax){
            display.dynamicStartY += display.Ycrement; //Move down
            display.selectedHeap += display.gridSize; //Selected heap is changed down
        } else if (Nunchuk.state.joy_y_axis > Nunchuk.centerValue + Nunchuk.deadzone && display.dynamicStartY != display.startY){
            display.dynamicStartY -= display.Ycrement; //Move up
            display.selectedHeap -= display.gridSize; //Selected heap is changed up
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
            display.molePlacedTime = timer1.overflowCount; //Save the time the mole was placed
            display.molePlacedHeap = display.selectedHeap; //Save the heap the mole was placed in
        }
        
        moleScoreCalculator(); //Calculate the moles score based on recieved and local data
    }

    //If character is hammer
    else{
        //If the hammers movement is not blocked
        if (timer1.overflowCount - display.lastHammerUse >= timeHammerDown) { // 30 overflows ≈ 1 second
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
                display.lastHammerUse = timer1.overflowCount;
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

    if (display.time == 0 || proc == recieveScore) {
        // Game over
        gameOver();
    }
}

void Game::updateDifficulty(bool buttonPressed){
    //Remove the old selected difficulty circle
    display._tft.fillCircle(display.difficultyCircleX, display.difficultyCircleY, 5, ILI9341_GREEN);

    //Read movement and change selected difficulty accordingly
    //If joystick is moved down, move the circle down and change the difficulty to the value under it
    if(Nunchuk.state.joy_y_axis < Nunchuk.centerValue - Nunchuk.deadzone && display.selectedDifficulty != Display::sixteen){
        //move down by changing the Y value of the circle
        display.difficultyCircleY += 50;
        //When moving down, change the difficulty to the value under it
        if(display.selectedDifficulty == Display::four){
            display.selectedDifficulty = Display::nine;
        }
        else if(display.selectedDifficulty == Display::nine){
            display.selectedDifficulty = Display::sixteen;
        }
    //If joystick is moved up, move the circle up and change the difficulty to the value above it
    } else if (Nunchuk.state.joy_y_axis > Nunchuk.centerValue + Nunchuk.deadzone && display.selectedDifficulty != Display::four){
        //move up
        display.difficultyCircleY -= 50;
        //When moving up, change the difficulty to the value above it
        if(display.selectedDifficulty == Display::sixteen){
            display.selectedDifficulty = Display::nine;
        }
        else if(display.selectedDifficulty == Display::nine){
            display.selectedDifficulty = Display::four;
        }
    }
    //Place the new selected difficulty circle
    display._tft.fillCircle(display.difficultyCircleX, display.difficultyCircleY, 5, ILI9341_BLACK);

    //Start the game with the selected difficulty when button is pressed
    if(buttonPressed){
        sendStart(display.characterMole, display.selectedDifficulty); //Send start game process to other console, contains character and difficulty
        score = 0; //Reset score after choosing a character
        display.drawGame(display.selectedDifficulty); //Draw the game screen with the selected difficulty
    }
}

void Game::loopRecievedProcess(){
    //This process needs to be looped because of the time check
    if(proc == moleUp){
        if((recievedMoleIsUp && display.hammerJustHit) &&
        (display.selectedHeap == recievedMoleHeap) &&
        (timer1.overflowCount - scoreIncrementedTime >= timeHammerDown)){
            score += hammerHitMolePoints;
            scoreIncrementedTime = timer1.overflowCount;
        }
        //If mole is up, check if it has been up for 2 seconds
        if(recievedMoleIsUp && (timer1.overflowCount - processCurrentTime >= timeMoleUp)) {
            //Remove mole after 2 seconds
            display.drawOrRemoveMole(oldRecievedMoleHeap, false);
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

void Game::moleScoreCalculator(){
    //If mole is placed and the hammer is hitting the same hole, the mole was hit
    if((display.molePlaced && recievedHammerHitting) && (display.selectedHeap == recievedMoleHeap)){
        moleWasHit = true;
    }

    //If mole is placed and time is up, remove mole
    if(display.molePlaced && (timer1.overflowCount - display.molePlacedTime >= timeMoleUp)){
        display.drawOrRemoveMole(display.molePlacedHeap, false); //Remove the placed mole
        //If the mole wasn't hit, increment the score
        if (!moleWasHit && (timer1.overflowCount - scoreIncrementedTime >= timeMoleUp)) {
            score += moleAvoidPoints;
            scoreIncrementedTime = timer1.overflowCount; //Avoid score incrementing multiple times
        }
        //Reset the variables for the next mole
        display.molePlaced = false;
        moleWasHit = false;
    }
}