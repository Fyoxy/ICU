#include "controller.hpp"

Controller::Controller() {

    controller = open( Controller::m_device, O_RDONLY );

    if (controller == -1)
        perror("Could not open controller");

}

Controller::~Controller() {
    close(Controller::controller);
}