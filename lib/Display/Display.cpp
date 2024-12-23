#include "Display.h"
#include "Nunchuk.h"
#include "Audio.h"

uint32_t gameTimeTracker = 0;
uint32_t gameStartTime = 0;

// Combined palette converted to RGB565 format
const uint16_t combined_palette[82] = {
    0x0000,     // 0  - Black (shared)
    0xFFFF,     // 1  - Original mole[1]
    0x1082,     // 2  - Original mole[2]
    0xA427,     // 3  - Original mole[3]
    0x6AA5,     // 4  - Original mole[4]
    0xAD11,     // 5  - Original mole[5]
    0xA4F0,     // 6  - Original mole[6]
    0xCD8F,     // 7  - Original mole[7]
    0x9BA6,     // 8  - Original mole[8]
    0x8B65,     // 9  - Original mole[9]
    0x82E3,     // 10 - Original mole[10]
    0xE6D8,     // 11 - Original mole[11]
    0xC52B,     // 12 - Original mole[12]
    0x3144,     // 13 - Original mole[13]
    0xEF5C,     // 14 - Original mole[14]
    0xD5D1,     // 15 - Original mole[15]
    0xB489,     // 16 - Original mole[16]
    0x8BA9,     // 17 - Original mole[17]
    0x9386,     // 18 - Original mole[18]
    0x8368,     // 19 - Original mole[19]
    0x8325,     // 20 - Original mole[20]
    0x8B23,     // 21 - Original mole[21]
    0x8303,     // 22 - Original mole[22]
    0x8B23,     // 23 - Original mole[23]
    0x82E3,     // 24 - Original mole[24]
    0xCD6E,     // 25 - Original mole[25]
    0x7AC2,     // 26 - Original mole[26]
    0x5204,     // 27 - Original mole[27]
    0x5A02,     // 28 - Original mole[28]
    0x5A43,     // 29 - Original mole[29]
    0x41C7,     // 30 - Original mole[30]
    0x51E2,     // 31 - Original mole[31]
    0x59E1,     // 32 - Original mole[32]
    0x59E1,     // 33 - Original mole[33]
    0x49A0,     // 34 - Original mole[34]
    0x3166,     // 35 - Original mole[35]
    0x2124,     // 36 - Original mole[36]
    0x2124,     // 37 - Original mole[37]
    0x3920,     // 38 - Original mole[38]
    0x20C3,     // 39 - Original mole[39]
    0x0000,     // 40 - Original mole[0]
    0x6A20,     // 41 - Original hole[1]
    0x59E0,     // 42 - Original hole[2]
    0x59E0,     // 43 - Original hole[3]
    0x6A41,     // 44 - Original hole[4]
    0x6A20,     // 45 - Original hole[5]
    0x4160,     // 46 - Original hole[6]
    0x59E1,     // 47 - Original hole[7]
    0x3120,     // 48 - Original hole[8]
    0x3180,     // 49 - Original hole[9]
    0x6221,     // 50 - Original hole[10]
    0x4180,     // 51 - Original hole[11]
    0x3120,     // 52 - Original hole[12]
    0x3120,     // 53 - Original hole[13]
    0x3920,     // 54 - Original hole[14]
    0x28E0,     // 55 - Original hole[15]
    0x3800,     // 56 - Original hammer[1]
    0xFE42,     // 57 - Original hammer[2]
    0xFDA3,     // 58 - Original hammer[3]
    0xFD03,     // 59 - Original hammer[4]
    0xFD23,     // 60 - Original hammer[5]
    0xFCA3,     // 61 - Original hammer[6]
    0xF4A3,     // 62 - Original hammer[7]
    0xF463,     // 63 - Original hammer[8]
    0xF423,     // 64 - Original hammer[9]
    0xFBE3,     // 65 - Original hammer[10]
    0xF986,     // 66 - Original hammer[11]
    0xEEA2,     // 67 - Original hammer[12]
    0xAB45,     // 68 - Original hammer[13]
    0x938B,     // 69 - Original hammer[14]
    0x8287,     // 70 - Original hammer[15]
    0xFFFB,     // 71 - Original hammer[16]
    0x7226,     // 72 - Original hammer[17]
    0x5984,     // 73 - Original hammer[18]
    0x59C4,     // 74 - Original hammer[19]
    0x5123,     // 75 - Original hammer[20]
    0x5000,     // 76 - Original hammer[21]
    0x38E0,     // 77 - Original hammer[22]
    0xAC4E,     // 78 - Original hammer[23]
    0x3800,     // 79 - Original hammer[24]
    0x02AA      // 80 - Original hammer[25]
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
Display::Display(int backlight_pin, int tft_cs, int tft_dc, Timer1Overflow &timer1, SevenSegment &sevenSegment)
    : _tft(tft_cs, tft_dc), sevenSegment(sevenSegment), timer1(timer1) {
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
    _tft.setTextColor(ILI9341_BLACK);
    _tft.setFont(&InriaSans_Regular8pt7b);
}

void Display::refreshBacklight() {
    PORTD |= (1<<_backlight_pin);
    // if(!(ADCSRA & (1<<ADSC))){
    //     // OCR0B = ADCH;
    // }

    // ADCSRA |= (1<<ADSC);
}

// Draw a pixelarray with its corresponding palette
void Display::drawPixelArray(const uint8_t *pixels, uint8_t pixelSize, 
                            int xStart, int yStart, int xSize, int ySize) {
    for (int y = 0; y < ySize; y++) {
        for (int x = 0; x < xSize; x++) {
            uint8_t pixelIndex = *(pixels + y * xSize + x);
            
            if (pixelIndex == 0) continue; // Skip black pixels

            int xPos = xStart + x * pixelSize;
            int yPos = yStart + y * pixelSize;
            
            // Use pre-converted color directly from the palette
            _tft.fillRect(xPos, yPos, pixelSize, pixelSize, combined_palette[pixelIndex]);
        }
    }
}

void Display::drawGame(Difficulty selectedDifficulty){
    displayedScreen = game;
    gameOverUpdated = false;
    this->characterMole = characterMole;

    timer1.resetOverflow();
    gameTimeTracker = 0;
    // gameStartTime = timer1.overflowCount;
    time = 60;

    //Draw sky and field
    _tft.fillRect(0, 0, SCREEN_WIDTH, 37, SKY_BLUE);
    drawPixelField(37);

    //Write text
    _tft.setFont(&IrishGrover_Regular8pt7b);
        _tft.setCursor(calcCenterScreenText(title, 2), 30);
        _tft.print(title);

    _tft.setFont(&InriaSans_Regular8pt7b);
        _tft.setTextSize(1);
        _tft.setCursor(2, 15);
        _tft.print("Time");
        
        _tft.getTextBounds(scoreText, 0, 0, &x1, &y1, &textWidth, &textHeight);
        // Center the text on the screen
        _tft.setCursor(SCREEN_WIDTH - textWidth - 2, 15);
        _tft.print(scoreText);

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
        drawPixelArray(*mole, multiplySize, xPos, yPos, 8, 8);
        drawPixelArray(*hole, multiplySize, xPos, yPos + 4*multiplySize, 8, 4);
    } else {
        _tft.fillRect(xPos, yPos, selectWidthHeight, selectWidthHeight, ILI9341_GREEN);
        drawPixelArray(*hole, multiplySize, xPos, yPos + 4*multiplySize, 8, 4);
    }
}

