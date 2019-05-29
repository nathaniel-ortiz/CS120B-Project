#include <avr/io.h>

#define DATA_PORT PORTC //port for output(if using portc make sure to disable jtag on atmega1284)
#define DATA_PIN 0 //serial data line
#define SRCLK_PIN 1 //srclk
#define RCLK_PIN 2 //rclk

#define DATA_low()  DATA_PORT&=~_BV(DATA_PIN)
#define DATA_high() DATA_PORT|=_BV(DATA_PIN)
#define SRCLK_low()  DATA_PORT&=~_BV(SRCLK_PIN)
#define SRCLK_high() DATA_PORT|=_BV(SRCLK_PIN)
#define RCLK_low()  DATA_PORT&=~_BV(RCLK_PIN)
#define RCLK_high() DATA_PORT|=_BV(RCLK_PIN)

void serialWrite(unsigned short int data)
{
	RCLK_low();
	SRCLK_low();
	for (int i = 0; i < 16; i++) { //shift bits in, change the number of iterations of the for loop if not using two shift registers. by changing the number of iterations you can daisy chain as many registers as needed
		if ((data & 0x01) == 0x0001) { //send data
			DATA_high();
		}
		else {
			DATA_low();
		}
		
		RCLK_high(); //pulse the clock
		RCLK_low();
		data = data >> 1; //bit shift right so leading bit is always the current bit to be written
	}
	SRCLK_high(); //stop writing to the shift register
}