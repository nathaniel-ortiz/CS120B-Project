#include <avr/io.h>
#include "io.h"
#include "timer.h"
#include "serialWrite.c"
#include "io.c"

int main(void) {
	DDRA = 0xFF; PORTA = 0x00;
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	
	unsigned short int pattern = 0x0000; //16 bits requires two shift registers by daisy chaining them together
	
	while(1) {
		
	}
}