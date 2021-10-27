#include "led.hpp"

void Led::SetLed( Led::LedColor color ) {
    // Set all LED GPIOs low
    SetLedsLow();

    switch (color) {
        
        case GREEN:
            gpioWrite(GREEN, 1);
            break;
        
        case BLUE:
            gpioWrite(BLUE, 1);
            break;
        
        case RED:
            gpioWrite(RED, 1);
            break;
    }
}

void Led::SetLedsLow() {
    gpioWrite(GREEN, 0);
    gpioWrite(BLUE, 0);
    gpioWrite(RED, 0);
}


Led::Led() {
    if ( !(gpioInitialise() >= 0) ) {
        gpioInitialise();
    }
}

Led::~Led() {
    
}