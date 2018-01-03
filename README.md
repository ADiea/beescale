# beeScale
Outdoor scale for monitoring a beehive's weight
Based on hacked version of commercial 180kg scale and HX711 opamp. MCU board is Atmel Butterfly.

## Credits
HX711 driver based on https://github.com/bogde/HX711

Source code is based on gcc port by Martin Thomas of initial Atmel provided code sample. 
Martin has relased the source code into [public domain](http://community.atmel.com/projects/avr-butterfly-application-gcc-port) and an outdated version can be found [here](http://ohm.bu.edu/~pbohn/__Atmel_Butterfly_Demo_Board/Programming_Projects/Source_Code/AVR_Orginal/butterfly_demo_gcc/). The original [location of the sourcecode](http://siwawi.bauing.uni-kl.de/avr_projects/) is no longer available 
Atmel IAR source code can be found [here](http://www.atmel.com/tools/avrbutterfly.aspx)

Current project only makes use of a subset of the original code, mainly the BSP/drivers, as the final functionality is different.
