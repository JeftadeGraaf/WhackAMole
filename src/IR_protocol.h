#ifndef IR_PROTOCOL_H
#define IR_PROTOCOL_H

#include <avr/io.h>

void init_protocol();

void send(uint16_t send_data);

#endif