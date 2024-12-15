#include "Display.h"
#include "Nunchuk.h"

uint32_t gameTimeTracker = 0;
uint32_t *timer1_all_overflows;

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
const uint8_t mole_palette[40][3] = {
    { 0, 0, 2 },
    { 254, 252, 249 },
    { 20, 17, 18 },
    { 166, 132, 61 },
    { 108, 86, 44 },
    { 175, 162, 139 },
    { 167, 156, 135 },
    { 204, 177, 122 },
    { 154, 119, 48 },
    { 139, 109, 47 },
    { 128, 93, 24 },
    { 229, 219, 198 },
    { 199, 164, 90 },
    { 50, 43, 34 },
    { 238, 234, 227 },
    { 210, 186, 136 },
    { 177, 144, 74 },
    { 140, 118, 75 },
    { 144, 114, 53 },
    { 135, 111, 67 },
    { 131, 101, 40 },
    { 138, 102, 28 },
    { 133, 99, 29 },
    { 138, 101, 28 },
    { 128, 95, 30 },
    { 203, 175, 117 },
    { 123, 90, 21 },
    { 82, 64, 34 },
    { 91, 67, 21 },
    { 91, 72, 31 },
    { 64, 59, 57 },
    { 86, 62, 17 },
    { 88, 63, 14 },
    { 88, 62, 14 },
    { 78, 53, 4 },
    { 48, 46, 53 },
    { 39, 39, 39 },
    { 36, 36, 36 },
    { 57, 38, 0 },
    { 33, 26, 26 }
};


const uint8_t hole[4][8] = {
    {0, 0, 0, 4, 4, 14, 13, 9},
    {7, 7, 10, 5, 1, 5, 6, 15},
    {2, 2, 1, 1, 1, 1, 6, 8},
    {11, 2, 3, 1, 1, 3, 8, 12},
};
const uint8_t hole_palette[16][3] = {
    { 0, 0, 0 },
    { 105, 69, 4 },
    { 90, 61, 6 },
    { 94, 62, 4 },
    { 111, 73, 9 },
    { 108, 71, 4 },
    { 65, 44, 5 },
    { 93, 63, 9 },
    { 54, 37, 5 },
    { 51, 51, 0 },
    { 102, 71, 10 },
    { 69, 48, 5 },
    { 55, 39, 5 },
    { 53, 37, 5 },
    { 58, 39, 0 },
    { 45, 31, 4 }
};


const uint8_t hammerHori[5][8] = {
    {21, 19, 25, 0,  0, 0, 0, 0 },
    {1,  18, 15, 10, 9, 8, 6, 7 },
    {1,  20, 13, 4,  5, 3, 2, 12},
    {24, 17, 14, 0,  0, 0, 0, 0 },
    {22, 23, 16, 0,  0, 0, 0, 0 },
};
const uint8_t hammerVert[8][5] = {
    {22, 24, 1,  1,  21},
    {23, 17, 20, 18, 19},
    {16, 14, 13, 15, 25},
    {0,  0,  4,  10, 0},
    {0,  0,  5,  9,  0},
    {0,  0,  3,  8,  0},
    {0,  0,  2,  6,  0},
    {0,  0,  12, 7,  0},
};
const uint8_t hammerPalette[26][3] = {
    { 0, 0, 0 },
    { 60, 2, 0 },
    { 248, 201, 20 },
    { 251, 180, 24 },
    { 253, 161, 25 },
    { 250, 166, 25 },
    { 250, 150, 29 },
    { 244, 148, 28 },
    { 247, 141, 29 },
    { 246, 133, 31 },
    { 249, 127, 30 },
    { 255, 51, 51 },
    { 235, 215, 19 },
    { 174, 106, 43 },
    { 146, 114, 94 },
    { 135, 81, 56 },
    { 255, 255, 218 },
    { 114, 69, 53 },
    { 93, 48, 33 },
    { 92, 57, 34 },
    { 86, 39, 27 },
    { 85, 0, 0 },
    { 63, 31, 0 },
    { 171, 136, 118 },
    { 59, 2, 0 },
    { 0, 85, 85 }
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

    // Fast PWM mode, non-inverting
    TCCR0A |= (1<<WGM00) | (1<<WGM01);
    TCCR0A |= (1<<COM0B1);
    //Prescaler 64
    TCCR0B |= (1<<CS00) | (1<<CS01);

    // Set the backlight pin as output
    DDRD |= (1<<_backlight_pin);

    _tft.begin();
    _tft.setRotation(1);
}

