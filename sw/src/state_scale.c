#include "main.h"
#include "hx711.h"
#include "button.h"
#include "LCD_functions.h"
#include "usart.h"
#include "dataflash.h"

void doCalibration(void)
{
	float val = HX711_get_value(10);
	HX711_set_scale(val / 2);
	showLongHex(val);
}

void doScaleMeasure(void)
{
	float units = HX711_get_units(10);
	long val = (long)(units*10);
	showLongHex(val);
}

char StateScaleAutoMeasureFunc(char input)
{
}

char StateScaleFormat(char input)
{
	unsigned int i;
	LCD_puts("xx START", 1);
	
	char hf[]="0123456789ABCDEF"; //todo progmem
	unsigned char status = Read_DF_status();
	
	LCD_putc(0, hf[status >> 4]);
	LCD_putc(1, hf[status & 0xF]);
	
	LCD_UpdateRequired(1, 1);
	
	//for(i=0;i<

	Delay(1500);
	return ST_SCALE_FORMAT;
}

char StateScaleDownload(char input)
{
	static char enter = 1;

    if (enter)
    {
        enter = 0;
		Usart_puts(" 0123456789Aaz");
    }
	else
	{
		Usart_puts("TARE");
	}
	return ST_SCALE_DOWN;
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
	}
	return ST_SCALE_CALIB;
}