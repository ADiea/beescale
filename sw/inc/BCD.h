//***************************************************************************
//
//  File........: BCD.h
//
//  Author(s)...: ATMEL Norway
//
//  Target(s)...: ATmega169
//
//  Compiler....: IAR EWAAVR 2.28a
//
//  Description.: AVR Butterfly main module
//
//  Revisions...: 1.0
//
//  YYYYMMDD - VER. - COMMENT                                       - SIGN.
//
//  20030116 - 1.0  - Created                                       - KS
//
//***************************************************************************

// Function declarations
char CHAR2BCD2(char input);
unsigned int CHAR2BCD3(unsigned int input);
unsigned int CHAR2BCD4(unsigned int input);
void CHAR2BCD8_signed(long input, char* out, char sz);