void Display::drawOrRemoveHammer(uint8_t heapNumber, bool draw, bool horizontal) {
    uint16_t xPos, yPos;
    calculateHeapPosition(heapNumber, xPos, yPos);

    if (draw) {
        if(!horizontal){
            drawPixelArray(*hammerVert, multiplySize, xPos + (picturePixelSize * multiplySize), yPos, 5, 8);
        }
        else{
            drawPixelArray(*hammerHori, multiplySize, xPos + (2 * multiplySize), yPos, 8, 5);
        }
    } else {
        if(!horizontal){
            _tft.fillRect(xPos  + (picturePixelSize * multiplySize), yPos, (picturePixelSize * multiplySize), (picturePixelSize * multiplySize), ILI9341_GREEN);
            drawPixelArray(*hole, multiplySize, xPos, yPos + 4*multiplySize, 8, 4);
        }
        else{
            drawPixelArray(*hole, multiplySize, xPos, yPos + 4*multiplySize, 8, 4);
            _tft.fillRect(xPos + (2 * multiplySize), yPos - multiplySize, (picturePixelSize * multiplySize), (picturePixelSize * multiplySize), ILI9341_GREEN);
        }
    }
}

void Display::drawOrRemoveHole(uint8_t heapNumber, bool draw) {
    uint16_t xPos, yPos;
    calculateHeapPosition(heapNumber, xPos, yPos);

    if (draw) {
        drawPixelArray(*hole, multiplySize, xPos, yPos + 4*multiplySize, 8, 4);
    } else {
        _tft.fillRect(xPos, yPos, selectWidthHeight, selectWidthHeight, ILI9341_GREEN);
    }
}

