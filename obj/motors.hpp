#pragma once

#include <iostream>
#include "MotorDriver.h"
#include "DEV_Config.h"

#define SERVO_BASE_ANGLE 75 // Base 90 degree angle for servo to drive straight
#define SERVO_MAX_ANGLE 90 // Limit servo maximum turning angle
#define SERVO_MIN_ANGLE 60 // Limit servo minimum turning angle
#define SERVO_PIN 18 // BCM hardware PWM servo pin

class Motors {

public:
	
	enum ControlType : char {
		ControlAutomatic = 0, ControlController, ControlManual
	};

	void SetSpeed( int speed );

	/* Set the motor control type */
	void SetControlType ( ControlType type, Motors motor );

	/* Sets the steering angle of the servo */
	void SetAngle ( int angle, int multiplier = 1 );

	/* Intialize rpi motors */
	void MotorsInit();

private:

	ControlType m_ControlType = ControlController; 
	int m_angle = SERVO_BASE_ANGLE;

	void MotorsPWMControl( int speed );

	void SmoothPWMControl( int speed );
	//void ControllerInput( Controller device );
};