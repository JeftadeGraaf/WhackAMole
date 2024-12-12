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

// Instance of IR object
IRComm ir;

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
Display display(BACKLIGHT_PIN, TFT_CS, TFT_DC);

// prototypes
bool nunchuck_show_state_TEST();    //!Print Nunchuk state for tests !USES NUNCHUK_WAIT DELAY!
bool init_nunchuck();               //Initialise connection to nunchuk
void buttonListener();

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
    uint16_t msg = 0b00000000000;

    Game game = Game(ir);

	// Initialize backlight
	display.init();     
	display.refreshBacklight();
	display.clearScreen();
    init_nunchuck();

    // pass the timer1 overflow variable from the IR protocol to the Display lib
    uint32_t* timer1_overflow_count = ir.getOverflowCountPtr();
    display.setTimingVariable(timer1_overflow_count);

    display.drawStartMenu();

	while (1) {
        // Refresh the backlight (simulate brightness adjustments)
        display.refreshBacklight();

        buttonListener();

        if(ir.isBufferReady()){
            uint16_t data = ir.decodeIRMessage();
            Serial.print("Received data: ");
            Serial.println(data);
            msg = data + 1;
            //_delay_ms(200);
        } else {
            // msg++;
            ir.sendFrame(msg);
        }
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
