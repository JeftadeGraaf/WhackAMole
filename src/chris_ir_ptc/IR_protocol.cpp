#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "BasicFunctions.h"
#include "IR_protocol.h"

#define RX 2
#define TX 6
#define BIT_DURATION 100  // microseconds
#define CPU_FREQ 16000000

#define ULONG_MAX 4294967295  // max value of unsigned long, used for overflow protection

// Total transmission size: 26 or 42 bits; 
// 8 bits start, 1 bit null, 1 bit 16or32bitsDataSize 16 bits data 
// OR
// 8 bits start, 1 bit null, 1 bit 16or32bitsDataSize, 32 bits data

// 16or32bitsDataSize tells us if the incoming data is 16 bits or 32 bits


// pretty much equivalent to the Arduino millis() function, but for our 38kHz timer
volatile unsigned long timer0_cycles;

bool toneInitDone = false;
bool tonePlaying = false;
bool transmitMessageFlag = false;
bool sendStartSignalFlag = false;
bool sendBigDataFormatFlag = false;
bool waitingForStartSignal = false;

int tempDestination;
int* dataDestination = &tempDestination;

volatile bool dataFormatSet = false;

volatile uint8_t startSignalBuffer;
volatile uint8_t transmitBufferIndex;
volatile uint8_t maxTransmitBufferIndex;
volatile uint8_t recvBufferIndex;
volatile uint8_t desiredSignalEndCycle;
volatile uint8_t lastPulseValue = LOW;
volatile uint8_t pulseValue;
volatile uint8_t messageSize = 16;

volatile unsigned long lastPulseEndTime;
volatile uint8_t bits_passed;

volatile unsigned long lastPulseStartTime = 0;
volatile unsigned long expectedMessageStartTime;

bool bigDataFormat = false;
unsigned long inBuffer;  
unsigned long outBuffer;


uint8_t readNthBitOfInt(int num, int n) {
  return (num >> n) & 1;
}

void startPulse38kHz() {
  tonePlaying = true;
  sei();
}

void endPulse38kHz() {
  tonePlaying = false;
  BF_digitalWrite(TX, LOW);
}

bool checkChecksum(int data, bool big) {
  int checksum = data >> (big ? 16 : 8);
  int dataPart = data & (big ? 0xFFFF : 0xFF);

  int calculatedChecksum = 0;
  for (int i = 0; i < (big ? 16 : 8); i++) {
    calculatedChecksum += (dataPart >> i) & 1;
  }

  return calculatedChecksum == checksum;
}

// ISR for the receiver
void RX_ISR() {
  pulseValue = BF_digitalRead(RX);

  // if the start signal is done, start putting bits into the inBuffer
  // (we put this here because there is an extra 0 to break segments of the message apart that would 
  // otherwise be interpreted as data.)
  if (timer0_cycles >= expectedMessageStartTime) {
    // because the receiver only triggers on CHANGE, we need to interpret how many bits have passed since the last pulse
    // we know the message is supposed to begin at cycle number expectedMessageStartTime

    // We measure the current cycles and compare it to the expectedMessageStartTime
    // The amount of cycles that have passed since the expectedMessageStartTime is equal to the number of bits that have passed

    if (recvBufferIndex >= messageSize) {
      // we have received all the bits we need
      // we can now check the checksum
        bool checksumCorrect = checkChecksum(inBuffer, bigDataFormat);
        if (checksumCorrect) {
          // checksum is correct, we can now extract the data
          *dataDestination = inBuffer & (bigDataFormat ? 0xFFFF : 0xFF);
          // send the data to the main program
          // we can now reset the variables
          inBuffer = 0;
          recvBufferIndex = 0;
          dataFormatSet = false;
          waitingForStartSignal = true;
        } else {
          // checksum is incorrect, we can now reset the variables
          inBuffer = 0;
          recvBufferIndex = 0;
          dataFormatSet = false;
          waitingForStartSignal = true;
        }
    }

    bits_passed = timer0_cycles - lastPulseEndTime;

    if (!dataFormatSet) {
      bits_passed = timer0_cycles - expectedMessageStartTime;

      // the first bit always determines the format of the message
      bigDataFormat = bits_passed == 0 ? pulseValue : bigDataFormat;
      messageSize = bigDataFormat ? 32 : 16;
      dataFormatSet = true;
      bits_passed--;  // we don't want to include the data format bit in the data
    }
    
    // add the inverse of the pulseValue to the inBuffer
    for (uint8_t i = 0; i < bits_passed; i++) {
      // left shift the inBuffer by 1, then add the pulseValue
      inBuffer <<= 1;
      inBuffer += pulseValue == HIGH ? 0 : 1;
      recvBufferIndex++;
    }

    lastPulseEndTime = timer0_cycles;

  }

  if (waitingForStartSignal) {
    // if pulseValue is HIGH and lastPulseValue is LOW, note down the current cycle in lastPulseStartTime
    if (pulseValue == HIGH && lastPulseValue == LOW) {
      lastPulseStartTime = timer0_cycles;
    } 
    
    // if pulseValue becomes LOW while there are insufficient bits in the buffer, continue. If there are enough bits, stop.
    else if (pulseValue == LOW && lastPulseValue == HIGH) {
      if (!(timer0_cycles - lastPulseStartTime < 8)) {
        waitingForStartSignal = false;
        expectedMessageStartTime = timer0_cycles + 1;
      }
    }
  }
}

