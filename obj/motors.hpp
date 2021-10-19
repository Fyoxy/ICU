#pragma once

#include <iostream>
#include <pigpio.h>
#include "MotorDriver.hpp"
#include "DEV_Config.hpp"

#define SERVO_BASE_ANGLE 1500 // Base 90 degree angle for servo to drive straight
#define SERVO_MAX_ANGLE 1700 // Limit servo maximum turning angle
#define SERVO_MIN_ANGLE 1300 // Limit servo minimum turning angle
#define SERVO_PIN 18 // BCM hardware PWM servo pin

class Motors {

public:

	// Declare motors constructor and deconstructor
	Motors();
	~Motors();
	
	enum ControlType : char {
		ControlAutomatic = 0, ControlController
	};

	void SetSpeed( int speed );

	// Set the motor control type
	void SetControlType( ControlType type, Motors motor );

	// Sets the steering angle of the servo
	void SetAngle( int angle, int multiplier = 1 );

	// Intialize rpi motorsS
	//void MotorsInit();

private:

	ControlType m_ControlType = ControlController; 

	int m_angle = SERVO_BASE_ANGLE;

	void MotorsPWMControl( int speed );
};