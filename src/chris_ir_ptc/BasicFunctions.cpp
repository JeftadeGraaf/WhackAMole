#include <avr/io.h>
#include <avr/interrupt.h>

#include "BasicFunctions.h"

// Helper functions for the basic functions ahead
uint8_t __get_port_by_pin_number(uint8_t _pin) {
    if (_pin >= 0 && _pin <= 7) {
        return PORTD;
    } else if (_pin >= 8 && _pin <= 13) {
        return PORTB;
    } else if (_pin >= 14 && _pin <= 19) {
        return PORTC;
    } else {
        return -1;
    }
}

uint8_t __get_pin_position_by_pin_number(uint8_t _pin) {
    uint8_t port = __get_port_by_pin_number(_pin);

    if (port == PORTD) {
        return _pin;  // PD0 - PD7 are the same as the _pin value
    }

    if (port == PORTB) {
        return _pin - 8;  // PB0 - PB5 are the same as _pin - 8
    }

    if (port == PORTC) {
        return _pin - 14;  // PC0 - PC5 are the same as _pin - 14
    }
}

uint8_t __get_ddr_by_pin_number(uint8_t _pin) {
    uint8_t port = __get_port_by_pin_number(_pin);

    if (port == PORTD) {
        return DDRD;
    }

    if (port == PORTB) {
        return DDRB;
    }

    if (port == PORTC) {
        return DDRC;
    }
}

uint8_t __get_pin_register_by_pin_number(uint8_t _pin) {
    uint8_t port = __get_port_by_pin_number(_pin);

    if (port == PORTD) {
        return PIND;
    }

    if (port == PORTB) {
        return PINB;
    }

    if (port == PORTC) {
        return PINC;
    }
}

// Basic functions to make readability easier
void BF_pinMode(uint8_t _pin, uint8_t _mode) {
    uint8_t ddr = __get_ddr_by_pin_number(_pin);
    uint8_t pin = __get_pin_position_by_pin_number(_pin);

    if (_mode == OUTPUT) {
        ddr |= (1 << pin);  // set as OUTPUT
    } else {
        ddr &= ~(1 << pin);  // set as INPUT
    }
}

void BF_digitalWrite(uint8_t _pin, uint8_t _val) {
    uint8_t port = __get_port_by_pin_number(_pin);
    uint8_t pin = __get_pin_position_by_pin_number(_pin);

    if (_val == HIGH) {
        port |= (1 << pin);  // write HIGH
    } else {
        port &= ~(1 << pin);  // write LOW
    }
}

uint8_t BF_digitalRead(uint8_t _pin) {
    uint8_t pin_register = __get_pin_register_by_pin_number(_pin);
    uint8_t pin = __get_pin_position_by_pin_number(_pin);

    return (pin_register & (1 << pin)) >> pin;
}

int BF_floor(float val) {
    return (int)val;
}

uint8_t BF_digitalPinToInterrupt(uint8_t _pin) {
    if (_pin == 2) {
        return 0;
    } else if (_pin == 3) {
        return 1;
    } else {
        return -1;
    }
}

void BF_attachInterrupt(uint8_t interrupt_number, void (*_isr)(), uint8_t _mode) {
    if (interrupt_number == 0) {
      switch (_mode) {
        case CHANGE:
          EICRA |= (1 << ISC00) | (1 << ISC01);  // set interrupt to trigger on any change
          break;
        case FALLING:
          EICRA |= (1 << ISC01);  // set interrupt to trigger on falling edge
          break;
        case RISING:
          EICRA |= (1 << ISC00);  // set interrupt to trigger on rising edge
          break;
      }
        
    } else if (interrupt_number == 1) {
        switch (_mode) {
          case CHANGE:
            EICRA |= (1 << ISC10) | (1 << ISC11);  // set interrupt to trigger on any change
            break;
          case FALLING:
            EICRA |= (1 << ISC11);  // set interrupt to trigger on falling edge
            break;
          case RISING:
            EICRA |= (1 << ISC10);  // set interrupt to trigger on rising edge
            break;
        }
    }

    sei();  // enable global interrupts
}
