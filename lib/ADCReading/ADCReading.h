#ifndef ADCREADING_H
#define ADCREADING_H

#include <Arduino.h>

class ADCReading {
public:
    ADCReading(uint8_t pin);
    void begin();
    int read();

private:
    uint8_t _pin;
};

#endif // ADCREADING_H