#pragma once

#include <pigpio.h>

class Led {

public:

    Led();
    ~Led(); 

    enum LedColor {
        GREEN = 13,
        BLUE = 19,
        RED = 26,
    };

    void SetLed( Led::LedColor color );

private:
    void SetLedsLow();
};