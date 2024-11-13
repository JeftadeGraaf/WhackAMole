#include "ADCReading.h"
#include <avr/io.h>

ADCReading::ADCReading(uint8_t pin) : _pin(pin) {}

void ADCReading::begin() {
    // Reference voltage set to AVCC (5V), ADC0 as input
    ADMUX = (1<<REFS0);
    // Enable ADC, set prescaler to 128 for accuracy (16MHz / 128 = 125kHz)
    ADCSRA = (1<<ADEN) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);
}

int ADCReading::read() {
    // Select ADC channel with safety masking
    ADMUX = (ADMUX & 0xF8) | (_pin & 0x07);
    // Start single conversion
    ADCSRA |= (1<<ADSC);
    // Wait for conversion to complete
    while (ADCSRA & (1<<ADSC));
    return ADC;
}
