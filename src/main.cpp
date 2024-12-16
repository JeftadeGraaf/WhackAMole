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

//Nunchuk variables
const uint8_t NUNCHUK_ADDRESS = 0x52;       //Nunchuk I2c address
const uint16_t NUNCHUCK_WAIT = 1000;        //Wait for nunchuk test function

//Save button state
bool ZPressed;
bool CPressed;

//Game variables
uint16_t score = 100;

//For recieved data
enum process{
    startGame = 1,
    moleUp = 2,
    hammerPositionHit = 3,
    recieveScore = 4,
    invalidProcess
};

//Display variables
#define BACKLIGHT_PIN 5
#define TFT_DC 9
#define TFT_CS 10

// Create display objects
Display display(BACKLIGHT_PIN, TFT_CS, TFT_DC);

// prototypes
bool nunchuck_show_state_TEST();    //!Print Nunchuk state for tests !USES NUNCHUK_WAIT DELAY!
bool init_nunchuck();               //Initialise connection to nunchuk
void activeListener();              //Makes button input react, reacts to recieved data
void reactToRecievedData(uint16_t data);
process readRecievedProcess(uint16_t data);

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
    // uint16_t msg = 0b00000000000;

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

    uint16_t data = 0b0000000100000100; //Start process, hammer, 4x4
    reactToRecievedData(data);
    data = 0b0000001000000111; //Moleup, heap 7

	while (1) {
        // Refresh the backlight (simulate brightness adjustments)
        display.refreshBacklight();

        activeListener();
        reactToRecievedData(data);

        // if(ir.isBufferReady()){
        //     uint16_t data = ir.decodeIRMessage();
        //     Serial.print("Received data: ");
        //     Serial.println(data);
        //     msg = data + 1;
        //     _delay_ms(200);
        // } else {
        //     ir.sendFrame(msg);
        // }
        _delay_ms(10);
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

void activeListener() {
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

void reactToRecievedData(uint16_t data){
    process proc = readRecievedProcess(data);

    switch(proc){
        case startGame: {
            if(display.displayedScreen != Display::game){
                display.characterMole = (data >> 3) & 1; //set character based on bit 3

                uint8_t lastThreeBits = data & 0x7; //Set difficulty based on 3 LSBs
                if(lastThreeBits == 1){ //Bit 0 is set -> 2x2
                    display.selectedDifficulty = Display::four;
                }
                else if(lastThreeBits == 2){ //Bit 1 is set -> 3x3
                    display.selectedDifficulty = Display::nine;
                }
                else if(lastThreeBits == 4){ //Bit 2 is set -> 4x4
                    display.selectedDifficulty = Display::sixteen;
                }
                else{ //If invalid difficulty is recieved
                    Serial.println("Difficulty set error");
                    //TODO terugsturen en terug ontvangen voor correcte check
                }
                display.drawGame(display.selectedDifficulty);
            }
            break;
        }

        case moleUp:{
            uint8_t recievedMoleHeap = data & 0xF;
            Serial.println(recievedMoleHeap);
            display.drawOrRemoveMole(0, recievedMoleHeap);
            break;
        }

        case hammerPositionHit:
            Serial.println("error");
            break;
        
        case recieveScore:
            Serial.println("error");
            break;

        default:
            Serial.println("error");
            break;
    }
}

process readRecievedProcess(uint16_t data){
    data = data >> 8;
    if(data == 1){
        return startGame;
    }
    else if(data == 2){
        return moleUp;
    }
    else if(data == 3){
        return hammerPositionHit;
    }
    else if(data == 4){
        return recieveScore;
    }
    return invalidProcess;
}
