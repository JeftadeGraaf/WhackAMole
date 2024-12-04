#include "Display.h"

const uint8_t mol[8][8] = {
    {36, 13, 34, 10, 26, 38, 13, 37},
    {39, 4, 17, 6, 6, 19, 4, 2},
    {0, 23, 16, 5, 5, 3, 24, 2},
    {35, 31, 15, 14, 14, 7, 28, 30},
    {4, 21, 11, 1, 1, 11, 8, 20},
    {9, 12, 25, 1, 1, 7, 12, 18},
    {27, 8, 22, 3, 3, 10, 9, 29},
    {0, 2, 0, 33, 32, 0, 0, 0},
};
const uint8_t mol_palette[120] = {
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

const uint8_t hol[8][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 4, 4, 14, 13, 9},
    {7, 7, 10, 5, 1, 5, 6, 15},
    {2, 2, 1, 1, 1, 1, 6, 8},
    {11, 2, 3, 1, 1, 3, 8, 12},
};
const uint8_t hol_palette[48] = {
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

const uint8_t hamer[8][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {22, 23, 16, 0, 0, 0, 0, 0},
    {24, 17, 14, 0, 0, 0, 0, 0},
    {1, 20, 13, 4, 5, 3, 2, 12},
    {1, 18, 15, 10, 9, 8, 6, 7},
    {21, 19, 25, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
};
const uint8_t hamer_palette[78] = {
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

// Draw a pixelarray with the palette
void Display::drawPixelArray(const uint8_t pixels[8][8], const uint8_t palette[], uint8_t pixelSize, int xStart, int yStart) {
  // Iterate through each pixel in the 8x8 pixel array
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            // Get the pixel index from the array
            uint8_t pixelIndex = pixels[y][x];

            // If the pixel is black (assuming black is represented by 0 in the palette)
            if (pixelIndex == 0) {
                continue; // Skip drawing black pixels
            }

            // Get the corresponding color from the palette
            uint8_t red = palette[pixelIndex * 3];
            uint8_t green = palette[pixelIndex * 3 + 1];
            uint8_t blue = palette[pixelIndex * 3 + 2];

            // Calculate the position where the pixel will be drawn
            int xPos = xStart + x * pixelSize;
            int yPos = yStart + y * pixelSize;

            // Draw the pixel (or rectangle for the given pixel size)
            _tft.fillRect(xPos, yPos, pixelSize, pixelSize, _tft.color565(red, green, blue));
        }
    }
}

//TODO tekenen dynamische molshopen
void Display::drawGame(uint8_t heaps){
    _tft.fillRect(0, 0, SCREEN_WIDTH, 37, SKY_BLUE);
    drawPixelField(37);

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

        drawPixelArray(mol, mol_palette, 4, 100, 100);
        drawPixelArray(hol, hol_palette, 4, 100, 160);
        drawPixelArray(hamer, hamer_palette, 4, 150, 100);
}

//TODO tijd afnemen
//TODO score incrementen
void Display::updateGame(uint8_t score){
    _tft.setFont(&InriaSans_Regular8pt7b);
    _tft.setTextSize(1);

    _tft.setTextColor(SKY_BLUE);
        _tft.setCursor(2, 30);
        _tft.print(String(time));
        
        text = String(oldScore);
        calcCenterScreenText(text, 1);
        _tft.setCursor(SCREEN_WIDTH - textWidth - 2, 30);
        _tft.print(text);

    _tft.setTextColor(ILI9341_BLACK);
        _tft.setCursor(2, 30);
        _tft.print(String(time--)); //Nieuwe tijd meegeven
        
        text = String(score);
        calcCenterScreenText(text, 1);
        _tft.setCursor(SCREEN_WIDTH - textWidth - 2, 30);
        _tft.print(text);
}

//TODO knoppen reageren
void Display::drawGameOverMenu(uint8_t player_score, uint8_t opponent_score, bool mol_win){
    _tft.fillRect(0, 0, SCREEN_WIDTH, 37, SKY_BLUE);
    drawPixelField(37);

    _tft.setTextColor(ILI9341_BLACK);

    _tft.setFont(&IrishGrover_Regular8pt7b);
        text = "Whack a Mole";
        calcCenterScreenText(text, 2);
        _tft.setCursor(x, 30);
        _tft.print(text);

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

    if(mol_win){
        drawPixelArray(mol, mol_palette, 8, 150, 150);
        drawPixelArray(hol, hol_palette, 8, 150, 160);
        drawPixelArray(hamer, hamer_palette, 8, 230, 150);
    } else {
        drawPixelArray(hol, hol_palette, 8, 180, 160);
        drawPixelArray(hamer, hamer_palette, 8, 200, 150);
    }
}

//TODO geselecteerd knop duidelijk maken
//TODO knoppen reageren
void Display::drawStartMenu(){
    _tft.fillRect(0, 0, SCREEN_WIDTH, 155, SKY_BLUE);
    drawPixelField(155);

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

    drawPixelArray(mol, mol_palette, 10, 200, 50);
    drawPixelArray(hol, hol_palette, 10, 200, 130);
}

//TODO geselecteerd karakter duidelijk maken
//Todo selectie reageert
void Display::drawChooseCharacter(){
    _tft.fillRect(0, 0, SCREEN_WIDTH, 155, SKY_BLUE);
    drawPixelField(155);

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
        text = "Mole";
        calcCenterScreenText(text, 2);
        _tft.setCursor(x / 2 - 20, 120);
        _tft.print(text);
        drawPixelArray(mol, mol_palette, 8, x / 2 - 20, 150);
        drawPixelArray(hol, hol_palette, 8, x / 2 - 20, 160);
        text = "Hammer";
        calcCenterScreenText(text, 2);
        _tft.setCursor(x * 1.5 + 20, 120);
        _tft.print(text);
        drawPixelArray(hamer, hamer_palette, 8, x * 2 + 10, 150);

}

void Display::drawHighscores(){
    _tft.fillRect(0, 0, SCREEN_WIDTH, 189, SKY_BLUE);
    drawPixelField(189);

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