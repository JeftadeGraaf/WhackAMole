#ifndef _H_IR_PROTOCOL
#define _H_IR_PROTOCOL

#include <avr/io.h>

// Send 8 or 16 bits of data over IR
void send(uint16_t data);

// Set a variable where received data can be accessed
void setDataDestination(int* destination);

// Initialize the IR transmitter
void init_IR_transmitter_timer0();

#endif