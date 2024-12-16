#include "Display.h"
#include "Nunchuk.h"
#include "Game.h"

uint32_t gameTimeTracker = 0;
uint32_t *timer1_all_overflows;
Game *game_logic;

uint32_t get_t1_overflows(){
    return *timer1_all_overflows;
}

void reset_t1_overflows(){
    *timer1_all_overflows = 0;
}

const uint8_t mole[8][8] = {
    {0, 13, 34, 10, 26, 38, 13, 0},
    {39, 4, 17, 6, 6, 19, 4, 2},
    {0, 23, 16, 5, 5, 3, 24, 2},
    {35, 31, 15, 14, 14, 7, 28, 30},
    {4, 21, 11, 1, 1, 11, 8, 20},
    {9, 12, 25, 1, 1, 7, 12, 18},
    {27, 8, 22, 3, 3, 10, 9, 29},
    {0, 2, 0, 33, 32, 0, 0, 0},
};
const uint8_t mole_palette[120] = {
    0, 0, 2,
    254, 252, 249,
    20, 17, 18,
    166, 132, 61,
    108, 86, 44,
    175, 162, 139,
    167, 156, 135,
    204, 177, 122,
    154, 119, 48,
    139, 109, 47,
    128, 93, 24,
    229, 219, 198,
    199, 164, 90,
    50, 43, 34,
    238, 234, 227,
    210, 186, 136,
    177, 144, 74,
    140, 118, 75,
    144, 114, 53,
    135, 111, 67,
    131, 101, 40,
    138, 102, 28,
    133, 99, 29,
    138, 101, 28,
    128, 95, 30,
    203, 175, 117,
    123, 90, 21,
    82, 64, 34,
    91, 67, 21,
    91, 72, 31,
    64, 59, 57,
    86, 62, 17,
    88, 63, 14,
    88, 62, 14,
    78, 53, 4,
    48, 46, 53,
    39, 39, 39,
    36, 36, 36,
    57, 38, 0,
    33, 26, 26,
};

const uint8_t hole[8][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 4, 4, 14, 13, 9},
    {7, 7, 10, 5, 1, 5, 6, 15},
    {2, 2, 1, 1, 1, 1, 6, 8},
    {11, 2, 3, 1, 1, 3, 8, 12},
};
const uint8_t hole_palette[48] = {
    0, 0, 0,
    105, 69, 4,
    90, 61, 6,
    94, 62, 4,
    111, 73, 9,
    108, 71, 4,
    65, 44, 5,
    93, 63, 9,
    54, 37, 5,
    51, 51, 0,
    102, 71, 10,
    69, 48, 5,
    55, 39, 5,
    53, 37, 5,
    58, 39, 0,
    45, 31, 4,
};