void Display::updateGameTimeScore(uint8_t score){
        _tft.setTextSize(1);
        //Remove old text
        _tft.setTextColor(SKY_BLUE);
        _tft.setCursor(2, 30);
        _tft.print(time);
        
        _tft.setCursor(SCREEN_WIDTH - textWidth - 2, 30);
        _tft.print(oldScore);

    // update time variable
    if (timer1.overflowCount - gameTimeTracker > 30) {
        time--;
        gameTimeTracker = timer1.overflowCount;
        if(time < 10){
            sevenSegment.displayDigit(time);
        }
    }

        //Write new text
        _tft.setTextColor(ILI9341_BLACK);
        _tft.setCursor(2, 30);
        _tft.print(time);
        
        _tft.setCursor(SCREEN_WIDTH - textWidth - 2, 30);
        _tft.print(score);

    oldScore = score;
}

void Display::drawChooseCharacter(){
    displayedScreen = chooseCharacter;
    //Draw sky and field
    _tft.fillRect(0, 0, SCREEN_WIDTH, 155, SKY_BLUE);
    drawPixelField(155);

    //Write text
    _tft.setFont(&IrishGrover_Regular8pt7b);
        _tft.setCursor(calcCenterScreenText(chooseYour, 2), 40);
        _tft.print(chooseYour);

        _tft.setCursor(calcCenterScreenText(character, 2), 80);
        _tft.print(character);

    _tft.setFont(&InriaSans_Regular8pt7b);
        textYCoor = 120;

        moleTextXCoor = calcCenterScreenText(moleText, 2) - 70;
        _tft.setCursor(moleTextXCoor, textYCoor);
        _tft.print(moleText);
        //Draw mole character
        drawPixelArray(*mole, 8, moleTextXCoor, 150, 8, 8);
        drawPixelArray(*hole, 8, moleTextXCoor, 192, 8, 4);

        _tft.setTextSize(2);
        _tft.getTextBounds(hammerText, 0, 0, &x1, &y1, &textWidth, &textHeight);
        // Center the text on the screen
        x = (SCREEN_WIDTH - textWidth) / 2;

        hammerTextXCoor = x * 1.5 + 20;
        _tft.setCursor(hammerTextXCoor, textYCoor);
        _tft.print(hammerText);
        //Draw hammerHori character
        drawPixelArray(*hammerHori, 8, x * 2 + 10, 150, 8, 5);
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
        text = moleText;
        x = moleTextXCoor;
    }
    else{
        text = hammerText;
        x = hammerTextXCoor;
    }
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
    _tft.setFont(&IrishGrover_Regular8pt7b);
        _tft.setCursor(calcCenterScreenText(title, 2), 30);
        _tft.print(title);

    _tft.setFont(&InriaSans_Regular8pt7b);
        _tft.setTextSize(3);
        _tft.setCursor(25, 80);
        _tft.print("4 holes");

        _tft.setCursor(25, 130);
        _tft.print("9 holes");

        _tft.setCursor(25, 180);
        _tft.print("16 holes");

    drawPixelArray(*mole, 10, 210, 50, 8, 8);
    drawPixelArray(*hole, 10, 210, 170, 8, 4);
}

