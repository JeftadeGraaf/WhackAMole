#include <Adafruit_GFX.h>
#include <Arduino.h>
#include <HardwareSerial.h>

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <Wire.h>

#include <Nunchuk.h>

// OCR2A = (Clock_freq / (2 * Prescaler * Target_freq)) - 1
const int OCR0A_waarde = (16000000 / (2 * 1 * 56000)) - 1;

#define BAUDRATE					9600

#define NUNCHUK_ADDRESS 	0x52
#define NUNCHUCK_WAIT			1000

// prototypes
bool nunchuck_show_state_TEST(void);
bool init_nunchuck();
void init_IR_transmitter_timer0();

int main(void) {
	sei(); 	// enable global interrupts

	Serial.begin(BAUDRATE); // initialise serial

	Wire.begin(); // join I2C bus as master

	init_nunchuck(); // initialiseer nunchuck
	init_IR_transmitter_timer0(); // initialiseer Timer0 voor IR transmitter

	// endless loop
	while(1) {

	}

	return(0);
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

	Serial.print("\t\t\tAccel Z: ");
	Serial.println(Nunchuk.state.accel_z_axis);

	// wait a while
	_delay_ms(NUNCHUCK_WAIT);

	return(true);
}

void init_IR_transmitter_timer0(){
	TCCR0A |= (1 << WGM01); //CTC mode (reset bij bereiken OCR)
	TCCR0A |= (1 << COM0A0); // toggle mode
	OCR0A = OCR0A_waarde;
}