#pragma once

#include "joystick.hpp"
#include "motors.hpp"
#include <iostream>

#define CONTROLLER_AXIS_MAX 32767

class Controller {

public:
/*
    Controller();
    ~Controller();*/

    enum ButtonType {
        X = 0, 
        O, 
        T, 
        S, 
        L1, 
        L2, 
        Reverse, 
        Throttle, 
        Share, 
        Options, 
        Home, 
        L_stick, 
        R_stick
    };

    int controller;
    struct js_event event;
    size_t axis;
    struct axis_state axes[3] = {0};

    bool throttle = false;
    bool reverse = false;

    void ControllerInit();
    
private:
    const char* m_device = "/dev/input/js0";
};

void ControllerListener( Controller device, Motors motor );