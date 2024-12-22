#include <util/delay.h>
#include <Wire.h>
#include "Nunchuk.h"

// Nunchuk memory addresses
constexpr uint8_t NCSTATE = 0x00; // Address of state (6 bytes)
constexpr uint8_t NCID = 0xFA;   // Address of ID (4 bytes)

constexpr uint8_t CHUNKLEN = 32;
constexpr uint8_t STATELEN = 6;
constexpr uint8_t WAITFORREAD = 1; // ms

// Nibble to hex ASCII conversion stored in flash memory
constexpr char BTOA[] PROGMEM = {'0', '1', '2', '3', '4', '5', '6', '7',
                                 '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

// Constructor
NunChuk::NunChuk() {}

// Public methods

/**
 * Perform the handshake with the Nunchuk device.
 */
bool NunChuk::begin(uint8_t address) {
    Wire.beginTransmission(address);
    Wire.write(0xF0);
    Wire.write(0x55);
    Wire.endTransmission(true);

    Wire.beginTransmission(address);
    Wire.write(0xFB);
    Wire.write(0x00);
    Wire.endTransmission(true);

    // Retrieve the ID
    return _getId(address);
}

/**
 * Retrieve the current state of the Nunchuk device.
 */
bool NunChuk::getState(uint8_t address) {
    uint8_t localBuffer[STATELEN]; // Use a local buffer

    if (_read(address, NCSTATE, STATELEN, localBuffer) != STATELEN) {
        return false;
    }

    // Parse state data
    state.joy_x_axis = localBuffer[0];
    state.joy_y_axis = localBuffer[1];
    state.accel_x_axis = (localBuffer[2] << 2) | ((localBuffer[5] & 0x0C) >> 2);
    state.accel_y_axis = (localBuffer[3] << 2) | ((localBuffer[5] & 0x30) >> 4);
    state.accel_z_axis = (localBuffer[4] << 2) | ((localBuffer[5] & 0xC0) >> 6);
    state.z_button = !(localBuffer[5] & 0x01);    // Z button pressed
    state.c_button = !((localBuffer[5] & 0x02) >> 1); // C button pressed

    return true;
}

// Private methods

/**
 * Retrieve the device ID (expected ID: 0xA4200000 for Nunchuk).
 */
bool NunChuk::_getId(uint8_t address) {
    uint8_t localBuffer[IDLEN]; // Use a local buffer

    if (_read(address, NCID, IDLEN, localBuffer) != IDLEN) {
        return false;
    }

    // Convert ID to string
    id[0] = '0';
    id[1] = 'x';
    for (uint8_t i = 0; i < IDLEN; ++i) {
        id[2 + 2 * i] = pgm_read_byte(&BTOA[localBuffer[i] >> 4]);
        id[3 + 2 * i] = pgm_read_byte(&BTOA[localBuffer[i] & 0x0F]);
    }
    id[2 * IDLEN + 2] = '\0';

    return true;
}

/**
 * Read data from the device.
 */
uint8_t NunChuk::_read(uint8_t address, uint8_t offset, uint8_t len, uint8_t* buffer) {
    uint8_t n = 0;

    Wire.beginTransmission(address);
    Wire.write(offset);
    Wire.endTransmission(true);

    _delay_ms(WAITFORREAD);

    Wire.requestFrom(address, len);

    while (Wire.available() && n < len) {
        buffer[n++] = Wire.read();
    }

    return n;
}

//Init nunchuk
bool NunChuk::init_nunchuck(){
	//Serial.print("-------- Connecting to nunchuk at address 0x");
	//Serial.println(NUNCHUK_ADDRESS, HEX);

    //Make connection to Nunchuk
	if (!Nunchuk.begin(NUNCHUK_ADDRESS)) {
        //If nunchuk is not found, print error and return false
		//Serial.println("******** No nunchuk found");
		//Serial.flush();
		return(false);
	}
    //After succesful handshake, print Nunchuk ID
	//Serial.print("-------- Nunchuk with Id: ");
	//Serial.println(Nunchuk.id);
	return true;
}

//Nunchuk test function
bool NunChuk::nunchuck_show_state_TEST() {
    //Print Nunchuk state
	if (!Nunchuk.getState(NUNCHUK_ADDRESS)) {
        //If nunchuk is not found, print error and return false
		//Serial.println("******** No nunchuk found");
		//Serial.flush();
		return (false);
	}
    //Serial.println("------State data--------------------------");
    //Serial.print("Joy X: ");
    //Serial.print(Nunchuk.state.joy_x_axis);
    //Serial.print("\t\tButton C: ");
    //Serial.println(Nunchuk.state.c_button);

    //Serial.print("Joy Y: ");
    //Serial.print(Nunchuk.state.joy_y_axis);
    //Serial.print("\t\tButton Z: ");
    //Serial.println(Nunchuk.state.z_button);

		// wait a while
		_delay_ms(NUNCHUCK_WAIT);

		return(true);
}

// Preinstantiate Nunchuk object
NunChuk Nunchuk;