const uint8_t hammerHori[8][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {22, 23, 16, 0, 0, 0, 0, 0},
    {24, 17, 14, 0, 0, 0, 0, 0},
    {1, 20, 13, 4, 5, 3, 2, 12},
    {1, 18, 15, 10, 9, 8, 6, 7},
    {21, 19, 25, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
};
const uint8_t hammerVert[8][8] = {
    {0, 1, 2, 2, 25, 1, 0, 0},
    {0, 20, 21, 22, 19, 13, 0, 0},
    {0, 18, 16, 14, 15, 17, 0, 0},
    {0, 0, 10, 4, 0, 0, 0, 0},
    {0, 0, 9, 5, 0, 0, 0, 0},
    {0, 0, 8, 3, 0, 0, 0, 0},
    {0, 0, 6, 12, 0, 0, 0, 0},
    {0, 0, 7, 23, 0, 0, 0, 0},
};
const uint8_t hammerVert_palette[78] = {
    0, 0, 0,
    85, 0, 0,
    61, 2, 0,
    247, 177, 24,
    255, 161, 26,
    247, 166, 25,
    245, 148, 29,
    246, 149, 28,
    245, 139, 29,
    246, 131, 31,
    255, 136, 29,
    255, 51, 51,
    248, 199, 20,
    168, 133, 110,
    175, 108, 43,
    148, 113, 94,
    135, 81, 56,
    255, 255, 218,
    102, 102, 51,
    111, 68, 52,
    92, 57, 34,
    89, 46, 32,
    86, 39, 27,
    255, 233, 21,
    255, 0, 0,
    59, 0, 0,
};
const uint8_t hammerHori_palette[78] = {
    0, 0, 0,
    60, 2, 0,
    248, 201, 20,
    251, 180, 24,
    253, 161, 25,
    250, 166, 25,
    250, 150, 29,
    244, 148, 28,
    247, 141, 29,
    246, 133, 31,
    249, 127, 30,
    255, 51, 51,
    235, 215, 19,
    174, 106, 43,
    146, 114, 94,
    135, 81, 56,
    255, 255, 218,
    114, 69, 53,
    93, 48, 33,
    92, 57, 34,
    86, 39, 27,
    85, 0, 0,
    63, 31, 0,
    171, 136, 118,
    59, 2, 0,
    0, 85, 85,
};

// Initialize the display
Display::Display(int backlight_pin, int tft_cs, int tft_dc)
    : _tft(tft_cs, tft_dc) {
    // Constructor
    _backlight_pin = backlight_pin;
}

void Display::init() {
    // Reference voltage set to AVCC (5V), ADC0 as input and left adjusted
    ADMUX &= ~((1<<MUX0)|(1<<MUX1)|(1<<MUX2)|(1<<MUX3));
    ADMUX |= (1<<REFS0) | (1<<ADLAR);
    // Enable ADC, set prescaler to 128 for accuracy (16MHz / 128 = 125kHz)
    ADCSRA = (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);
    // Enable ADC
    ADCSRA |= (1<<ADEN);

    // Set the backlight pin as output
    DDRD |= (1<<_backlight_pin);

    _tft.begin();
    _tft.setRotation(1);
}

void Display::refreshBacklight() {
    // Add code to refresh the backlight as needed
    PORTD |= (1<<_backlight_pin);
    // if(!(ADCSRA & (1<<ADSC))){
    //     // OCR0B = ADCH;
    // }

    // ADCSRA |= (1<<ADSC);
}

void Display::drawPixelArray(const uint8_t pixels[8][8], const uint8_t palette[], uint8_t pixelSize, int xStart, int yStart) {
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            // Get the pixel index from the array
            uint8_t pixelIndex = pixels[y][x];

            // If the pixel is black, don't draw it
            if (pixelIndex == 0) {
                continue;
            }

            // Get the corresponding color from the palette
            uint8_t red = palette[pixelIndex * 3];
            uint8_t green = palette[pixelIndex * 3 + 1];
            uint8_t blue = palette[pixelIndex * 3 + 2];

            // Calculate the position where the pixel will be drawn
            int xPos = xStart + x * pixelSize;
            int yPos = yStart + y * pixelSize;

            // Draw the pixel
            _tft.fillRect(xPos, yPos, pixelSize, pixelSize, _tft.color565(red, green, blue));
        }
    }
}

