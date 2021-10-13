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

	MotorsPWMControl( speed );
}

/* Set the motor control type */
void Motors::SetControlType ( ControlType type, Motors motor ) {

	if ( type == ControlController ) {
	
		m_ControlType = ControlController;
		
	} 
	else if (type == ControlAutomatic ) m_ControlType = ControlAutomatic;
	else m_ControlType = ControlManual;
}

/* Sets the steering angle of the servo */
void Motors::SetAngle ( int angle, int multiplier ) {

	if (angle > SERVO_MAX_ANGLE) m_angle = SERVO_MAX_ANGLE;
	else if (angle < SERVO_MIN_ANGLE) m_angle = SERVO_MIN_ANGLE;
	else m_angle = angle;

	std::cout << angle << std::endl;

	pwmWrite( SERVO_PIN, m_angle );
}

/* Intialize rpi motors */
void Motors::MotorsInit() {

	if(DEV_ModuleInit())
        exit(0);

	// Call rpi driver function in MotorDriver.hpp
	Motor_Init();

	// Initialize servo
	pullUpDnControl(SERVO_PIN, PUD_OFF);
	pinMode(SERVO_PIN, PWM_OUTPUT);
	pwmSetMode(PWM_MODE_MS);
	pwmSetRange(2000);
	pwmSetClock(192);
}

void Motors::MotorsPWMControl( int speed ) {

	// In case of speed is positive go forward
	if (speed > 0) {
		Motor_Run(MOTORB, FORWARD, speed);
		Motor_Run(MOTORA, FORWARD, speed);
	}
	// If speed is negative go backwards 
	else if (speed < 0) {
		Motor_Run(MOTORB, BACKWARD, -speed);
		Motor_Run(MOTORA, BACKWARD, -speed);
	}
	else {
		Motor_Stop(MOTORB);
		Motor_Stop(MOTORA);
	}
	
}

/*

int main() {

	Motors motors;

	motors.SetControlType(Motors::ControlController);

	for (;;) {
		motors.ControllerInput(  )
	}

}*/