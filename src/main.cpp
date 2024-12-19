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
#include <Game.h>
#include <Timer1Overflow.h>
#include <SevenSegment.h>

Timer1Overflow timer1;  // Instance of Timer1Overflow object
SevenSegment sevenSegment(0x21);  // Instance of SevenSegment object

// OCR value for Timer0, IR transmitter
// OCR2A = (Clock_freq / (2 * Prescaler * Target_freq)) - 1
const uint8_t OCR0A_value = (16000000 / (2 * 1 * 56000)) - 1;

const uint16_t BAUDRATE = 9600;             //UART baud rate

const uint8_t NUNCHUK_ADDRESS = 0x52;       //Nunchuk I2c address
const uint16_t NUNCHUCK_WAIT = 1000;        //Wait for nunchuk test function

//Game variables
uint16_t score = 100;

//Display pins
#define BACKLIGHT_PIN 5
#define TFT_DC 9
#define TFT_CS 10

// Instance of IR object
IRComm ir(timer1);
// Create display object
Display display(BACKLIGHT_PIN, TFT_CS, TFT_DC, timer1, sevenSegment);
// Create game object
Game game(ir, display, timer1);

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
    sevenSegment.begin();
    timer1.init();  // Initialize Timer1Overflow object
    ir.initialize();
    sei(); // Enable global interrupts

    // Initialize the display
    display.init();
    display.drawStartMenu();
    // Initialize the nunchuk
    Nunchuk.init_nunchuck(NUNCHUK_ADDRESS);

	while (1) {
        // Refresh the backlight (simulate brightness adjustments)
        display.refreshBacklight();

        game.buttonListener();

        //Check for IR messages
        if(ir.isBufferReady()){
            uint16_t data = ir.decodeIRMessage();
            game.reactToRecievedData(data, timer1.overflowCount);
        }

        _delay_ms(10);
    }
	//never reach
	return 0;
}

