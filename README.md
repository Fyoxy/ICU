
# TalTech Robotics Club Project ICU aka Folkrace with a camera

ICU is a folkrace robot that uses visual input to race in a competition known as folkrace. The type of folkrace we intend to compete is hosted by Robotex International. You can find the ruleset from their website https://robotex.international/

## Setup

To set this project up for yourself you should have at least OpenCV version 4 installed (I have not tested it with any other versions). Pigpio is also required for servo control.

The program is meant to be used along with a dualshock 4 controller, but any controller should work which can be read by [this](https://gist.github.com/jasonwhite/c5b2048c15993d285130) C code.

Use your favourite compiler to compile the code and make necessary changes in the Makefile. Be sure to have a bin folder created inside the project root in order to run the Makefile.

The main file is meant to be run on a Raspberry Pi 4 (or 3) with [this](https://www.waveshare.com/motor-driver-hat.htm) motor driver hat. Other components can be found from the following sheet:

![Electronics scheme](https://gitlab.com/Fyoxy/icu/-/raw/main/readme/ICU_Scheme.PNG)

## Usage

Be sure to run the Pigpio daemon and have a controller connected before running the executable!

After compiling with `make` and on success, you should have `main.out` file which should be ran as sudo in order to use the GPIO library by Pigpio.
```bash
sudo ./main.out
```

## Development log

### Footage from code in action

#### Program live feed
![Live feed from program](https://gitlab.com/Fyoxy/icu/-/raw/main/readme/icu_in_action.mp4)

#### Video from the competition
![Video from the competition](Missing yt link)
soon

#### Pictures of the robot
![Icu Pic 2](https://gitlab.com/Fyoxy/icu/-/raw/main/readme/icu_robot_2.png) ![Icu Pic 1](https://gitlab.com/Fyoxy/icu/-/raw/main/readme/icu_robot_1.png)

### 13.12.2021
This project is finished and we successfully competed at Robotex International 2021. I'd like to thank my project team members @helenents1 and @sander_kajak for helping me make this project a reality.

This project might see a continuation in the following year.

## Contributing

Fork it, send pull requests, add issues, basically do whatever and I'll be notified and check out your input.
Feel free to ask me anything about the project.
