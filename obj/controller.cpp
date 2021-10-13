#include "controller.hpp"

void Controller::InitController() {
    controller = open(m_device, O_RDONLY);

    if (controller == -1)
        perror("Could not open controller");

}

void ControllerListener( Controller& device, Motors& motor ) {

    // Create controller to listen for
    std::cout << "Controller listener started" << std::endl;
    while ( read_event( device.controller, &device.event ) == 0 ) {

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
                device.axis = get_axis_state(&device.event, device.axes);/*
                if (device.axis < 3)
                    printf("Axis %zu at (%6d, %6d)\n", 
                           device.axis, device.axes[device.axis].x, device.axes[device.axis].y);*/
                break;
            default:
                /* Ignore init events. */
                break;
        }

        std::cout << "Throttle: " << device.throttle << std::endl;
        std::cout << "Reverse: " << device.reverse << std::endl;

        if ( device.throttle && device.reverse ) {
            // Kill motors, no throttle or reverse pressed
            std::cout << "Kill motors" << std::endl;
        }
        else {
            // Converting controller data to acceptable PWM range
			int axis = device.axes[device.axis].y;
			
			// Split CONTROLLER_AXIS_MAX into 50 segments for negative and positive values
			int divider = CONTROLLER_AXIS_MAX / 50;
			int temp = 0;
			int speed;

			for (speed = 1; abs( axis ) >= temp; temp += divider, speed++);

            std::cout << "Speed" << std::endl;
			std::cout << speed << std::endl;
        }

        //std::cout << "Flushing" << std::endl;


        fflush(stdout);
    }

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