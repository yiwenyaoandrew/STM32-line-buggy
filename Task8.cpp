#include "mbed.h"
#include "QEI.h"
#include "C12832.h"

C12832 lcd(D11, D13, D12, D7, D10);
Serial pc(D1, D0);         // Initialize serial connection to PC
Serial hm10(PA_11, PA_12); // UART6 TX,RX
// LED1 is the blue LED on the board and the pin is D8
DigitalOut myLed(D8); // Initialize the digital pin LED1 as an output

Timer t;

char command;

int main()
{
    hm10.baud(9600); // Set the baud rate to 9600
    myLed = 1;       // Turn off LED1 initially (assuming 1 turns off the LED)

    char command; // Variable to store the command read from Bluetooth
    while (true)
    {
        if (hm10.readable())
        {
            command = hm10.getc(); // Read command from Bluetooth
            pc.printf("Command received: %c\n", command);
            // Turn on LED1 when the command is '1'
            if (command == '1')
            {
                myLed = 0; // Turn on LED
            }
            else if (command == '0')
            {
                myLed = 1; // Turn off LED
            }
        }
    }
}