#ifndef BASICFUNCTIONS_H
#define BASICFUNCTIONS_H

#define LOW 0
#define HIGH 1

#define INPUT 0
#define OUTPUT 1

#define CHANGE 1
#define FALLING 2
#define RISING 3

#include <avr/io.h>

void BF_pinMode(uint8_t _pin, uint8_t _mode);

void BF_digitalWrite(uint8_t _pin, uint8_t _val);

uint8_t BF_digitalRead(uint8_t _pin);

int BF_floor(float val);

uint8_t BF_digitalPinToInterrupt(uint8_t _pin);

void BF_attachInterrupt(uint8_t interrupt_number, void (*_isr)(), uint8_t _mode);

#endif