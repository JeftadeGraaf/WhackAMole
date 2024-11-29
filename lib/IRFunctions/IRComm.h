#ifndef IR_COMM_H
#define IR_COMM_H

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <HardwareSerial.h>

class IRComm {
public:
    // Constructor and initialization
    IRComm();
    void begin();

    // Sending functions
    void sendFrame(uint16_t data);

    // Interrupt handlers
    void handleReceiveInterrupt();
    void handleTimer1Overflow();
    void handleTimer0Compare();

    bool isBufferReady(); // Checks if any buffer is ready
    void decodeIRMessage(); // Decodes the IR message

private:
    // Transmission state
    uint8_t tx_frame[16];
    volatile uint8_t tx_bit_index;
    volatile bool tx_active;
    volatile bool tx_high;

    // Receiving state
    volatile uint8_t timer1_overflows;
    volatile uint16_t previous_timer_value;
    volatile uint16_t half_bit_duration;
    volatile bool is_first_interrupt;

    volatile bool half_bit_buffers[2][32];
    volatile uint8_t active_buffer_index;
    volatile uint8_t buffer_positions[2];
    volatile bool buffer_ready_flags[2];
    bool decoded_frame[16];
    bool is_frame_ready;
    bool is_frame_valid;

    // Helper methods
    void createFrame(uint16_t data);
    void processBit(bool current_pin_state, uint16_t duration);
    void resetActiveBuffer();
    void decodeBuffer();
    void processBuffer(uint8_t buffer_index);
    void validateFrame();
    void debugDecodedFrame();
};

#endif // IR_COMM_H
