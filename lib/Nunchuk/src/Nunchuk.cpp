#include <util/delay.h>
#include <Wire.h>
#include "Nunchuk.h"

// Nunchuk memory addresses
constexpr uint8_t NCSTATE = 0x00; // Address of state (6 bytes)
constexpr uint8_t NCID = 0xFA;   // Address of ID (4 bytes)

constexpr uint8_t CHUNKLEN = 32;
constexpr uint8_t STATELEN = 6;
constexpr uint8_t WAITFORREAD = 1; // ms

// Nibble to hex ASCII conversion
constexpr char BTOA[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                         '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

// Initialize static buffer
uint8_t NunChuk::buffer[CHUNKLEN] = {0};

// Constructor
NunChuk::NunChuk() {}

// Public methods

/**
 * Perform the handshake with the Nunchuk device.
 */
bool NunChuk::begin(uint8_t address) {
    // Initialize the device
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
    // Read state from memory
    if (_read(address, NCSTATE, STATELEN) != STATELEN) {
        return false;
    }

    // Parse state data
    state.joy_x_axis = buffer[0];
    state.joy_y_axis = buffer[1];
    state.accel_x_axis = (buffer[2] << 2) | ((buffer[5] & 0x0C) >> 2);
    state.accel_y_axis = (buffer[3] << 2) | ((buffer[5] & 0x30) >> 4);
    state.accel_z_axis = (buffer[4] << 2) | ((buffer[5] & 0xC0) >> 6);
    state.z_button = !(buffer[5] & 0x01);   // Z button pressed
    state.c_button = !((buffer[5] & 0x02) >> 1); // C button pressed

    return true;
}

// Private methods

/**
 * Retrieve the device ID (expected ID: 0xA4200000 for Nunchuk).
 */
bool NunChuk::_getId(uint8_t address) {
    // Read the ID from the device
    if (_read(address, NCID, IDLEN) != IDLEN) {
        return false;
    }

    // Convert ID to string
    id[0] = '0';
    id[1] = 'x';
    for (uint8_t i = 0; i < IDLEN; ++i) {
        id[2 + 2 * i] = BTOA[(buffer[i] >> 4)];
        id[3 + 2 * i] = BTOA[(buffer[i] & 0x0F)];
    }
    id[2 * IDLEN + 2] = '\0';

    return true;
}

/**
 * Read data from the device.
 */
uint8_t NunChuk::_read(uint8_t address, uint8_t offset, uint8_t len) {
    uint8_t n = 0;

    // Send the memory offset
    Wire.beginTransmission(address);
    Wire.write(offset);
    Wire.endTransmission(true);

    // Wait for the data to be ready
    _delay_ms(WAITFORREAD);

    // Request the data
    Wire.requestFrom(address, len);

    // Read the requested data into the buffer
    while (Wire.available() && n < len) {
        buffer[n++] = Wire.read();
    }

    return n;
}

// Preinstantiate Nunchuk object
NunChuk Nunchuk;
