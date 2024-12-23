#include <IRComm.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <Nunchuk.h>

#include <Display.h>
#include <Game.h>
#include <SevenSegment.h>
#include <Timer1Overflow.h>
#include <Audio.h>

const uint16_t BAUDRATE = 9600;             //UART baud rate

//Game variables
uint16_t score = 100;

//Display pins
#define BACKLIGHT_PIN 5
#define TFT_DC 9
#define TFT_CS 10

//Create a timer object
Timer1Overflow timer1;
//Create a seven segment object
SevenSegment sevenSegment(0x21);
//Create an IR object
IRComm ir(timer1);
//Create a display object
Display display(BACKLIGHT_PIN, TFT_CS, TFT_DC, timer1, sevenSegment);
//Create a game object
Game game(ir, display, timer1);
// Create audio object
Audio audio(timer1);


//Interrupts
ISR(INT0_vect){
    ir.onReceiveInterrupt();
}

ISR(TIMER1_OVF_vect){
    timer1.onTimer1Overflow();
    audio.handleTimer1ISR();
}

ISR(TIMER0_COMPA_vect){
    ir.onTimer0CompareMatch();
}

int main(void) {
    sevenSegment.begin();
    timer1.init();
    //Serial.begin(BAUDRATE);
    ir.initialize();
    sei(); // Enable global interrupts

	// Initialize backlight
	display.init();     
	display.refreshBacklight();
	display.clearScreen();
    Nunchuk.init_nunchuck();

    ir.decodeIRMessage();

    display.drawStartMenu(); //Draw the initial screen

    audio.playSound(Audio::Sound::StartUp);
    
	while (1) {
        // Refresh the backlight (simulate brightness adjustments)
        display.refreshBacklight();
        game.buttonListener();

        if(ir.isBufferReady()){
            uint16_t data = ir.decodeIRMessage();
            game.reactToRecievedData(data, timer1.overflowCount);
        }
    }
	//never reach
	return 0;
}
