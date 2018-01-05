//hx711 driver based on https://github.com/bogde/HX711
//Author github.com/adiea

#include <stdint.h>
#include <avr/io.h>
#include "main.h"
#include "hx711.h"

#define HX711_POUT PORTE
#define HX711_DDR DDRE
#define HX711_PIN PINE

#define HX711_CK 6
#define HX711_IN 5

//byte PD_SCK;	// Power Down and Serial Clock Input Pin
//byte DOUT;		// Serial Data Output Pin
char GAIN=1;		// amplification factor
long OFFSET = 0;	// used for tare weight
float SCALE = 1.0f; // used to return weight in grams, kg, ounces, whatever


//based on Arduino impl in wiring_shift.c
char shiftIn() {
	char value = 0;
	char i;
	__asm__ __volatile__ ("nop");
	__asm__ __volatile__ ("nop");

	for (i = 0; i < 8; ++i) {
		HX711_POUT |= 1<<HX711_CK;
		__asm__ __volatile__ ("nop");
		__asm__ __volatile__ ("nop");
		if(HX711_PIN & 1<<HX711_IN)
			value |= 1 << (7 - i);
		HX711_POUT &= ~(1<<HX711_CK);
		__asm__ __volatile__ ("nop");
		__asm__ __volatile__ ("nop");
	}
	return value;
}

void HX711_begin(/*char dout, char pd_sck,*/ char gain) {

	HX711_DDR |= 1<<HX711_CK;
	HX711_DDR &= ~(1<<HX711_IN);

	HX711_set_gain(gain);
}

char HX711_is_ready() {
	return !(HX711_PIN & 1<<HX711_IN);
}

void HX711_set_gain(char gain) {
	switch (gain) {
		case 128:		// channel A, gain factor 128
			GAIN = 1;
			break;
		case 64:		// channel A, gain factor 64
			GAIN = 3;
			break;
		case 32:		// channel B, gain factor 32
			GAIN = 2;
			break;
	}

	HX711_POUT &= ~(1<<HX711_CK);
	HX711_read();
}

long HX711_read() {
	// wait for the chip to become ready
	while (!HX711_is_ready()) {

	}

	long value = 0;
	char data[3] = { 0 };
	char filler = 0x00;

	// pulse the clock pin 24 times to read the data
	data[2] = shiftIn();
	data[1] = shiftIn();
	data[0] = shiftIn();

	// set the channel and the gain factor for the next reading using the clock pin
	for (unsigned int i = 0; i < GAIN; i++) {
		HX711_POUT |= 1<<HX711_CK;
		__asm__ __volatile__ ("nop");
		__asm__ __volatile__ ("nop");
		HX711_POUT &= ~(1<<HX711_CK);
		__asm__ __volatile__ ("nop");
		__asm__ __volatile__ ("nop");
	}

	// Replicate the most significant bit to pad out a 32-bit signed integer
	if (data[2] & 0x80) {
		filler = 0xff;
	} else {
		filler = 0x00;
	}

	// Construct a 32-bit signed integer
	value = ( (unsigned long)(filler) << 24
			| (unsigned long)(data[2]) << 16
			| (unsigned long)(data[1]) << 8
			| (unsigned long)(data[0]) );

	return value;
}

long HX711_read_average(char times) {
	long sum = 0;
	for (char i = 0; i < times; i++) {
		sum += HX711_read();
	}
	return sum / times;
}

long HX711_get_value(char times) {
	return HX711_read_average(times) - OFFSET;
}

float HX711_get_units(char times) {
	return HX711_get_value(times) / SCALE;
}

void HX711_tare(char times) {
	double sum = HX711_read_average(times);
	HX711_set_offset(sum);
}

void HX711_set_scale(float scale) {
	SCALE = scale;
}

float HX711_get_scale() {
	return SCALE;
}

void HX711_set_offset(long offset) {
	OFFSET = offset;
}

long HX711_get_offset() {
	return OFFSET;
}

void HX711_power_down() {
	HX711_POUT &= ~(1<<HX711_CK);
	__asm__ __volatile__ ("nop");
	__asm__ __volatile__ ("nop");
	HX711_POUT |= 1<<HX711_CK;
	__asm__ __volatile__ ("nop");
	__asm__ __volatile__ ("nop");
}

void HX711_power_up() {
	HX711_POUT &= ~(1<<HX711_CK);
	__asm__ __volatile__ ("nop");
}