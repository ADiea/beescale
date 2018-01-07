//Revisions number
#define SWHIGH  0
#define SWLOW   7
// mt
#define SWLOWLOW 6

#include "util.h"

void Initialization(void);
unsigned char StateMachine(char state, unsigned char stimuli);
char PowerSaveFunc(char input);
char AutoPower(char input);
void Delay(unsigned int millisec);
void OSCCAL_calibration(void);

char StateScaleCalibFunc(char input);
char StateScaleMeasureFunc(char input);
char StateScaleAutoMeasureFunc(char input);
char StateScaleFormat(char input);
char StateScaleDownload(char input);

#define BOOL    char

#ifndef FALSE
#define FALSE   0
#define TRUE    (!FALSE)
#endif

#ifndef NULL
// mt/ca:
// #define NULL    0
#define NULL ((void *)0)
#endif

#define AUTO    3

// Macro definitions
//mtA - 
// sbi and cbi are not longer supported by the avr-libc
// to avoid version-conflicts the macro-names have been 
// changed to sbiBF/cbiBF "everywhere"
#define sbiBF(port,bit)  (port |= (1<<bit))   //set bit in port
#define cbiBF(port,bit)  (port &= ~(1<<bit))  //clear bit in port
//mtE

// Menu state machine states
#define ST_AVRBF                        10
#define ST_AVRBF_REV                    11
#define ST_TIME                         20
#define ST_TIME_CLOCK                   21
#define ST_TIME_CLOCK_FUNC              22
#define ST_TIME_CLOCK_ADJUST            23
#define ST_TIME_CLOCK_ADJUST_FUNC       24
#define ST_TIME_CLOCKFORMAT_ADJUST      25
#define ST_TIME_CLOCKFORMAT_ADJUST_FUNC 36
#define ST_TIME_DATE                    27
#define ST_TIME_DATE_FUNC               28
#define ST_TIME_DATE_ADJUST             29
#define ST_TIME_DATE_ADJUST_FUNC        30
#define ST_TIME_DATEFORMAT_ADJUST       31
#define ST_TIME_DATEFORMAT_ADJUST_FUNC  32
#define ST_TEMPERATURE                  60
#define ST_TEMPERATURE_FUNC             61
#define ST_VOLTAGE                      70
#define ST_VOLTAGE_FUNC                 71
#define ST_LIGHT                        80
#define ST_LIGHT_FUNC                   81
#define ST_OPTIONS                      90
#define ST_OPTIONS_DISPLAY              91
#define ST_OPTIONS_DISPLAY_CONTRAST     92
#define ST_OPTIONS_DISPLAY_CONTRAST_FUNC 93
#define ST_OPTIONS_POWER_SAVE           96
#define ST_OPTIONS_POWER_SAVE_FUNC      97
#define ST_OPTIONS_AUTO_POWER_SAVE      98
#define ST_OPTIONS_AUTO_POWER_SAVE_FUNC 99

#define ST_SCALE 						100
#define ST_SCALE_START					101
#define ST_SCALE_DOWN					102
#define ST_SCALE_FORMAT					103
#define ST_SCALE_WORKING				104
#define ST_SCALE_DOWN_FUNC				105
#define ST_SCALE_FORMAT_FUNC			106


#define ST_SCALE_MEASURE				107
#define ST_SCALE_CALIB					108
