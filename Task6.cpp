#include "mbed.h"
#include "QEI.h"
#include "C12832.h"

PwmOut pwm1(PB_14);
PwmOut pwm2(PB_13);
DigitalOut bipolar1(PA_13);
DigitalOut bipolar2(PA_14);
DigitalOut enablePin(PC_8);
DigitalOut direction1(PC_5);
DigitalOut direction2(PC_6);
C12832 lcd(D11, D13, D12, D7, D10);
Serial pc(D1, D0); // Initialize serial connection to PC

QEI leftEncoder(PB_1, PB_15, NC, 512, QEI::X2_ENCODING);
QEI rightEncoder(PB_12, PB_2, NC, 512, QEI::X2_ENCODING);

Timer t;

char command;

// Constants for movement
const float FORWARD_SPEED = 0.7f;
const float STOP_SPEED = 0.5f; // Assuming 0.5 is the neutral duty cycle for stopping
const float STOP_TIME = 0.5f;  // Time to pause after each action

class Motor
{
private:
    PwmOut &motor;          // Reference to the PWM output for this motor
    QEI &encoder;           // Reference to the encoder for this motor
    float dutyCycle;        // PWM duty cycle
    float period;           // PWM period
    int previousPulseCount; // Previous pulse count for speed calculation
    Timer timer;            // Timer to measure elapsed time for speed calculation
    float wheelVelocity;    // Wheel velocity in m/s
    char motorIdentifier;   // Identifier for the motor
public:
    Motor(PwmOut &motorPin, QEI &encoderPin, char identifier) : motor(motorPin), encoder(encoderPin), motorIdentifier(identifier)
    {
        dutyCycle = 0.50f;                        // Set a default duty cycle
        period = 0.0001f;                         // Set a default PWM period
        motor.period(period);                     // Configure the PWM period
        motor.write(dutyCycle);                   // Set the initial PWM duty cycle
        previousPulseCount = encoder.getPulses(); // Initialize previous pulse count
        timer.start();                            // Start the timer
    }

    // Member function to set the PWM value
    void setDutyCycle(float newDutyCycle)
    {
        if (newDutyCycle < 0.0f)
            newDutyCycle = 0.0f;
        if (newDutyCycle > 1.0f)
            newDutyCycle = 1.0f;
        dutyCycle = newDutyCycle;
        motor.write(dutyCycle);
    }

    void setPeriod(float newPeriod)
    {
        motor.period(newPeriod); // Set the PWM period
    }

    void stop()
    {
        setDutyCycle(0.0f); // Stop the motor
    }

    // Member function to get the PWM value
    float getPwmValue() const
    {
        return dutyCycle;
    }

    // New method to calculate and return the motor speed
    float getSpeed()
    {
        float wheelDiameter = 0.0834f; // in meters
        int gearRatio = 12.75;         // 2:1 gear ratio
        int cpr = 512;                 // Encoder counts per revolution

        // Reset and start timer for sampling period
        t.reset();
        t.start();

        int previousPulseCount = encoder.getPulses();

        // Sample time (dt) - Wait 100ms
        wait_ms(100);

        t.stop();

        float elapsed_time = t.read(); // No change here

        int currentPulseCount = encoder.getPulses();
        int pulseDiff = currentPulseCount - previousPulseCount;

        float encoderTickRate = static_cast<float>(pulseDiff) / elapsed_time; // Convert to seconds
        wheelVelocity = (encoderTickRate * 3.14f * wheelDiameter) / (gearRatio * cpr);

        printf("%c Motor Speed: %.2f m/s\n", motorIdentifier, wheelVelocity);

        return wheelVelocity;
    }
};

class Potentiometer
{                                                    // Begin potentiometer class definition
private:                                             // Private data member declaration
    AnalogIn inputSignal;                            // Declaration of AnalogIn object
public:                                              // Public declarations
    Potentiometer(PinName pin) : inputSignal(pin) {} // Constructor - user provided pin name assigned to AnalogIn

    float read(void)
    {
        return inputSignal.read();
    }
};

void moveForward(Motor &leftMotor, Motor &rightMotor, float time)
{
    leftMotor.setDutyCycle(FORWARD_SPEED);
    rightMotor.setDutyCycle(FORWARD_SPEED);
    wait(time);
    leftMotor.setDutyCycle(STOP_SPEED);
    rightMotor.setDutyCycle(STOP_SPEED);
    wait(STOP_TIME);
}

void turnRight(Motor &leftMotor, Motor &rightMotor, float time)
{
    leftMotor.setDutyCycle(FORWARD_SPEED);
    rightMotor.setDutyCycle(STOP_SPEED); // Assuming right turn means slowing down or stopping the right motor
    wait(time);
    leftMotor.setDutyCycle(STOP_SPEED);
    rightMotor.setDutyCycle(STOP_SPEED);
    wait(STOP_TIME);
}

void turnLeft(Motor &leftMotor, Motor &rightMotor, float time)
{
    leftMotor.setDutyCycle(STOP_SPEED); // Assuming left turn means slowing down or stopping the left motor
    rightMotor.setDutyCycle(FORWARD_SPEED);
    wait(time);
    leftMotor.setDutyCycle(STOP_SPEED);
    rightMotor.setDutyCycle(STOP_SPEED);
    wait(STOP_TIME);
}

int main()
{
    // Pin configuration for bipolar mode
    enablePin.write(1);
    bipolar1.write(1);
    bipolar2.write(1);
    direction1.write(1);
    direction2.write(1);

    // Create Motor instances for left and right motors
    Motor leftMotor(pwm1, leftEncoder, 'L');
    Motor rightMotor(pwm2, rightEncoder, 'R');

    while (true)
    {
        // Square
        moveForward(leftMotor, rightMotor, 1.7);
        turnRight(leftMotor, rightMotor, 0.7);
        moveForward(leftMotor, rightMotor, 1.7);
        turnRight(leftMotor, rightMotor, 0.7);
        moveForward(leftMotor, rightMotor, 1.7);
        turnRight(leftMotor, rightMotor, 0.7);
        moveForward(leftMotor, rightMotor, 1.7);

        // Turn buggy around
        turnRight(leftMotor, rightMotor, 1.2);

        // Make buggy trace the square
        moveForward(leftMotor, rightMotor, 1.7);
        turnLeft(leftMotor, rightMotor, 0.7);
        moveForward(leftMotor, rightMotor, 1.7);
        turnLeft(leftMotor, rightMotor, 0.7);
        moveForward(leftMotor, rightMotor, 1.7);
        turnLeft(leftMotor, rightMotor, 0.7);
        moveForward(leftMotor, rightMotor, 1.7);

        // Stop buggy
        leftMotor.stop();
        rightMotor.stop();
        wait(0);
    }
}