// Change the brightness of the display based on the potmeter value
void Display::refreshBacklight() {
    // Add code to refresh the backlight as needed
    if(!(ADCSRA & (1<<ADSC))){
        OCR0B = ADCH;
    }

    ADCSRA |= (1<<ADSC);
}

// Draw a pixelarray with its corresponding palette
void Display::drawPixelArray(const uint8_t *pixels, const uint8_t palette[][3], uint8_t pixelSize, int xStart, int yStart, int xSize, int ySize) {
    for (int y = 0; y < ySize; y++) {
        for (int x = 0; x < xSize; x++) {
            // Get the pixel value (the pixel array is a linearized 2D array)
            uint8_t pixelIndex = *(pixels + y * xSize + x);

            // If the pixel is black, don't draw it
            if (pixelIndex == 0) {
                continue;
            }

            // Get the corresponding color from the palette
            uint8_t red = palette[pixelIndex][0];
            uint8_t green = palette[pixelIndex][1];
            uint8_t blue = palette[pixelIndex][2];

            // Calculate the position where the pixel will be drawn
            int xPos = xStart + x * pixelSize;
            int yPos = yStart + y * pixelSize;

            // Draw the pixel
            _tft.fillRect(xPos, yPos, pixelSize, pixelSize, _tft.color565(red, green, blue));
        }
    }
}

//Draw game
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
                drawPixelArray(*hole, hole_palette, multiplySize, startX, startY + multiplySize*4, 8, 4);
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
                drawPixelArray(*hole, hole_palette, multiplySize, startX, startY + multiplySize*4, 8, 4);
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
                drawPixelArray(*hole, hole_palette, multiplySize, startX, startY + multiplySize*4, 8, 4);
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

//Update selection and react to button press
//TODO joystick (debounce)
//TODO calculate score
//TODO change drawGameOver() inputs
void Display::updateGame(uint8_t score, bool ZPressed){
    //Dynamic Time and Score
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

    oldSelectedHeap = selectedHeap;
    oldDynamicStartX = dynamicStartX;
    oldDynamicStartY = dynamicStartY;

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
        if(ZPressed){
            if(!moleArray[0]){
                //if Z button is pressed, draw mole and hole on top
                drawPixelArray(*mole, mole_palette, multiplySize, dynamicStartX, dynamicStartY);
                drawPixelArray(*hole, hole_palette, multiplySize, dynamicStartX, dynamicStartY + multiplySize*4, 8, 4); 
                moleArray[0] = 0b00000001;
                moleArray[1] = dynamicStartX;
                moleArray[2] = dynamicStartY;
                moleArray[3] = get_t1_overflows();
            }
        }
        if(oldSelectedHeap != selectedHeap){
            //Remove old selector rectange
            _tft.drawRect(oldDynamicStartX-2, oldDynamicStartY-2, selectWidthHeight+4, selectWidthHeight+4, ILI9341_GREEN);
        }
        if(moleArray[0]){
            //If mole is drawn, remove it after 2 seconds
            if(get_t1_overflows() - moleArray[3] >= 60){
                _tft.fillRect(moleArray[1], moleArray[2], selectWidthHeight, selectWidthHeight, ILI9341_GREEN);
                drawPixelArray(*hole, hole_palette, multiplySize, moleArray[1], moleArray[2] + multiplySize*4, 8, 4);
                moleArray[0] = 0;
            }
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
                drawPixelArray(*hole, hole_palette, multiplySize, dynamicStartX, dynamicStartY + multiplySize*4, 8, 4);
                drawPixelArray(&hammerVert[0][0], hammerPalette, multiplySize, dynamicStartX+30, dynamicStartY, 5, 8);
                hammerJustHit = false;
            }
            if(oldSelectedHeap != selectedHeap){
                //If other heap is selected, remove old selector
                _tft.fillRect(oldDynamicStartX+20, oldDynamicStartY, selectWidthHeight+5, selectWidthHeight, ILI9341_GREEN);
                drawPixelArray(*hole, hole_palette, multiplySize, oldDynamicStartX, oldDynamicStartY + multiplySize*4, 8, 4);
                //Draw selector hammer
                drawPixelArray(*hole, hole_palette, multiplySize, dynamicStartX, dynamicStartY + multiplySize*4, 8, 4);
                drawPixelArray(&hammerVert[0][0], hammerPalette, multiplySize, dynamicStartX+30, dynamicStartY, 5, 8);
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
                drawPixelArray(*hole, hole_palette, multiplySize, oldDynamicStartX, oldDynamicStartY + multiplySize*4, 8, 4);
                // Perform hammer action
                drawPixelArray(*hammerHori, hammerPalette, multiplySize, dynamicStartX + (2 * multiplySize), dynamicStartY - (1 * multiplySize), 8, 5);
            }
            hammerJustHit = true;
        }
    }

    if (time == 0) {
        // Game over
        drawGameOverMenu(10, 10, true); //TODO send scores and winner
    }
}

