#include "controller.hpp"
#include "motors.hpp"
#include "detection.hpp"
#include "led.hpp"
#include <thread>
#include <chrono>
#include <iostream>
#include <signal.h>


#define TRIGGER 22
#define ECHO 23

void SigintHandler(int s){
    printf("Caught signal %d\n",s);
    exit(1); 
}

void Ultrasonic( Motors* motor ) {

    gpioSetMode(TRIGGER, PI_OUTPUT);  // Set GPIO22 as input.
    gpioSetMode(ECHO, PI_INPUT); // Set GPIO23 as output.

    int distanceArr[20] = {0};
    int counter;
    int average = 0;
    

    auto start = std::chrono::system_clock::now();
    auto end = std::chrono::system_clock::now();

    for (counter = 0;; counter++) {
        // set Trigger to HIGH
        gpioWrite(TRIGGER, 1);
        std::this_thread::sleep_for(std::chrono::microseconds(10));
        gpioWrite(TRIGGER, 0);

        while ( gpioRead(ECHO) == 0 ) {
            start = std::chrono::system_clock::now();
        }

        while ( gpioRead(ECHO) == 1 ) {
            end = std::chrono::system_clock::now();
        }

        std::chrono::duration<float> timeElapsed = end - start;

        auto distanceCm = (timeElapsed.count() * 34300) / 2;

        distanceArr[counter] = (int) distanceCm;

        if ( counter >= 20 ) {

            for (int i = 0; i < 20; i++) {
                average += distanceArr[i];
            }

            average /= counter;
            std::cout << "Average: " << average << std::endl;

            if ( average <= 15 ) {
                // Set robot stuck
                std::cout << "Robot stuck" << std::endl;
                motor->robotStuck = 1;

                // Wait until detection algorithm reverses
                while ( motor->robotStuck ) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                }
                
            }
            

            counter = 0;
        }

        // Sleep to prevent ultrasonic sensor from crashing
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

    }

}

int main() {

    // Window initialization to show image from DisplayHistogram
    //cv::namedWindow("Source and Histogram", cv::WINDOW_AUTOSIZE);

    // Control-c handler
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = SigintHandler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);

	std::cout << "Motors init" << std::endl;
	Motors* motor = new Motors();
	
	std::cout << "Controller init" << std::endl;
	Controller device;

    Led led;

    led.SetLed( Led::LedColor::RED );

    std::thread detectionT;
    std::thread ultraSonicT;

    ultraSonicT = std::thread( Ultrasonic, motor );
	
	std::cout << "Starting controller listener" << std::endl;
    while ( read_event( device.controller, &device.event ) == 0 ) {

        // Get controller input
        switch (device.event.type)
        {
            case JS_EVENT_BUTTON:
                if ( device.event.number == device.ButtonType::T ) {
                    motor->SetControlType( Motors::ControlType::Automatic );
                    detectionT = std::thread( Detection, motor );
                    detectionT.detach();
                    led.SetLed( Led::LedColor::BLUE );
                    break;
                }
                else if ( device.event.number == device.ButtonType::O ) {
                    motor->SetControlType( Motors::ControlType::Manual );
                    led.SetLed( Led::LedColor::GREEN );
                    motor->SetSpeed( 0 );
                    break;
                }
                if ( device.event.number == device.ButtonType::Throttle ) {
                    device.throttle = ( device.event.value ) ? true : false;
                    break;
                }
                else if ( device.event.number == device.ButtonType::Reverse ) {
                    device.reverse = ( device.event.value ) ? true : false;
                    break;
                }
            case JS_EVENT_AXIS:
                device.axis = get_axis_state(&device.event, device.axes);
                if ( device.axis ) {

                }/*
                if (device.axis < 3)
                    printf("Axis %zu at (%6d, %6d)\n", 
                           device.axis, device.axes[device.axis].x, device.axes[device.axis].y);*/
                break;
            default:
                /* Ignore init events. */
                break;
        }

        if ( motor->GetControlType() ) {

            // If input throttle or reverse
            if ( ( !device.throttle && !device.reverse ) ) {
                // Kill motors, no throttle or reverse pressed
                motor->SetSpeed( 0 );
            }
            // Check if L2 axis or R2 axis
            else if ( device.axis == 1 || device.axis == 2 ) {
                // Get axis state because otherwise it may get button pressed before axis.
                //device.axis = get_axis_state(&device.event, device.axes);

                // Converting controller data to acceptable PWM range
                int axis = ( device.throttle ) ? device.axes[device.axis].y : device.axes[device.axis].x;

                // Split CONTROLLER_AXIS_MAX into 50 segments for negative and positive values
                // DS4 Controller sends values from -32767 to 32767
                int divider = CONTROLLER_AXIS_MAX / 50;
                int speed = ( int ) abs( axis ) / divider;

                // Check check if throttle or anything else
                if ( device.throttle ) {
                    // Check if axis was negative or positive to determine speed segment 1 - 50 or 51 - 100
                    speed = ( device.axes[device.axis].y > 0 ) ? ( 50 + speed ) : ( 51 - speed );
                    motor->SetSpeed( speed );
                }
                else {
                    // Same process for reverse
                    speed = ( device.axes[device.axis].x > 0 ) ? ( 50 + speed ) : ( 51 - speed );
                    // Invert speed to reverse
                    motor->SetSpeed( -speed );
                }
            }

            // If steering with left thumbstick
            if ( device.axis == 0 ) {
                if ( device.axes[device.axis].x == 0 ) {
                    motor->SetAngle( SERVO_BASE_ANGLE );
                } 
                else {
                    // Converting controller data to acceptable PWM range
                    int axis = device.axes[device.axis].x;

                    // Split CONTROLLER_AXIS_MAX into 200 segments for negative and positive values
                    int divider = CONTROLLER_AXIS_MAX / 200;

                    // Conversion loop to get angle up to 200
                    int angle = ( int ) abs( axis ) / divider;


                    if ( device.axes[device.axis].x < 0 ) {
                        motor->SetAngle( SERVO_BASE_ANGLE + angle - 1 );
                    }  
                    else if ( device.axes[device.axis].x > 0 ) { 
                        motor->SetAngle( SERVO_BASE_ANGLE - angle + 1 );
                    }
                } 
            }
        }
        
        fflush(stdout);
    }
    
    close(device.controller);
	
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