void Display::drawGame(Difficulty selectedDifficulty){
    displayedScreen = game;
    this->characterMole = characterMole;

    reset_t1_overflows();
    gameTimeTracker = 0;
    time = 60;

    //Draw sky and field
    _tft.fillRect(0, 0, SCREEN_WIDTH, 37, SKY_BLUE);
    drawPixelField(37);

    //Write text
    _tft.setTextColor(ILI9341_BLACK);
    _tft.setFont(&IrishGrover_Regular8pt7b);
        text = "Whack a Mole";
        calcCenterScreenText(text, 2);
        _tft.setCursor(x, 30);
        _tft.print(text);

    _tft.setFont(&InriaSans_Regular8pt7b);
        _tft.setTextSize(1);
        _tft.setCursor(2, 15);
        _tft.print("Time");
        
        text = "Score";
        calcCenterScreenText(text, 1);
        _tft.setCursor(SCREEN_WIDTH - textWidth - 2, 15);
        _tft.print(text);

    //Apply settings for selectedDifficulty level, used also in selection process
    if(selectedDifficulty == 4){
        multiplySize= 6;
        startX      = 60;
        startY      = 70;
        Xcrement    = 150;
        Ycrement    = 100;
        Xmax        = 210;
        Ymax        = 170;
        gridSize    = 2;
        for(uint8_t i = 0; i < gridSize; i++){
            for(uint8_t i = 0; i < gridSize; i++){
                drawPixelArray(hole, hole_palette, multiplySize, startX, startY);
                startX += Xcrement;
            }
            startX = 60;
            startY += Ycrement;
        }
        startY = 70;
        dynamicStartX      = startX;
        dynamicStartY      = startY;
    }

    if(selectedDifficulty == 9){
        multiplySize= 5;
        startX      = 50;
        startY      = 55;
        Xcrement    = 90;
        Ycrement    = 70;
        Xmax        = 230;
        Ymax        = 195;
        gridSize    = 3;
        for(uint8_t i = 0; i < gridSize; i++){
            for(uint8_t i = 0; i < gridSize; i++){
                drawPixelArray(hole, hole_palette, multiplySize, startX, startY);
                startX += Xcrement;
            }
            startX = 50;
            startY += Ycrement;
        }
        startY = 55;
        dynamicStartX      = startX;
        dynamicStartY      = startY;
    }

    if(selectedDifficulty == 16){
        multiplySize= 4;
        startX      = 15;
        startY      = 54;
        Xcrement    = 88;
        Ycrement    = 45;
        Xmax        = 279;
        Ymax        = 189;
        gridSize    = 4;
        for(uint8_t i = 0; i < gridSize; i++){
            for(uint8_t i = 0; i < gridSize; i++){
                drawPixelArray(hole, hole_palette, multiplySize, startX, startY);
                startX += Xcrement;
            }
            startX = 15;
            startY += Ycrement;
        }
        startY = 54;
        dynamicStartX      = startX;
        dynamicStartY      = startY;
    }

    selectWidthHeight = picturePixelSize * multiplySize;
}

