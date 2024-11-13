#include <Adafruit_GFX.h>
#include <Arduino.h>
#include <HardwareSerial.h>

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <Wire.h>

#include <Nunchuk.h>

#define BAUDRATE					9600

#define NUNCHUK_ADDRESS 	0x52
#define NUNCHUCK_WAIT			1000

// prototypes
bool show_state(void);
bool init_nunchuck();

int main(void) {
	sei(); 	// enable global interrupts

	Serial.begin(BAUDRATE); // initialise serial

	Wire.begin(); // join I2C bus as master

	init_nunchuck(); // initialise nunchuck

	// endless loop
	while(1) {
		show_state();

		// wait a while
		_delay_ms(NUNCHUCK_WAIT);
	}

	return(0);
}

bool init_nunchuck(){
	Serial.print("-------- Connecting to nunchuk at address 0x");
	Serial.println(NUNCHUK_ADDRESS, HEX);
	if (!Nunchuk.begin(NUNCHUK_ADDRESS))
	{
		Serial.println("******** No nunchuk found");
		Serial.flush();
		return(false);
	}
	Serial.print("-------- Nunchuk with Id: ");
	Serial.println(Nunchuk.id);
	return true;
}

bool show_state(void)
{
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

	return(true);
}
