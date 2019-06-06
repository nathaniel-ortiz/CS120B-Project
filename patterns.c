#include <avr/io.h>
#include "patterns.h"

void snakePattern(void) {
	unsigned short int snake[] = {0x8030, 0x4030, 0x2030, 0x1030, 0x0830, 0x0430, 0x0230, 0x0130, 0x00B0, 0x8050, 0x4050, 0x2050, 0x1050, 0x0850, 0x0450, 0x0250, 0x0150, 0x00D0, 0x8060, 0x4060, 0x2060, 0x1060, 0x0860, 0x0460, 0x0260, 0x0160, 0x00E0};
	for(int i = 0; i < 27; i++) {
		serialWrite(snake[i]);
		delay_ms(50);
	}
	for(int i = 25; i >= 0; i--) {
		serialWrite(snake[i]);
		delay_ms(50);
	}
}

void wavePattern(void) {
	unsigned short int wave[] = {0xE000, 0x1C00, 0x0380};
	for(int j = 0; j < 3; j++) {
		for(int i = 0; i < 3; i++) {
			serialWrite(wave[i]);
			delay_ms(50);
		}
	}
}