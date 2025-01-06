#include "Display.h"
#include "Nunchuk.h"

uint32_t gameTimeTracker = 0;

// Combined palette in direct hex values (RGB565 format)
const uint16_t combined_palette[] PROGMEM = {
    0x0000,  // 0  - Black
    0xFFFF,  // 1  - White
    0x1082,  // 2  - Grayish Blue
    0xA427,  // 3  - Light Brown
    0x6AA5,  // 4  - Grayish Teal
    0xAD11,  // 5  - Pink-Brown
    0xA4F0,  // 6  - Mauve
    0xCD8F,  // 7  - Light Pink
    0x9BA6,  // 8  - Grayish Purple
    0x8B65,  // 9  - Dull Purple
    0x82E3,  // 10 - Gray-Brown
    0xE6D8,  // 11 - Light Pink-White
    0xC52B,  // 12 - Pink
    0x3144,  // 13 - Dark Gray-Blue
    0xEF5C,  // 14 - Light Pink
    0xD5D1,  // 15 - Light Gray-Pink
    0xB489,  // 16 - Grayish Pink
    0x8BA9,  // 17 - Gray-Purple
    0x9386,  // 18 - Dull Pink
    0x8368,  // 19 - Gray
    0x8325,  // 20 - Dark Gray
    0x8B23,  // 21 - Purple-Gray
    0x8303,  // 22 - Dark Gray
    0xCD6E,  // 23 - Light Pink
    0x7AC2,  // 24 - Gray-Blue
    0x5204,  // 25 - Dark Blue-Gray
    0x5A02,  // 26 - Dark Gray-Blue
    0x5A43,  // 27 - Gray-Blue
    0x41C7,  // 28 - Dark Blue
    0x51E2,  // 29 - Blue-Gray
    0x59E1,  // 30 - Gray-Blue
    0x49A0,  // 31 - Dark Gray-Blue
    0x3166,  // 32 - Very Dark Blue
    0x2124,  // 33 - Darkest Blue
    0x3920,  // 34 - Dark Blue-Black
    0x20C3,  // 35 - Almost Black Blue
    0x6A20,  // 36 - Medium Gray-Blue
    0x59E0,  // 37 - Light Gray-Blue
    0x6A41,  // 38 - Blue-Tinted Gray
    0x4160,  // 39 - Dark Gray-Blue
    0x3120,  // 40 - Very Dark Blue
    0x3180,  // 41 - Dark Blue-Gray
    0x6221,  // 42 - Medium Blue-Gray
    0x4180,  // 43 - Dark Gray-Blue
    0x3920,  // 44 - Very Dark Blue
    0x28E0,  // 45 - Darkest Gray-Blue
    0x3800,  // 46 - Almost Black Blue
    0xFE42,  // 47 - Near White
    0xFDA3,  // 48 - Very Light Pink
    0xFD03,  // 49 - Light Pink-White
    0xFD23,  // 50 - Light Pink
    0xFCA3,  // 51 - Light Pink-Gray
    0xF4A3,  // 52 - Light Gray-Pink
    0xF463,  // 53 - Gray-Pink
    0xF423,  // 54 - Pink-Gray
    0xFBE3,  // 55 - Very Light Pink
    0xF986,  // 56 - Light Gray-Pink
    0xEEA2,  // 57 - Gray-Pink
    0xAB45,  // 58 - Medium Gray
    0x938B,  // 59 - Dark Gray
    0x8287,  // 60 - Very Dark Gray
    0xFFFB,  // 61 - Almost White
    0x7226,  // 62 - Medium Gray
    0x5984,  // 63 - Dark Gray-Blue
    0x59C4,  // 64 - Blue-Gray
    0x5123,  // 65 - Dark Blue-Gray
    0x5000,  // 66 - Very Dark Blue
    0x38E0,  // 67 - Darkest Blue
    0xAC4E,  // 68 - Light Gray
    0x02AA   // 69 - Very Dark Blue-Green
};

