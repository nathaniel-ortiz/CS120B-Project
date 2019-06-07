#include <avr/io.h>
#include <avr/eeprom.h>
#include "patterns.h"

enum state {init, menu1, menu1_wait, menu2, menu2_wait, eeprom, eeprom_written, pattern, pattern_wait} state; //sm states

//bitmaps for pins
#define UP (PINB == 0xFD)
#define DOWN (PINB == 0xFE)
#define LEFT (PINB == 0xFB)
#define RIGHT (PINB == 0xF7)
#define SELECT (PINB == 0xEF)

unsigned char menu_state; //global state variable
unsigned int cursorPos = 1; //current position of cursor on the LCD
unsigned int xyz[] = {0, 0, 0}; //x is horizontal axis, y is vertical axis, z is level
unsigned short int data = 0x0000; //global variable for data to write to matrix/eeprom
const unsigned int pinout[3][3] = { //corresponds pins to a 2d coordinate
	{0, 1, 2},
	{3, 4, 5},
	{6, 7, 8}
};

//updateCube converts a 1d array of coordinates (i.e. (0,0,0) to the corresponding pin on the cube and sets it high
void updateCube(unsigned int coord[]) {
	data = 0x0000;
	
	switch(coord[2]) {
		case 0:
			data = 0x0030; //pull down the 1st layer to ground
			data = data | (0x8000 >> (pinout[xyz[0]][xyz[1]])); //convert 2d coordinate to a 1d coordinate of bits, then bit shift to the corresponding pin
			serialWrite(data);
			break;
		case 1:
			data = 0x0050; //2nd layer
			data = data | (0x8000 >> (pinout[xyz[0]][xyz[1]]));
			serialWrite(data);
			break;
		case 2:
			data = 0x0060; //3rd layer
			data = data | (0x8000 >> (pinout[xyz[0]][xyz[1]]));
			serialWrite(data);
			break;
	}
}

void displaySM(unsigned char state) {
	switch(state) {
		case init:
			LCD_init();
			serialWrite(eeprom_read_word(0x0000)); //always load value stored in EEPROM and write to the cube, eeprom is always written to the first address
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
				while(!UP); //debounce
				delay_ms(100);
				if(xyz[cursorPos - 1] != 2) {
					xyz[cursorPos - 1]++; //increment coordinate that cursor is over
				}
				state = menu1;
			}
			else if(DOWN && (cursorPos == 1 || cursorPos == 2 || cursorPos == 3)) {
				while(!DOWN);
				delay_ms(100);
				if(xyz[cursorPos - 1] != 0) {
					xyz[cursorPos - 1]--; //decrement coordinate that cursor is over
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
					updateCube(xyz); //update cube with current coords
				}
				else if(cursorPos == 9) {
					data = 0x0000; //clear data since nothing is on the display
					serialWrite(0x0000); //turn off all leds since only one LED is on at a time
				}
				else if(cursorPos == 14) {
					cursorPos = 1; //go to next menu page
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
					state = menu1; //back to first menu
				}
				else if(cursorPos == 3) {
					state = eeprom; //write current coords to eeprom
				}
				else if(cursorPos == 9) {
					cursorPos = 1;
					state = pattern; //go to pattern menu
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
			eeprom_write_word(0x0000, data); //write the current led on to the eeprom
			state = eeprom_written;
			break;
		case eeprom_written:
			LCD_ClearScreen();
			LCD_DisplayString(1, "Saved to EEPROM!"); //inform user eeprom was written successfully
			delay_ms(1000);
			state = menu2;
		case pattern:
			LCD_ClearScreen();
			LCD_DisplayString(1, "< *Wave *Snake"); //available patterns
			LCD_Cursor(cursorPos);
			state = pattern_wait;
			break;
		case pattern_wait:
			if(SELECT) {
				while(!SELECT);
				delay_ms(100);
				if(cursorPos == 1) {
					state = menu2; //back to 2nd menu
				}
				else if(cursorPos == 3) {
					wavePattern(); //display pattern
					serialWrite(data); //set cube back to previous led
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