# Line Following Buggy

[![Language](https://img.shields.io/badge/language-C%2B%2B-blue)](https://www.cplusplus.com/)
[![Framework](https://img.shields.io/badge/framework-Mbed-blue)](https://www.mbed.com/)

This codebase contains the software which controls an autonomous buggy. Within this codebase, you can find functionalities such as controlling motors, reading sensors, implementing PID control, and Bluetooth communication.

## Table of Contents
- [Line Following Buggy](#line-following-buggy)
  * [Table of Contents](#table-of-contents)
  * [Features](#features)
  * [Requirements](#requirements)
  * [Getting Started](#getting-started)
    + [Integration](#integration)
  * [Classes and Functions](#classes-and-functions)
  * [Basic setup](#basic-setup)
  * [Example code](#example-code)

## Features

- **Motor Control**: Control the speed and direction of the buggy's motors.
- **Sensor Reading**: Read data from infrared sensors for line following.
- **PID Control**: Implement PID control for precise motor speed adjustment.
- **Bluetooth Communication**: Interact with the buggy via Bluetooth commands.

## Requirements

- [mbed OS 2](https://os.mbed.com/)
- Compatible microcontroller (e.g., STM32, NXP)

## Getting Started

1. **Integration**: Clone this repository, or copy code from "main.cpp" into IDE of choice (E.g kiel studio).
2. **Library Dependencies**:
   - Download the [mbed 2 library](https://os.mbed.com/users/mbed_official/code/mbed/builds/176b8275d35d) and include it in your project.
   - Download the [QEI.h library](https://os.mbed.com/users/aberk/code/QEI/file/5c2ad81551aa/QEI.h/) and include it in your project.
   - Download the [C12832 LCD library](https://os.mbed.com/teams/components/code/C12832/docs/tip/C12832_8h_source.html) and include it in your project.

 Example library declarations:
 ```
#include "mbed.h"
#include "QEI.h"
#include "C12832.h"
```

## Classes, Functions and variables

### Motor Class

Controls the operation of a motor using PWM output and encoder feedback.

| Function Name   | Description                                               | How to Call                         |
|-----------------|-----------------------------------------------------------|-------------------------------------|
| Motor           | Constructor to initialize a motor with given parameters.  | `Motor motorName(pwmPin, encoderPin, identifier);` |
| setDutyCycle    | Sets the PWM duty cycle for the motor.                    | `motorName.setDutyCycle(newDutyCycle);` |
| stop            | Stops the motor by setting the duty cycle to neutral.     | `motorName.stop();` |
| getPwmValue     | Returns the current PWM duty cycle value.                 | `float duty = motorName.getPwmValue();` |
| getRPM          | Calculates and returns the motor RPM.                     | `float rpm = motorName.getRPM();` |
| getPulse        | Returns the pulse count from the encoder.                 | `int pulseCount = motorName.getPulse();` |
| getSpeed        | Calculates and returns the motor's speed in m/s.          | `float speed = motorName.getSpeed();` |


### PID Control Functions

Provides functionality for calculating PID values and adjusting motor control based on sensor input.

| Function Name        | Description                                                  | How to Call                                        |
|----------------------|--------------------------------------------------------------|----------------------------------------------------|
| calculatePID         | Calculates the PID value based on error.                     | `float pidValue = calculatePID(error);`            |
| calculatePositionalError | Calculates the error based on sensor readings.            | `float error = calculatePositionalError();`        |


### PID Control Variables

Configurable parameters for PID control tuning. Can also adjust `Ki` or `Kd` to `0` utilise different control algorithms (P, I, D, PI, PD, PID).

| Variable | Description                       | Example Value |
|----------|-----------------------------------|---------------|
| `Kp`     | Proportional gain coefficient.    | `0.2`         |
| `Ki`     | Integral gain coefficient.        | `0.01`        |
| `Kd`     | Derivative gain coefficient.      | `0.05`        |

**Example**: To enable PD control, set `Ki = 0` and adjust `Kp` and `Kd` as needed.



### Motor Control Functions

Contains functions to control the movements and adjustments of the buggy's motors.

| Function Name        | Description                                                  | How to Call                                        |
|----------------------|--------------------------------------------------------------|----------------------------------------------------|
| turnBuggy            | Commands to turn the buggy.                                  | `turnBuggy();`                                     |
| motorPIDcontrol      | Adjusts the motor speeds based on the PID output and error.  | `motorPIDcontrol(pidOutput, error);`               |


### Bluetooth Communication Functions

Handles Bluetooth communication, allowing remote interaction with the buggy by responding to specific commands and initiating actions based on the received Bluetooth signal.

| Function Name        | Description                                                                         | How to Call                                        |
|----------------------|-------------------------------------------------------------------------------------|----------------------------------------------------|
| bluetoothCallback    | Callback function to handle and respond to Bluetooth commands. Initiates actions such as turning the buggy based on the command received. Example implementation below. | `hm10.attach(&bluetoothCallback);`                 |

Example implementation of `bluetoothCallback` function:

```cpp
void bluetoothCallback()
{
    if (hm10.readable())
    {
        char command = hm10.getc(); // Read command from Bluetooth
        switch(command)
        {
            case 't': // Example command to turn
                mode = TURN;
                break;
            // Add more cases for other commands
        }
    }
}

hm10.attach(&bluetoothCallback);
```


## Basic Setup

Here's an example of the basic implementation of a line following buggy using the PID algorithm

```cpp
#include "mbed.h"
// Include other necessary headers here

// Instantiate global instances of your classes here
// Example: Motor leftMotor(pwmPin, encoderPin, 'L');

float Kp = 0.075; // Proportional gain
float Ki = 0.01;  // Integral gain
float Kd = 0.05;  // Derivative gain

int main()
{
    // Enable/Disable pins

    // Create Motor instances for left and right motors
    Motor leftMotor(pwm1, leftEncoder, 'L');
    Motor rightMotor(pwm2, rightEncoder, 'R');

    while (true)
    {
        calculatePositionalError();

        switch (mode)
        {
        case STOPPED:
            leftMotor.stop();
            rightMotor.stop();
            break;
        case FOLLOW_LINE:
            calculatePID();
            motorPIDcontrol(leftMotor, rightMotor);
            break;
        case TURN:
            turnBuggy(leftMotor, rightMotor);
            break;
        }
        wait(0.1);
    }
}
```

## Example Code

For a complete example of a `main.cpp` implementation, see the [main.cpp file](src/main.cpp) in this repository.

