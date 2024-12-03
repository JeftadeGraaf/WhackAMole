#include "Display.h"


const uint32_t SCREEN_WIDTH = 320;
const uint16_t SCREEN_HEIGHT = 240;

//calcCenterScreenText function:
uint16_t x1, y1;
uint16_t textWidth, textHeight;
uint16_t x, y;
String text;

//Draw screens functions
const uint8_t pixelSize = 10;

//updateGame function
uint8_t time = 60;    //starting time
uint8_t oldScore = 0;   //starting score

const uint32_t mol[16][16] = {
    {0x00000000, 0x0A191919, 0x3B1E1919, 0x0D131327, 0x00000000, 0x06000000, 0x7A493616, 0xCF6B501B, 0xC8664C1A, 0x52372812, 0x00000000, 0x00000000, 0x19141428, 0x29181218, 0x00000000, 0x00000000},
    {0x00000000, 0x3F282424, 0xE8352F2A, 0x7B2D251A, 0x04000000, 0x96624717, 0xFF976F1D, 0xFFAB7F22, 0xFFA67B21, 0xF7886419, 0x514B3812, 0x0E000000, 0xA8362C1E, 0xC8342D2B, 0x33282323, 0x00000000},
    {0x00000000, 0x4519161D, 0xF95C4723, 0xF9886726, 0xA7402F12, 0xFE7D6027, 0xFF947A45, 0xFF5C4D34, 0xFF685637, 0xFFA08651, 0xE3614A20, 0xBB4F3A15, 0xFF94702A, 0xEC493B24, 0x2F1B1B20, 0x00000000},
    {0x00000000, 0x220F0F16, 0xEE66522D, 0xFFB08F4B, 0xFF7D5B17, 0xFFABA59D, 0xFFF8FAFE, 0xFF8E8D90, 0xFFB1B1B4, 0xFFFBFDFF, 0xFF988E7E, 0xFF8A651A, 0xFFAC8F53, 0xDC4A3B22, 0x15181818, 0x00000000},
    {0x00000000, 0x02000000, 0xA35D4619, 0xFFAC8025, 0xFF9E741E, 0xFFB1A99A, 0xFFD8D4CD, 0xFFB4AEA6, 0xFFADA69B, 0xFFCDC9C2, 0xFFA39375, 0xFFA67A1E, 0xFF9F7724, 0x74463418, 0x00000000, 0x00000000},
    {0x00000000, 0x00000000, 0x41362713, 0xFC956F21, 0xFFAD8022, 0xFF9C7524, 0xFF9B8250, 0xFF826938, 0xFF836937, 0xFF9B7F47, 0xFFA0751D, 0xFFAE8224, 0xEF896620, 0x1617170B, 0x3319140F, 0x3C191111},
    {0x3A1A1111, 0x2E1B1010, 0x00000000, 0xB9664A1A, 0xFFAD8123, 0xFFDDCAA3, 0xFFFCFCFB, 0xFFD8D5D0, 0xFFDEDCD9, 0xFFFDFCFA, 0xFFD5BE8F, 0xFFAA7E20, 0x925C4418, 0x430B0F1E, 0xF43F3B3C, 0xED363231},
    {0xFD393434, 0xF1413E3F, 0x6F19161D, 0xC5684B12, 0xFFC7A662, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFEFEFD, 0xFFBD9A52, 0xD95C4211, 0xF15C4722, 0xFF4A4541, 0xF1434043},
    {0xFF444143, 0xFF4E453A, 0xFF7A5D24, 0xFF735213, 0xFFD1BD93, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFC4AF83, 0xFF886317, 0xFFB08223, 0xFF926E23, 0xEE4F412D},
    {0xEE4C3E28, 0xFF9C7422, 0xFFA97D1F, 0xFFA17519, 0xFFCEB889, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFCDB480, 0xFFA4771A, 0xFF9C782D, 0xFFAB8027, 0xD77A5A1B},
    {0xA6634917, 0xFFA67E29, 0xFFB1955A, 0xFFA17B2D, 0xFFB79346, 0xFFFBF9F6, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFCFAF7, 0xFFB69144, 0xFFA48038, 0xFFDEC48C, 0xFFA8863D, 0xA2614614},
    {0x462F200A, 0xFD89692B, 0xFFF1D596, 0xFFC4A86B, 0xFFA0741A, 0xFFC7A969, 0xFFF4EEE2, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFF8F4EC, 0xFFCCB178, 0xFFA27619, 0xFFBEA163, 0xFFF1D597, 0xFD8E6F31, 0x4633200A},
    {0x02000000, 0xAB4D3814, 0xFFB5985B, 0xFFBEA163, 0xFFA67B22, 0xFF9B711C, 0xFFB18833, 0xFFC09F5B, 0xFFC2A361, 0xFFB78F3C, 0xFF9C721D, 0xFFA17721, 0xFFAC883D, 0xFFA98947, 0xA75D4416, 0x02000000},
    {0x00000000, 0x1800000A, 0xC24D3815, 0xFF8A6720, 0xFD7D5D1E, 0x86332613, 0xBF584015, 0xFFA3771D, 0xFFA87B1D, 0xD1604615, 0x7A342514, 0x954D3817, 0xDE72551A, 0xA7523D13, 0x1300000D, 0x00000000},
    {0x00000000, 0x00000000, 0x09000000, 0x4C241A10, 0x3F181410, 0x00000000, 0x0D000000, 0xD774561C, 0xF17B5C1D, 0x2100000F, 0x00000000, 0x00000000, 0x0B000000, 0x00000000, 0x00000000, 0x00000000},
    {0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x43221A13, 0x62271C12, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000}
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

void Display::refresh_backlight() {
    // Add code to refresh the backlight as needed
    if(!(ADCSRA & (1<<ADSC))){
        OCR0B = ADCH;
    }

    ADCSRA |= (1<<ADSC);
}

void Display::drawGraphicalCursor(int x, int y, int size, uint16_t color, const uint8_t cursor[]) {
    // Use the tft object
    _tft.drawBitmap(x, y, cursor, size, size, color);
}

void Display::drawPixelArray(const uint32_t pixelArray[16][16], uint8_t pixelSize, uint16_t startX, uint16_t startY){
    for (int y = 0; y < 16; y++) {
        for (int x = 0; x < 16; x++) {
            uint32_t color = pixelArray[y][x];

            // Extract RGBA components
            uint8_t alpha = (color >> 24) & 0xFF;  // Extract alpha (transparency)
            uint8_t r = (color >> 16) & 0xFF;      // Extract red
            uint8_t g = (color >> 8) & 0xFF;       // Extract green
            uint8_t b = color & 0xFF;              // Extract blue

            // If alpha is 0, skip drawing the pixel (transparent)
            if (alpha == 0) {
                continue;  // Skip the pixel, making it transparent
            }
            // Otherwise, draw the pixel using RGB565 color format
            _tft.fillRect(startX + x * pixelSize, startY + y * pixelSize, pixelSize, pixelSize, _tft.color565(r, g, b));
        }
    }
}


//TODO Mol of hamer tekenen
//TODO knoppen reageren
void Display::drawGameOverMenu(uint8_t player_score, uint8_t opponent_score){
    _tft.fillRect(0, 0, SCREEN_WIDTH, 37, SKY_BLUE);

    for(uint16_t j = 0; j < SCREEN_HEIGHT / pixelSize; j++){
        for (uint16_t i = 0; i < SCREEN_WIDTH / pixelSize; i++)
        {
            // Generate random RGB values biased towards green
            uint8_t red = 32 + rand() % 32;     // Red: 32 to 63 (brighter)
            uint8_t green = 200 + rand() % 56; // Green: 200 to 255 (dominant)
            uint8_t blue = 16 + rand() % 32;   // Blue: 16 to 47 (reduced range)

            // Convert to RGB565
            uint16_t color = ((red >> 3) << 11) | ((green >> 2) << 5) | (blue >> 3);

            // Draw the rectangle with the random green shade
            _tft.fillRect(i * pixelSize, 37 + j * pixelSize, pixelSize, pixelSize, color);
        }
    }

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
}

void Display::drawGame(){
    _tft.fillRect(0, 0, SCREEN_WIDTH, 37, SKY_BLUE);

    for(uint16_t j = 0; j < SCREEN_HEIGHT / pixelSize; j++){
        for (uint16_t i = 0; i < SCREEN_WIDTH / pixelSize; i++)
        {
            // Generate random RGB values biased towards green
            uint8_t red = 32 + rand() % 32;     // Red: 32 to 63 (brighter)
            uint8_t green = 200 + rand() % 56; // Green: 200 to 255 (dominant)
            uint8_t blue = 16 + rand() % 32;   // Blue: 16 to 47 (reduced range)

            // Convert to RGB565
            uint16_t color = ((red >> 3) << 11) | ((green >> 2) << 5) | (blue >> 3);

            // Draw the rectangle with the random green shade
            _tft.fillRect(i * pixelSize, 37 + j * pixelSize, pixelSize, pixelSize, color);
        }
    }

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

        drawPixelArray(mol, 6, 100, 100);
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
        _tft.print(String(time--));
        
        text = String(score);
        calcCenterScreenText(text, 1);
        _tft.setCursor(SCREEN_WIDTH - textWidth - 2, 30);
        _tft.print(text);
}

void Display::calcCenterScreenText(String text, uint8_t textSize){
    _tft.setTextSize(textSize);
    _tft.getTextBounds(text, 0, 0, &x1, &y1, &textWidth, &textHeight);
    // Center the text on the screen
    x = (SCREEN_WIDTH - textWidth) / 2;
    y = (SCREEN_HEIGHT - textHeight) / 2;
}

void Display::clearScreen() {
    _tft.fillScreen(ILI9341_BLACK);
}