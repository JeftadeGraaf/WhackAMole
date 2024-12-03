#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <Wire.h>
#include <HardwareSerial.h>
#include <Nunchuk.h>

#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include <Display.h>

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
uint16_t cursor_x = 160;                    //Starting cursor x coordinate
uint8_t cursor_y = 130;                     //Starting cursor y coordinate
uint16_t last_cursor_x = 0;                 //Used to temporarily store last cursor x coordinate for screen refresh
uint8_t last_cursor_y = 0;                  //Used to temporarily store last cursor y coordinate for screen refresh

#define BACKLIGHT_PIN 5

// For the Adafruit shield, these are the default.
#define TFT_DC 9
#define TFT_CS 10

//Bitmap for cursor
const uint8_t cursorBitmap[128] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, // Row 1
  0x00, 0x00, 0x00, 0x00, // Row 2
  0x00, 0x0C, 0x00, 0x00, // Row 3
  0x00, 0x12, 0x00, 0x00, // Row 4
  0x00, 0x12, 0x00, 0x00, // Row 5
  0x00, 0x12, 0x00, 0x00, // Row 6
  0x00, 0x12, 0x00, 0x00, // Row 7
  0x00, 0x12, 0x00, 0x00, // Row 8
  0x00, 0x13, 0x80, 0x00, // Row 9
  0x00, 0x12, 0x78, 0x00, // Row 10
  0x00, 0x12, 0x78, 0x00, // Row 11
  0x00, 0x12, 0x46, 0x00, // Row 12
  0x00, 0x12, 0x45, 0x00, // Row 13
  0x01, 0xD2, 0x44, 0x80, // Row 14
  0x01, 0xD0, 0x04, 0x80, // Row 15
  0x01, 0x30, 0x04, 0x80, // Row 16
  0x01, 0x10, 0x00, 0x80, // Row 17
  0x00, 0x80, 0x00, 0x80, // Row 18
  0x00, 0x40, 0x00, 0x80, // Row 19
  0x00, 0x40, 0x00, 0x80, // Row 20
  0x00, 0x40, 0x00, 0x80, // Row 21
  0x00, 0x20, 0x00, 0x80, // Row 22
  0x00, 0x20, 0x01, 0x00, // Row 23
  0x00, 0x10, 0x01, 0x00, // Row 24
  0x00, 0x10, 0x01, 0x00, // Row 25
  0x00, 0x10, 0x01, 0x00, // Row 26
  0x00, 0x08, 0x02, 0x00, // Row 27
  0x00, 0x08, 0x02, 0x00, // Row 28
  0x00, 0x08, 0x02, 0x00, // Row 29
  0x00, 0x0F, 0xFE, 0x00, // Row 30
  0x00, 0x00, 0x00, 0x00, // Row 31
  0x00, 0x00, 0x00, 0x00  // Row 32
};

// Create display objects
Display display(BACKLIGHT_PIN, TFT_CS, TFT_DC);

// prototypes
bool nunchuck_show_state_TEST();    //Print Nunchuk state for tests !USES NUNCHUK_WAIT DELAY!
void update_cursor_coordinates();   //Update the cursors coordinate based on nunchuk movement
void draw_cursor();
bool init_nunchuck();               //Initialise connection to nunchuk
void init_IR_transmitter_timer0();  //initialise Timer0 for IR transmitter

int main(void) {
	sei(); // Enable global interrupts
	Serial.begin(BAUDRATE);
	// Initialize backlight
	display.init();     
	display.refresh_backlight();
	display.clearScreen();
    init_nunchuck();

    display.drawGameOverMenu(123, 123);

	while (1) {
    // Refresh the backlight (simulate brightness adjustments)
    display.refresh_backlight();

    _delay_ms(10);  // Small delay for stability
}
	//never reach
	return 0;
}

void redraw_cursor(){
    if (cursor_x != last_cursor_x || cursor_y != last_cursor_y) {
        // Erase the previous cursor position
        display.drawGraphicalCursor(last_cursor_x, last_cursor_y, 32, ILI9341_BLACK, cursorBitmap);

        // Draw the new cursor position
        display.drawGraphicalCursor(cursor_x, cursor_y, 32, ILI9341_WHITE, cursorBitmap);
    }

    // Update previous cursor coordinates
    last_cursor_x = cursor_x;
    last_cursor_y = cursor_y;
}

void update_cursor_coordinates(){
	Nunchuk.getState(NUNCHUK_ADDRESS);      //Update Nunchuk state

    //Retrieve values from class
	uint8_t NunchukX = Nunchuk.state.joy_x_axis;    
	uint8_t NunchukY = Nunchuk.state.joy_y_axis;

    //Horizontal movement
	if (NunchukX > NUNCHUK_CENTER_VALUE + NUNCHUK_DEADZONE && cursor_x < DISPLAY_MAX_X) {
        cursor_x += NUNCHUK_X_SENSITIVITY;      //move right
    } else if (NunchukX < NUNCHUK_CENTER_VALUE - NUNCHUK_DEADZONE && cursor_x > DISPLAY_MIN_X) {
        cursor_x -= NUNCHUK_X_SENSITIVITY;  //move left
    }

    //Vertical movement
    if (NunchukY > NUNCHUK_CENTER_VALUE + NUNCHUK_DEADZONE && cursor_y > DISPLAY_MIN_Y) {
        cursor_y -= NUNCHUK_Y_SENSITIVITY;  //move up
    } else if (NunchukY < NUNCHUK_CENTER_VALUE - NUNCHUK_DEADZONE && cursor_y < DISPLAY_MAX_Y) {
        cursor_y += NUNCHUK_Y_SENSITIVITY;  //move down
    }

    redraw_cursor();
}

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
    Serial.print("\t\tAccel X: ");
    Serial.print(Nunchuk.state.accel_x_axis);
    Serial.print("\t\tButton C: ");
    Serial.println(Nunchuk.state.c_button);

    Serial.print("Joy Y: ");
    Serial.print(Nunchuk.state.joy_y_axis);
    Serial.print("\t\tAccel Y: ");
    Serial.print(Nunchuk.state.accel_y_axis);
    Serial.print("\t\tButton Z: ");
    Serial.println(Nunchuk.state.z_button);

		// wait a while
		_delay_ms(NUNCHUCK_WAIT);

		return(true);
}

void init_IR_transmitter_timer0(){
	DDRD |= (1 << DDD6);        // IR LED output
	TCCR0B |= (1 << CS00);      // no prescaler
	TCCR0A |= (1 << WGM01);     // CTC mode (reset at OCR)
	TCCR0A |= (1 << COM0A0);    // toggle mode
	OCR0A = OCR0A_value;
}