// Mole sprite (8x8)
const uint8_t mole[8][8] PROGMEM = {
    {0, 13, 29, 10, 19, 32, 13, 0},   // Row 0
    {33, 4, 17, 6, 6, 17, 4, 2},      // Row 1
    {0, 21, 16, 5, 5, 3, 21, 2},      // Row 2
    {30, 27, 15, 14, 14, 7, 24, 26},  // Row 3
    {4, 21, 11, 1, 1, 11, 8, 20},     // Row 4
    {9, 12, 7, 1, 1, 7, 12, 18},      // Row 5
    {23, 8, 22, 3, 3, 10, 9, 25},     // Row 6
    {0, 2, 0, 28, 28, 0, 0, 0}        // Row 7
};

// Hole sprite (4x8)
const uint8_t hole[4][8] PROGMEM = {
    {0, 0, 0, 38, 38, 40, 43, 40},    // Row 0
    {37, 37, 42, 36, 36, 36, 39, 45}, // Row 1
    {37, 37, 36, 36, 36, 36, 39, 40}, // Row 2
    {43, 37, 37, 36, 36, 37, 40, 43}  // Row 3
};

// Hammer horizontal sprite (5x8)
const uint8_t hammerHori[5][8] PROGMEM = {
    {66, 64, 69, 0, 0, 0, 0, 0},      // Row 0
    {46, 63, 60, 55, 54, 53, 51, 52}, // Row 1
    {46, 65, 58, 49, 50, 48, 47, 57}, // Row 2
    {46, 62, 59, 0, 0, 0, 0, 0},      // Row 3
    {67, 68, 61, 0, 0, 0, 0, 0}       // Row 4
};

// Hammer vertical sprite (8x5)
const uint8_t hammerVert[8][5] PROGMEM = {
    {67, 46, 46, 46, 66},             // Row 0
    {68, 62, 65, 63, 64},             // Row 1
    {61, 59, 58, 60, 69},             // Row 2
    {0, 0, 49, 55, 0},                // Row 3
    {0, 0, 50, 54, 0},                // Row 4
    {0, 0, 48, 53, 0},                // Row 5
    {0, 0, 47, 51, 0},                // Row 6
    {0, 0, 57, 52, 0}                 // Row 7
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
         // OCR0B = ADCH;
    // }

    // ADCSRA |= (1<<ADSC);
}

void Display::drawPixelArray(const uint8_t *pixels PROGMEM, uint8_t pixelSize, 
                           int xStart, int yStart, int xSize, int ySize) {
    for (int y = 0; y < ySize; y++) {
        for (int x = 0; x < xSize; x++) {
            // Read pixel from PROGMEM
            uint8_t pixelIndex = pgm_read_byte(pixels + y * xSize + x);
            
            if (pixelIndex == 0) continue; // Skip black pixels

            int xPos = xStart + x * pixelSize;
            int yPos = yStart + y * pixelSize;
            
            // Use pre-converted color directly from the palette
            _tft.fillRect(xPos, yPos, pixelSize, pixelSize, pgm_read_word(&combined_palette[pixelIndex]));
        }
    }
}

void Display::drawGame(Difficulty selectedDifficulty){
    displayedScreen = game;
    gameOverUpdated = false;
    this->characterMole = characterMole;

    timer1.resetOverflow();
    gameTimeTracker = 0;
    time = 60;

    //Draw sky and field
    _tft.fillRect(0, 0, SCREEN_WIDTH, 37, SKY_BLUE);
    drawPixelField(40);

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

    if(!characterMole){
        drawOrRemoveHammer(selectedHeap, true, false);
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
        drawPixelArray(*mole, multiplySize, xPos, yPos, 8, 8);
    } else {
        redrawBackGround(xPos, yPos, selectWidthHeight, selectWidthHeight);
    }
    drawPixelArray(*hole, multiplySize, xPos, yPos + 4*multiplySize, 8, 4);
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
            redrawBackGround(xPos  + (picturePixelSize * multiplySize), yPos, (picturePixelSize * multiplySize), (picturePixelSize * multiplySize));
            drawPixelArray(*hole, multiplySize, xPos, yPos + 4*multiplySize, 8, 4);
        }
        else{
            drawPixelArray(*hole, multiplySize, xPos, yPos + 4*multiplySize, 8, 4);
            redrawBackGround(xPos + (2 * multiplySize), yPos - multiplySize, (picturePixelSize * multiplySize), (picturePixelSize * multiplySize));
        }
    }
}