//TODO joystick (debounce)
//TODO calculate score
//TODO change drawGameOver() inputs
void Display::updateGame(uint8_t score, bool ZPressed){
    //Dynamic Time and Score
    updateGameTimeScore(score);

    oldSelectedHeap = selectedHeap;
    oldDynamicStartX = dynamicStartX;
    oldDynamicStartY = dynamicStartY;

    //Read movement
    if((!characterMole && !hammerJustHit) || characterMole){
        if(Nunchuk.state.joy_x_axis > Nunchuk.centerValue + Nunchuk.deadzone && dynamicStartX != Xmax){
            dynamicStartX+=Xcrement; //Move right
            selectedHeap += 1;
        } else if (Nunchuk.state.joy_x_axis < Nunchuk.centerValue - Nunchuk.deadzone && dynamicStartX != startX){
            dynamicStartX-=Xcrement; //Move left
            selectedHeap -= 1;
        }

        if(Nunchuk.state.joy_y_axis < Nunchuk.centerValue - Nunchuk.deadzone && dynamicStartY != Ymax){
            dynamicStartY+=Ycrement; //Move down
            selectedHeap += gridSize;
        } else if (Nunchuk.state.joy_y_axis > Nunchuk.centerValue + Nunchuk.deadzone && dynamicStartY != startY){
            dynamicStartY-=Ycrement; //Move up
            selectedHeap -= gridSize;
        }
    }

    //If character is mole
    if(characterMole){
        //Draw selector rectangle
        _tft.drawRect(dynamicStartX-2, dynamicStartY-2, selectWidthHeight+4, selectWidthHeight+4, ILI9341_BLACK);
        //If other heap is selected, remove old selector
        if(oldSelectedHeap != selectedHeap){
            _tft.drawRect(oldDynamicStartX-2, oldDynamicStartY-2, selectWidthHeight+4, selectWidthHeight+4, ILI9341_GREEN);
        }

        //If Z is pressed and mole is not placed, draw mole
        if(ZPressed && !molePlaced){
            accessGame().sendMoleUp(selectedHeap);
            drawOrRemoveMole(selectedHeap, true);
            molePlaced = 0x1;
            molePlacedTime = get_t1_overflows();
            molePlacedHeap = selectedHeap;
        }
        //If mole is placed and time is up, remove mole
        if(molePlaced && (get_t1_overflows() - molePlacedTime >= 60)){
            drawOrRemoveMole(molePlacedHeap, false);
            molePlaced = 0x0;
        }
    }
    //If character is hammer
    else{
        //If the hammers movement is not blocked
        if (get_t1_overflows() - lastHammerUse >= 30) { // 30 overflows ≈ 1 second
            if(hammerJustHit){
                //Remove horizontal hammer and hole
                _tft.fillRect(dynamicStartX, dynamicStartY, selectWidthHeight+25, selectWidthHeight, ILI9341_GREEN);
                //Place selector hammer and hole
                drawPixelArray(hole, hole_palette, multiplySize, dynamicStartX, dynamicStartY);
                drawPixelArray(hammerVert, hammerVert_palette, multiplySize, dynamicStartX+30, dynamicStartY);
                hammerJustHit = false;
            }
            if(oldSelectedHeap != selectedHeap){
                //If other heap is selected, remove old selector
                _tft.fillRect(oldDynamicStartX+20, oldDynamicStartY, selectWidthHeight+5, selectWidthHeight, ILI9341_GREEN);
                drawPixelArray(hole, hole_palette, multiplySize, oldDynamicStartX, oldDynamicStartY);
                //Draw selector hammer
                drawPixelArray(hole, hole_palette, multiplySize, dynamicStartX, dynamicStartY);
                drawPixelArray(hammerVert, hammerVert_palette, multiplySize, dynamicStartX+30, dynamicStartY);
                accessGame().sendHammerMove(selectedHeap, false);
            }
            if(ZPressed) {
                // Update last usage timestamp
                lastHammerUse = get_t1_overflows();
            }
        }
        //If the hammer is blocked
        else{
            //Remove selector hammer
            if(hammerJustHit == false){
                _tft.fillRect(dynamicStartX+20, dynamicStartY, selectWidthHeight+5, selectWidthHeight, ILI9341_GREEN);
                drawPixelArray(hole, hole_palette, multiplySize, oldDynamicStartX, oldDynamicStartY);
                // Perform hammer action
                drawPixelArray(hammerHori, hammerHori_palette, multiplySize, dynamicStartX + (2 * multiplySize), dynamicStartY - (1 * multiplySize));
                accessGame().sendHammerMove(selectedHeap, true);
            }
            hammerJustHit = true;
        }
    }

    if (time == 0) {
        // Game over
        drawGameOverMenu(10, 10, true); //TODO send scores and winner
    }
}

void Display::calculateHeapPosition(uint8_t heapNumber, uint16_t& xPos, uint16_t& yPos) {
    xPos = startX + (heapNumber % gridSize) * Xcrement;
    yPos = startY + (heapNumber / gridSize) * Ycrement;
}

void Display::drawOrRemoveMole(uint8_t heapNumber, bool draw) {
    uint16_t xPos, yPos;
    calculateHeapPosition(heapNumber, xPos, yPos);
    
    if (draw) {
        drawPixelArray(mole, mole_palette, multiplySize, xPos, yPos);
        drawPixelArray(hole, hole_palette, multiplySize, xPos, yPos);
    } else {
        _tft.fillRect(xPos, yPos, selectWidthHeight, selectWidthHeight, ILI9341_GREEN);
        drawPixelArray(hole, hole_palette, multiplySize, xPos, yPos);
    }
}

void Display::updateGameTimeScore(uint8_t score){
    _tft.setFont(&InriaSans_Regular8pt7b);
        _tft.setTextSize(1);
        //Remove old text
        _tft.setTextColor(SKY_BLUE);
        _tft.setCursor(2, 30);
        _tft.print(String(time));
        
        text = String(oldScore);
        calcCenterScreenText(text, 1);
        _tft.setCursor(SCREEN_WIDTH - textWidth - 2, 30);
        _tft.print(text);

    // update time variable
    if (get_t1_overflows() - gameTimeTracker > 30) {
        time--;
        gameTimeTracker = get_t1_overflows();
    }

        //Write new text
        _tft.setTextColor(ILI9341_BLACK);
        _tft.setCursor(2, 30);
        _tft.print(String(time));
        
        text = String(score);
        calcCenterScreenText(text, 1);
        _tft.setCursor(SCREEN_WIDTH - textWidth - 2, 30);
        _tft.print(text);
}

