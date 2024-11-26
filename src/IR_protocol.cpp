#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <HardwareSerial.h>

// demo/debug
#define DO_PRINTS 0

// doesnt want to listen to defines so well settle for consts
const uint8_t DEMO_ENABLE_TRANSMITTER = 1;
const uint8_t DEMO_ENABLE_RECEIVER = 1;

#define MARK 1
#define SPACE 0

// these values have been carefully determined using the serial monitor and pulseview
#define LOGICAL_1_RECV_MIN_TIME 1100
#define LOGICAL_0_RECV_MIN_TIME 200

// any signal outside of these bounds is considered invalid and will reset the receiver
#define RECV_SIGNAL_MAX_LENGTH 8000
#define RECV_SIGNAL_MIN_LENGTH LOGICAL_0_RECV_MIN_TIME

volatile unsigned long TCNT_total = 0;

volatile uint8_t nec_state = 0;
volatile uint16_t next_duration = 0;
volatile uint8_t send_carrier = 0;
volatile uint8_t bit_index = 0;
volatile uint16_t received_data = 0;
volatile uint16_t received_data_definitive = 0;
volatile uint8_t data_ready = 0;

uint16_t data = 0x5555; // 16-bit data to send
uint8_t nec_buffer[16];

unsigned long get_TCNT_time() {
    uint16_t tcnt1;
    tcnt1 = TCNT1;
    unsigned long total = TCNT_total;
    return total + tcnt1;  // Return total ticks
}

// Initialize Timer0 for 38kHz Carrier Signal
void timer0_init() {
    TCCR0A = (1 << WGM01) | (1 << COM0A0); // CTC mode, toggle OC0A
    TCCR0B = (1 << CS00);                  // No prescaler
    OCR0A = 209;                           // Compare match value for 38kHz
    DDRD |= (1 << PD6);                    // Set OC0A (PD6) as output
}

void enable_carrier() {  // carrier = 38kHz transmit signal
    TCCR0A |= (1 << COM0A0); // enable compare match
}

void disable_carrier() {
    TCCR0A &= ~(1 << COM0A0); // disable compare match
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
    // add the current OCR1A value here, because at the end the value is different. 
    // Also adding the value here means that the value is only added when the time has actually elapsed.
    TCNT_total += OCR1A;

    if (nec_state == MARK) {
        disable_carrier();
        send_carrier = SPACE;
        nec_state = SPACE;

        if (bit_index == 0) {
            // After Leader Code, send space
            next_duration = 4500;
        } else if (bit_index <= 16) {
            // Send bits
            next_duration = 560; // Mark duration
        } else {
            // End of frame
            TIMSK1 &= ~(1 << OCIE1A); // Disable Timer1 interrupts
            return;
        }
    } else if (nec_state == SPACE) {
        if (bit_index <= 16) {
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

volatile uint8_t pulse_state; // Read PD2 (receiver state)
volatile unsigned long int pulse_start = 0;
volatile unsigned long int last_pulse_duration = 0;
volatile int8_t received_bits = 0; // cannot be unsigned because -1 is used as a special value
volatile uint8_t receiving = 0;  // Flag to track if we are actively receiving data

// Interrupt for receiving NEC signal on PD2 (INT0)
ISR(INT0_vect) {
    /*
    Theory:

    First FALLING means a transmission is starting.

    First RISING means a bit is being started.
        - Save time of RISING edge.

    Next RISING marks the end of the last bit, and the start of a new one.
        - Determine the bit based on elapsed time.

    When the buffer index reaches 16, the next  RISING edge means the transmission is over.
    */

    pulse_state = PIND & (1 << PD2); // Read PD2 (receiver state)

    // this check is in place to prevent the logic from triggering on the first FALLING edge, causing issues.
    if (receiving) {
        // Serial.println("Receiving");
        // the first RISING edge means no data, but it does mean a new bit is incoming
        if (received_bits == -1) {
            if (pulse_state) {
                pulse_start = get_TCNT_time();
                received_bits++;
            }
        } 

        else {
            if (pulse_state) {
                // this block executes when there is new data. this must be processed and then a new bit is expected.
                last_pulse_duration = get_TCNT_time() - pulse_start;

                if (DO_PRINTS) {
                    Serial.print("L: ");  // short for last
                    Serial.println(last_pulse_duration);

                    Serial.print("B: ");  // short for bits
                    Serial.println(received_bits);
                }

                // filter out invalid values
                if (last_pulse_duration > RECV_SIGNAL_MAX_LENGTH || last_pulse_duration < RECV_SIGNAL_MIN_LENGTH) {
                    received_bits = -1;  // restart the process
                    receiving = 0;
                    received_data = 0;
                    return;
                }

                else if (last_pulse_duration > LOGICAL_1_RECV_MIN_TIME) {
                    // Logical 1 is longest so handle first
                    received_data |= (1 << received_bits);  // set bit at index received_bits
                    received_bits++;
                }

                else if (last_pulse_duration > LOGICAL_0_RECV_MIN_TIME) {
                    received_data &= ~(1 << received_bits);  // clear bit at index received_bits
                    received_bits++;
                }

                // else do nothing, value is invalid
                else {}

                pulse_start = get_TCNT_time();
            }
        }
    }

    // start transmission on first FALLING edge.
    if (!receiving && pulse_state == 0) {
        receiving = 1;
        received_bits = -1;  // -1 because it needs to receive 1 extra to signal the start of a message
        received_data = 0;
        last_pulse_duration = 0;
        return;
    }

    
    if (received_bits >= 16) {
        cli();
        receiving = 0;
        received_data_definitive = received_data;
        data_ready = 1;
        sei();
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

    while (TIMSK1 & (1 << OCIE1A)) {
        // Wait for the message to be sent
    }
}

uint16_t get_received_data() {
    if (data_ready) {
        data_ready = 0;
        uint16_t recv_data = received_data_definitive;
        received_data_definitive = 0;
        return recv_data;
    }
    return 0;
}

void init_protocol() {
    cli();
    receiver_init();
    timer0_init();
    timer1_init();
    sei();
}

// from https://forum.arduino.cc/t/leading-zeros-for-binary-in-serial-monitor-console/456892/4 
void SerialPrintFormatBinaryNumber(uint16_t number) {
    char binstr[]="0000000000000000";
    uint8_t i=0;
    uint16_t n=number;

   while(n>0 && i<16){
      binstr[16-1-i]=n%2+'0';
      ++i;
      n/=2;
   }
	
   Serial.println(binstr);
}

void TEST_IR_PROTOCOL() {
    // send(0x0F0F);

    if (DO_PRINTS && DEMO_ENABLE_RECEIVER) {
        // only print if the receiver is enabled
        uint16_t received_data = get_received_data();
        if (received_data) {
            Serial.println(received_data, HEX);
            SerialPrintFormatBinaryNumber(received_data);
        }
    }
    _delay_ms(250);

    // send(0xF0F0);

    if (DO_PRINTS && DEMO_ENABLE_RECEIVER) {
        // only print if the receiver is enabled
        uint16_t received_data = get_received_data();
        if (received_data) {
            Serial.println(received_data, HEX);
            SerialPrintFormatBinaryNumber(received_data);
        }
    }
    _delay_ms(250);
}