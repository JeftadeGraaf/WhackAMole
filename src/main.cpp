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
#include <Audio.h>

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
IRComm ir;
// Create display object
Display display(BACKLIGHT_PIN, TFT_CS, TFT_DC);
// Create game object
Game game(ir, display);
// Create audio object
Audio audio;

uint16_t recievedData; //!TEMP recieved data

//local functions
bool nunchuck_show_state_TEST();    //!Print Nunchuk state for tests !USES NUNCHUK_WAIT DELAY!
bool init_nunchuck();               //Initialise connection to nunchuk
void init_IR_transmitter_timer0();  //initialise Timer0 for IR transmitter


//Interrupts
ISR(INT0_vect){
    ir.onReceiveInterrupt();
}

ISR(TIMER1_OVF_vect){
    audio.handleTimer1ISR();
    ir.onTimer1Overflow();
}

ISR(TIMER0_COMPA_vect){
    ir.onTimer0CompareMatch();
}

int main(void) {
    Serial.begin(BAUDRATE);
    ir.initialize();
    sei(); // Enable global interrupts
    // uint16_t msg = 0b00000000000;

	// Initialize backlight
	display.init();     
	display.refreshBacklight();
	display.clearScreen();
    init_nunchuck();

    audio.init();
    audio.playSound(Audio::Sound::StartUp);

    // pass the timer1 overflow variable from the IR protocol to the Display lib
    uint32_t* timer1_overflow_count = ir.getOverflowCountPtr();
    display.setTimingVariable(timer1_overflow_count);
    audio.setTimingVariable(timer1_overflow_count);

    display.drawStartMenu();
    
	while (1) {
        // Refresh the backlight (simulate brightness adjustments)
        display.refreshBacklight();

        game.buttonListener();

        // if(ir.isBufferReady()){
        //     uint16_t data = ir.decodeIRMessage();
        //     game.reactToRecievedData(data, *timer1_overflow_count);
        // } else {
        //     game.sendStart(false, Display::Difficulty::nine);
        // }

        // _delay_ms(10);

        audio.playSound(Audio::Sound::ThemeSong1);
    }
	//never reach
	return 0;
}

//Init nunchuk
bool init_nunchuck(){
	Serial.print("-------- Connecting to nunchuk at address 0x");
	Serial.println(NUNCHUK_ADDRESS, HEX);

    //Make connection to Nunchuk
	if (!Nunchuk.begin(NUNCHUK_ADDRESS)) {
        //If nunchuk is not found, print error and return false
		Serial.println("******** No nunchuk found");
		Serial.flush();
		return(false);
	}
    //After succesful handshake, print Nunchuk ID
	Serial.print("-------- Nunchuk with Id: ");
	Serial.println(Nunchuk.id);
	return true;
}

//Nunchuk test function
bool nunchuck_show_state_TEST() {
    //Print Nunchuk state
	if (!Nunchuk.getState(NUNCHUK_ADDRESS)) {
        //If nunchuk is not found, print error and return false
		Serial.println("******** No nunchuk found");
		Serial.flush();
		return (false);
	}
    Serial.println("------State data--------------------------");
    Serial.print("Joy X: ");
    Serial.print(Nunchuk.state.joy_x_axis);
    Serial.print("\t\tButton C: ");
    Serial.println(Nunchuk.state.c_button);

    Serial.print("Joy Y: ");
    Serial.print(Nunchuk.state.joy_y_axis);
    Serial.print("\t\tButton Z: ");
    Serial.println(Nunchuk.state.z_button);

		// wait a while
		_delay_ms(NUNCHUCK_WAIT);

		return(true);
}