void Display::drawChooseCharacter(){
    displayedScreen = chooseCharacter;
    //Draw sky and field
    _tft.fillRect(0, 0, SCREEN_WIDTH, 155, SKY_BLUE);
    drawPixelField(155);

    //Write text
    _tft.setTextColor(ILI9341_BLACK);
    _tft.setFont(&IrishGrover_Regular8pt7b);
        text = "Choose your";
        calcCenterScreenText(text, 2);
        _tft.setCursor(x, 40);
        _tft.print(text);
        text = "character";
        calcCenterScreenText(text, 2);
        _tft.setCursor(x, 80);
        _tft.print(text);

    _tft.setFont(&InriaSans_Regular8pt7b);
        textYCoor = 120;
        text = "Mole";
        calcCenterScreenText(text, 2);
        moleTextXCoor = x / 2 - 20;
        _tft.setCursor(moleTextXCoor, textYCoor);
        _tft.print(text);
        //Draw mole character
        drawPixelArray(mole, mole_palette, 8, moleTextXCoor, 150);
        drawPixelArray(hole, hole_palette, 8, moleTextXCoor, 160);
        text = "Hammer";
        calcCenterScreenText(text, 2);
        hammerTextXCoor = x * 1.5 + 20;
        _tft.setCursor(hammerTextXCoor, textYCoor);
        _tft.print(text);
        //Draw hammerHori character
        drawPixelArray(hammerHori, hammerHori_palette, 8, x * 2 + 10, 150);
}

void Display::updateChooseCharacter(bool buttonPressed){
    //Selection logic
    if(Nunchuk.state.joy_x_axis > Nunchuk.centerValue + Nunchuk.deadzone && characterMole == true){
        characterMole = false; //Move right
    } else if (Nunchuk.state.joy_x_axis < Nunchuk.centerValue - Nunchuk.deadzone && characterMole == false){
        characterMole = true; //Move left
    }

    _tft.drawRect(x1 - 4, y1 - 4, textWidth + 8, textHeight + 8, SKY_BLUE);

    //Change selection coördinates
    uint8_t x = 0;
    if(characterMole){
        text = "Mole";
        x = moleTextXCoor;
    }
    else{
        text = "Hammer";
        x = hammerTextXCoor;
    }
    _tft.setFont(&InriaSans_Regular8pt7b);
    _tft.setTextSize(2);
    _tft.getTextBounds(text, x, textYCoor, &x1, &y1, &textWidth, &textHeight);
    _tft.drawRect(x1 - 4, y1 - 4, textWidth + 8, textHeight + 8, ILI9341_BLACK);

    //When character is selected, go to choose selectedDifficulty screen
    if(buttonPressed){
        drawDifficulty();
    }
}

void Display::drawDifficulty(){
    displayedScreen = difficulty;
    //Draw sky and field
    _tft.fillRect(0, 0, SCREEN_WIDTH, 37, SKY_BLUE);
    drawPixelField(37);

    //Write text
    _tft.setTextColor(ILI9341_BLACK);
    _tft.setFont(&IrishGrover_Regular8pt7b);
        text = "Whack a Mole";
        calcCenterScreenText(text, 2);
        _tft.setCursor(x, 30);
        _tft.print(text);

    _tft.setFont(&InriaSans_Regular8pt7b);
        _tft.setTextSize(3);
        _tft.setCursor(25, 80);
        _tft.print("Easy");

        _tft.setCursor(25, 130);
        _tft.print("Medium");

        _tft.setCursor(25, 180);
        _tft.print("Hard");

    drawPixelArray(mole, mole_palette, 10, 210, 50);
    drawPixelArray(hole, hole_palette, 10, 210, 130);
}

