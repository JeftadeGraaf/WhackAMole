#ifndef IR_PROTOCOL_H
#define IR_PROTOCOL_H

#include <avr/io.h>

void init_protocol();

void send(uint16_t send_data);

uint16_t get_received_data();

void TEST_IR_PROTOCOL();

#endif