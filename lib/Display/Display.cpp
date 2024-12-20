#include "Display.h"
#include "Nunchuk.h"

uint32_t gameTimeTracker = 0;
uint32_t *timer1_all_overflows;

uint32_t Display::get_t1_overflows(){
    return *timer1_all_overflows;
}

void reset_t1_overflows(){
    *timer1_all_overflows = 0;
}

// Combined palette for mole, hole, and hammer sprites
const uint8_t combined_palette[82][3] = {
    { 0, 0, 0 },        // 0  - Black (shared)
    { 254, 252, 249 },  // 1  - Original mole[1]
    { 20, 17, 18 },     // 2  - Original mole[2]
    { 166, 132, 61 },   // 3  - Original mole[3]
    { 108, 86, 44 },    // 4  - Original mole[4]
    { 175, 162, 139 },  // 5  - Original mole[5]
    { 167, 156, 135 },  // 6  - Original mole[6]
    { 204, 177, 122 },  // 7  - Original mole[7]
    { 154, 119, 48 },   // 8  - Original mole[8]
    { 139, 109, 47 },   // 9  - Original mole[9]
    { 128, 93, 24 },    // 10 - Original mole[10]
    { 229, 219, 198 },  // 11 - Original mole[11]
    { 199, 164, 90 },   // 12 - Original mole[12]
    { 50, 43, 34 },     // 13 - Original mole[13]
    { 238, 234, 227 },  // 14 - Original mole[14]
    { 210, 186, 136 },  // 15 - Original mole[15]
    { 177, 144, 74 },   // 16 - Original mole[16]
    { 140, 118, 75 },   // 17 - Original mole[17]
    { 144, 114, 53 },   // 18 - Original mole[18]
    { 135, 111, 67 },   // 19 - Original mole[19]
    { 131, 101, 40 },   // 20 - Original mole[20]
    { 138, 102, 28 },   // 21 - Original mole[21]
    { 133, 99, 29 },    // 22 - Original mole[22]
    { 138, 101, 28 },   // 23 - Original mole[23]
    { 128, 95, 30 },    // 24 - Original mole[24]
    { 203, 175, 117 },  // 25 - Original mole[25]
    { 123, 90, 21 },    // 26 - Original mole[26]
    { 82, 64, 34 },     // 27 - Original mole[27]
    { 91, 67, 21 },     // 28 - Original mole[28]
    { 91, 72, 31 },     // 29 - Original mole[29]
    { 64, 59, 57 },     // 30 - Original mole[30]
    { 86, 62, 17 },     // 31 - Original mole[31]
    { 88, 63, 14 },     // 32 - Original mole[32]
    { 88, 62, 14 },     // 33 - Original mole[33]
    { 78, 53, 4 },      // 34 - Original mole[34]
    { 48, 46, 53 },     // 35 - Original mole[35]
    { 39, 39, 39 },     // 36 - Original mole[36]
    { 36, 36, 36 },     // 37 - Original mole[37]
    { 57, 38, 0 },      // 38 - Original mole[38]
    { 33, 26, 26 },     // 39 - Original mole[39]
    { 0, 0, 2 },        // 40 - Original mole[0]
    { 105, 69, 4 },     // 41 - Original hole[1]
    { 90, 61, 6 },      // 42 - Original hole[2]
    { 94, 62, 4 },      // 43 - Original hole[3]
    { 111, 73, 9 },     // 44 - Original hole[4]
    { 108, 71, 4 },     // 45 - Original hole[5]
    { 65, 44, 5 },      // 46 - Original hole[6]
    { 93, 63, 9 },      // 47 - Original hole[7]
    { 54, 37, 5 },      // 48 - Original hole[8]
    { 51, 51, 0 },      // 49 - Original hole[9]
    { 102, 71, 10 },    // 50 - Original hole[10]
    { 69, 48, 5 },      // 51 - Original hole[11]
    { 55, 39, 5 },      // 52 - Original hole[12]
    { 53, 37, 5 },      // 53 - Original hole[13]
    { 58, 39, 0 },      // 54 - Original hole[14]
    { 45, 31, 4 },      // 55 - Original hole[15]
    // Hammer palette colors (56-81)
    { 60, 2, 0 },       // 56 - Original hammer[1]
    { 248, 201, 20 },   // 57 - Original hammer[2]
    { 251, 180, 24 },   // 58 - Original hammer[3]
    { 253, 161, 25 },   // 59 - Original hammer[4]
    { 250, 166, 25 },   // 60 - Original hammer[5]
    { 250, 150, 29 },   // 61 - Original hammer[6]
    { 244, 148, 28 },   // 62 - Original hammer[7]
    { 247, 141, 29 },   // 63 - Original hammer[8]
    { 246, 133, 31 },   // 64 - Original hammer[9]
    { 249, 127, 30 },   // 65 - Original hammer[10]
    { 255, 51, 51 },    // 66 - Original hammer[11]
    { 235, 215, 19 },   // 67 - Original hammer[12]
    { 174, 106, 43 },   // 68 - Original hammer[13]
    { 146, 114, 94 },   // 69 - Original hammer[14]
    { 135, 81, 56 },    // 70 - Original hammer[15]
    { 255, 255, 218 },  // 71 - Original hammer[16]
    { 114, 69, 53 },    // 72 - Original hammer[17]
    { 93, 48, 33 },     // 73 - Original hammer[18]
    { 92, 57, 34 },     // 74 - Original hammer[19]
    { 86, 39, 27 },     // 75 - Original hammer[20]
    { 85, 0, 0 },       // 76 - Original hammer[21]
    { 63, 31, 0 },      // 77 - Original hammer[22]
    { 171, 136, 118 },  // 78 - Original hammer[23]
    { 59, 2, 0 },       // 79 - Original hammer[24]
    { 0, 85, 85 }       // 80 - Original hammer[25]
};