//TODO hard is 4 for mole and 16 for hammer. Change needed
void Display::updateDifficulty(bool buttonPressed){
    _tft.fillCircle(difficultyCircleX, difficultyCircleY, 5, ILI9341_GREEN);
    if(Nunchuk.state.joy_y_axis < Nunchuk.centerValue - Nunchuk.deadzone && selectedDifficulty != sixteen){
        //move down
        difficultyCircleY += 50;
        //When moving down, change the difficulty to the value under it
        if(selectedDifficulty == four){
            selectedDifficulty = nine;
        }
        else if(selectedDifficulty == nine){
            selectedDifficulty = sixteen;
        }
    } else if (Nunchuk.state.joy_y_axis > Nunchuk.centerValue + Nunchuk.deadzone && selectedDifficulty != four){
        //move up
        difficultyCircleY -= 50;
        //When moving down, change the difficulty to the value above it
        if(selectedDifficulty == sixteen){
            selectedDifficulty = nine;
        }
        else if(selectedDifficulty == nine){
            selectedDifficulty = four;
        }
    }
    _tft.fillCircle(difficultyCircleX, difficultyCircleY, 5, ILI9341_BLACK);

    //Start the game with the selected difficulty when button is pressed
    if(buttonPressed){
        drawGame(selectedDifficulty);
    }
}

void Display::drawStartMenu(){
    displayedScreen = startMenu;
    //Draw sky and field
    _tft.fillRect(0, 0, SCREEN_WIDTH, 155, SKY_BLUE);
    drawPixelField(155);

    //Write text
    _tft.setTextColor(ILI9341_BLACK);
    _tft.setFont(&IrishGrover_Regular8pt7b);
        text = "Whack a Mole";
        calcCenterScreenText(text, 2);
        _tft.setCursor(x, 30);
        _tft.print(text);

    _tft.setFont(&InriaSans_Regular8pt7b);
        _tft.setTextSize(2);
        _tft.setCursor(30, 80);
        _tft.print("Start");

        _tft.setCursor(30, 115);
        _tft.print("Highscores");

    drawPixelArray(mole, mole_palette, 10, 200, 50);
    drawPixelArray(hole, hole_palette, 10, 200, 130);
}

void Display::updateStartMenu(bool buttonPressed){
    _tft.fillCircle(startCircleX, startCircleY, 5, SKY_BLUE);
    if(Nunchuk.state.joy_y_axis < Nunchuk.centerValue - Nunchuk.deadzone && startButtonSelected){
        //move down
        startCircleY += 35;
        startButtonSelected = false;
    } else if (Nunchuk.state.joy_y_axis > Nunchuk.centerValue + Nunchuk.deadzone && !startButtonSelected){
        //move up
        startCircleY -= 35;
        startButtonSelected = true;
    }
    _tft.fillCircle(startCircleX, startCircleY, 5, ILI9341_BLACK);

    if(buttonPressed && startButtonSelected){
        drawChooseCharacter();
    }
    else if(buttonPressed && !startButtonSelected){
        drawHighscores();
    }
}

void Display::drawGameOverMenu(uint8_t player_score, uint8_t opponent_score, bool mole_win){
    displayedScreen = gameOver;
    //Draw sky and field
    _tft.fillRect(0, 0, SCREEN_WIDTH, 37, SKY_BLUE);
    drawPixelField(37);

    //Write text
    _tft.setTextColor(ILI9341_BLACK);
    _tft.setFont(&IrishGrover_Regular8pt7b);
        text = "Whack a Mole";
        calcCenterScreenText(text, 2);
        _tft.setCursor(x, 30);
        _tft.print(text);

        //If player won
        if(player_score > opponent_score){
            text = "You Won!";
        } else {
            text = "You Lost!";
        }
        calcCenterScreenText(text, 2);
        _tft.setCursor(x, 90);
        _tft.print(text);

    _tft.setFont(&InriaSans_Regular8pt7b);
        text = "Your score: " + String(player_score);
        calcCenterScreenText(text, 1);
        _tft.setCursor(x, 120);
        _tft.print(text);

        text = "Opponents score: " + String(opponent_score);
        calcCenterScreenText(text, 1);
        _tft.setCursor(x, 136);
        _tft.print(text);

        text = "Z: Return to menu";
        _tft.setCursor(11, 200);
        _tft.print(text);

        text = "C: Save name";
        _tft.setCursor(11, 220);
        _tft.print(text);

    //If mole won, draw mole. Else, draw hammerHori
    if(mole_win){
        drawPixelArray(mole, mole_palette, 8, 150, 150);
        drawPixelArray(hole, hole_palette, 8, 150, 160);
        drawPixelArray(hammerHori, hammerHori_palette, 8, 230, 150);
    } else {
        drawPixelArray(hole, hole_palette, 8, 180, 160);
        drawPixelArray(hammerHori, hammerHori_palette, 8, 200, 150);
    }
}

