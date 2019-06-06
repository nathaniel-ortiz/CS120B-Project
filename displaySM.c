#include <avr/io.h>
#include <avr/eeprom.h>
#include "patterns.h"

unsigned int cursorPos = 1;
enum state {init, menu1, menu1_wait, menu2, menu2_wait, eeprom, eeprom_written, pattern, pattern_wait} state;
unsigned char menu_state;

#define UP (PINB == 0xFD)
#define DOWN (PINB == 0xFE)
#define LEFT (PINB == 0xFB)
#define RIGHT (PINB == 0xF7)
#define SELECT (PINB == 0xEF)

unsigned int xyz[] = {0, 0, 0}; //x is horizontal axis, y is vertical axis, z is level
unsigned short int data = 0x0000;
const unsigned int pinout[3][3] = {
	{3, 6, 9},
	{2, 5, 8},
	{1, 4, 7}
};

void updateCube(unsigned int coord[]) {
	data = 0x0000;
	
	switch(coord[2]) {
		case 0:
			data = 0x0030;
			data = data | (0x8000 >> (pinout[xyz[1]][xyz[0]] - 1));
			serialWrite(data);
			break;
		case 1:
			data = 0x0050;
			data = data | (0x8000 >> (pinout[xyz[1]][xyz[0]] - 1));
			serialWrite(data);
			break;
		case 2:
			data = 0x0060;
			data = data | (0x8000 >> (pinout[xyz[1]][xyz[0]] - 1));
			serialWrite(data);
			break;
	}
}

void displaySM(unsigned char state) {
	switch(state) {
		case init:
			LCD_init();
			serialWrite(eeprom_read_word(0x0000)); //always load value stored in EEPROM and write to the cube
			state = menu1;
			break;
		case menu1:
			LCD_DisplayString(1, "XYZ *On *Off >");
			for(int i = 0; i < 3; i++) {
				LCD_Cursor(17 + i);
				LCD_WriteData(xyz[i] + '0');
			}
			LCD_Cursor(cursorPos);
			state = menu1_wait;
			break;
		case menu1_wait:
			if(UP && (cursorPos == 1 || cursorPos == 2 || cursorPos == 3)) {
				while(!UP);
				delay_ms(100);
				if(xyz[cursorPos - 1] != 2) {
					xyz[cursorPos - 1]++;
				}
				state = menu1;
			}
			else if(DOWN && (cursorPos == 1 || cursorPos == 2 || cursorPos == 3)) {
				while(!DOWN);
				delay_ms(100);
				if(xyz[cursorPos - 1] != 0) {
					xyz[cursorPos - 1]--;
				}
				state = menu1;
			}
			else if(LEFT) {
				while(!LEFT);
				delay_ms(100);
				if(cursorPos == 2 || cursorPos == 3) {
					cursorPos--;
				}
				else if(cursorPos == 5) {
					cursorPos = 3;
				}
				else if(cursorPos == 9) {
					cursorPos = 5;
				}
				else if(cursorPos == 14) {
					cursorPos = 9;
				}
				state = menu1;
			}
			else if(RIGHT) {
				while(!RIGHT);
				delay_ms(100);
				if(cursorPos == 1 || cursorPos == 2) {
					cursorPos++;
				}
				else if(cursorPos == 3) {
					cursorPos = 5;
				}
				else if(cursorPos == 5) {
					cursorPos = 9;
				}
				else if(cursorPos == 9) {
					cursorPos = 14;
				}
				state = menu1;
			}
			else if(SELECT) {
				while(!SELECT);
				delay_ms(100);
				if(cursorPos == 5) {
					updateCube(xyz);
				}
				else if(cursorPos == 9) {
					serialWrite(0x0000);
				}
				else if(cursorPos == 14) {
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
			LCD_DisplayString(1, "< *Save *Pattern");
			LCD_Cursor(cursorPos);
			state = menu2_wait;
			break;
		case menu2_wait:
			if(SELECT) {
				while(!SELECT);
				delay_ms(100);
				if(cursorPos == 1) {
					state = menu1;
				}
				else if(cursorPos == 3) {
					state = eeprom;
				}
				else if(cursorPos == 9) {
					cursorPos = 1;
					state = pattern;
				}
			}
			else if(LEFT) {
				while(!LEFT);
				delay_ms(100);
				if(cursorPos == 3) {
					cursorPos = 1;
				}
				else if(cursorPos == 9) {
					cursorPos = 3;
				}
				state = menu2;
			}
			else if(RIGHT) {
				while(!RIGHT);
				delay_ms(100);
				if(cursorPos == 1) {
					cursorPos = 3;
				}
				else if(cursorPos == 3) {
					cursorPos = 9;
				}
				state = menu2;
			}
			else {
				state = menu2_wait;
			}
			break;
		case eeprom:
			eeprom_write_word(0x0000, data);
			state = eeprom_written;
			break;
		case eeprom_written:
			LCD_ClearScreen();
			LCD_DisplayString(1, "Saved to EEPROM!");
			delay_ms(1000);
			state = menu2;
		case pattern:
			LCD_ClearScreen();
			LCD_DisplayString(1, "< *Wave *Snake");
			LCD_Cursor(cursorPos);
			state = pattern_wait;
			break;
		case pattern_wait:
			if(SELECT) {
				while(!SELECT);
				delay_ms(100);
				if(cursorPos == 1) {
					state = menu2;
				}
				else if(cursorPos == 3) {
					wavePattern();
					serialWrite(data);
					state = pattern;
				}
				else if(cursorPos == 9) {
					snakePattern();
					serialWrite(data);
					state = pattern;
				}
			}
			else if(LEFT) {
				while(!LEFT);
				delay_ms(100);
				if(cursorPos == 3) {
					cursorPos = 1;
					LCD_Cursor(cursorPos);
				}
				else if(cursorPos == 9) {
					cursorPos = 3;
					LCD_Cursor(cursorPos);
				}
				state = pattern;
			}
			else if(RIGHT) {
				while(!RIGHT);
				delay_ms(100);
				if(cursorPos == 1) {
					cursorPos = 3;
					LCD_Cursor(cursorPos);
				}
				else if(cursorPos == 3) {
					cursorPos = 9;
					LCD_Cursor(cursorPos);
				}
				state = pattern;
			}
			else {
				state = pattern_wait;
			}
			break;
	}
	menu_state = state;
} 