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

#define SCANS 10 // Amount of scans for ultrasonic sensor
#define STUCK_CM 15
#define US_MIN_CM 10
#define US_MAX_CM 30

int sensor = 1; 

void SigintHandler(int s){
    printf("Caught signal %d\n",s);
    exit(1); 
}

void Ultrasonic( Motors* motor ) {

    if (!sensor)
        std::terminate();

    // Initialize ultrasonic sensor
    gpioSetMode(TRIGGER, PI_OUTPUT);  // Set GPIO22 as input.
    gpioSetMode(ECHO, PI_INPUT); // Set GPIO23 as output.

    // Declare variables
    int distanceArr[SCANS] = {0};
    int averageArr[5] = {0};
    int averageCounter = 0;
    int counter;
    int average = 0;

    auto start = std::chrono::system_clock::now();
    auto end = std::chrono::system_clock::now();

    // Infinite loop to read sensor data
    for (counter = 0;; counter++) {

        // Set sensor Trigger to HIGH to send out pulse
        gpioWrite(TRIGGER, 1);
        // Wait 10 microseconds for the pulse to be sent
        std::this_thread::sleep_for(std::chrono::microseconds(10));
        // Set trigger low to stop sending pulses
        gpioWrite(TRIGGER, 0);

        // Wait until pulse gets sent back to echo
        while ( gpioRead(ECHO) == 0 ) {
            start = std::chrono::system_clock::now();
        }

        while ( gpioRead(ECHO) == 1 ) {
            end = std::chrono::system_clock::now();
        }

        // Calculate time elapsed between sent and received pulse
        std::chrono::duration<float> timeElapsed = end - start;
        // Calculate the pulse traveled distance in cm
        // ( Time * speed of sound ) / Divided by 2 since distance was traveled to the object and back
        int distanceCm = (timeElapsed.count() * 34300) / 2;

        // Add value to the distance array
        distanceArr[counter] = distanceCm;

        // Adjust speed according to distance from object(s)
        
        if ( distanceCm <= US_MIN_CM ) {
            motor->ultrasonicMultiplier = 0;
        }
        else if ( distanceCm >= US_MAX_CM ) {
            motor->ultrasonicMultiplier = 1;
        }
        else {
            motor->ultrasonicMultiplier = ( float ) ( ( distanceCm - 10 ) * 0.05 );
            std::cout << motor->ultrasonicMultiplier << std::endl;
        }

        // Reset the motor speed to adjust with sensor
        motor->SetSpeed( motor->currentSpeed );

        if ( counter >= SCANS ) {

            for ( int i = 0; i < SCANS; i++ ) {
                average += distanceArr[ i ];
            }

            average /= counter;

            averageArr[ averageCounter ] = average;
            averageCounter++;

            if ( averageCounter == 5 ) {

                int tempAverage = 0;
                averageCounter = 0;

                for ( int i = 0; i < 5; i++ ) {
                    tempAverage += averageArr[ i ];
                }

                tempAverage /= 5;

                for ( int i = 0; i < 5; i++) {
                    
                    int upperThreshold = tempAverage + ( tempAverage * 0.1 );
                    int lowerThreshold = tempAverage - ( tempAverage * 0.1 );


                    if ( averageArr[ i ] >= upperThreshold || averageArr[ i ] <= lowerThreshold ) {
                        break;
                    } 
                    else {
                        averageCounter++;
                    } 
                }

                if ( averageCounter == 5 ) {
                    // Set robot stuck
                    std::cout << "Robot stuck 2" << std::endl;
                    motor->robotStuck = 2;

                    // Wait until detection algorithm reverses
                    while ( motor->robotStuck ) {
                        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                    }
                }
                
                averageCounter = 0;
            }
            else if ( average <= STUCK_CM ) {
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
                if ( ( device.event.number == device.ButtonType::T )  && device.event.value) {
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
                else if ( device.event.number == device.ButtonType::X ) {
                    motor->robotStuck = 0;
                    break;
                }
                else if ( device.event.number == device.ButtonType::S ) {
                    if (sensor) sensor = 0;
                    else sensor = 1;
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

    std::cout << "Controller thread stopped" << std::endl;
    motor->SetSpeed( 90 );
    for (;;);
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