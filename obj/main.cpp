#include "controller.hpp"
#include "motors.hpp"
#include <thread>
#include <chrono>
#include <iostream>

void ControllerListener( Controller* device, Motors* motor ) {

    while ( read_event( device->controller, &device->event ) == 0 ) {
        // Get controller input
        switch (device->event.type)
        {
            case JS_EVENT_BUTTON:
                /*if ( device->event.number == device->ButtonType::T ) {
                    // auto detectionT = std::thread detection( Detection, motor );
                }
                else if ( device->event.number == device->ButtonType::O ) {
                    //std::terminate( );
                }*/
                if ( device->event.number == device->ButtonType::Throttle ) {
                    device->throttle = ( device->event.value ) ? true : false;
                }
                else if ( device->event.number == device->ButtonType::Reverse ) {
                    device->reverse = ( device->event.value ) ? true : false;
                }
                break;
            case JS_EVENT_AXIS:
                device->axis = get_axis_state(&device->event, device->axes);
                if ( device->axis ) {

                }/*
                if (device->axis < 3)
                    printf("Axis %zu at (%6d, %6d)\n", 
                           device->axis, device->axes[device->axis].x, device->axes[device->axis].y);*/
                break;
            default:
                /* Ignore init events. */
                break;
        }

        // If input throttle or reverse
        if ( ( !device->throttle && !device->reverse ) ) {
            // Kill motors, no throttle or reverse pressed
            motor->SetSpeed( 0 );
        }
        else {
            // Converting controller data to acceptable PWM range
            std::cout << "Axis" << std::endl;
            std::cout << device->axes[device->axis].y << std::endl;
            std::cout << "Axis2" << std::endl;
            int axis = ( device->throttle ) ? device->axes[device->axis].y : device->axes[device->axis].y;

            std::cout << "Divider/speed" << std::endl;
			// Split CONTROLLER_AXIS_MAX into 50 segments for negative and positive values
            // DS4 Controller sends values from -32767 to 32767
			int divider = CONTROLLER_AXIS_MAX / 50;
            int speed = ( int ) abs( axis ) / divider;
			
            std::cout << "SEtSpeed" << std::endl;
            // Check check if throttle or anything else
            if ( device->throttle ) {
                // Check if axis was negative or positive to determine speed segment 1 - 50 or 51 - 100
                speed = ( device->axes[device->axis].y > 0 ) ? ( 50 + speed ) : ( 51 - speed );
                motor->SetSpeed( speed );
            }
            else {
                // Same process for reverse
                speed = ( device->axes[device->axis].x > 0 ) ? ( 50 + speed ) : ( 51 - speed );
                // Invert speed to reverse
                motor->SetSpeed( -speed );
            }
        }
        
        // If steering with left thumbstick
        if ( device->axis == 0 ) {
            if ( device->axes[device->axis].x == 0 ) {
                motor->SetAngle( SERVO_BASE_ANGLE );
            } 
            else {
                // Converting controller data to acceptable PWM range
                int axis = device->axes[device->axis].x;

                // Split CONTROLLER_AXIS_MAX into 200 segments for negative and positive values
			    int divider = CONTROLLER_AXIS_MAX / 200;

                // Conversion loop to get angle up to 200
			    int angle = ( int ) abs( axis ) / divider;

                if ( device->axes[device->axis].x < 0 ) {
                    motor->SetAngle( SERVO_BASE_ANGLE + angle - 1 );
                }  
                else if ( device->axes[device->axis].x > 0 ) { 
                    motor->SetAngle( SERVO_BASE_ANGLE - angle + 1 );
                }
            } 
        }
        
        
        fflush(stdout);
    }
    
    close(device->controller);
}

int main() {

	std::cout << "Motors init" << std::endl;
	Motors motors;
	
	std::cout << "Controller init" << std::endl;
	Controller device;
	
	std::cout << "Starting controller thread" << std::endl;

    auto controllerT = std::thread( ControllerListener, &device, &motors );

	// motors.SetControlType( Motors::ControlController, motors );ControllerListener

	std::cout << "Waiting for controller thread" << std::endl;

	controllerT.join();
	
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