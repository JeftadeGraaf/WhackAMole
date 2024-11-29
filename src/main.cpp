#include <IRComm.h>

// Instantiate IRComm object
IRComm ir;

ISR(INT0_vect)
{
    ir.handleReceiveInterrupt();
}

ISR(TIMER1_OVF_vect)
{
    ir.handleTimer1Overflow();
}

ISR(TIMER0_COMPA_vect)
{
    ir.handleTimer0Compare();
}

int main()
{
    Serial.begin(9600);
    ir.begin();
    sei(); // Enable global interrupts

    while (1)
    {
        if (ir.isBufferReady())
        {
            ir.decodeIRMessage();
        }
    }

    return 0;
}
