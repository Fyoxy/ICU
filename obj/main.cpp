#include "controller.hpp"
#include "motors.hpp"
#include "detection.hpp"
#include <thread>
#include <chrono>
#include <iostream>

void ManualDriving( Controller* device, Motors* motor ) {

    for (; motor->GetControlType() == Motors::Manual; ) {
        // If input throttle or reverse
        if ( ( !( device->event.number == device->ButtonType::Throttle ) &&
            !( device->event.number == device->ButtonType::Reverse ) ) ) {

                std::cout << "Kill motors" << std::endl;

            // Kill motors, no throttle or reverse pressed
            motor->SetSpeed( 0 );
        }
        else {
            std::cout << "Else motors" << std::endl;
            // Get axis state because otherwise it may get button pressed before axis
            device->axis = get_axis_state(&device->event, device->axes);

            // Converting controller data to acceptable PWM range
            int axis = ( device->event.number == device->ButtonType::Throttle ) ?
                device->axes[device->axis].y : device->axes[device->axis].x;

            // Split CONTROLLER_AXIS_MAX into 50 segments for negative and positive values
            // DS4 Controller sends values from -32767 to 32767
            int divider = CONTROLLER_AXIS_MAX / 50;
            int speed = ( int ) abs( axis ) / divider;
            
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
    }
        
}

int main() {

	std::cout << "Motors init" << std::endl;
	Motors motors;
	
	std::cout << "Controller init" << std::endl;
	Controller device;

    // Initialize thread variable for manual and automatic control
    std::thread controlThread;
	
	std::cout << "Starting controller listener" << std::endl;

    while ( read_event( device.controller, &device.event ) == 0 ) {
        bool manual;
        manual = motors.GetControlType() ? true : false;

        // Get controller input
        if (device.event.type == JS_EVENT_BUTTON ) {

            switch ( device.event.number ) {

                case device.ButtonType::T :
                    std::cout << "Setting to Automatic" << std::endl;
                    motors.SetControlType( Motors::Automatic );
                    // Wait for thread to close if already running
                    if ( controlThread.joinable() ) controlThread.join();

                    // Start Autonomous driving thread and give it motors to control
                    controlThread = std::thread( AutonomousDriving, &motors );

                    break;
                
                case device.ButtonType::O :
                    if ( device.event.value ) {
                        motors.SetControlType( Motors::Manual );
                        // Wait for previous thread to close if already running
                        if ( controlThread.joinable() ) controlThread.join();

                        std::cout << "Started manual thread" << std::endl;
                        // Start Manual driving thread and give it motors to control and controller to get input
                        controlThread = std::thread( ManualDriving, &device, &motors );
                    }
                    break;

                default:
                    break;

            }
            
        }
        
        fflush(stdout);
    }
    
    close(device.controller);



	// motors.SetControlType( Motors::ControlController, motors );ControllerListener

	std::cout << "Program exit" << std::endl;
}