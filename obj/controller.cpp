#include "controller.hpp"

Controller::Controller() {
    controller = open(m_device, O_RDONLY);

    if (controller == -1)
        perror("Could not open controller");

}

Controller::~Controller() {
    close(Controller::controller);
}

void ControllerListener( Controller device, Motors motor ) {

    // Create controller to listen for
    std::cout << "Controller listener started" << std::endl;
    while ( read_event( device.controller, &device.event ) == 0 ) {

        // Get controller input
        switch (device.event.type)
        {
            case JS_EVENT_BUTTON:
                if ( device.event.number == device.ButtonType::Throttle ) {
                    device.throttle = ( device.event.value ) ? true : false;
                }
                else if ( device.event.number == device.ButtonType::Reverse ) {
                    device.reverse = ( device.event.value ) ? true : false;
                }
                break;
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

        // If input throttle or reverse
        if ( ( !device.throttle && !device.reverse ) ) {
            // Kill motors, no throttle or reverse pressed
            std::cout << "Stopped motors" << std::endl;
            motor.SetSpeed( 0 );
        }
        else {
            // Converting controller data to acceptable PWM range
            int axis = ( device.throttle ) ? device.axes[device.axis].y : device.axes[device.axis].x;
			
			// Split CONTROLLER_AXIS_MAX into 50 segments for negative and positive values
			int divider = CONTROLLER_AXIS_MAX / 50;
            int speed = ( int ) abs( axis ) / divider;
			
            // Check check if throttle or anything else
            if ( device.throttle ) {
                // Check if axis was negative or positive to determine speed segment 1 - 50 or 51 - 100
                speed = ( device.axes[device.axis].y > 0 ) ? ( 50 + speed ) : ( 51 - speed );
                std::cout << "Speed + motors" << std::endl;
                motor.SetSpeed( speed );
            }
            else {
                // Same process for reverse
                speed = ( device.axes[device.axis].x > 0 ) ? ( 50 + speed ) : ( 51 - speed );
                // Invert speed to reverse
                std::cout << "Speed - motors" << std::endl;
                motor.SetSpeed( -speed );
            }
        }
        
        // If steering with left thumbstick
        if ( device.axis == 0 ) {
            if ( device.axes[device.axis].x == 0 ) {
                std::cout << "Angle base" << std::endl;
                motor.SetAngle( SERVO_BASE_ANGLE );
            } 
            else {
                // Converting controller data to acceptable PWM range
                int axis = device.axes[device.axis].x;

                // Split CONTROLLER_AXIS_MAX into 15 segments for negative and positive values
			    int divider = CONTROLLER_AXIS_MAX / 15;

                // Conversion loop to get angle up to 15
			    int angle = ( int ) abs( axis ) / divider;

                if ( device.axes[device.axis].x < 0 ) {
                    motor.SetAngle( SERVO_BASE_ANGLE + angle - 1 );
                }  
                else if ( device.axes[device.axis].x > 0 ) { 
                    motor.SetAngle( SERVO_BASE_ANGLE - angle + 1 );
                }
            } 
        }
        
        
        fflush(stdout);
    }

    std::cout << "Closing controller" << std::endl;
    std::cout << read_event( device.controller, &device.event ) << std::endl;

    close(device.controller);
}
/*
int main()
{
    std::thread listener( ControllerListener );

    for ( ;; );

    //Listener( controller );

    return 0;
}*/