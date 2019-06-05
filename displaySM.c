#include <avr/io.h>

unsigned int cursorPos = 1;
enum state {init, menu1, menu1_wait, menu2, menu2_wait, eeprom, pattern} state;
unsigned char menu_state;

#define UP (PINB == 0xFD)
#define DOWN (PINB == 0xFE)
#define LEFT (PINB == 0xF7)
#define RIGHT (PINB == 0xFB)
#define SELECT (PINB == 0xEF)

unsigned int xyz[] = {0, 0, 0};

void displaySM(unsigned char state) {
	switch(state) {
		case init:
			LCD_init();
			state = menu1;
			break;
		case menu1:
			LCD_DisplayString(1, "XYZ * >");
			for(int i = 0; i < 3; i++) {
				LCD_Cursor(17 + i);
				LCD_WriteData(xyz[i] + '0');
			}
			LCD_Cursor(cursorPos);
			state = menu1_wait;
			break;
		case menu1_wait:
			if(UP && (cursorPos == 1 || cursorPos == 2 || cursorPos == 3)) {
				delay_ms(200);
				if(xyz[cursorPos - 1] != 2) {
					xyz[cursorPos - 1]++;
				}
				state = menu1;
			}
			else if(DOWN && (cursorPos == 1 || cursorPos == 2 || cursorPos == 3)) {
				delay_ms(200);
				if(xyz[cursorPos - 1] != 0) {
					xyz[cursorPos - 1]--;
				}
				state = menu1;
			}
			else if(LEFT && cursorPos != 1) {
				delay_ms(200);
				if(cursorPos == 5 || cursorPos == 7) {
					cursorPos -= 2;
				}
				else {
					cursorPos--;
				}
				state = menu1;
			}
			else if(RIGHT && cursorPos != 7) {
				delay_ms(200);
				if(cursorPos == 3 || cursorPos == 5) {
					cursorPos += 2;
				}
				else {
					cursorPos++;
				}
				state = menu1;
			}
			else if(SELECT) {
				if(cursorPos == 5) {
					//flip selected LED on/off
				}
				if(cursorPos == 7) {
					cursorPos = 1;
					state = menu2;
				}
			}
			else {
				state = menu1_wait;
			}
			break;
		case menu2:
			LCD_ClearScreen();
			LCD_DisplayString(1, "<*Save *Patterns");
			LCD_Cursor(cursorPos);
			state = menu2_wait;
			break;
		case menu2_wait:
			if(SELECT) {
				delay_ms(200);
				if(cursorPos == 1) {
					state = menu1;
				}
				else if(cursorPos == 2) {
					state = eeprom;
				}
				else if(cursorPos == 7) {
					state = pattern;
				}
			}
			else if(LEFT) {
				delay_ms(200);
				if(cursorPos == 2) {
					cursorPos = 1;
				}
				else if(cursorPos == 7) {
					cursorPos = 2;
				}
				state = menu2;
			}
			else if(RIGHT) {
				delay_ms(200);
				if(cursorPos == 1) {
					cursorPos = 2;
				}
				if(cursorPos == 2) {
					cursorPos = 7;
				}
				state = menu2;
			}
			else {
				state = menu2_wait;
			}
			break;
		case eeprom:
			state = menu1;
			break;
		case pattern:
			state = menu1;
	}
	menu_state = state;
}