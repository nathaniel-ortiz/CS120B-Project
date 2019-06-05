#include <avr/io.h>
#include "io.h"
#include "timer.h"
#include "serialWrite.c"
#include "io.c"
#include "displaySM.c"
#include "patterns.c"

#define SET_BIT(p,i) ((p) |= (1 << (i)))
#define CLR_BIT(p,i) ((p) &= ~(1 << (i)))
#define GET_BIT(p,i) ((p) & (1 << (i)))

unsigned char menu_state = 0;

int main(void) {
	DDRA = 0xFF; PORTA = 0x00;
	DDRB = 0x00; PORTB = 0xFF;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	
	LCD_init();
	
	//unsigned short int pattern = 0x0000; //16 bits requires two shift registers by daisy chaining them together
		
	while(1) {
		serialWrite(0xFF80);
		displaySM(menu_state);
		wavePattern();
	}
}