//Draw character screen
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
        drawPixelArray(*mole, mole_palette, 8, moleTextXCoor, 150);
        drawPixelArray(*hole, hole_palette, 8, moleTextXCoor, 196, 8, 4);
        text = "Hammer";
        calcCenterScreenText(text, 2);
        hammerTextXCoor = x * 1.5 + 20;
        _tft.setCursor(hammerTextXCoor, textYCoor);
        _tft.print(text);
        //Draw hammerHori character
        drawPixelArray(*hammerHori, hammerPalette, 8, x * 2 + 10, 150, 8, 5);
}

//Update selection and react to button press
void Display::updateChooseCharacter(bool buttonPressed){
    //Selection logic
    if(Nunchuk.state.joy_x_axis > Nunchuk.centerValue + Nunchuk.deadzone && moleSelected == true){
        moleSelected = false; //Move right
    } else if (Nunchuk.state.joy_x_axis < Nunchuk.centerValue - Nunchuk.deadzone && moleSelected == false){
        moleSelected = true; //Move left
    }

    _tft.drawRect(x1 - 4, y1 - 4, textWidth + 8, textHeight + 8, SKY_BLUE);

    //Change selection coördinates
    uint8_t x = 0;
    if(moleSelected){
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
        characterMole = moleSelected;
        drawDifficulty();
    }
}

//Draw difficulty screen
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

    drawPixelArray(*mole, mole_palette, 10, 210, 50);
    drawPixelArray(*hole, hole_palette, 10, 210, 170, 8, 4);
}

//Update selection and react to button press
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

//Draw start menu
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

    drawPixelArray(*mole, mole_palette, 10, 200, 50);
    drawPixelArray(*hole, hole_palette, 10, 200, 170, 8, 4);
}

//Update selection and react to button press
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

//Draw game over menu, no update needed
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
        drawPixelArray(*mole, mole_palette, 8, 150, 150);
        drawPixelArray(*hole, hole_palette, 8, 150, 196, 8, 4);
        drawPixelArray(*hammerHori, hammerPalette, 8, 230, 150, 8, 5);
    } else {
        drawPixelArray(*hole, hole_palette, 8, 180, 196, 8, 4);
        drawPixelArray(*hammerHori, hammerPalette, 8, 200, 150, 8, 5);
    }
}

//Draw highscores menu, no update needed
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

//Used to calculate the center of the screen for a given text
void Display::calcCenterScreenText(String text, uint8_t textSize){
    _tft.setTextSize(textSize);
    _tft.getTextBounds(text, 0, 0, &x1, &y1, &textWidth, &textHeight);
    // Center the text on the screen
    x = (SCREEN_WIDTH - textWidth) / 2;
    y = (SCREEN_HEIGHT - textHeight) / 2;
}

//Used to draw a field of certain height. The field consists of different shades of green pixels
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

// Function to draw grid based on difficulty
void Display::drawDifficultyGrid(int multiplySize, int startX, int startY, int Xincrement, int Yincrement, int gridSize) {
    int fixedStartX = startX;
    int fixedStartY = startY;
    for(uint8_t i = 0; i < gridSize; i++){
            for(uint8_t i = 0; i < gridSize; i++){
                drawPixelArray(*hole, hole_palette, multiplySize, startX, startY + multiplySize*4, 8, 4);
                startX += Xcrement;
            }
            startX = fixedStartX;
            startY += Ycrement;
        }
        startY = fixedStartY;
        dynamicStartX      = startX;
        dynamicStartY      = startY;
}



//Used for keeping the time in the game
void Display::setTimingVariable(uint32_t *timer1_overflows_32ms){
    timer1_all_overflows = timer1_overflows_32ms;
}