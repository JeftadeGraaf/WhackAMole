#include <HardwareSerial.h>
#include "IR_protocol.h"

int main() {
    Serial.begin(9600);

    init_protocol();

    while (1) {
        TEST_IR_PROTOCOL();
    }


    return 0;
}