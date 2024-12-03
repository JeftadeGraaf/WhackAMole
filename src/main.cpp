#include <IRComm.h>
#include <util/delay.h>

// Instantiate IRComm object
IRComm ir;

ISR(INT0_vect)
{
    ir.onReceiveInterrupt();
}

ISR(TIMER1_OVF_vect)
{
    ir.onTimer1Overflow();
}

ISR(TIMER0_COMPA_vect)
{
    ir.onTimer0CompareMatch();
}

int main()
{
    Serial.begin(9600);
    ir.initialize();
    sei(); // Enable global interrupts
    uint16_t msg = 0b00000000000;

    while (1)
    {
        
        if(ir.isBufferReady())
        {
            uint16_t data = ir.decodeIRMessage();
            Serial.print("Received data: ");
            Serial.println(data);
            msg = data + 1;
            _delay_ms(200);
        }
        else {
            ir.sendFrame(msg);
        }
    }

    return 0;
}
