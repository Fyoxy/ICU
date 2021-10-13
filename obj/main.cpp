#include "controller.hpp"
#include "motors.hpp"
#include <thread>

int main() {

	std::cout << "Starting main" << std::endl;

	Motors motors;
	std::cout << "Motors init" << std::endl;
	motors.MotorsInit();

	Controller device;
	std::cout << "Controller init" << std::endl;
	device.InitController();
	
	std::cout << "Starting controller thread" << std::endl;

	std::thread controller( ControllerListener, std::ref( device ), std::ref( motors ) );

	//motors.SetControlType( Motors::ControlController, motors );

	std::cout << "Starting loop" << std::endl;

	for(;;);
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