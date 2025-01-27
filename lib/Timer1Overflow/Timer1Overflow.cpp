#include "Timer1Overflow.h"
#include <avr/io.h>
#include <avr/interrupt.h>

void Timer1Overflow::init()
{
    TCCR1A = 0;
    TCCR1B |= (1 << CS11); // Prescaler of 8 for Timer1
    TCNT1 = 0;
    TIMSK1 |= (1 << TOIE1); // Enable Timer1 overflow interrupt

    // Enable Timer1 overflow interrupt
    TIMSK1 |= (1 << TOIE1);
}

void Timer1Overflow::onTimer1Overflow()
{
    IROverflowCount++;
    overflowCount++;
    joystickDebounceCount++;
}

void Timer1Overflow::resetIR()
{
    IROverflowCount = 0;
}

void Timer1Overflow::resetOverflow()
{
    overflowCount = 0;
}

void Timer1Overflow::resetJoystickDebounce()
{
    joystickDebounceCount = 0;
}