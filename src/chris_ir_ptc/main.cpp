#include <avr/io.h>
#include <avr/interrupt.h>

#include "BasicFunctions.h"
#include "IR_protocol.h"

int recvData;

int main() {
  // Set up the IR transmitter
  init_IR_transmitter_timer0();
  
  // Set the destination for the received data
  setDataDestination(&recvData);

  while (1) {
    send(0x1234);
  }

  return 0;
}