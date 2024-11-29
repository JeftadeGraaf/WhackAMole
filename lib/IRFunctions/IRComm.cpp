#include "IRComm.h"

// Constructor
IRComm::IRComm()
    : tx_bit_index(0), tx_active(false), tx_high(false),
      timer1_overflows(0), previous_timer_value(0), half_bit_duration(0),
      is_first_interrupt(true), active_buffer_index(0), is_frame_ready(false),
      is_frame_valid(false)
{
    for (uint8_t i = 0; i < 2; i++)
    {
        buffer_positions[i] = 0;
        buffer_ready_flags[i] = false;
    }
}

// Public Methods
void IRComm::begin()
{
    // Initialize peripherals
    EICRA |= (1 << ISC00); // Trigger on any logical change
    EIMSK |= (1 << INT0);  // Enable INT0 interrupt
    DDRD &= ~(1 << DDD2);  // Set PD2 as input

    TCCR1A = 0;
    TCCR1B |= (1 << CS11); // Prescaler of 8
    TCNT1 = 0;
    TIMSK1 |= (1 << TOIE1); // Enable Timer1 overflow interrupt

    DDRD |= (1 << DDD6);     // Set PD6 as output
    TCCR0A = (1 << WGM01);   // CTC mode
    TCCR0B = (1 << CS01);    // Prescaler 8
    OCR0A = 210;             // Match value for ~889 µs
    TIMSK0 |= (1 << OCIE0A); // Enable compare interrupt
}

void IRComm::sendFrame(uint16_t data)
{
    createFrame(data);

    tx_bit_index = 0;
    tx_high = tx_frame[0];
    tx_active = true;

    TCCR0A |= (1 << COM0A0); // Enable toggle on match
}

// Interrupt handlers (called by the actual ISR handlers)
void IRComm::handleReceiveInterrupt()
{
    uint16_t current_timer_value = TCNT1;
    bool current_pin_state = (PIND & (1 << PIND2)) >> PIND2;

    if (is_first_interrupt)
    {
        previous_timer_value = current_timer_value;
        timer1_overflows = 0;
        is_first_interrupt = false;
    }
    else
    {
        uint32_t elapsed_time = (timer1_overflows * 65536UL) + current_timer_value;
        half_bit_duration = (elapsed_time - previous_timer_value) / 2;

        previous_timer_value = current_timer_value;
        timer1_overflows = 0;

        processBit(current_pin_state, half_bit_duration);
    }
}

void IRComm::handleTimer1Overflow()
{
    timer1_overflows++;
}

void IRComm::handleTimer0Compare()
{
    if (!tx_active)
    {
        return;
    }

    if (tx_high)
    {
        PORTD |= (1 << PD6);
    }
    else
    {
        PORTD &= ~(1 << PD6);
    }

    tx_high = !tx_high;
    static uint8_t pulse_count = 0;
    pulse_count++;

    if (pulse_count >= 2)
    {
        pulse_count = 0;

        tx_bit_index++;
        if (tx_bit_index < 16)
        {
            tx_high = tx_frame[tx_bit_index];
        }
        else
        {
            tx_active = false;
            tx_bit_index = 0;
            TCCR0A &= ~(1 << COM0A0);
            PORTD &= ~(1 << PD6);
        }
    }
}

// Private Methods
void IRComm::createFrame(uint16_t data)
{
    tx_frame[0] = 1;
    tx_frame[1] = 1;

    for (int i = 0; i < 12; i++)
    {
        tx_frame[i + 2] = (data >> (11 - i)) & 0x01;
    }

    bool even = false;
    for (int i = 2; i < 14; i++)
    {
        if (tx_frame[i])
        {
            even = !even;
        }
    }
    tx_frame[14] = even;

    tx_frame[15] = 1;
}

// Helper function to process individual bits based on timing
void IRComm::processBit(bool current_pin_state, uint16_t duration)
{
    if (duration > 750 && duration < 980)
    {
        // Single bit
        half_bit_buffers[active_buffer_index][buffer_positions[active_buffer_index]++] = current_pin_state;
    }
    else if (duration > 1600 && duration < 1850)
    {
        // Double bit
        half_bit_buffers[active_buffer_index][buffer_positions[active_buffer_index]++] = current_pin_state;
        half_bit_buffers[active_buffer_index][buffer_positions[active_buffer_index]++] = current_pin_state;
    }
    else if (duration > 1860)
    {
        // Reset buffer due to invalid duration
        resetActiveBuffer();
    }

    // Check buffer overflow
    if (buffer_positions[active_buffer_index] >= 31)
    {
        buffer_ready_flags[active_buffer_index] = true;
        buffer_ready_flags[!active_buffer_index] = false;
        active_buffer_index = !active_buffer_index;
        buffer_positions[active_buffer_index] = 0;
    }
}

// Helper function to reset the active buffer
void IRComm::resetActiveBuffer()
{
    buffer_positions[active_buffer_index] = 0;
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
void IRComm::processBuffer(uint8_t buffer_index)
{
    uint8_t bit_index = 0;

    for (uint8_t i = 0; i < 32; i += 2)
    {
        if (half_bit_buffers[buffer_index][i] && !half_bit_buffers[buffer_index][i + 1])
        {
            decoded_frame[bit_index++] = 1;
        }
        else if (!half_bit_buffers[buffer_index][i] && half_bit_buffers[buffer_index][i + 1])
        {
            decoded_frame[bit_index++] = 0;
        }
        else
        {
            buffer_ready_flags[buffer_index] = false;
            Serial.println("Invalid bit detected");
            return;
        }
    }

    is_frame_ready = true;
    buffer_ready_flags[buffer_index] = false;
}

// Helper function to validate the decoded frame
void IRComm::validateFrame()
{
    if (!is_frame_ready)
        return;

    is_frame_ready = false;

    // Validate start and stop bits
    if (!(decoded_frame[0] && decoded_frame[1] && decoded_frame[15]))
    {
        Serial.println("Start/Stop bit error");
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
    decodeBuffer(); // Decode the buffer into the frame
    validateFrame(); // Validate the frame

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