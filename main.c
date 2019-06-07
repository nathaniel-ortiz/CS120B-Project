#include <avr/io.h>
#include <avr/eeprom.h>
#include "io.h"
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
	
	data = eeprom_read_word(0x0000); //ensure global data variable is updated with the current data from the eeprom
			
	while(1) {
		displaySM(menu_state);
	}
}