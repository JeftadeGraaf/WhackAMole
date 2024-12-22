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

// OCR value for Timer0, IR transmitter
// OCR2A = (Clock_freq / (2 * Prescaler * Target_freq)) - 1
const uint8_t OCR0A_value = (16000000 / (2 * 1 * 56000)) - 1;

const uint16_t BAUDRATE = 9600;             //UART baud rate

//Game variables
uint16_t score = 100;

//Display pins
#define BACKLIGHT_PIN 5
#define TFT_DC 9
#define TFT_CS 10

// Instance of IR object
IRComm ir;
// Create display object
Display display(BACKLIGHT_PIN, TFT_CS, TFT_DC);
// Create game object
Game game(ir, display);

uint16_t recievedData; //!TEMP recieved data

void init_IR_transmitter_timer0();  //initialise Timer0 for IR transmitter

//Interrupts
ISR(INT0_vect){
    ir.onReceiveInterrupt();
}

ISR(TIMER1_OVF_vect){
    ir.onTimer1Overflow();
}

ISR(TIMER0_COMPA_vect){
    ir.onTimer0CompareMatch();
}

int main(void) {
    Serial.begin(BAUDRATE);
    ir.initialize();
    sei(); // Enable global interrupts

	// Initialize backlight
	display.init();     
	display.refreshBacklight();
	display.clearScreen();
    NunChuk();

    ir.decodeIRMessage();

    // pass the timer1 overflow variable from the IR protocol to the Display lib
    uint32_t* timer1_overflow_count = ir.getOverflowCountPtr();
    display.setTimingVariable(timer1_overflow_count);

    display.drawStartMenu(); //Draw the initial screen
    
	while (1) {
        // Refresh the backlight (simulate brightness adjustments)
        display.refreshBacklight();

        game.buttonListener();

        if(ir.isBufferReady()){
            uint16_t data = ir.decodeIRMessage();
            game.reactToReceivedData(data, *timer1_overflow_count);
        }

        if(display.displayedScreen == Display::game){
            game.loopReceivedProcess();
        }
        
        _delay_ms(10);
    }
	//never reach
	return 0;
}

//Init IR settings
void init_IR_transmitter_timer0(){
	DDRD |= (1 << DDD6);        // IR LED output
	TCCR0B |= (1 << CS00);      // no prescaler
	TCCR0A |= (1 << WGM01);     // CTC mode (reset at OCR)
	TCCR0A |= (1 << COM0A0);    // toggle mode
	OCR0A = OCR0A_value;
}

