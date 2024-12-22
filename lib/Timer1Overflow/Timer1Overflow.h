#ifndef TIMER1OVERFLOW_H
#define TIMER1OVERFLOW_H

#include <Arduino.h>

class Timer1Overflow {
public:
    void init();

    void onTimer1Overflow();
    void resetIR();
    void resetOverflow();

    uint32_t overflowCount = 0;
    uint16_t IROverflowCount = 0;
};

#endif // TIMER1OVERFLOW_H