#include "controller.hpp"
#include "motors.hpp"
#include <thread>

int main() {

	std::cout << "Starting main" << std::endl;
	std::cout << "Motors init" << std::endl;
	Motors motors;
	
	std::cout << "Controller init" << std::endl;
	Controller device;
	
	std::cout << "Starting controller thread" << std::endl;

	std::thread controller( ControllerListener, device , motors );

	//motors.SetControlType( Motors::ControlController, motors );

	std::cout << "Starting loop" << std::endl;

	controller.join();
	/*
	initialize motors
	run motor inits

	create motor object

	input is controller
	-> thread opened for controller control

	.. Send motor object to controller control

	input is keyboard
	-> waitkey

	.. Send data to motor object


	input is autonomous
	-> open thread for motors

	.. Send curve and speed to motors thread


	process image
	-> process image
*/
}