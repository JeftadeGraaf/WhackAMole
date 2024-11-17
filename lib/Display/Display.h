#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>

class Display {
public:
    Display(int backlight_pin);
    void init();
    void refresh_backlight();

private:
    // Add private members and methods as needed
    int _backlight_pin;
};

#endif // DISPLAY_H