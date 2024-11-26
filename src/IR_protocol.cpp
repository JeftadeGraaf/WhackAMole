#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define MARK 1
#define SPACE 0

volatile uint8_t nec_state = 0;
volatile uint16_t next_duration = 0;
volatile uint8_t send_carrier = 0;
volatile uint8_t bit_index = 0;
volatile uint32_t received_data = 0;

uint16_t data = 0x5555; // 16-bit data to send
uint8_t nec_buffer[32];

// Initialize Timer0 for 38kHz Carrier Signal
void timer0_init() {
    TCCR0A = (1 << WGM01) | (1 << COM0A0); // CTC mode, toggle OC0A
    TCCR0B = (1 << CS00);                  // No prescaler
    OCR0A = 209;                           // Compare match value for 38kHz
    DDRD |= (1 << PD6);                    // Set OC0A (PD6) as output
}

// Enable the 38kHz Carrier
void enable_carrier() {
    TCCR0A |= (1 << COM0A0); // Toggle OC0A on compare match
}

// Disable the 38kHz Carrier
void disable_carrier() {
    TCCR0A &= ~(1 << COM0A0); // Stop toggling OC0A
}

// Initialize Timer1 for NEC Protocol Timing
void timer1_init() {
    TCCR1A = 0;                           // Normal mode
    TCCR1B = (1 << WGM12) | (1 << CS11);  // CTC mode with prescaler = 8
    OCR1A = 0;                            // placeholder value
    TIMSK1 = (1 << OCIE1A);               // Enable compare match interrupt
}

// Prepare and send 16 bits of data
void nec_send_bit(uint8_t bit) {
    if (bit) {
        next_duration = 1690; // Logical '1'
    } else {
        next_duration = 560;  // Logical '0'
    }
    send_carrier = MARK;
    nec_state = MARK;
    bit_index++;
}

void nec_prepare_data() {
    uint8_t i;

    // Fill buffer with the 16-bit data (data)
    for (i = 0; i < 16; i++) {
        nec_buffer[i] = (data >> i) & 0x01;
    }

    // Leader code first
    next_duration = 9000;
    send_carrier = MARK;
    nec_state = MARK;
    bit_index = 0;
}

// Timer1 Compare Match Interrupt for NEC Protocol Timing
ISR(TIMER1_COMPA_vect) {
    if (nec_state == MARK) {
        disable_carrier();
        send_carrier = SPACE;
        nec_state = SPACE;

        if (bit_index == 0) {
            // After Leader Code, send space
            next_duration = 4500;
        } else if (bit_index <= 32) {
            // Send bits
            next_duration = 560; // Mark duration
        } else {
            // End of frame
            TIMSK1 &= ~(1 << OCIE1A); // Disable Timer1 interrupts
            return;
        }
    } else if (nec_state == SPACE) {
        if (bit_index <= 32) {
            // Send next bit
            nec_send_bit(nec_buffer[bit_index - 1]);
        } else {
            // End of frame
            send_carrier = MARK;
            next_duration = 560; // Final mark
            nec_state = MARK;
        }
    }

    // Set next timer compare match duration
    OCR1A = (next_duration * 2); // Convert microseconds to ticks
    if (send_carrier == MARK) {
        enable_carrier();
    } else {
        disable_carrier();
    }
}

// Interrupt for receiving NEC signal on PD2 (INT0)
ISR(INT0_vect) {
    static uint16_t pulse_start = 0;
    static uint8_t received_bits = 0;
    static uint16_t last_pulse_duration = 0;

    // Measure the duration of the pulse
    if (PIND & (1 << PD2)) {  // Rising edge
        pulse_start = TCNT1;
    } else {  // Falling edge
        last_pulse_duration = TCNT1 - pulse_start;
        
        // Decode the pulse
        if (last_pulse_duration > 1000) {  // Mark (1)
            if (received_bits < 32) {
                received_data |= (1 << received_bits);
            }
        } else if (last_pulse_duration > 400) {  // Space (0)
            // Bit is 0, nothing to do here
        }

        received_bits++;
        
        // Check if we've received all bits (16 bits)
        if (received_bits >= 16) {
            received_bits = 0;  // Reset bit counter
        }
    }
}

// Initialize the receiver on PD2 (INT0)
void receiver_init() {
    // Set PD2 as input
    DDRD &= ~(1 << PD2);
    
    // Enable external interrupt on INT0 (PD2)
    EICRA |= (1 << ISC00);  // Trigger interrupt on any edge (rising or falling)
    EIMSK |= (1 << INT0);    // Enable INT0 interrupt
}

void send(uint16_t send_data) {
    if (TIMSK1 & (1 << OCIE1A)) {
        return; // discard message, one is already being sent
    }

    data = send_data;
    nec_prepare_data();
    TIMSK1 |= (1 << OCIE1A); // Enable Timer1 interrupts
}

void init_protocol() {
    cli();
    timer0_init();
    timer1_init();
    receiver_init();
    sei();
}