// Updated mole sprite with new indices
const uint8_t mole[8][8] = {
    {0, 13, 31, 10, 24, 35, 13, 0},
    {36, 4, 17, 6, 6, 19, 4, 2},
    {0, 21, 16, 5, 5, 3, 21, 2},
    {32, 29, 15, 14, 14, 7, 26, 28},
    {4, 21, 11, 1, 1, 11, 8, 20},
    {9, 12, 7, 1, 1, 7, 12, 18},
    {25, 8, 22, 3, 3, 10, 9, 27},
    {0, 2, 0, 30, 30, 0, 0, 0}
};

// Updated hole sprite - using original hole colors
const uint8_t hole[4][8] = {
    {0, 0, 0, 44, 44, 53, 52, 48},    // Making sure black is 0, keeping other hole colors at original indices
    {47, 47, 50, 45, 41, 45, 46, 55},
    {42, 42, 41, 41, 41, 41, 46, 48},
    {51, 42, 43, 41, 41, 43, 48, 52}
};

// Updated hammer horizontal - using original hammer colors
const uint8_t hammerHori[5][8] = {
    {76, 74, 80, 0, 0, 0, 0, 0},      // Making sure black is 0, keeping other hammer colors at original indices
    {56, 73, 70, 65, 64, 63, 61, 62},
    {56, 75, 68, 59, 60, 58, 57, 67},
    {79, 72, 69, 0, 0, 0, 0, 0},
    {77, 78, 71, 0, 0, 0, 0, 0}
};

// Updated hammer vertical - using original hammer colors
const uint8_t hammerVert[8][5] = {
    {77, 79, 56, 56, 76},
    {78, 72, 75, 73, 74},
    {71, 69, 68, 70, 80},
    {0, 0, 59, 65, 0},
    {0, 0, 60, 64, 0},
    {0, 0, 58, 63, 0},
    {0, 0, 57, 61, 0},
    {0, 0, 67, 62, 0}
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
    PORTD |= (1<<_backlight_pin);
    // if(!(ADCSRA & (1<<ADSC))){
    //     // OCR0B = ADCH;
    // }

    // ADCSRA |= (1<<ADSC);
}

