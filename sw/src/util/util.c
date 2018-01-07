#include "LCD_functions.h"

void showLongHex(long input)
{
	char hf[]="0123456789ABCDEF"; //todo progmem
	char screen[9];
	char i;

	for(i=0;i<8;i++)
	{
		screen[7-i] = hf[(input & 0xF)];
		
		input = input >> 4;
	}

	screen[8]=0;

	LCD_puts(screen, 1);
}