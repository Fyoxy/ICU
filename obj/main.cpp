#include "controller.hpp"
#include "motors.hpp"
#include <thread>
#include <chrono>
#include <iostream>

void SetMotorz( Motors& motor ) {
    std::cout << "SS func" << std::endl;
    motor.SetAngle( SERVO_MAX_ANGLE );

}

int main() {

	std::cout << "Motors init" << std::endl;
	Motors motors;
	motors.MotorsInit();

	auto motorsT = std::thread( SetMotorz, std::ref(motors));

	std::chrono::milliseconds(5000);
	motors.SetAngle(SERVO_BASE_ANGLE);
	
	/*
	std::cout << "Controller init" << std::endl;
	Controller device;

	device.ControllerInit();
	
	std::cout << "Starting controller thread" << std::endl;

	std::thread controller( ControllerListener, device , motors );

	motors.SetControlType( Motors::ControlController, motors );

	std::cout << "Waiting for controller thread" << std::endl;

	controller.join();*/
	/*
	initialize motors
	run motor inits

	create motor object

	input is controller
	-> thread opened for controller control

	.. Send motor object to controller control

	input is autonomous
	-> open thread for motors

	.. Send curve and speed to motors thread


	process image
	-> process image
*/
}