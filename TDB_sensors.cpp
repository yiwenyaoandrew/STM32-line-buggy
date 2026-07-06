#include "mbed.h"
#include "QEI.h"
#include "C12832.h"
#include <algorithm>

C12832 lcd(D11, D13, D12, D7, D10);

DigitalOut LineFollowSensorSwitch1(PC_12);
DigitalOut LineFollowSensorSwitch2(PA_15);
DigitalOut LineFollowSensorSwitch3(PC_11);
DigitalOut LineFollowSensorSwitch4(PD_2);
DigitalOut LineFollowSensorSwitch5(PC_9);

// Line sensor1 being the leftmost sensor
DigitalIn LineFollowSensor1(PC_3);
DigitalIn LineFollowSensor2(PC_2);
DigitalIn LineFollowSensor3(PC_4);
DigitalIn LineFollowSensor4(PB_1);
DigitalIn LineFollowSensor5(PC_5);
int LFSensor[5] = {0, 0, 0, 0, 0};

int main()
{
    LineFollowSensorSwitch1.write(1);
    LineFollowSensorSwitch2.write(1);
    LineFollowSensorSwitch3.write(1);
    LineFollowSensorSwitch4.write(1);
    LineFollowSensorSwitch5.write(1);

    while (true)
    {
        // print an output for each sensor
        LFSensor[0] = !LineFollowSensor1.read();
        LFSensor[1] = !LineFollowSensor2.read();
        LFSensor[2] = !LineFollowSensor3.read();
        LFSensor[3] = !LineFollowSensor4.read();
        LFSensor[4] = !LineFollowSensor5.read();

        // print two in  one line
        lcd.cls();
        lcd.locate(0, 0);
        lcd.printf("Sensor1: %d", LFSensor[0]);
        lcd.locate(50, 0);
        lcd.printf("Sensor2: %d", LFSensor[1]);
        lcd.locate(0, 10);
        lcd.printf("Sensor3: %d", LFSensor[2]);
        lcd.locate(50, 10);
        lcd.printf("Sensor4: %d", LFSensor[3]);
        lcd.locate(0, 20);
        lcd.printf("Sensor5: %d", LFSensor[4]);

        wait(0.1);
    }
}