void Display::drawStartMenu(){
    displayedScreen = startMenu;
    //Draw sky and field
    _tft.fillRect(0, 0, SCREEN_WIDTH, 155, SKY_BLUE);
    drawPixelField(155);

    //Write text
    _tft.setFont(&IrishGrover_Regular8pt7b);
        
        _tft.setTextSize(2);
        _tft.getTextBounds(title, 0, 0, &x1, &y1, &textWidth, &textHeight);
        // Center the text on the screen
        x = (SCREEN_WIDTH - textWidth) / 2;
        
        _tft.setCursor(x, 30);
        _tft.print(title);

    _tft.setFont(&InriaSans_Regular8pt7b);
        _tft.setCursor(30, 80);
        _tft.print("Start");

        _tft.setCursor(30, 115);
        _tft.print(highscoresText);

    drawPixelArray(*mole, 10, 200, 50, 8, 8);
    drawPixelArray(*hole, 10, 200, 170, 8, 4);
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
    sevenSegment.clearDisplay();
    //Draw sky and field
    _tft.fillRect(0, 0, SCREEN_WIDTH, 37, SKY_BLUE);
    drawPixelField(37);

    //Write text
    _tft.setFont(&IrishGrover_Regular8pt7b);
        _tft.setCursor(calcCenterScreenText(title, 2), 30);
        _tft.print(title);
    _tft.setFont(&InriaSans_Regular8pt7b);
}

void Display::updateGameOver(uint8_t player_score, uint8_t opponent_score, bool mole_win){
    //If player won
    if(player_score > opponent_score){
        text = "You Won!";
    }
    else if (player_score < opponent_score){
        text = "You Lost!";
    }
    else{
        text = "It's a tie!";
    }
    _tft.setCursor(calcCenterScreenText(text, 2), 90);
    _tft.print(text);

        _tft.setCursor(calcCenterScreenText(yourScoreText, 1), 120);
        _tft.print(yourScoreText);
        _tft.print(player_score);

        _tft.setCursor(calcCenterScreenText(opponentsScoreText, 1), 136);
        _tft.print(opponentsScoreText);
        _tft.print(opponent_score);
    
    //If mole won, draw mole. Else, draw hammerHori
    if(mole_win){
        drawPixelArray(*mole, 8, 150, 150, 8, 8);
        drawPixelArray(*hole, 8, 150, 192, 8, 4);
        drawPixelArray(*hammerHori, 8, 230, 150, 8, 5);
    } else {
        drawPixelArray(*hole, 8, 180, 192, 8, 4);
        drawPixelArray(*hammerHori, 8, 200, 150, 8, 5);
    }
}

//TODO highscores opslaan en displayen (EEPROM)
void Display::drawHighscores(){
    displayedScreen = highscores;
    //Draw sky and field
    _tft.fillRect(0, 0, SCREEN_WIDTH, 189, SKY_BLUE);
    drawPixelField(189);

    //Write text
    _tft.setFont(&IrishGrover_Regular8pt7b);
        text = title;
        _tft.setCursor(calcCenterScreenText(text, 2), 35);
        _tft.print(text);
    
    _tft.drawLine(20, 70, 300, 70, ILI9341_BLACK);      //Horizontal
    _tft.drawLine(160, 55, 160, 180, ILI9341_BLACK);    //Vertical

    _tft.setFont(&InriaSans_Regular8pt7b);
        _tft.setCursor(calcCenterScreenText(highscoresText, 1), 51);
        _tft.print(highscoresText);
        text = moleText;
        _tft.setCursor(calcCenterScreenText(text, 1) * 1.5, 68);
        _tft.print(text);
        text = hammerText;
        _tft.setCursor(calcCenterScreenText(text, 1) / 2, 68);
        _tft.print(text);

        uint8_t highscoresY = 85;
        for(uint8_t i = 0; i < 5; i++){
            _tft.setCursor(24, highscoresY);
            _tft.print(123);            //TODO aanpassen naar highscore
            _tft.setCursor(90, highscoresY);    //TODO tweaken voor lengte naam
            _tft.print("opponent");             //TODO aanpassen naar speler naam

            _tft.setCursor(164, highscoresY);
            _tft.print(123);            //TODO aanpassen naar highscore
            _tft.setCursor(230, highscoresY);   //TODO tweaken voor lengte naam
            _tft.print("opponent");             //TODO aanpassen naar speler naam
            highscoresY+=18;
        }
}

int Display::calcCenterScreenText(String text, uint8_t textSize){
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