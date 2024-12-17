#include <IRComm.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <Wire.h>
#include <HardwareSerial.h>
#include <Nunchuk.h>

#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include <Display.h>
#include <Timer1Overflow.h>

Timer1Overflow timer1;  // Instance of Timer1Overflow object

// Instance of IR object
IRComm ir(timer1);

// OCR value for Timer0, IR transmitter
// OCR2A = (Clock_freq / (2 * Prescaler * Target_freq)) - 1
const uint8_t OCR0A_value = (16000000 / (2 * 1 * 56000)) - 1;

const uint16_t BAUDRATE = 9600;             //UART baud rate

const uint8_t NUNCHUK_ADDRESS = 0x52;       //Nunchuk I2c address
const uint16_t NUNCHUCK_WAIT = 1000;        //Wait for nunchuk test function

const uint8_t NUNCHUK_DEADZONE = 30;        //Deadzone against drift
const uint8_t NUNCHUK_CENTER_VALUE = 128;   //value of x and y when joystick is idle
const uint8_t NUNCHUK_X_SENSITIVITY = 5;    //sensitivity of cursor horizontal movements
const uint8_t NUNCHUK_Y_SENSITIVITY = 5;    //sensitivity of cursor vertical movements
const uint16_t DISPLAY_MAX_X = 300;         //Max horizontal movement of cursor (right)
const uint8_t DISPLAY_MIN_X = 0;            //Min horizontal movement of cursor (left)
const uint8_t DISPLAY_MAX_Y = 220;          //Max vertical movement of cursor (down)
const uint8_t DISPLAY_MIN_Y = 0;            //Min vertical movement of cursor (up)

//Save button state
bool ZPressed;
bool CPressed;

//Game variables
uint16_t score = 100;

#define BACKLIGHT_PIN 5

#define TFT_DC 9
#define TFT_CS 10

// Create display objects
Display display(BACKLIGHT_PIN, TFT_CS, TFT_DC, timer1);

// prototypes
void buttonListener();

//Interrupts
ISR(INT0_vect){
    ir.onReceiveInterrupt();
}

ISR(TIMER1_OVF_vect){
    timer1.onTimer1Overflow();
}

ISR(TIMER0_COMPA_vect){
    ir.onTimer0CompareMatch();
}

int main(void) {

    Serial.begin(BAUDRATE);
    timer1.init();  // Initialize Timer1Overflow object
    ir.initialize();
    sei(); // Enable global interrupts
    uint16_t msg = 0b00000000000;


	while (1) {
        // Refresh the backlight (simulate brightness adjustments)
        if(ir.isBufferReady()){
            msg = ir.decodeIRMessage();
            Serial.println(msg);
        }
        _delay_ms(10);
    }
	//never reach
	return 0;
}

void buttonListener() {
    //update button state
    Nunchuk.getState(NUNCHUK_ADDRESS);
    ZPressed = Nunchuk.state.z_button;
    CPressed = Nunchuk.state.c_button;

    //Switch between different screens
    switch(display.displayedScreen) {
        case Display::game:
            display.updateGame(0, ZPressed);
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
            display.updateDifficulty(ZPressed);
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
