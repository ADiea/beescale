#include "main.h"
#include "hx711.h"
#include "button.h"
#include "LCD_functions.h"
#include "usart.h"
#include "dataflash.h"
#include "state_scale.h"
#include "RTC.h"
#include "timer0.h"
#include "ADC.h"
#include "BCD.h"

#define SCALE_AUTO_MEASURE_INTERVAL (unsigned long)(18000) 

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
	//1. Find empty page
	unsigned int i, nPages, nPageSz, writePtr=0, pagePtr, display;
	char ch;
	unsigned long lastMeasurementTime;
	
	tScaleSample sample;
	tScaleHdr header;
	
	LCD_puts("FIND", 1);
	
	nPages = dfGetPageCount();
	nPageSz = dfGetPageSize();

	for(i=0; i<nPages; ++i)
	{
		dfContFlashReadEnable(i, 0);
		ch = DF_SPI_RW(0x00);
		
		if(ch == PAGE_MAGIC_IS_EMPTY)
		{
			break;
		}
	}
		
	if(i < nPages)
	{
		//2. Create header
		pagePtr = i;
		
		header.version = SCALELOG_VERSION;
		
		header.tSecond = gSECOND;
		header.tMinute = gMINUTE;
		header.tHour = gHOUR;
		header.tDay = gDAY;
		header.tMonth = gMONTH;
		header.tYear = gYEAR;
		
		LCD_puts("PG    ", 1);
		display = CHAR2BCD4(i);
		LCD_putc(2, (display>>12 & 0xF) + '0');
		LCD_putc(3, (display>>8 & 0xF) + '0');
		LCD_putc(4, (display>>4 & 0xF) + '0');
		LCD_putc(5, (display & 0xF) + '0');
		LCD_UpdateRequired(1, 0);
		
		Delay(2000);
		
		LCD_puts("TARE", 1);
		HX711_begin(128); 
		HX711_power_up();
		header.tareValue = HX711_read_average_filtered();
		
		dfBufferWriteByte(1, 0, PAGE_MAGIC_HAS_HEADER);
		writePtr = 1;
		writePtr += dfBufferWriteStream(1, writePtr, sizeof(tScaleHdr), (unsigned char*)&header);

		//3. start measurements
		LCD_puts("M xxxx xxxx", 1);
		i=0;
		
		ADC_init(TEMPERATURE_SENSOR);
		ch = KEY_NULL;
		
		do
		{
			display = CHAR2BCD4(++i);
			LCD_putc(2, (display>>12 & 0xF) + '0');
			LCD_putc(3, (display>>8 & 0xF) + '0');
			LCD_putc(4, (display>>4 & 0xF) + '0');
			LCD_putc(5, (display & 0xF) + '0');
			LCD_UpdateRequired(1, 0);

			sample.scale = HX711_read_average_filtered();
			sample.scaleAve = HX711_read_average_window() + 1;
			sample.temp = ADC_read();

			if(writePtr + sizeof(tScaleSample) >= nPageSz)
			{
				//Write current page from buffer to flash, and move to next page
				dfBufferToPage(1, pagePtr++);
				dfBufferWriteByte(1, 0, PAGE_MAGIC_HAS_DATA);
				writePtr = 1;
			}

			writePtr += dfBufferWriteStream(1, writePtr, sizeof(tScaleSample), (unsigned char*)&sample);
			
			lastMeasurementTime = gSystemTick;
			while(gSystemTick - lastMeasurementTime < SCALE_AUTO_MEASURE_INTERVAL)
			{
			
				/*display = CHAR2BCD4(SCALE_AUTO_MEASURE_INTERVAL - (gSystemTick - lastMeasurementTime));
				LCD_putc(2, (display>>12 & 0xF) + '0');
				LCD_putc(3, (display>>8 & 0xF) + '0');
				LCD_putc(4, (display>>4 & 0xF) + '0');
				LCD_putc(5, (display & 0xF) + '0');
				LCD_UpdateRequired(1, 1);*/
			
				ch = getkey();
				if(ch == KEY_PREV)
				{
					//Fill rest of buffer with FF
					while(writePtr + sizeof(tScaleSample) < nPageSz)
					{
						dfBufferWriteByte(1, writePtr++, 0xFF);
					}
					
					//Write current page from buffer to flash
					dfBufferToPage(1, pagePtr);
					
					LCD_puts("DONE", 1);
					break;
				}
				else
				{
					//turn on the LCD
				}
			}
		}while(ch != KEY_PREV);
	}
	else // freePage >= nSize
	{
		LCD_puts("NOPAGE", 1);
	}
	
	Delay(2000);
	return ST_SCALE_START;
}

