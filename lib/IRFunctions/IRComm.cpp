#include "IRComm.h"
#include <util/delay.h>
#include <Timer1Overflow.h>

// Constructor
IRComm::IRComm(Timer1Overflow &timer1)
    : half_bit_buffers{{0}}, active_buffer_idx(0), buffer_position{0}, buffer_ready_flags{false},
      decoded_frame{0}, bit_index(0), is_tx_active(false), is_tx_high(false),
      prev_timer_value(0), bit_duration(0), is_first_interrupt(true), is_frame_ready(false),
      is_frame_valid(false), tx_frame{0}
{
    for (uint8_t i = 0; i < 2; i++)
    {
        buffer_position[i] = 0;
        buffer_ready_flags[i] = false;
    }

    this->timer1 = &timer1;
}



// Public Methods
void IRComm::initialize()
{
    DDRD |= (1 << DDD6) | (1 << DDD5);          // Set PD6 and PD5 as output (IR LED)
    TCCR0A = (1 << WGM01);                      // Configure Timer0 in CTC mode
    TCCR0B = (1 << CS00);                       // Set no prescaler (fastest clock)
    OCR0A = 210;                                // Set OCR0A for ~889 µs pulse width

    EICRA |= (1 << ISC00);                      // Trigger on any logical change for INT0
    EIMSK |= (1 << INT0);                       // Enable INT0 interrupt
    DDRD &= ~(1 << DDD2);                       // Set PD2 as input (IR receiver)
}

// Interrupt Handlers
void IRComm::onReceiveInterrupt()
{
    uint16_t current_timer_value = TCNT1;
    bool current_pin_state = (PIND & (1 << PIND2)) >> PIND2;

    if (is_first_interrupt)
    {
        prev_timer_value = current_timer_value;
        timer1->resetIR();
        is_first_interrupt = false;
    }
    else
    {
        bit_duration = ((timer1->IROverflowCount << 16) + current_timer_value - prev_timer_value) / 2;

        prev_timer_value = current_timer_value;
        timer1->resetIR();

        processReceivedBit(current_pin_state, bit_duration);
    }
}

void IRComm::onTimer0CompareMatch()
{
    if (!is_tx_active)
    {
        return;
    }

    if (is_tx_high)
    {
        PORTD |= (1 << PD6);  // Set PD6 high (IR LED)
    }
    else
    {
        PORTD &= ~(1 << PD6);  // Set PD6 low (IR LED)
    }

    is_tx_high = !is_tx_high;
    static uint8_t pulse_counter = 0;
    pulse_counter++;

    if (pulse_counter >= 2)
    {
        pulse_counter = 0;
        bit_index++;

        if (bit_index < 16)
        {
            is_tx_high = tx_frame[bit_index];
        }
        else
        {
            is_tx_active = false;
            bit_index = 0;
            TCCR0A &= ~(1 << COM0A0); // Stop toggling OC0A (PD6)
            PORTD &= ~(1 << PD6);     // Explicitly turn off the IR LED
        }
    }
}

// Helper function to process received bits based on timing
void IRComm::processReceivedBit(bool pin_state, uint16_t duration)
{
    if (duration > 750 && duration < 980)
    {
        // Single bit received (duration in expected range)
        half_bit_buffers[active_buffer_idx][buffer_position[active_buffer_idx]++] = pin_state;
    }
    else if (duration > 1600 && duration < 1850)
    {
        // Double bit received (duration in expected range for a double pulse)
        half_bit_buffers[active_buffer_idx][buffer_position[active_buffer_idx]++] = pin_state;
        half_bit_buffers[active_buffer_idx][buffer_position[active_buffer_idx]++] = pin_state;
    }
    else if (duration > 1860)
    {
        // Reset buffer if an invalid duration is detected
        resetActiveBuffer();
    }

    // Check for buffer overflow
    if (buffer_position[active_buffer_idx] >= 31)
    {
        buffer_ready_flags[active_buffer_idx] = true;
        buffer_ready_flags[!active_buffer_idx] = false;
        active_buffer_idx = !active_buffer_idx;
        buffer_position[active_buffer_idx] = 0;
    }
}

// Helper function to reset the active buffer
void IRComm::resetActiveBuffer()
{
    buffer_position[active_buffer_idx] = 0;
}

// Helper function to decode the buffer into a frame
void IRComm::decodeBuffer()
{
    for (uint8_t i = 0; i < 2; i++)
    {
        if (buffer_ready_flags[i])
        {
            processBuffer(i);
        }
    }
}