void Display::drawOrRemoveHole(uint8_t heapNumber, bool draw) {
    uint16_t xPos, yPos;
    calculateHeapPosition(heapNumber, xPos, yPos);

    if (draw) {
        drawPixelArray(*hole, multiplySize, xPos, yPos + 4*multiplySize, 8, 4);
    } else {
        redrawBackGround(xPos, yPos, selectWidthHeight, selectWidthHeight);
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
    _tft.fillRect(0, 0, SCREEN_WIDTH, 150, SKY_BLUE);
    drawPixelField(150);

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
        drawPixelArray(*hole, 8, moleTextXCoor, 190, 8, 4);

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
    // Selection logic with debounce
    if(timer1.joystickDebounceCount > 5){
        if (Nunchuk.state.joy_x_axis > Nunchuk.centerValue + Nunchuk.deadzone && characterMole == true) {
            characterMole = false; // Move right
            _tft.drawRect(x1 - 4, y1 - 4, textWidth + 8, textHeight + 8, SKY_BLUE);
            timer1.resetJoystickDebounce(); // Update last move time
        } else if (Nunchuk.state.joy_x_axis < Nunchuk.centerValue - Nunchuk.deadzone && characterMole == false) {
            characterMole = true; // Move left
            _tft.drawRect(x1 - 4, y1 - 4, textWidth + 8, textHeight + 8, SKY_BLUE);
            timer1.resetJoystickDebounce(); // Update last move time
        }
    }

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
    _tft.fillRect(0, 0, SCREEN_WIDTH, 40, SKY_BLUE);
    drawPixelField(40);

    //Write text
    _tft.setFont(&IrishGrover_Regular8pt7b);
        _tft.setCursor(calcCenterScreenText(title, 2), 30);
        _tft.print(title);

    _tft.setFont(&InriaSans_Regular8pt7b);
        _tft.setTextSize(3);
        _tft.setCursor(30, 80);
        _tft.print("4 holes");

        _tft.setCursor(30, 130);
        _tft.print("9 holes");

        _tft.setCursor(30, 180);
        _tft.print("16 holes");

    _tft.fillCircle(difficultyCircleX, difficultyCircleY, 5, ILI9341_BLACK);

    drawPixelArray(*mole, 10, 210, 50, 8, 8);
    drawPixelArray(*hole, 10, 210, 170, 8, 4);
}

void Display::drawStartMenu(){
    sevenSegment.displayDigit(10);
    displayedScreen = startMenu;
    //Draw sky and field
    _tft.fillRect(0, 0, SCREEN_WIDTH, 150, SKY_BLUE);
    drawPixelField(150);

    _tft.fillCircle(startCircleX, startCircleY, 5, ILI9341_BLACK);

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
    // Selection logic with debounce
    if (timer1.joystickDebounceCount > 5) {
        // Moving down
        if (Nunchuk.state.joy_y_axis < Nunchuk.centerValue - Nunchuk.deadzone && startButtonSelected == true) {
            // Clear the current selection
            _tft.fillCircle(startCircleX, startCircleY, 5, SKY_BLUE);

            // Update the selector's position and state
            startCircleY += 35; // Move the selector down
            startButtonSelected = false;

            // Redraw the selector in the new position
            _tft.fillCircle(startCircleX, startCircleY, 5, ILI9341_BLACK);

            // Reset the debounce counter
            timer1.resetJoystickDebounce();
        }
        // Moving up
        else if (Nunchuk.state.joy_y_axis > Nunchuk.centerValue + Nunchuk.deadzone && startButtonSelected == false) {
            // Clear the current selection
            _tft.fillCircle(startCircleX, startCircleY, 5, SKY_BLUE);

            // Update the selector's position and state
            startCircleY -= 35; // Move the selector up
            startButtonSelected = true;

            // Redraw the selector in the new position
            _tft.fillCircle(startCircleX, startCircleY, 5, ILI9341_BLACK);

            // Reset the debounce counter
            timer1.resetJoystickDebounce();
        }
    }



    // Button press handling
    if (buttonPressed && startButtonSelected) {
        drawChooseCharacter();
    } else if (buttonPressed && !startButtonSelected) {
        drawHighscores();
    }
}

void Display::drawGameOverMenu(){
    displayedScreen = gameOver;
    //Draw sky and field
    _tft.fillRect(0, 0, SCREEN_WIDTH, 40, SKY_BLUE);
    drawPixelField(40);

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

        text = "Your score: ";
        calcCenterScreenText(text, 1);
        _tft.setCursor(SCREEN_WIDTH/2 - textWidth, 120);
        _tft.print(text);
    
        _tft.setCursor(SCREEN_WIDTH/2 + 10, 120); 
        _tft.print(player_score);      

        text = "Opponents score: ";
        calcCenterScreenText(text, 1);
        _tft.setCursor(SCREEN_WIDTH/2 - textWidth, 136);
        _tft.print(text);

        _tft.setCursor(SCREEN_WIDTH/2 + 10, 136); 
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
    sevenSegment.displayDigit(10);
}

//TODO highscores opslaan en displayen (EEPROM)
void Display::drawHighscores(){
    displayedScreen = highscores;
    //Draw sky and field
    _tft.fillRect(0, 0, SCREEN_WIDTH, 190, SKY_BLUE);
    drawPixelField(190);

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

int Display::calcCenterScreenText(const char* text, uint8_t textSize){
    _tft.setTextSize(textSize);
    _tft.getTextBounds(text, 0, 0, &x1, &y1, &textWidth, &textHeight);
    // Center the text on the screen
    return ((SCREEN_WIDTH - textWidth) / 2);
}

void Display::drawPixelField(uint8_t y){
    for(uint16_t j = y / backgroundPixelSize; j < SCREEN_HEIGHT / backgroundPixelSize; j++){
        uint64_t row = backgroundBitmap[j];
        for (uint16_t i = 0; i < SCREEN_WIDTH / backgroundPixelSize; i++){
           
            uint64_t mask = 0x3ULL << (62 - i * 2); // Create a mask with 2 bits set at the desired index
            uint8_t extractedBits = (row & mask) >> (62 - i * 2); // Extract the 2 bits using the mask and shift them to the right

            uint16_t color  = greenBiasedColors[extractedBits];

            // Draw the rectangle with the random green shade
            _tft.fillRect(i * backgroundPixelSize, j * backgroundPixelSize, backgroundPixelSize, backgroundPixelSize, color);
        }
    }
}

void Display::redrawBackGround(uint16_t x, uint8_t y, uint8_t width, uint8_t height) {
    int xRounded = ((x-2) / 10) * 10; // Round down to the nearest 10
    int yRounded = ((y-2) / 10) * 10; // Round down to the nearest 10

    int xRounded2 = ((x-2 + width + 4 + 9) / 10) * 10; // Round up to the nearest 10
    int yRounded2 = ((y-2 + height + 4 + 9) / 10) * 10; // Round up to the nearest 10

    width = xRounded2 - xRounded;
    height = yRounded2 - yRounded;

    // Iterate over the grid based on the specified width and height
    for (uint16_t j = yRounded / backgroundPixelSize; j < (height + yRounded) / backgroundPixelSize; j++) {
        // Ensure we don't go out of bounds of backgroundBitmap
        if (j >= 24) break;

        uint64_t row = backgroundBitmap[j];

        for (uint16_t i = xRounded / backgroundPixelSize; i < (width + xRounded) / backgroundPixelSize; i++) {
            // Calculate square's top-left corner position using the original method
            uint16_t xPos = i * backgroundPixelSize;
            uint16_t yPos = j * backgroundPixelSize;

            uint64_t mask = 0x3ULL << (62 - i * 2); // Exact same mask as original
            uint8_t extractedBits = (row & mask) >> (62 - i * 2); // Exact same bit extraction

            uint16_t color = greenBiasedColors[extractedBits];

            // Draw the square
            _tft.fillRect(xPos, yPos, backgroundPixelSize, backgroundPixelSize, color);
        }
    }
}

void Display::clearScreen() {
    _tft.fillScreen(ILI9341_BLACK);
}