char StateScaleFormat(char input)
{
	unsigned int i, nSize, display;
	LCD_puts("F     ", 1);
	
	(void) Read_DF_status();
	nSize = dfGetBlockCount();
	
	for(i=0; i<nSize; ++i)
	{
		display = CHAR2BCD3(i);
		LCD_putc(2, (display>>8 & 0xF) + '0');
		LCD_putc(3, (display>>4 & 0xF) + '0');
		LCD_putc(4, (display & 0xF) + '0');
		LCD_UpdateRequired(1, 0);
		
		dfBlockErase(i);
	}

	nSize = dfGetPageSize();
	dfBufferWriteEnable(1, 0);
	for(i=0; i<nSize; ++i)
	{
		DF_SPI_RW(0xFF);
	}
	
	nSize = dfGetPageCount();
	for(i=0; i<nSize; ++i)
	{
		display = CHAR2BCD4(i);
		LCD_putc(2, (display>>12 & 0xF) + '0');
		LCD_putc(3, (display>>8 & 0xF) + '0');
		LCD_putc(4, (display>>4 & 0xF) + '0');
		LCD_putc(5, (display & 0xF) + '0');
		LCD_UpdateRequired(1, 0);
		
		if(!dfPageBufferCompare(1, i))
		{
			LCD_putc(1, '!');
			LCD_UpdateRequired(1, 0);
			break;
		}
	}

	Delay(2000);
	return ST_SCALE_FORMAT;
}

char StateScaleDownload(char input)
{
	//Usart_puts("TARE");
	unsigned int i, j=0, nPages, nPageSz, display, readPtr=0;
	char ch, buf[12];
		
	tScaleSample sample;
	tScaleHdr header;
	
	LCD_puts("D     ", 1);
	Usart_puts("DOWNLOAD START\n");
	
	nPages = dfGetPageCount();
	nPageSz = dfGetPageSize();

	for(i=0; i<nPages; ++i)
	{
		display = CHAR2BCD4(i);
		LCD_putc(2, (display>>12 & 0xF) + '0');
		LCD_putc(3, (display>>8 & 0xF) + '0');
		LCD_putc(4, (display>>4 & 0xF) + '0');
		LCD_putc(5, (display & 0xF) + '0');
		LCD_UpdateRequired(1, 0);
		
		j=0;
		
		dfContFlashReadEnable(i, 0);
		ch = DF_SPI_RW(0x00);
		readPtr = 1;
		
		if(ch == PAGE_MAGIC_IS_EMPTY)
			break;
		
		dfPageToBuffer(i, 1);
		
		if(ch & PAGE_MAGIC_HAS_HEADER)
		{
			Usart_puts("HDR\n");
			//read header
			readPtr += dfBufferReadStream(1, readPtr, sizeof(tScaleHdr), (unsigned char*)&header );
						
			CHAR2BCD8_signed(header.version, buf, sizeof(buf));
			Usart_puts(buf); Usart_Tx('|');
			CHAR2BCD8_signed(header.tHour, buf, sizeof(buf));
			Usart_puts(buf); Usart_Tx(':');
			CHAR2BCD8_signed(header.tMinute, buf, sizeof(buf));
			Usart_puts(buf); Usart_Tx(':');
			CHAR2BCD8_signed(header.tSecond, buf, sizeof(buf));
			Usart_puts(buf); Usart_Tx('|');
			CHAR2BCD8_signed(header.tDay, buf, sizeof(buf));
			Usart_puts(buf); Usart_Tx('/');
			CHAR2BCD8_signed(header.tMonth, buf, sizeof(buf));
			Usart_puts(buf); Usart_Tx('/');
			CHAR2BCD8_signed(header.tYear, buf, sizeof(buf));
			Usart_puts(buf); Usart_Tx('|');
			CHAR2BCD8_signed(header.tareValue, buf, sizeof(buf));
			Usart_puts(buf); Usart_Tx('\n');
		}
		
		while(readPtr + sizeof(tScaleSample) < nPageSz)
		{
			readPtr += dfBufferReadStream(1, readPtr, sizeof(tScaleSample), (unsigned char*)&sample );
			//Usart_Tx('(');
			
			CHAR2BCD8_signed(i, buf, sizeof(buf));
			Usart_puts(buf);
			
			Usart_Tx('|');
			
			CHAR2BCD8_signed(++j, buf, sizeof(buf));
			Usart_puts(buf);
			
			Usart_Tx('|');
			
			CHAR2BCD8_signed(sample.scale, buf, sizeof(buf));
			Usart_puts(buf);
			
			Usart_Tx('|');
			
			CHAR2BCD8_signed(sample.scaleAve, buf, sizeof(buf));
			Usart_puts(buf);
			
			Usart_Tx('|');
			
			CHAR2BCD8_signed(sample.temp, buf, sizeof(buf));
			Usart_puts(buf);			
			
			Usart_Tx('\n');
		}
		
		
		//read data
	}
	
	Delay(2000);
	
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