// Helper function to process a specific buffer and extract bits
void IRComm::processBuffer(uint8_t buffer_idx)
{
    uint8_t bit_idx = 0;

    volatile bool* buffer = half_bit_buffers[buffer_idx];  // Reference the current buffer

    for (uint8_t i = 0; i < 32; i += 2)
    {
        bool bit1 = buffer[i];
        bool bit2 = buffer[i + 1];

        if (bit1 && !bit2)
        {
            decoded_frame[bit_idx] = 1;
        }
        else if (!bit1 && bit2)
        {
            decoded_frame[bit_idx] = 0;
        }
        else
        {
            buffer_ready_flags[buffer_idx] = false;
            Serial.println("Invalid bit detected");
            return;
        }

        bit_idx++;  // Move to the next bit position
    }

    is_frame_ready = true;
    buffer_ready_flags[buffer_idx] = false;
}

// Helper function to validate the decoded frame
void IRComm::validateFrame()
{
    if (!is_frame_ready)
        return;

    is_frame_ready = false;

    // Validate start bits
    if (!(decoded_frame[0] && decoded_frame[1]))
    {
        Serial.println("Start bit error");
        return;
    }

    // Validate parity bit
    bool parity_check = false;
    for (int i = 2; i < 14; i++)
    {
        parity_check ^= decoded_frame[i];
    }

    if (parity_check != decoded_frame[14])
    {
        Serial.println("Parity error");
        return;
    }

    is_frame_valid = true;
}

bool IRComm::isBufferReady()
{
    return buffer_ready_flags[0] || buffer_ready_flags[1];
}

uint16_t IRComm::decodeIRMessage()
{
    decodeBuffer();    // Decode the buffer into the frame
    validateFrame();   // Validate the frame

    // Extract the message (12-bit data field) from the frame
    uint16_t message = 0;
    for (int i = 2; i < 14; i++)
    {
        message <<= 1;
        message |= decoded_frame[i];
    }

    is_frame_valid = false; // Reset the frame validity flag for the next message
    return message; // Return the extracted message
}

// Sending-related initialization functions
void IRComm::initTimer0() {
    // Set OC0A (PD6) and OC0B (PD5) as output for PWM
    DDRD |= (1 << DDD6) | (1 << DDD5);

    // Configure Timer 0
    TCCR0A = (1 << WGM01); // CTC mode
    TCCR0B = (1 << CS00);  // No prescaler (fastest clock)
  
    // Set OCR0A for 38 kHz frequency
    OCR0A = 210; // 38.1 kHz (16 MHz / (2 * (OCR0A + 1)))
}

// Send the 16-bit frame (12 bits data and control bits)
void IRComm::sendFrame(uint16_t data)
{
    // Create the frame to send
    createFrame(data, tx_frame);  // Fill tx_frame with the 16-bit frame

    // Start sending the starting burst
    sendStartingBurst();

    // Send the frame bit by bit
    for (int i = 0; i < 16; i++) {
        sendHalfBit(tx_frame[i]);
    }

    // Stop sending and turn off the IR LED
    stopSending();

    // Optionally, you can print the frame for debugging
    Serial.print("Sending frame: ");
    for (int i = 0; i < 16; i++) {
        Serial.print(tx_frame[i]);
    }
    Serial.println();
}

void IRComm::createFrame(uint16_t data, bool (&frame)[16])
{
    // Frame starts with 2 start bits (11)
    frame[0] = 1;
    frame[1] = 1;

    // Fill the frame with the 12 data bits
    for (int i = 0; i < 12; i++) {
        frame[i + 2] = (data >> (11 - i)) & 0x01;
    }

    // Calculate the parity bit (even parity)
    bool parity = false;
    for (int i = 2; i < 14; i++) {
        parity ^= frame[i];
    }
    frame[14] = parity;
}


// Send the starting burst of the frame
void IRComm::sendStartingBurst() {
    // Start sending the burst signal
    TCCR0A |= (1 << COM0A0);
    _delay_ms(9); // Time for 9 ms burst
    TCCR0A &= ~(1 << COM0A0); // Stop toggling on OC0A
    PORTD &= ~(1 << PD6);     // Explicitly turn off the IR LED
    _delay_ms(4.5); // Time for 4.5 ms pause
}

// Send a half bit (1 or 0) for IR communication
void IRComm::sendHalfBit(bool bit) {
    if (bit) {
        // Send 1: 10 pulse
        TCCR0A |= (1 << COM0A0);
        _delay_us(IR_PULSE_DURATION); // Time for the pulse
        TCCR0A &= ~(1 << COM0A0);
        PORTD &= ~(1 << PD6); // Turn off IR LED
        _delay_us(IR_PULSE_DURATION); // Time for the pause
    } else {
        // Send 0: 01 pulse
        TCCR0A &= ~(1 << COM0A0); // No toggle on OC0A
        PORTD &= ~(1 << PD6);     // Turn off IR LED
        _delay_us(IR_PULSE_DURATION); // Time for the pulse
        TCCR0A |= (1 << COM0A0);
        _delay_us(IR_PULSE_DURATION); // Time for the pause
    }
}

// Stop sending the frame and turn off the IR LED
void IRComm::stopSending() {
    TCCR0A &= ~(1 << COM0A0); // Stop toggling on OC0A
    PORTD &= ~(1 << PD6);     // Turn off IR LED
}
