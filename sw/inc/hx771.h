#ifndef _HX711_H_
#define _HX711_H_

// Allows to set the pins and gain later than in the constructor
void HX711_begin(char gain = 128);

// check if HX711 is ready
// from the datasheet: When output data is not ready for retrieval, digital output pin DOUT is high. Serial clock
// input PD_SCK should be low. When DOUT goes to low, it indicates data is ready for retrieval.
bool HX711_is_ready();

// set the gain factor; takes effect only after a call to read()
// channel A can be set for a 128 or 64 gain; channel B has a fixed 32 gain
// depending on the parameter, the channel is also set to either A or B
void HX711_set_gain(char gain/* = 128*/);

// waits for the chip to be ready and returns a reading
long HX711_read();

// returns an average reading; times = how many times to read
long HX711_read_average(byte times = 10);

// returns (read_average() - OFFSET), that is the current value without the tare weight; times = how many readings to do
double HX711_get_value(byte times = 1);

// returns get_value() divided by SCALE, that is the raw value divided by a value obtained via calibration
// times = how many readings to do
float HX711_get_units(byte times = 1);

// set the OFFSET value for tare weight; times = how many times to read the tare value
void HX711_tare(byte times = 10);

// set the SCALE value; this value is used to convert the raw data to "human readable" data (measure units)
void HX711_set_scale(float scale = 1.f);

// get the current SCALE
float HX711_get_scale();

// set OFFSET, the value that's subtracted from the actual reading (tare weight)
void HX711_set_offset(long offset = 0);

// get the current OFFSET
long HX711_get_offset();

// puts the chip into power down mode
void HX711_power_down();

// wakes up the chip after power down mode
void HX711_power_up();


#endif