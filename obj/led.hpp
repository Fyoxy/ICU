#pragma once

#include <pigpio.h>

class Led {

public:

    Led();
    ~Led(); 

    enum LedColor {
        GREEN = 19,
        BLUE = 13,
        RED = 26,
    };

    void SetLed( Led::LedColor color );

private:
    void SetLedsLow();
};