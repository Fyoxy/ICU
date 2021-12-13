#include "motors.hpp"

void Motors::SetSpeed( int speed ) {

	// Limit driving speed
	if ( speed > 100 ) {
		std::cout << "Speed limit is -100 to 100" << std::endl;
		std::cout << "Setting speed to 100" << std::endl;
		speed = 100;
	} 
	else if ( speed < -100 ) {
		std::cout << "Speed limit is -100 to 100" << std::endl;
		std::cout << "Setting speed to -100" << std::endl;
		speed = -100;
	}

	std::cout << "Set speed is " << speed << std::endl;

	MotorsPWMControl( speed );
}

/* Set the motor control type */
void Motors::SetControlType ( ControlType type ) {

	if ( type == Automatic ) {
	
		m_ControlType = Automatic;
		
	} 
	else m_ControlType = Manual;
}

/* Sets the steering angle of the servo */
void Motors::SetAngle ( int angle, int multiplier ) {

	if (angle > SERVO_MAX_ANGLE) m_angle = SERVO_MAX_ANGLE;
	else if (angle < SERVO_MIN_ANGLE) m_angle = SERVO_MIN_ANGLE;
	else m_angle = angle;

	gpioServo(SERVO_PIN, m_angle);
}

/* Intialize rpi motors */
Motors::Motors() {

	if(DEV_ModuleInit())
    exit(0);

	// Call rpi driver function in MotorDriver.hpp
	Motor_Init();

	// Initialize servo
	if (gpioInitialise() < 0) exit(0);
	//gpioSetSignalFunc(SIGINT, stop);

}

Motors::~Motors() {
	// Close servo
	gpioServo(SERVO_PIN, 0);
	gpioTerminate();

	// Close motors
	Motor_Stop(MOTORA);
    Motor_Stop(MOTORB);
    DEV_ModuleExit();
	
}


void Motors::MotorsPWMControl( int speed ) {

	Motors::currentSpeed = speed;

	// In case of speed is positive go forwards
	if (speed > 0) {
		Motor_Run(MOTORB, BACKWARD, ( speed * ultrasonicMultiplier ) );
		Motor_Run(MOTORA, BACKWARD, ( speed * ultrasonicMultiplier ) );
	}
	// If speed is negative go backwards 
	else if (speed < 0) {
		Motor_Run(MOTORB, FORWARD, -speed );
		Motor_Run(MOTORA, FORWARD, -speed );
	}
	else {
		Motor_Stop(MOTORB);
		Motor_Stop(MOTORA);
	}
	
}

int Motors::GetControlType() {
	return (ControlType) Motors::m_ControlType;
}


/*

int main() {

	Motors motors;

	motors.SetControlType(Motors::ControlController);

	for (;;) {
		motors.ControllerInput(  )
	}

}*/