// ISR for the transmitter
ISR(TIMER0_COMPA_vect) {
  if (tonePlaying) {
    PORTD ^= _BV(PORTD6);  // Toggle pin 6
  }

  if (sendStartSignalFlag) {
    // Send 38kHz signal for 8 cycles
    desiredSignalEndCycle = timer0_cycles + 8;
    if (timer0_cycles >= desiredSignalEndCycle) {
      sendStartSignalFlag = false;
      transmitMessageFlag = true;
    }
  }

  if (transmitMessageFlag) {
    // first check if index exceeds buffer size
    maxTransmitBufferIndex = sendBigDataFormatFlag ? 33 : 17;
    if (transmitBufferIndex >= maxTransmitBufferIndex) {
      transmitMessageFlag = false;

    // send message bit by bit
    } else {
      if (readNthBitOfInt(outBuffer, transmitBufferIndex) == 0) {
        endPulse38kHz();  // if bit is 0, send nothing for 1 clock cycle
      } else {
        startPulse38kHz();
      }

      transmitBufferIndex++;
    }
  }

  // overflow only happens every 30 hours or so, but just in case
  if (timer0_cycles >= ULONG_MAX) {
    timer0_cycles = 0;
  } else {
    timer0_cycles++;
  }
}

// startPulse38kHz() and endPulse38kHz() are adapted from the tone() and noTone() functions in the Arduino core. 
// Source: https://github.com/arduino/ArduinoCore-avr/blob/master/cores/arduino/Tone.cpp
void init_IR_transmitter_timer0() {
  BF_pinMode(RX, INPUT);
  BF_pinMode(TX, OUTPUT);

  BF_attachInterrupt(BF_digitalPinToInterrupt(RX), RX_ISR, CHANGE);

  OCR0A = F_CPU / 38000 / 2 - 1;
  TCCR0A = _BV(WGM01);  // CTC mode
  TCCR0B = _BV(CS00);   // No prescaling
  TIMSK0 |= _BV(OCIE0A);  // Enable timer compare interrupt

  sei();
}

void prepareOutBuffer(uint16_t data, bool big) {
  // big means 16 bits data, 16 bits checksum
  // otherwise 8 bits data, 8 bits checksum
  // MSB is part of checksum, LSB of data.

  if (big) {
    outBuffer = ~data;
    outBuffer <<= 16;
    outBuffer += data;
    
  } else {
    outBuffer = ~data;
    outBuffer <<= 8;
    outBuffer += data;
  }
}

void send(uint16_t data) {
  bool big = data > 255;

  prepareOutBuffer(data, big);
  sendStartSignalFlag = true;
}

void setDataDestination(int* destination) {
  dataDestination = destination;
}