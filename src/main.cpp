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

// OCR0A = (Clock_freq / (2 * Prescaler * Target_freq)) - 1
const uint8_t OCR0A_value = (16000000 / (2 * 1 * 38000)) - 1;

// Define UART baud rate
#define BAUDRATE 9600

#define NUNCHUK_ADDRESS 0x52
#define NUNCHUCK_WAIT 1000
#define BACKLIGHT_PIN 5

// For the Adafruit shield, these are the default.
#define TFT_DC 9
#define TFT_CS 10

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
bool nunchuck_show_state_TEST(void);
bool init_nunchuck();
void init_IR_transmitter_timer0();

int main(void) {
	Serial.begin(BAUDRATE);
	// Initialize backlight
	display.init();     
	display.refresh_backlight();
	display.clearScreen();

	sei(); // Enable global interrupts

	// Draw the initial cursor
	display.drawGraphicalCursor(120, 160, 32, ILI9341_WHITE, cursorBitmap);
	while (1) {
		// Refresh the backlight (simulate brightness adjustments)
		display.refresh_backlight();
		_delay_ms(10);  // Small delay for stability
	}
}
bool init_nunchuck(){
	Serial.print("-------- Connecting to nunchuk at address 0x");
	Serial.println(NUNCHUK_ADDRESS, HEX);
	if (!Nunchuk.begin(NUNCHUK_ADDRESS)) {
		Serial.println("******** No nunchuk found");
		Serial.flush();
		return(false);
	}
	Serial.print("-------- Nunchuk with Id: ");
	Serial.println(Nunchuk.id);
	return true;
}

bool nunchuck_show_state_TEST(void) {
	if (!Nunchuk.getState(NUNCHUK_ADDRESS)) {
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
	DDRD |= (1 << DDD6);
	TCCR0B |= (1 << CS00);
	TCCR0A |= (1 << WGM01); //CTC mode (reset at OCR)
	TCCR0A |= (1 << COM0A0); // toggle mode
	OCR0A = OCR0A_value;
}