//TODO highscores opslaan en displayen (EEPROM)
void Display::drawHighscores(){
    displayedScreen = highscores;
    //Draw sky and field
    _tft.fillRect(0, 0, SCREEN_WIDTH, 189, SKY_BLUE);
    drawPixelField(189);

    //Write text
    _tft.setTextColor(ILI9341_BLACK);
    _tft.setFont(&IrishGrover_Regular8pt7b);
        text = "Whack a Mole";
        calcCenterScreenText(text, 2);
        _tft.setCursor(x, 35);
        _tft.print(text);
    
    _tft.drawLine(20, 70, 300, 70, ILI9341_BLACK);      //Horizontal
    _tft.drawLine(160, 55, 160, 180, ILI9341_BLACK);    //Vertical

    _tft.setFont(&InriaSans_Regular8pt7b);
        text = "Highscores";
        calcCenterScreenText(text, 1);
        _tft.setCursor(x, 51);
        _tft.print(text);

        text = "Mole";
        calcCenterScreenText(text, 1);
        _tft.setCursor(x * 1.5, 68);
        _tft.print(text);
        text = "Hammer";
        calcCenterScreenText(text, 1);
        _tft.setCursor(x / 2, 68);
        _tft.print(text);

        uint8_t highscoresY = 85;
        for(uint8_t i = 0; i < 5; i++){
            _tft.setCursor(24, highscoresY);
            _tft.print(String(123));            //TODO aanpassen naar highscore
            _tft.setCursor(90, highscoresY);    //TODO tweaken voor lengte naam
            _tft.print("opponent");             //TODO aanpassen naar speler naam

            _tft.setCursor(164, highscoresY);
            _tft.print(String(123));            //TODO aanpassen naar highscore
            _tft.setCursor(230, highscoresY);   //TODO tweaken voor lengte naam
            _tft.print("opponent");             //TODO aanpassen naar speler naam
            highscoresY+=18;
        }
}

void Display::calcCenterScreenText(String text, uint8_t textSize){
    _tft.setTextSize(textSize);
    _tft.getTextBounds(text, 0, 0, &x1, &y1, &textWidth, &textHeight);
    // Center the text on the screen
    x = (SCREEN_WIDTH - textWidth) / 2;
    y = (SCREEN_HEIGHT - textHeight) / 2;
}

void Display::drawPixelField(uint8_t y){
    for(uint16_t j = 0; j < SCREEN_HEIGHT / pixelSize; j++){
        for (uint16_t i = 0; i < SCREEN_WIDTH / pixelSize; i++)
        {
            // Generate random RGB values biased towards green
            uint8_t red = 32 + rand() % 32;     // Red: 32 to 63 (brighter)
            uint8_t green = 200 + rand() % 56;  // Green: 200 to 255 (dominant)
            uint8_t blue = 16 + rand() % 32;    // Blue: 16 to 47 (reduced range)

            // Convert to RGB565
            uint16_t color = ((red >> 3) << 11) | ((green >> 2) << 5) | (blue >> 3);

            // Draw the rectangle with the random green shade
            _tft.fillRect(i * pixelSize, y + j * pixelSize, pixelSize, pixelSize, color);
        }
    }
}

void Display::clearScreen() {
    _tft.fillScreen(ILI9341_BLACK);
}

void Display::setTimingVariable(uint32_t *timer1_overflows_32ms){
    timer1_all_overflows = timer1_overflows_32ms;
}

void Display::setGameClass(Game *gameClass){
    game_logic = gameClass;
}

Game accessGame(){
    return *game_logic;
}