// Draw a pixelarray with its corresponding palette
void Display::drawPixelArray(const uint8_t *pixels, const uint8_t (&palette)[82][3], uint8_t pixelSize, 
                           int xStart, int yStart, int xSize, int ySize) {
    uint16_t colorCache[82] = {0}; // Cache for converted colors
    bool colorCacheInitialized[82] = {false};

    for (int y = 0; y < ySize; y++) {
        for (int x = 0; x < xSize; x++) {
            uint8_t pixelIndex = *(pixels + y * xSize + x);
            
            if (pixelIndex == 0) continue; // Skip black pixels

            // Use cached color if available
            if (!colorCacheInitialized[pixelIndex]) {
                colorCache[pixelIndex] = _tft.color565(
                    palette[pixelIndex][0],
                    palette[pixelIndex][1],
                    palette[pixelIndex][2]
                );
                colorCacheInitialized[pixelIndex] = true;
            }

            int xPos = xStart + x * pixelSize;
            int yPos = yStart + y * pixelSize;
            
            _tft.fillRect(xPos, yPos, pixelSize, pixelSize, colorCache[pixelIndex]);
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
        _tft.setCursor(calcCenterScreenText(text, 2), 30);
        _tft.print(text);

    _tft.setFont(&InriaSans_Regular8pt7b);
        _tft.setTextSize(1);
        _tft.setCursor(2, 15);
        _tft.print("Time");
        
        text = "Score";

        _tft.setTextSize(1);
        _tft.getTextBounds(text, 0, 0, &x1, &y1, &textWidth, &textHeight);
        // Center the text on the screen
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
        for(uint8_t i = 0; i < (gridSize * gridSize); i++){
            drawOrRemoveHole(i, true);
        }
        
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
        for(uint8_t i = 0; i < (gridSize * gridSize); i++){
            drawOrRemoveHole(i, true);
        }

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
        for(uint8_t i = 0; i < (gridSize * gridSize); i++){
            drawOrRemoveHole(i, true);
        }
    }

    dynamicStartX      = startX;
    dynamicStartY      = startY;
    selectWidthHeight = picturePixelSize * multiplySize;
}

void Display::calculateHeapPosition(uint8_t heapNumber, uint16_t& xPos, uint16_t& yPos) {
    xPos = startX + (heapNumber % gridSize) * Xcrement;
    yPos = startY + (heapNumber / gridSize) * Ycrement;
}

void Display::drawOrRemoveMole(uint8_t heapNumber, bool draw) {
    uint16_t xPos, yPos;
    calculateHeapPosition(heapNumber, xPos, yPos);
    
    if (draw) {
        drawPixelArray(*mole, combined_palette, multiplySize, xPos, yPos, 8, 8);
        drawPixelArray(*hole, combined_palette, multiplySize, xPos, yPos + multiplySize*4, 8, 4);
    } else {
        _tft.fillRect(xPos, yPos, selectWidthHeight, selectWidthHeight, ILI9341_GREEN);
        drawPixelArray(*hole, combined_palette, multiplySize, xPos, yPos + multiplySize*4, 8, 4);
    }
}

void Display::drawOrRemoveHammer(uint8_t heapNumber, bool draw, bool horizontal) {
    uint16_t xPos, yPos;
    calculateHeapPosition(heapNumber, xPos, yPos);

    if (draw) {
        if(!horizontal){
            drawPixelArray(*hammerVert, combined_palette, multiplySize, xPos + (picturePixelSize * multiplySize), yPos, 5, 8);
        }
        else{
            drawPixelArray(*hammerHori, combined_palette, multiplySize, xPos + (2 * multiplySize), yPos, 8, 5);
        }
    } else {
        if(!horizontal){
            _tft.fillRect(xPos  + (picturePixelSize * multiplySize), yPos, (picturePixelSize * multiplySize), (picturePixelSize * multiplySize), ILI9341_GREEN);
            drawPixelArray(*hole, combined_palette, multiplySize, xPos, yPos + multiplySize*4, 8, 4);
        }
        else{
            drawPixelArray(*hole, combined_palette, multiplySize, xPos, yPos + multiplySize*4, 8, 4);
            _tft.fillRect(xPos + (2 * multiplySize), yPos - multiplySize, (picturePixelSize * multiplySize), (picturePixelSize * multiplySize), ILI9341_GREEN);
        }
    }
}

void Display::drawOrRemoveHole(uint8_t heapNumber, bool draw) {
    uint16_t xPos, yPos;
    calculateHeapPosition(heapNumber, xPos, yPos);

    if (draw) {
        drawPixelArray(*hole, combined_palette, multiplySize, xPos, yPos + multiplySize*4, 8, 4);
    } else {
        _tft.fillRect(xPos, yPos, selectWidthHeight, selectWidthHeight, ILI9341_GREEN);
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
        _tft.setCursor(SCREEN_WIDTH - textWidth - 2, 30);
        _tft.print(text);

    oldScore = score;
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
        _tft.setCursor(calcCenterScreenText(text, 2), 40);
        _tft.print(text);
        text = "character";
        _tft.setCursor(calcCenterScreenText(text, 2), 80);
        _tft.print(text);

    _tft.setFont(&InriaSans_Regular8pt7b);
        textYCoor = 120;
        text = "Mole";

        moleTextXCoor = calcCenterScreenText(text, 2) / 2 - 20;
        _tft.setCursor(moleTextXCoor, textYCoor);
        _tft.print(text);
        //Draw mole character
        drawPixelArray(*mole, combined_palette, 8, moleTextXCoor, 150, 8, 8);
        drawPixelArray(*hole, combined_palette, 8, moleTextXCoor, 160 + 32, 8, 4);
        text = "Hammer";

        _tft.setTextSize(2);
        _tft.getTextBounds(text, 0, 0, &x1, &y1, &textWidth, &textHeight);
        // Center the text on the screen
        x = (SCREEN_WIDTH - textWidth) / 2;

        hammerTextXCoor = x * 1.5 + 20;
        _tft.setCursor(hammerTextXCoor, textYCoor);
        _tft.print(text);
        //Draw hammerHori character
        drawPixelArray(*hammerHori, combined_palette, 8, x * 2 + 10, 150, 8, 5);
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
        _tft.setCursor(calcCenterScreenText(text, 2), 30);
        _tft.print(text);

    _tft.setFont(&InriaSans_Regular8pt7b);
        _tft.setTextSize(3);
        _tft.setCursor(25, 80);
        _tft.print("4 holes");

        _tft.setCursor(25, 130);
        _tft.print("9 holes");

        _tft.setCursor(25, 180);
        _tft.print("16 holes");

    drawPixelArray(*mole, combined_palette, 10, 210, 50, 8, 8);
    drawPixelArray(*hole, combined_palette, 10, 210, 130 + 40, 8, 4);
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
        
        _tft.setTextSize(2);
        _tft.getTextBounds(text, 0, 0, &x1, &y1, &textWidth, &textHeight);
        // Center the text on the screen
        x = (SCREEN_WIDTH - textWidth) / 2;
        
        _tft.setCursor(x, 30);
        _tft.print(text);

    _tft.setFont(&InriaSans_Regular8pt7b);
        _tft.setTextSize(2);
        _tft.setCursor(30, 80);
        _tft.print("Start");

        _tft.setCursor(30, 115);
        _tft.print("Highscores");

    drawPixelArray(*mole, combined_palette, 10, 200, 50, 8, 8);
    drawPixelArray(*hole, combined_palette, 10, 200, 130 + 40, 8, 4);
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

void Display::drawGameOverMenu(){
    displayedScreen = gameOver;
    //Draw sky and field
    _tft.fillRect(0, 0, SCREEN_WIDTH, 37, SKY_BLUE);
    drawPixelField(37);

    //Write text
    _tft.setTextColor(ILI9341_BLACK);
    _tft.setFont(&IrishGrover_Regular8pt7b);
        text = "Whack a Mole";
        _tft.setCursor(calcCenterScreenText(text, 2), 30);
        _tft.print(text);
}

void Display::updateGameOver(uint8_t player_score, uint8_t opponent_score, bool mole_win){
    //If player won
        if(player_score > opponent_score){
            text = "You Won!";
        } else {
            text = "You Lost!";
        }
        _tft.setCursor(calcCenterScreenText(text, 2), 90);
        _tft.print(text);

    _tft.setFont(&InriaSans_Regular8pt7b);
        text = "Your score: " + String(player_score);
        _tft.setCursor(calcCenterScreenText(text, 1), 120);
        _tft.print(text);

        text = "Opponents score: " + String(opponent_score);
        _tft.setCursor(calcCenterScreenText(text, 1), 136);
        _tft.print(text);
    
    //If mole won, draw mole. Else, draw hammerHori
    if(mole_win){
        drawPixelArray(*mole, combined_palette, 8, 150, 150, 8, 8);
        drawPixelArray(*hole, combined_palette, 8, 150, 160 + 32, 8, 4);
        drawPixelArray(*hammerHori, combined_palette, 8, 230, 150, 8, 5);
    } else {
        drawPixelArray(*hole, combined_palette, 8, 180, 160 + 32, 8, 4);
        drawPixelArray(*hammerHori, combined_palette, 8, 200, 150, 8, 5);
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
        _tft.setCursor(calcCenterScreenText(text, 2), 35);
        _tft.print(text);
    
    _tft.drawLine(20, 70, 300, 70, ILI9341_BLACK);      //Horizontal
    _tft.drawLine(160, 55, 160, 180, ILI9341_BLACK);    //Vertical

    _tft.setFont(&InriaSans_Regular8pt7b);
        text = "Highscores";
        _tft.setCursor(calcCenterScreenText(text, 1), 51);
        _tft.print(text);

        text = "Mole";
        _tft.setCursor(calcCenterScreenText(text, 1) * 1.5, 68);
        _tft.print(text);
        text = "Hammer";
        _tft.setCursor(calcCenterScreenText(text, 1) / 2, 68);
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

int Display::calcCenterScreenText(const String &text, uint8_t textSize){
    _tft.setTextSize(textSize);
    _tft.getTextBounds(text, 0, 0, &x1, &y1, &textWidth, &textHeight);
    // Center the text on the screen
    return ((SCREEN_WIDTH - textWidth) / 2);
}

void Display::drawPixelField(uint8_t y){
    for(uint16_t j = 0; j < SCREEN_HEIGHT / backgroundPixelSize; j++){
        for (uint16_t i = 0; i < SCREEN_WIDTH / backgroundPixelSize; i++)
        {
            // Generate random RGB values biased towards green
            uint8_t red = 32 + rand() % 32;     // Red: 32 to 63 (brighter)
            uint8_t green = 200 + rand() % 56;  // Green: 200 to 255 (dominant)
            uint8_t blue = 16 + rand() % 32;    // Blue: 16 to 47 (reduced range)

            // Convert to RGB565
            uint16_t color = ((red >> 3) << 11) | ((green >> 2) << 5) | (blue >> 3);

            // Draw the rectangle with the random green shade
            _tft.fillRect(i * backgroundPixelSize, y + j * backgroundPixelSize, backgroundPixelSize, backgroundPixelSize, color);
        }
    }
}

void Display::clearScreen() {
    _tft.fillScreen(ILI9341_BLACK);
}

void Display::setTimingVariable(uint32_t *timer1_overflows_32ms){
    timer1_all_overflows = timer1_overflows_32ms;
}