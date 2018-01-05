#include "main.h"
#include "hx711.h"
#include "button.h"
#include "LCD_functions.h"

void showLongDec(long input)
{
char c,i;
long k = 100000;

for(i=0;i<6;i++)
{
	
	c=0;
	while (input >= k)                // Count hundreds
	{
		c++;
		input -= k;
	}
	
	LCD_putc(i, c+ '0');
	
	k /= 10;
}

LCD_putc(6, '\0');
LCD_UpdateRequired(TRUE, 0);
}

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


void doCalibration(void)
{
float val = HX711_get_value(10);
HX711_set_scale(val / 2);
showLongHex(val);
/*
//units = (double)HX711_read_average(10);

if(units > 199)
units = 199;

if(units < 0)
units = 198;

if(units < 0.1)
units = 197;

char h, t, u, d;

h = (char)(((unsigned int)units) / 100);
t = (char)((((unsigned int)units) - ((unsigned int)h)*100)/10) ;
u = (char)((unsigned int)units - ((unsigned int)h)*100 - ((unsigned int)t)*10) ;
d = (char)((units - (unsigned int)h*100 - (unsigned int)t*10 - (unsigned int)u)*10) ;

	LCD_putc(0, h+ '0');
    LCD_putc(1, t+ '0');
    LCD_putc(2, u+ '0');
    LCD_putc(3, d+ '0');
    LCD_putc(4, 'x');
    LCD_putc(5, 'x');
    LCD_putc(6, '\0');
LCD_UpdateRequired(TRUE, 0);
*/
}

void doScaleMeasure(void)
{

float units = HX711_get_units(10);


long val = (long)(units*10);
showLongHex(val);

/*
if(units > 199)
units = 199;

if(units < 0)
units = 198;

if(units < 0.1)
units = 197;

char h, t, u, d;

h = (char)(((unsigned int)units) / 100);
t = (char)((((unsigned int)units) - ((unsigned int)h)*100)/10) ;
u = (char)((unsigned int)units - ((unsigned int)h)*100 - ((unsigned int)t)*10) ;
d = (char)((units - (unsigned int)h*100 - (unsigned int)t*10 - (unsigned int)u)*10) ;

	LCD_putc(0, h+ '0');
    LCD_putc(1, t+ '0');
    LCD_putc(2, u+ '0');
    LCD_putc(3, d+ '0');
    LCD_putc(4, 'k');
    LCD_putc(5, 'g');
    LCD_putc(6, '\0');
LCD_UpdateRequired(TRUE, 0);
*/
}

char StateScaleMeasureFunc(char input)
{
    static char enter = 1;
	static char nr=0;
    
    if (enter)
    {
        enter = 0;
		
		LCD_puts("TARE", 1);
    
        HX711_begin(128); 
		HX711_power_up();
		HX711_tare(10);
		
		doScaleMeasure();
    }
	else
	{
    
		if (input == KEY_PREV)
		{
			doScaleMeasure();
			
			return ST_SCALE_MEASURE;
		}
		if (input == KEY_NEXT)
		{
			return ST_SCALE_CALIB;
		}
		else if (input == KEY_PLUS)
		{   
			//HX711_power_down();
			//enter = 1;
			return ST_OPTIONS;
		}
		else if (input == KEY_MINUS)
		{
			//HX711_power_down();
			//enter = 1;
			return ST_TIME;
		}
	}
    
    return ST_SCALE_MEASURE;
}

char StateScaleCalibFunc(char input)
{
static char enter = 1;
    
    if (enter)
    {
        enter = 0;
		
		doCalibration();
    }
	else
	{
    
		if (input == KEY_PREV)
		{
			return ST_SCALE_MEASURE;
		}
		if (input == KEY_NEXT)
		{
			doCalibration();
		}
		else if (input == KEY_PLUS)
		{   
			
		}
		else if (input == KEY_MINUS)
		{
			
		}
	}

	return ST_SCALE_CALIB;
}