#include "mbed.h"
#include "QEI.h"
#include "C12832.h"

PwmOut pwm1(PB_14);
PwmOut pwm2(PB_13);
DigitalOut bipolar1(PC_14);
DigitalOut bipolar2(PC_15);
DigitalOut enablePin(PC_8);
DigitalOut direction1(PC_5);
DigitalOut direction2(PC_6);
C12832 lcd(D11, D13, D12, D7, D10);
Serial pc(D1, D0); // Initialize serial connection to PC

QEI leftEncoder(PB_1, PB_15, NC, 512, QEI::X2_ENCODING);
QEI rightEncoder(PB_12, PB_2, NC, 512, QEI::X2_ENCODING);

Timer t;

char command;

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
        float wheelDiameter = 0.15f; // in meters
        int gearRatio = 12.75;       // 2:1 gear ratio
        int cpr = 512;               // Encoder counts per revolution

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

int main()
{
    // Pin configuration for bipolar mode
    enablePin.write(1);
    bipolarLeft.write(1);
    bipolarRight.write(1);
    direction1.write(1);
    direction2.write(1);

    hm10.baud(9600); // Set the baud rate to 9600

    // Create Motor instances for left and right motors
    Motor leftMotor(pwm1, leftEncoder, 'L');
    Motor rightMotor(pwm2, rightEncoder, 'R');

    // Create Potentiometer instance
    Potentiometer potentiometerLeft(A0);
    Potentiometer potentiometerRight(A1);

    char command;
    while (true)
    {
        // Task 3: Change the duty cycle of the motors based on the potentiometer readings
        // Task 4: Drive both motors independently and set their speed based on the potentiometer readings
        float leftPotValue = potentiometerLeft.read();
        float rightPotValue = potentiometerRight.read();
        leftMotor.setDutyCycle(leftPotValue);
        rightMotor.setDutyCycle(rightPotValue);

        // Task 5: Read the speed from encoder and show RPM and pulse count on the LCD
        float leftSpeed = leftMotor.getSpeed();
        float rightSpeed = rightMotor.getSpeed();
        lcd.cls(); // Clear the screen
        lcd.locate(0, 3);
        lcd.printf("Left RPM: %.2f", leftSpeed * 60);
        lcd.locate(0, 15);
        lcd.printf("Right RPM: %.2f", rightSpeed * 60);
        lcd.locate(0, 27);
        lcd.printf("Left Pulse: %d", leftEncoder.getPulses());
        lcd.locate(0, 39);
        lcd.printf("Right Pulse: %d", rightEncoder.getPulses());

        // Task 6: make robot drive in a 1m by 1m square, once the robot has completed the square,turn the robot around and trace the square again

        // Task 8: Show a visual response in response to the command received from the Bluetooth module
        if (hm10.readable())
        {
            command = hm10.getc(); // Read command from Bluetooth
            // Display command on LCD
            lcd.cls(); // Clear the screen
            lcd.locate(0, 3);
            lcd.printf("Command: %c", command);
        }
    }
}