#include <HardwareSerial.h>
#include "IR_protocol.h"

int main() {
    init_protocol();

    Serial.begin(9600);

    while (1) {
        TEST_IR_PROTOCOL();
    }


    return 0;
}