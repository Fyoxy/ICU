#include <pigpio.h>
#include <chrono>
#include <thread>

#define TRIGGER 22
#define ECHO 23

extern float distanceCm;

void MinuSonic() {

    gpioSetMode(TRIGGER, PI_OUTPUT);  // Set GPIO22 as input.
    gpioSetMode(ECHO, PI_INPUT); // Set GPIO23 as output.

    for (;;) {
        // set Trigger to HIGH
        gpioWrite(TRIGGER, 1);
        std::this_thread::sleep_for(std::chrono::microseconds(10));
        gpioWrite(TRIGGER, 0);

        auto start = std::chrono::system_clock::now();
        auto end = std::chrono::system_clock::now();

        while ( gpioRead(ECHO) == 0 ) {
            start = std::chrono::system_clock::now();
        }

        while ( gpioRead(ECHO) == 1 ) {
            end = std::chrono::system_clock::now();
        }

        auto timeElapsed = end - start;

        auto distanceCm = (timeElapsed * 34300) / 2;
    }

}