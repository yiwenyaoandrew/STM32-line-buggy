#include "mbed.h"
#include "C12832.h"

PwmOut pwm1(PB_14);
PwmOut pwm2(PB_13);
DigitalOut bipolar1(PC_14);
DigitalOut bipolar2(PC_15);
DigitalOut enablePin(PC_8);
DigitalOut direction1(PC_5);
DigitalOut direction2(PC_6);
C12832 lcd(D11, D13, D12, D7, D10);

class Motor
{
private:
    PwmOut &motor;   // Reference to the PWM output for this motor
    float dutyCycle; // PWM duty cycle
    float period;    // PWM period
public:
    Motor(PwmOut &motorPin, char identifier) : motor(motorPin)
    {
        dutyCycle = 0.50f;      // Set a default duty cycle
        period = 0.0001f;       // Set a default PWM period
        motor.period(period);   // Configure the PWM period
        motor.write(dutyCycle); // Set the initial PWM duty cycle
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
};

class Potentiometer // Begin Potentiometer class definition
{
private:                                              // Private data member declaration
    AnalogIn inputSignal;                             // Declaration of AnalogIn object
    float VDD, currentSampleNorm, currentSampleVolts; // Float variables to speficy the value of VDD and most recent samples

public:                                                               // Public declarations
    Potentiometer(PinName pin, float v) : inputSignal(pin), VDD(v) {} // Constructor - user provided pin name assigned to AnalogIn...
    float amplitudeNorm(void)
    {
        return inputSignal.read(); // Returns the ADC value normalised to range 0.0 - 1.0
    }

    void sample(void) // Public member function to sample an analogue voltage
    {
        currentSampleNorm = inputSignal.read(); // Stores the current ADC value to the class's data member for normalised values (0.0 - 1.0)
    }
};

class SamplingPotentiometer : public Potentiometer
{

private:
    float samplingFrequency;
    Ticker sampler;

public:
    SamplingPotentiometer(PinName p, float v, float fs) : Potentiometer(p, v), samplingFrequency(fs)
    {
        sampler.attach(callback(this, &SamplingPotentiometer::sample), 1.0 / samplingFrequency);
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

    // Potentiometer instances
    SamplingPotentiometer left_p(A0, 3.3, 400), right_p(A1, 3.3, 400); // choose appropriately fast sampling frequency

    // Create Motor instances for left and right motors
    Motor leftMotor(pwm1, 'L'), rightMotor(pwm2, 'R');

    while (true)
    {
        // Read normalized potentiometer values
        float leftPotValue = left_p.amplitudeNorm();
        float rightPotValue = right_p.amplitudeNorm();

        // Map potentiometer values to PWM duty cycles
        leftMotor.setDutyCycle(leftPotValue);
        rightMotor.setDutyCycle(rightPotValue);

        lcd.cls();
        // Display the duty cycles on the LCD
        lcd.locate(0, 0);
        lcd.printf("Left Motor: %.2f%%", leftPotValue * 100);

        lcd.locate(0, 15);
        lcd.printf("Right Motor: %.2f%%", rightPotValue * 100);

        wait(0.1);
    }
}