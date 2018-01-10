//***************************************************************************
//
//  File........: BCD.c
//
//  Author(s)...: ATMEL Norway
//
//  Target(s)...: ATmega169
//
//  Compiler....: IAR EWAAVR 2.28a
//
//  Description.: AVR Butterfly BCD conversion algorithms
//
//  Revisions...: 1.0
//
//  YYYYMMDD - VER. - COMMENT                                       - SIGN.
//
//  20030116 - 1.0  - Created                                       - KS
//
//***************************************************************************



/*****************************************************************************
*
*   Function name : CHAR2BCD2
*
*   Returns :       Binary coded decimal value of the input (2 digits)
*
*   Parameters :    Value between (0-99) to be encoded into BCD 
*
*   Purpose :       Convert a character into a BCD encoded character.
*                   The input must be in the range 0 to 99.
*                   The result is byte where the high and low nibbles
*                   contain the tens and ones of the input.
*
*****************************************************************************/
char CHAR2BCD2(char input)
{
    char high = 0;
    
    
    while (input >= 10)                 // Count tens
    {
        high++;
        input -= 10;
    }

    return  (high << 4) | input;        // Add ones and return answer
}

/*****************************************************************************
*
*   Function name : CHAR2BCD3
*
*   Returns :       Binary coded decimal value of the input (3 digits)
*
*   Parameters :    Value between (0-999) to be encoded into BCD 
*
*   Purpose :       Convert a character into a BCD encoded character.
*                   The input must be in the range 0 to 999.
*                   The result is an integer where the three lowest nibbles
*                   contain the ones, tens and hundreds of the input.
*
*****************************************************************************/
unsigned int CHAR2BCD3(unsigned int input)
{
    int high = 0;
        
    while (input >= 100)                // Count hundreds
    {
        high++;
        input -= 100;
    }

    high <<= 4;
    
    while (input >= 10)                 // Count tens
    {
        high++;
        input -= 10;
    }

    return  (high << 4) | input;        // Add ones and return answer
}

/*****************************************************************************
*
*   Function name : CHAR2BCD3
*
*   Returns :       Binary coded decimal value of the input (4 digits)
*
*   Parameters :    Value between (0-9999) to be encoded into BCD 
*
*   Purpose :       Convert a character into a BCD encoded character.
*                   The input must be in the range 0 to 9999.
*                   The result is an integer where the three lowest nibbles
*                   contain the ones, tens and hundreds of the input.
*
*****************************************************************************/
unsigned int CHAR2BCD4(unsigned int input)
{
    int high = 0;

	 while (input >= 1000)                // Count thousands
    {
        high++;
        input -= 1000;
    }
	
	high <<= 4;
	
    while (input >= 100)                // Count hundreds
    {
        high++;
        input -= 100;
    }

    high <<= 4;
    
    while (input >= 10)                 // Count tens
    {
        high++;
        input -= 10;
    }

    return  (high << 4) | input;        // Add ones and return answer
}


void CHAR2BCD8_signed(long input, char* out, char sz)
{
	char ngt = (input < 0);
	unsigned char i=0;

	if(ngt)
		input = -input;
	
	if(sz < 12)
		return;
	
	if(ngt)
		out[i++] = '-';
	
	unsigned long p = 1000000000;
	
	while(p > 1)
	{
		char ch = 0;
		while(input >= p)
		{
			ch++;
			input -= p;
		}
		out[i++] = ch + '0';
		p /= 10;
	}
	out[i++] = input + '0';
	out[i++] = 0;
}