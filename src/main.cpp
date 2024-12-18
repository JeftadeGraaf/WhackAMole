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

// Instance of IR object
IRComm ir;

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

// Create display objects
Display display(BACKLIGHT_PIN, TFT_CS, TFT_DC);

// prototypes
bool nunchuck_show_state_TEST();    //!Print Nunchuk state for tests !USES NUNCHUK_WAIT DELAY!
bool init_nunchuck();               //Initialise connection to nunchuk
void init_IR_transmitter_timer0();  //initialise Timer0 for IR transmitter
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

        //Check for IR messages
        if(ir.isBufferReady()){
            uint16_t data = ir.decodeIRMessage();
            Serial.print("Received data: ");
            Serial.println(data);
            msg = data + 1;
            _delay_ms(200);
        } else {
            ir.sendFrame(msg);
        }
    }
	//never reach
	return 0;
}

