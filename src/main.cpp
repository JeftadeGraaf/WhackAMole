#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <Wire.h>
#include <HardwareSerial.h>
#include <Nunchuk.h>

#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include <Display.h>

// Define UART baud rate
#define BAUDRATE 9600
#define MYUBRR F_CPU/16/BAUD-1

#define NUNCHUK_ADDRESS 0x52
#define NUNCHUCK_WAIT 1000
#define BACKLIGHT_PIN 5

// For the Adafruit shield, these are the default.
#define TFT_DC 9
#define TFT_CS 10

// Create display objects
Display display(BACKLIGHT_PIN, TFT_CS, TFT_DC);

// Function prototypes
bool init_nunchuck();
bool show_state();

int main(void) {

    // Initialize backlight
    display.init();     
    display.refresh_backlight();
	display.clearScreen();

    sei(); // Enable global interrupts

    // Draw the initial cursor
	display.drawGraphicalCursor(120, 160, 10, ILI9341_WHITE);
    while (1) {
        // Refresh the backlight (simulate brightness adjustments)
        display.refresh_backlight();
        _delay_ms(10);  // Small delay for stability
    }
}

bool init_nunchuck() {
    Serial.print("-------- Connecting to nunchuk at address 0x");
    Serial.println(NUNCHUK_ADDRESS, HEX);

    if (!Nunchuk.begin(NUNCHUK_ADDRESS)) {
        Serial.println("******** No nunchuk found");
        Serial.flush();
        return false;
    }

    Serial.print("-------- Nunchuk with Id: ");
    Serial.println(Nunchuk.id);
    return true;
}

bool show_state(void) {
    if (!Nunchuk.getState(NUNCHUK_ADDRESS)) {
        Serial.println("******** No nunchuk found");
        Serial.flush();
        return false;
    }

    Serial.println("------State data--------------------------");
    Serial.print("Joy X: ");
    Serial.print(Nunchuk.state.joy_x_axis);
    Serial.print("\t\tAccel X: ");
    Serial.print(Nunchuk.state.accel_x_axis);
    Serial.print("\t\tButton C: ");
    Serial.println(Nunchuk.state.c_button);

    Serial.print("Joy Y: ");
    Serial.print(Nunchuk.state.joy_y_axis);
    Serial.print("\t\tAccel Y: ");
    Serial.print(Nunchuk.state.accel_y_axis);
    Serial.print("\t\tButton Z: ");
    Serial.println(Nunchuk.state.z_button);

    Serial.print("\t\t\tAccel Z: ");
    Serial.println(Nunchuk.state.accel_z_axis);

    return true;
}