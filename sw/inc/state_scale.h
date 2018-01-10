#ifndef STATE_SCALE_H
#define STATE_SCALE_H

#define PAGE_MAGIC_IS_EMPTY 0xFF
#define PAGE_MAGIC_HAS_HEADER 0x01
#define PAGE_MAGIC_HAS_DATA 0x02

#define SCALELOG_VERSION 0

typedef struct _tScaleHdr
{
	//version
	unsigned char version;

	//time
	unsigned char tSecond;
	unsigned char tMinute;
	unsigned char tHour;
	unsigned char tDay;
	unsigned char tMonth;
	unsigned int tYear;

	//tare value at measurement start
	long tareValue;
} tScaleHdr;

typedef struct _tScaleSample
{
	long scale;
	int temp;
} tScaleSample;

#endif /*STATE_SCALE_H*/