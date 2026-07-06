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

class Motor
{
private:
    PwmOut &motor;          // Reference to the PWM output for this motor
    QEI &encoder;           // Reference to the encoder for this motor
    float dutyCycle;        // PWM duty cycle
    float period;           // PWM period
    int previousPulseCount; // Previous pulse count for speed calculation
    Timer timer;            // Timer to measure elapsed time for speed calculation
    char motorIdentifier;   // Identifier for the motor
    float wheelVelocity;    // Wheel velocity in m/s

public:
    Motor(PwmOut &motorPin, QEI &encoderPin, char identifier) : motor(motorPin), encoder(encoderPin), motorIdentifier(identifier)
    {
        dutyCycle = 0.5f;                         // Initialize duty cycle to 50%
        period = 0.0001f;                         // Initialize period to 1ms
        motor.period(period);                     // Configure the PWM period
        motor.write(dutyCycle);                   // Set the initial PWM duty cycle
        previousPulseCount = encoder.getPulses(); // Initialize previous pulse count
        timer.start();                            // Start the timer
    }

    void setDutyCycle(float newDutyCycle)
    {
        if (newDutyCycle < 0.0f)
            newDutyCycle = 0.0f;
        if (newDutyCycle > 1.0f)
            newDutyCycle = 1.0f;
        dutyCycle = newDutyCycle;
        motor.write(dutyCycle);
    }

    // Method to get RPM
    float getRPM()
    {
        float elapsedTime = timer.read();
        timer.reset();
        int currentPulseCount = encoder.getPulses();
        int pulseDifference = currentPulseCount - previousPulseCount;
        previousPulseCount = currentPulseCount;
        float revolutions = static_cast<float>(pulseDifference) / static_cast<float>(512);
        float rps = revolutions / elapsedTime;
        float rpm = rps * 60.0f;

        pc.printf("%c Left RPM: %.2f\n", rpm);
        return rpm;
    }

    int getPulse()
    {
        printf("Left pulses: %d\n", encoder.getPulses());
        return encoder.getPulses();
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

        pc.printf("%c Left speed: %.2f m/s\n", motorIdentifier, wheelVelocity);

        return wheelVelocity;
    }
};

int main()
{
    // Pin configuration for bipolar mode
    enablePin.write(1);
    bipolar1.write(1);
    bipolar2.write(1);
    direction1.write(1);
    direction2.write(1);

    // Create Motor instances for left and right motors
    Motor leftMotor(pwm1, leftEncoder, 'L'), rightMotor(pwm2, rightEncoder, 'R');

    leftMotor.setDutyCycle(0.5f);
    rightMotor.setDutyCycle(0.5f);

    while (true)
    {
        // Call getRPM once and store the result for display
        float leftRPM = leftMotor.getRPM();
        float leftPulse = leftMotor.getPulse();
        // float leftSpeed = leftMotor.getSpeed();

        // Display RPM and pulses on LCD
        lcd.cls();
        lcd.locate(3, 3);
        lcd.printf("Left RPM: %.2f", leftRPM);
        lcd.locate(3, 15);
        lcd.printf("Left Pulses: %.2f", leftPulse);

        wait(0.1);
    }
}
