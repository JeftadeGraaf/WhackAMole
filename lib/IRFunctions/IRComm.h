#ifndef IR_COMM_H
#define IR_COMM_H

#include <Arduino.h>
#include <Timer1Overflow.h>

class IRComm
{
public:
    // Constructor
    IRComm(Timer1Overflow &timer1);

    // Initialization function
    void initialize();

    // Methods for sending and receiving data
    void sendFrame(uint16_t data);
    uint16_t decodeIRMessage();

    // Check if a buffer is ready for decoding
    bool isBufferReady();

    // Interrupt service routine for receiving IR data
    void onReceiveInterrupt();
    void onTimer0CompareMatch();

private:
    // Buffer processing methods
    void processReceivedBit(bool pin_state, uint16_t duration);
    void decodeBuffer();
    void processBuffer(uint8_t buffer_idx);
    void resetActiveBuffer();
    void validateFrame();

    // Helper methods for sending data
    void initTimer0();
    void sendStartingBurst();
    void sendHalfBit(bool bit);
    void stopSending();
    void createFrame(uint16_t data, bool (&frame)[16]);

    // Member variables for managing IR data
    volatile bool half_bit_buffers[2][32];   // Buffer for received bits
    uint8_t buffer_position[2];              // Current position in the buffer
    bool buffer_ready_flags[2];              // Flags to indicate buffer readiness
    uint8_t active_buffer_idx;               // Index of the active buffer
    uint8_t decoded_frame[16];               // Decoded IR frame
    uint8_t bit_index;                       // Index for sending bits
    bool is_tx_active;                       // Whether transmission is active
    bool is_tx_high;                         // IR LED state for sending
    uint16_t prev_timer_value;               // Previous timer value
    uint16_t bit_duration;                   // Duration of the received bit
    bool is_first_interrupt;                 // Flag for first interrupt
    bool is_frame_ready;                     // Flag indicating if a frame is ready
    bool is_frame_valid;                     // Flag indicating if a frame is valid

    // Transmission frame
    bool tx_frame[16];  // Array to store the current transmission frame

    // Timing constants
    static const uint16_t IR_PULSE_DURATION = 850; // Duration of each IR pulse in microseconds

    // Timer object for managing overflows
    Timer1Overflow* timer1;
};

#endif
